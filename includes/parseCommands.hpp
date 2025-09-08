#pragma once

#include <string>       // std::string
#include <list>         // std::list
#include <vector>       // std::vector
#include <algorithm>    // std::search
#include <map>          // map container

#define COMMAND_DELIMITER "\r\n"


struct CommandStruct
{
  unsigned int                        clientFD;
  std::string                         prefix;
  std::string                         command;
  std::vector<std::string>            parameters;
  std::string                         trailing;
  unsigned int                        errorCode;
//  int                                 type;
};

typedef bool (*checks[8])(CommandStruct &cmd, std::map<unsigned int, std::string> &out);

typedef void (*execs[8])(CommandStruct &cmd, std::map<unsigned int, std::string> &out);

std::list<CommandStruct>  parseCommands( std::string& message, unsigned int clientFD );

bool checkPass(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
bool checkNick(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
bool checkKick(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
bool checkInvite(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
bool checkTopic(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
bool checkPrivmsg(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
bool checkMode(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);

void execPass(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execNick(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execKick(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execInvite(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execTopic(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execPrivmsg(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execMode(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
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
