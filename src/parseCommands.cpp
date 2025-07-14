

#include "IrcServ.hpp"

/*
   Command structure for parsing IRC messages
   Example: ":nick!user@host PRIVMSG #channel :Hello, world!\r\n"
   This would be parsed into:
   prefix: "nick!user@host"
   command: "PRIVMSG"
   params: ["#channel"]
   trailing: "Hello, world!"
   it end with '\r\n'.

  struct CommandStruct
  {
    unsigned int              clientFD;    // Client file descriptor
    std::string               prefix;       // Optional prefix (starts with :)
    std::string               command;      // IRC command (e.g., NICK, USER, PRIVMSG)
    std::vector<std::string>  parameters;   // Parameters (excluding trailing)
    std::string               trailing;     // Last parameter (starts with :, may contain spaces)
  }
*/

// Parses multiple commands from a single message.
// The message can contain multiple commands separated by '\r\n'.
// this is a simplified version for demonstration purposes and test of
// the IrcServ class.
void IrcServ::parseCommands( std::string& message, unsigned int clientFD )
{
  CommandStruct     cmd;
  std::string   eoc = COMMAND_DELIMITER;

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
}
