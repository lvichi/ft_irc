#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include "../includes/replies.hpp"
#include <stdexcept>

static void normalizeInput(std::string &cmd){
  for (int i = 0; cmd[i]; i++)
    cmd[i] = std::toupper(cmd[i]);
}

//note: only check pass being implemented currently, execs are all placeholders
//note: list of clients, server pass, nicklist, channel list, mode list
static int findAndExec(CommandStruct &command, IrcServ &serv){
  const char *cmds[11] = {"PASS", "NICK", "USER", "JOIN", "PART", "QUIT", "KICK", "INVITE", "TOPIC", "PRIVMSG", "MODE"};
  const checks cf = {checkPass, checkNick, checkUser, checkJoin, checkPart, checkQuit, checkKick, checkInvite, checkTopic, checkPrivmsg, checkMode};
  const execs ef = {execPass, execNick, execUser, execJoin, execPart, execQuit, execKick, execInvite, execTopic, execPrivmsg, execMode};

  normalizeInput(command.command);
  for (int i = 0; i < 11; i++){
    if (command.command == cmds[i]){
      if (cf[i](command, serv))
        try{
          ef[i](command, serv);
          return 0;
        }catch(std::exception &e){
          std::cout << e.what() << std::endl;
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
    if (findAndExec(*it, server) == UNKNOWN){
      it->errorCode = UNKNOWN;
      std::cout << "\e[1;31m >> Command UNKNOWN: " << it->command << " << \e[0m" << std::endl;
      
      std::string errorMsg = ":" + std::string(SERVER_NAME) + " " + ERR_UNKNOWNCOMMAND + " * " + it->command + " :Unknown command";
      server.outgoingMessage(it->clientFD, errorMsg + "\r\n");
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
