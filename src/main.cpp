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
    return (std::cerr << "Caught exception: " << e.what() << std::endl, 1);
  }

  return 0;
}

