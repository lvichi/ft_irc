#include "../includes/parseCommands.hpp"
#include "../includes/macros.hpp"
#include <sstream>
#include <iostream>

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
  unsigned int                        err;
  };*/

//fist basic check
static bool is_valid_input(std::string& msg){

	if (msg.find_first_of("\a\0") != msg.npos)
		return false;
	return true;
}

static CommandStruct extractCommand(CommandStruct &cmd, std::string &cmdLine){
	std::istringstream fullCmd(cmdLine);
	std::string tok;
	bool hasPrefix = false;
	bool hasTrailing = false;
	bool firstPass = true;

	fullCmd >> tok;
	if (tok[0] == ':'){
		cmd.prefix = std::string(tok, 1, tok.length());
		hasPrefix = 1;
	}
	else
		cmd.command = tok;
	while (fullCmd >> tok){
		if (firstPass && hasPrefix){
			cmd.command = tok;
			firstPass = false;
			continue;
		}
		if (tok[0] == ':'){
			hasTrailing = true;
			break;
		}
		cmd.parameters.push_back(tok);
	}
	if (hasTrailing){
		cmd.trailing = std::string(cmdLine, cmdLine.find_first_of(':', 1) + 1, cmdLine.length() - 2);
	}
	return cmd;
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
		if (is_valid_input(commandLine)){
			extractCommand(cmd, commandLine);
			/*cmd.clientFD = clientFD;
			cmd.command = "PRIVMSG";
			cmd.prefix = "";
			cmd.parameters.push_back( "#all" );
			cmd.trailing = commandLine;
*/
			cmd.clientFD = clientFD;
		}
		commands.push_back( cmd );
		cmd.parameters.clear();
		message.erase( message.begin(), endOfCommand + 2 );
	}
	return commands;
}
