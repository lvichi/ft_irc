/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lvichi <lvichi@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:22:24 by lvichi            #+#    #+#             */
/*   Updated: 2025/07/09 13:22:24 by lvichi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/IrcServ.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"


volatile sig_atomic_t g_endServer = 0;


IrcServ::IrcServ( unsigned int port, const std::string& password, bool debug )
  : port( port ), password( password ), debug( debug ) {}


IrcServ::~IrcServ()
{
  // Delete all clients
  for ( std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it )
    delete it->second;
  
  // Delete all channels
  for ( std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it )
    delete it->second;
  
  for ( std::vector<pollfd>::iterator it = poolFDs.begin(); it != poolFDs.end(); ++it )
    if ( it->fd >= 0 )
      close( it->fd );
}


void IrcServ::handleSigint( int )
{
  g_endServer = 1;
}


void IrcServ::runServer()
{
  std::signal( SIGINT, handleSigint );

  createServerSocket();

  while ( !g_endServer ) {
    poll( &poolFDs[0], poolFDs.size(), -1 );

    std::vector<pollfd>::iterator it;
    for ( it = poolFDs.begin(); it != poolFDs.end(); ++it ) {
      if ( it->revents & POLLIN ) {

        if ( it->fd == poolFDs[0].fd ) {
          connectClient();
          break;
        }

        std::string message = receiveMessages( *it );
        if ( message.empty() )
          continue;

        parseCommands( *this, message, it->fd );
      }

      if ( it->revents & POLLOUT )
        sendMessages( *it );
    }

    for ( it = poolFDs.begin(); it != poolFDs.end(); ++it ) {
      if ( outgoingMessages.find( it->fd ) != outgoingMessages.end() ) {
        it->events = POLLIN | POLLOUT;
      } else {
        it->events = POLLIN;
      }
    }

  }
  std::cout << "\rServer shutting down." << std::endl;
}


void IrcServ::createServerSocket()
{
  pollfd serverFD;

  serverFD.fd = socket( AF_INET, SOCK_STREAM, 0 );

  if ( serverFD.fd < 0 )
    throw std::runtime_error( "Error creating socket." );

  serverFD.events = POLLIN;
  serverFD.revents = 0;

  fcntl( serverFD.fd, F_SETFL, O_NONBLOCK );

  struct sockaddr_in addr4 = {};
  addr4.sin_family = AF_INET;
  addr4.sin_addr.s_addr = INADDR_ANY;
  addr4.sin_port = htons( port );

  int yes = 1;
  if ( setsockopt( serverFD.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) < 0 )
    throw std::runtime_error( "Error setting socket options." );

  if ( bind( serverFD.fd, ( struct sockaddr* )&addr4, sizeof( addr4 ) ) < 0 )
    throw std::runtime_error( "Error binding socket." );

  if ( listen( serverFD.fd, 5 ) < 0 )
    throw std::runtime_error( "Error listening on socket." );

  std::cout << "Server listening on port " << port << std::endl;

  poolFDs.push_back( serverFD );
}


void IrcServ::connectClient()
{
  struct sockaddr_in  addr4;
  pollfd              clientPollFD;
  socklen_t           addrLen = sizeof(addr4);

  int clientFD = accept( poolFDs[0].fd, (struct sockaddr*)&addr4, &addrLen );

  if ( clientFD < 0 )
    return;

  fcntl( clientFD, F_SETFL, O_NONBLOCK );

  clientPollFD.fd = clientFD;
  clientPollFD.events = POLLIN;
  clientPollFD.revents = 0;

  poolFDs.push_back( clientPollFD );
  
  addClient( clientFD );

  std::cout << "Client connected. fd: " << clientPollFD.fd << std::endl;
}


void IrcServ::closeClient( pollfd& clientFD )
{
  std::cout << "Closed client connection. fd: " << clientFD.fd << std::endl;

  removeClient( clientFD.fd );

  close( clientFD.fd );
  fdBuffers.erase( clientFD.fd );
  outgoingMessages.erase( clientFD.fd );

  clientFD.fd = -1;
}


std::string IrcServ::receiveMessages( pollfd& clientFD )
{
  char          buffer[IRC_BUFFER_SIZE + 1];
  int           bytesRead;
  std::string   message;

  bytesRead = read( clientFD.fd, buffer, IRC_BUFFER_SIZE );

  if ( bytesRead <= 0  ) {
    if ( bytesRead < 0 )
      std::cerr << "Error reading from client. fd: " << clientFD.fd << std::endl;

    closeClient( clientFD );

    return message;
  }

  fdBuffers[clientFD.fd] += std::string(buffer, bytesRead);

  while ( true ) {
    unsigned int pos = fdBuffers[clientFD.fd].find("\r\n");
    if ( pos > fdBuffers[clientFD.fd].size() )
      break;

    if ( pos + 2 > IRC_BUFFER_SIZE ) {
      std::cerr << "Error: Command too long. fd: " << clientFD.fd << std::endl;
      fdBuffers[clientFD.fd].erase( 0, pos + 2 );
      continue;
    }

    message += fdBuffers[clientFD.fd].substr( 0, pos + 2);
    fdBuffers[clientFD.fd].erase( 0, pos + 2 );
  }

  if ( fdBuffers[clientFD.fd].size() > IRC_BUFFER_SIZE ) {
    std::cerr << "Error: Buffer overflow. fd: " << clientFD.fd << std::endl;
    closeClient( clientFD );
  }

  return message;
}


void IrcServ::sendMessages( pollfd& clientFD )
{
  std::string&   buffer = outgoingMessages[clientFD.fd];
  int            bytesSent;

  if ( buffer.empty() ) {
    closeClient( clientFD );
    return;
  }

  bytesSent = send( clientFD.fd, buffer.c_str(), buffer.size(), 0 );

  if ( bytesSent <= 0 ) {
    std::cerr << "Error sending message to client. fd: " << clientFD.fd << std::endl;
    closeClient( clientFD );
    return;
  }

  buffer.erase( 0, bytesSent );

  if ( buffer.empty() )
    outgoingMessages.erase( clientFD.fd );

  return;
}

void IrcServ::outgoingMessage( int clientFD, const std::string& message )
{
  outgoingMessages[clientFD] += message;
}

bool IrcServ::isPasswordValid( const std::string& pass ) const
{
  return pass == password;
}

bool IrcServ::isDebug() const
{
  return debug;
}

// Client management methods
void IrcServ::addClient( int fd )
{
  clients[fd] = new Client(fd);
}

void IrcServ::removeClient( int fd )
{
  std::map<int, Client*>::iterator it = clients.find(fd);
  if ( it != clients.end() ) {
    delete it->second;
    clients.erase(it);
  }
}

Client* IrcServ::getClient( int fd )
{
  std::map<int, Client*>::iterator it = clients.find(fd);
  return ( it != clients.end() ) ? it->second : NULL;
}

Client* IrcServ::getClientByNick(const std::string& nickname) {
    const std::map<int, Client*>& clients = getClients();
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getNickname() == nickname)
            return it->second;
    }
    return NULL;
}

const std::map<int, Client*>& IrcServ::getClients() const
{
  return clients;
}

// Channel management methods
Channel* IrcServ::getChannel( const std::string& name )
{
  std::map<std::string, Channel*>::iterator it = channels.find(name);
  return ( it != channels.end() ) ? it->second : NULL;
}

Channel* IrcServ::createChannel( const std::string& name )
{
  if ( channelExists(name) )
    return getChannel(name);
  
  channels[name] = new Channel(name);
  return channels[name];
}

void IrcServ::removeChannel( const std::string& name )
{
  std::map<std::string, Channel*>::iterator it = channels.find(name);
  if ( it != channels.end() ) {
    delete it->second;
    channels.erase(it);
  }
}

bool IrcServ::channelExists( const std::string& name ) const
{
  return channels.find(name) != channels.end();
}

const std::map<std::string, Channel*>& IrcServ::getChannels() const
{
  return channels;
}
