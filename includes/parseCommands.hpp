#pragma once

#include <string>       // std::string
#include <list>         // std::list
#include <vector>       // std::vector
#include <algorithm>    // std::search

#define COMMAND_DELIMITER "\r\n"


struct CommandStruct
{
  unsigned int                        clientFD;
  std::string                         prefix;
  std::string                         command;
  std::vector<std::string>            parameters;
  std::string                         trailing;
};


std::list<CommandStruct>  parseCommands( std::string& message, unsigned int clientFD );


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
