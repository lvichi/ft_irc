#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include <stdexcept>
#include <sstream>

static std::string errorToString(t_error error) {
    std::ostringstream oss;
    oss << error;
    return oss.str();
}

static std::string getErrorMessage(t_error error) {
    const t_error errorCodes[] = {
        ERR_NOSUCHNICK, ERR_NOSUCHCHANNEL, ERR_CANNOTSENDTOCHAN, ERR_TOOMANYCHANNELS,
        ERR_NOORIGIN, ERR_NORECIPIENT, ERR_NOTEXTTOSEND, ERR_NOTOPLEVEL,
        ERR_WILDTOPLEVEL, ERR_BADMASK, ERR_INPUTTOOLONG, ERR_UNKNOWNCOMMAND,
        ERR_NOMOTD, ERR_NONICKNAMEGIVEN, ERR_ERRONEUSNICKNAME, ERR_NICKNAMEINUSE,
        ERR_USERNOTINCHANNEL, ERR_NOTONCHANNEL, ERR_USERONCHANNEL, ERR_NOTREGISTERED,
        ERR_NEEDMOREPARAMS, ERR_ALREADYREGISTERED, ERR_PASSWDMISMATCH, ERR_YOUREBANNEDCREEP,
        ERR_CHANNELISFULL, ERR_UNKNOWNMODE, ERR_INVITEONLYCHAN, ERR_BANNEDFROMCHAN,
        ERR_BADCHANNELKEY, ERR_BADCHANMASK, ERR_NOCHANMODES, ERR_BANLISTFULL,
        ERR_NOPRIVILEGES, ERR_CHANOPRIVSNEEDED, ERR_NOOPERHOST, ERR_UMODEUNKNOWNFLAG,
        ERR_USERSDONTMATCH
    };
    
    const char* errorMessages[] = {
        CMT_NOSUCHNICK, CMT_NOSUCHCHANNEL, CMT_CANNOTSENDTOCHAN, CMT_TOOMANYCHANNELS,
        CMT_NOORIGIN, CMT_NORECIPIENT, CMT_NOTEXTTOSEND, CMT_NOTOPLEVEL,
        CMT_WILDTOPLEVEL, CMT_BADMASK, CMT_INPUTTOOLONG, CMT_UNKNOWNCOMMAND,
        CMT_NOMOTD, CMT_NONICKNAMEGIVEN, CMT_ERRONEUSNICKNAME, CMT_NICKNAMEINUSE,
        CMT_USERNOTINCHANNEL, CMT_NOTONCHANNEL, CMT_USERONCHANNEL, CMT_NOTREGISTERED,
        CMT_NEEDMOREPARAMS, CMT_ALREADYREGISTERED, CMT_PASSWDMISMATCH, CMT_YOUREBANNEDCREEP,
        CMT_CHANNELISFULL, CMT_UNKNOWNMODE, CMT_INVITEONLYCHAN, CMT_BANNEDFROMCHAN,
        CMT_BADCHANNELKEY, CMT_BADCHANMASK, CMT_NOCHANMODES, CMT_BANLISTFULL,
        CMT_NOPRIVILEGES, CMT_CHANOPRIVSNEEDED, CMT_NOOPERHOST, CMT_UMODEUNKNOWNFLAG,
        CMT_USERSDONTMATCH
    };
    
    const int numErrors = sizeof(errorCodes) / sizeof(errorCodes[0]);
    
    for (int i = 0; i < numErrors; i++) {
        if (errorCodes[i] == error) {
            return errorMessages[i];
        }
    }
    
    return ":Unknown error";
}

static void sendErrorToClient(CommandStruct &cmd, IrcServ &server, t_error errorCode,
		const std::string &target = "") {
    std::string nick = "*"; // Default nickname for unregistered clients
    std::string errorMsg = ":" + std::string(SERVER_NAME) + " " + errorToString(errorCode) + " " + nick;
    
    if (!target.empty()) {
        errorMsg += " " + target;
    }
    
    errorMsg += " " + getErrorMessage(errorCode);
    server.outgoingMessage(cmd.clientFD, errorMsg + "\r\n");
}

static void normalizeInput(std::string &cmd){
  for (int i = 0; cmd[i]; i++)
    cmd[i] = std::toupper(cmd[i]);
}

//note: only check pass being implemented currently, execs are all placeholders
//note: list of clients, server pass, nicklist, channel list, mode list
static int findAndExec(CommandStruct &command, IrcServ &serv){
  const char *cmds[12] = {"PING", "PRIVMSG", "PASS", "NICK", "USER", "JOIN",
    "PART", "QUIT", "KICK", "INVITE", "TOPIC", "MODE"};
  const checks cf = {checkPing, checkPrivmsg, checkPass, checkNick, checkUser,
    checkJoin, checkPart, checkQuit, checkKick, checkInvite, checkTopic, checkMode};
  const execs ef = {execPing, execPrivmsg, execPass, execNick, execUser, execJoin,
    execPart, execQuit, execKick, execInvite, execTopic, execMode};

  normalizeInput(command.command);
  for (int i = 0; i < 12; i++){
    if (command.command == cmds[i]){
      if (cf[i](command, serv) && command.errorCode != 0) {
        std::cout << RED "Command: " << command.command << RST << std::endl;
          ef[i](command, serv);
          return 0;
      } else {
        sendErrorToClient(command, serv, static_cast<t_error>(command.errorCode), command.command);
        return -2;
      }
    }
  }
  return UNKNOWN;
}

//:irc.example.net 001 <target-nick> :Welcome to IRC!

void executeCommands( std::list<CommandStruct>& commands, IrcServ& server )
{
  for ( std::list<CommandStruct>::iterator it = commands.begin(); it != commands.end(); ++it ) {
    int result = findAndExec(*it, server);
    if (result == UNKNOWN){
      it->errorCode = UNKNOWN;
      std::cout << "\e[1;31m >> Command UNKNOWN: " << it->command << " << \e[0m" << std::endl;
      
      sendErrorToClient(*it, server, ERR_UNKNOWNCOMMAND, it->command);
      continue ;
    }
  }
  commands.clear();
}

void printCommands( std::list<CommandStruct>& commands )
{
  int i = 0;

  for ( std::list<CommandStruct>::iterator it = commands.begin(); it != commands.end(); ++it ) {

    CommandStruct& cmd = *it;
    std::cout << "Commands[" << i++ << "] :" << std::endl
      << "  Client FD: " << cmd.clientFD << std::endl
      << "  Prefix: " << cmd.prefix << std::endl
      << "  Command: " << cmd.command << std::endl
      << "  Parameters: ";

    for ( std::vector<std::string>::iterator param = cmd.parameters.begin(); param != cmd.parameters.end(); ++param )
      std::cout << *param << " ";
    std::cout << std::endl;

    std::cout << "  Trailing: " << cmd.trailing << std::endl;
  }
}
