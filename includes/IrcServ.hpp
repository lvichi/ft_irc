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


#pragma once

#include <iostream>     // std::cerr std::cout std::endl
#include <string>       // std::string
#include <sstream>      // std::stringstream
#include <cstdlib>      // std::strtod
#include <vector>       // std::vector
#include <map>          // std::map
#include <deque>        // std::deque
#include <list>         // std::list
#include <stdexcept>    // std::runtime_error
#include <algorithm>    // for std::equal
#include <fcntl.h>      // fcntl, F_SETFL, O_NONBLOCK
#include <poll.h>       // struct pollfd
#include <sys/socket.h> // socket, AF_INET, AF_INET6, SOCK_STREAM
#include <unistd.h>     // for close()
#include <netinet/in.h> // struct sockaddr_in, struct sockaddr_in6, htons
#include <arpa/inet.h>  // inet_ntoa, ntohs
#include <errno.h>      // errno, EAGAIN, EWOULDBLOCK
#include <csignal>      // std::signal, SIGINT, SIGTERM

#define IRC_BUFFER_SIZE 512
#define COMMAND_DELIMITER "\r\n"


class IrcServ
{
  private:
    IrcServ();
    IrcServ( const IrcServ& original );
    IrcServ& operator=( const IrcServ& original );

    const unsigned int                    port;
    const std::string                     password;
    std::vector<pollfd>                   poolFDs;
    std::map<unsigned int, std::string>   fdBuffers;
    std::map<unsigned int, std::string>   outgoingMessages;

    struct CommandStruct
    {
      unsigned int                        clientFD;
      std::string                         prefix;
      std::string                         command;
      std::vector<std::string>            parameters;
      std::string                         trailing;
    };

    std::deque<CommandStruct>             commands;

    static void     handleSigint( int signal );
    void            createServerSocket();
    void            connectClient();
    std::string     handleClientInput( pollfd& clientFD );
    void            parseCommands( std::string& message, unsigned int clientFD );
    void            executeCommands();
    void            sendMessages( pollfd& clientFD );

    // DEBUG
    void            printCommands();

  public:
    IrcServ( const unsigned int port, const std::string& password );
    ~IrcServ();

    int             runServer();
};
