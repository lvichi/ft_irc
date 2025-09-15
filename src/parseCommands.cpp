#include "../includes/parseCommands.hpp"
#include "../includes/macros.hpp"
#include <sstream>
#include <iostream>

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
//prefix is only for server side messages, never for user messages.

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
//prefix is server side only, should not be supplied by client
static bool is_valid_input(std::string& msg){
	if (msg.find_first_of("\a\0") != msg.npos || msg[0] == ':')
		return false;
	return true;
}

static CommandStruct extractCommand(CommandStruct &cmd, std::string &cmdLine){
	std::istringstream fullCmd(cmdLine);
	std::string tok;
	bool hasTrailing = false;
	cmd.errorCode = 0;

	fullCmd >> tok;
	cmd.command = tok;
	while (fullCmd >> tok){
		if (tok[0] == ':'){
			hasTrailing = true;
			break;
		}
		cmd.parameters.push_back(tok);
	}
	if (cmd.parameters.size() > PARAM_MAX)
		cmd.errorCode = ERR_INPUTTOOLONG;
	if (hasTrailing){
		cmd.trailing = std::string(cmdLine, cmdLine.find_first_of(':', 1)
			+ 1, cmdLine.length() - 2);
	}
	return cmd;
}

void  parseCommands( IrcServ& server, std::string& message, unsigned int clientFD )
{
	std::list<CommandStruct>  commands;
	CommandStruct             cmd;
	std::string               eoc = CRLF;

	std::cout << PRP << message << RST << std::endl;
	while ( true ) {
		std::string::iterator it = message.begin();
		std::string::iterator endOfCommand;
		endOfCommand = std::search( it, message.end(), eoc.begin(), eoc.end() );
		if ( endOfCommand == message.end() )
			break;
		std::string commandLine( it, endOfCommand );
		if (is_valid_input(commandLine)){
			extractCommand(cmd, commandLine);
			cmd.clientFD = clientFD;
		}
		commands.push_back( cmd );
		cmd.parameters.clear();
		message.erase( message.begin(), endOfCommand + 2 );
	}
	executeCommands( commands, server );
}
