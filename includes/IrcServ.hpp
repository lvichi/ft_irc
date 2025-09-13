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

#include "parseCommands.hpp"
#include "executeCommands.hpp"

#define IRC_BUFFER_SIZE 512

class Client;
class Channel;
class IrcServ;

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
    std::map<int, Client*>                clients;
    std::map<std::string, Channel*>       channels;

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
    
    // Client management
    void            addClient( int fd );
    void            removeClient( int fd );
    Client*         getClient( int fd );
    Client*         getClientByNick(const std::string& nickname);
    const std::map<int, Client*>& getClients() const;
    
    // Channel management
    Channel*        getChannel( const std::string& name );
    Channel*        createChannel( const std::string& name );
    void            removeChannel( const std::string& name );
    bool            channelExists( const std::string& name ) const;
    const std::map<std::string, Channel*>& getChannels() const;
};

typedef struct s_serverInfo{
  const std::string       pass;
  std::list<std::string>  channels;
  std::list<std::string>  nicks;
  std::list<std::string>  users;
} t_serverInfo;

t_serverInfo  *server(void);
