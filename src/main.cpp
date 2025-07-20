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


void parseArguments( const int argc, char** argv, unsigned int* port, std::string* password );


int main( int argc, char** argv )
{
  unsigned int  port;
  std::string   password;

  try {
    parseArguments( argc, argv, &port, &password );
    IrcServ server( port , password );
    server.runServer();
  } catch ( std::exception& e ) {
    return ( std::cerr << e.what() << std::endl, 1 );
  }
}


void parseArguments( int argc, char** argv, unsigned int* port, std::string* password )
{
  char*    end = 0;
  double   portRaw;

  if ( argc != 3 )
    throw std::runtime_error( "Usage: " + std::string(argv[0]) + " <port> <password>" );

  portRaw = std::strtod( argv[1], &end );

  if ( *end != 0 || portRaw < 1 || portRaw > 65535 )
    throw std::runtime_error( "Invalid port: " + std::string(argv[1]) );

  if ( argv[2][0] == 0 )
    throw std::runtime_error( "Invalid password." );

  *port = static_cast<unsigned int>(portRaw);
  *password = argv[2];
}
