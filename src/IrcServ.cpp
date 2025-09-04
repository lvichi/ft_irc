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


#include "IrcServ.hpp"


volatile sig_atomic_t g_endServer = 0;


IrcServ::IrcServ( unsigned int port, const std::string& password )
  : port( port ), password( password ) {}


IrcServ::~IrcServ()
{
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

        // TO BE CORRECTLY IMPLEMENTED
        std::list<CommandStruct> commands = parseCommands( message, it->fd );
        executeCommands( commands, outgoingMessages );

      }

      if ( it->revents & POLLOUT )
        sendMessages( *it );

      outgoingMessages.find( it->fd ) != outgoingMessages.end() ?
         it->events = ( POLLOUT | POLLIN ) : it->events = POLLIN;

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

  std::cout << "Client connected. fd: " << clientPollFD.fd << std::endl;
}


void IrcServ::closeClient( pollfd& clientFD )
{
  std::cout << "Closed client connection. fd: " << clientFD.fd << std::endl;

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
    if ( errno != EAGAIN && errno != EWOULDBLOCK )
      return

    std::cerr << "Error sending message to client. fd: " << clientFD.fd << std::endl, void();
    closeClient( clientFD );
  }

  buffer.erase( 0, bytesSent );

  if ( buffer.empty() )
    outgoingMessages.erase(clientFD.fd);

  return;
}
