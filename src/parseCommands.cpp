#include "../includes/parseCommands.hpp"
#include "../includes/macros.hpp"

//NOTES on IRC message params sanitizer:
//prefix is optional, servernam or nick[!user]@host;
//commands are ALWAYS upper case, but may cause issues with client and
//may need to normalize to all capital letters
//command can also be 3 digit numbers.
//up to 15 parameters, last one is trailing with : may contain spaces after trailing sufix
//tags need to re-read subject
//CRLF must be present meaning \r\n;
//NULL is always forbidden and CRLF must always come at the end;
//0x01 can be supported for ctcp payload for PRIVMSG/NOTICE
//prefix ALWAYS starts with : no spaces between servername or nick[!user]@host
//ex. = :nick!user@host PRIVMSG #chat :hello world\r\n

//notes on commands:
//KICK: <#channel> <user> [<comment>]; channel exists, sender is in the channel
//sender has operator privileges, +o;
//target user is in the channel;
//INVITE: <nick> <channel>; check if target nick exists, and is connected as client
//check if channel has +i (invite only), case, sender must have +o operator privileges
//if the channel does not exist, it is possible to create a new one;
//TOPIC: <channel> [<topic>]; if channel mode +t is set need +o;
//if topic is empty string, should clear topic
//MODE: +i, +t, +k, +o, +l;
//+k = keypassword; +l = user limit to channel;
//PASS to be decided
//NICK

/*struct CommandStruct
{
  unsigned int                        clientFD;
  std::string                         prefix;
  std::string                         command;
  std::vector<std::string>            parameters;
  std::string                         trailing;
};*/

//fist basic check
static bool is_valid_input(std::string& msg){
  size_t tok = 0;
  size_t n_tok = 0;
  std::string cmd;
  bool has_prefix = 0;

  if (msg.find_first_of("\a\0") != msg.npos)
    return false;
  if (msg[0] == ':'){
    has_prefix = 1;
    tok = msg.find_first_of(32);
  }
  n_tok = msg.find_first_of(32, tok);
  return true;
}

//const char *val_cmd[8] = {"PASS", "NICK", "KICK", "INVITE", "TOPIC",
// "PRIVMSG", "MODE", NULL};

std::list<CommandStruct> parseCommands( std::string& message, unsigned int clientFD )
{
  std::list<CommandStruct>  commands;
  CommandStruct             cmd;
  std::string               eoc = CRLF;

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
