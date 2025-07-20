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


int parseArguments( const int argc, char** argv, unsigned int* port, std::string* password );


int main( int argc, char** argv )
{
  unsigned int  port;
  std::string   password;

  if ( parseArguments( argc, argv, &port, &password ) )
    return 1;

  IrcServ server( port , password );

  try {
    if ( server.runServer() )
      return 1;
  } catch ( std::exception& e ) {
    return ( std::cerr << e.what() << std::endl, 1 );
  }

  return 0;
}


int parseArguments( int argc, char** argv, unsigned int* port, std::string* password )
{
  char*    end = 0;
  double   portRaw;

  if ( argc != 3 )
    return ( std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl, 1 );

  portRaw = std::strtod( argv[1], &end );

  if ( *end != 0 || portRaw < 1 || portRaw > 65535 )
    return ( std::cerr << "Invalid port: " << argv[1] << std::endl, 1 );

  if ( argv[2][0] == 0 )
    return ( std::cerr << "Invalid password." << std::endl, 1 );

  *port = static_cast<unsigned int>(portRaw);
  *password = argv[2];

  return 0;
}
