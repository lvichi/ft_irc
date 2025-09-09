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
#include <vector>       // std::vector
#include <map>          // std::map
#include <cstdlib>      // std::strtod
#include <csignal>      // std::signal, SIGINT, SIGTERM
#include <cerrno>       // errno, EAGAIN, EWOULDBLOCK
#include <fcntl.h>      // fcntl, F_SETFL, O_NONBLOCK
#include <poll.h>       // struct pollfd
#include <sys/socket.h> // socket, AF_INET, AF_INET6, SOCK_STREAM
#include <netinet/in.h> // struct sockaddr_in, htons

#define IRC_BUFFER_SIZE 512

class IrcServ;

#include "parseCommands.hpp"


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

    static void     handleSigint( int );
    void            createServerSocket();
    void            connectClient();
    void            closeClient( pollfd& clientFD );
    std::string     receiveMessages( pollfd& clientFD );
    void            sendMessages( pollfd& clientFD );

  public:
    IrcServ( const unsigned int port, const std::string& password );
    ~IrcServ();

    void            runServer();
    void            outgoingMessage( int clientFD, const std::string& message );
    bool            isPasswordValid( const std::string& pass ) const;
};
