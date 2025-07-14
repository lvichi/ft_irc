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


static bool endServer = false;


IrcServ::IrcServ( unsigned int port, const std::string& password )
  : port( port ), password( password ) {}


IrcServ::~IrcServ()
{
  for ( std::vector<pollfd>::iterator it = poolFDs.begin(); it != poolFDs.end(); ++it )
    if ( it->fd >= 0 )
      close( it->fd );
}


int IrcServ::runServer()
{
  std::string  message;

  std::signal(SIGINT, handleSigint);

  if ( createServerSocket() )
    return 1;

  while ( !endServer ) {
    std::cout << "\nWaiting for connections/messages ..." << std::endl << std::endl;
    poll( &poolFDs[0], poolFDs.size(), -1 );

    for ( std::vector<pollfd>::iterator it = poolFDs.begin(); it != poolFDs.end(); ++it ) {
      if ( it->revents & POLLIN ) {

        if ( it->fd == poolFDs[0].fd ) {
          if ( connectClient() )
            return 1;
          break;
        }

        message = handleClientInput(*it);
        if ( message.empty() )
          continue;

        parseCommands( message, it->fd );

        executeCommands();
      }
    }
  }

  return 0;
}


int IrcServ::createServerSocket()
{
  pollfd serverFD = {};

  serverFD.fd = socket( AF_INET, SOCK_STREAM, 0 );

  if ( serverFD.fd < 0 )
    return ( std::cerr << "Error creating socket." << std::endl, 1 );

  serverFD.events = POLLIN;

  fcntl( serverFD.fd, F_SETFL, O_NONBLOCK );

  struct sockaddr_in addr4 = {};
  addr4.sin_family = AF_INET;
  addr4.sin_addr.s_addr = INADDR_ANY;
  addr4.sin_port = htons( port );

  int yes = 1;
  if ( setsockopt( serverFD.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) < 0 )
    return ( std::cerr << "Error setting socket options." << std::endl, 1 );

  if ( bind(serverFD.fd, ( struct sockaddr* )&addr4, sizeof( addr4 ) ) < 0 )
    return ( std::cerr << "Error binding socket." << std::endl, 1 );

  if ( listen(serverFD.fd, 5 ) < 0 )
    return ( std::cerr << "Error listening on socket." << std::endl, 1 );


  std::cout << "Server listening on port " << port << std::endl;

  poolFDs.push_back( serverFD );

  return 0;
}


int IrcServ::connectClient()
{
  struct sockaddr_in  addr4 = {};
  pollfd              clientPollFD;
  socklen_t           addrLen;

  addrLen = sizeof(addr4);

  int clientFD = accept( poolFDs[0].fd, (struct sockaddr*)&addr4, &addrLen );

  if ( clientFD < 0 && errno != EAGAIN && errno != EWOULDBLOCK )
    return ( std::cerr << "Error accepting connection." << std::endl, 1 );
  if ( clientFD < 0 )
    return 0;

  fcntl( clientFD, F_SETFL, O_NONBLOCK );

  clientPollFD.fd = clientFD;
  clientPollFD.events = POLLIN;
  clientPollFD.revents = 0;

  poolFDs.push_back( clientPollFD );

  std::cout << "Client connected. fd: " << clientPollFD.fd << std::endl;

  return 0;
}


void handleSigint(int)
{
  endServer = true;
  std::cout << "\rServer shutting down." << std::endl;
}


std::string IrcServ::handleClientInput( pollfd& clientFD )
{
  char          buffer[IRC_BUFFER_SIZE];
  int           bytesRead;
  std::string   message;

  bytesRead = read( clientFD.fd, buffer, sizeof(buffer) - 1 );

  if ( bytesRead <= 0  ) {
    if ( bytesRead < 0 )
      std::cerr << "Error reading from client. fd: " << clientFD.fd << std::endl;
    else
      std::cout << "Client closed connection. fd: " << clientFD.fd << std::endl;

    close( clientFD.fd );
    fdBuffers[clientFD.fd].clear();
    clientFD.fd = -1;

    return message;
  }

  fdBuffers[clientFD.fd] += std::string(buffer, bytesRead);

  if ( fdBuffers[clientFD.fd].size() > 2 &&
         std::equal( fdBuffers[clientFD.fd].end() - 2, fdBuffers[clientFD.fd].end(), "\r\n" ) ) {
    message = fdBuffers[clientFD.fd];
    fdBuffers[clientFD.fd].clear();
    }

  return message;
}


void  IrcServ::printCommands()
{
  int i = 0;

  for ( std::deque<CommandStruct>::iterator it = commands.begin(); it != commands.end(); ++it ) {

    CommandStruct& cmd = *it;
    std::cout << "Commands[" << i++ << "] :" << std::endl
              << "  Client FD: " << cmd.clientFD << std::endl
              << "  Prefix: " << cmd.prefix << std::endl
              << "  Command: " << cmd.command << std::endl
              << "  Parameters: ";

    for ( std::vector<std::string>::iterator param = cmd.parameters.begin(); param != cmd.parameters.end(); ++param )
      std::cout << *param << " ";
    std::cout << std::endl;

    std::cout << "  Trailing: " << cmd.trailing << std::endl;
  }
}
