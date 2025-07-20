#include "parseCommands.hpp"


std::list<CommandStruct> parseCommands( std::string& message, unsigned int clientFD )
{
  std::list<CommandStruct>  commands;
  CommandStruct             cmd;
  std::string               eoc = COMMAND_DELIMITER;

  while ( true ) {
    std::string::iterator it = message.begin();

    std::string::iterator endOfCommand;
    endOfCommand = std::search( it, message.end(), eoc.begin(), eoc.end() );

    if ( endOfCommand == message.end() )
      break;

    std::string commandLine( it, endOfCommand );

    cmd.clientFD = clientFD;
    cmd.command = "PRIVMSG";
    cmd.prefix = "";
    cmd.parameters.push_back( "#all" );
    cmd.trailing = commandLine;

    commands.push_back( cmd );

    cmd.parameters.clear();

    message.erase( message.begin(), endOfCommand + 2 );
  }

  return commands;
}
