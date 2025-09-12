#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include <stdexcept>

static void normalizeInput(std::string &cmd){
  for (int i = 0; cmd[i]; i++)
    cmd[i] = std::toupper(cmd[i]);
}

//note: only check pass being implemented currently, execs are all placeholders
//note: list of clients, server pass, nicklist, channel list, mode list
static int findAndExec(CommandStruct &command, IrcServ &serv){
  const char *cmds[8] = {"PASS", "NICK", "KICK", "INVITE", "TOPIC", "PRIVMSG", "MODE", NULL};
  const checks cf = {checkPass, checkNick, checkKick, checkInvite, checkTopic, checkPrivmsg, checkMode, NULL};
  const execs ef = {execPass, execNick, execKick, execInvite, execTopic, execPrivmsg, execMode, NULL};

  normalizeInput(command.command);
  for (int i = 0; cmds[i] != NULL; i++){
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

void executeCommands( std::list<CommandStruct>& commands, IrcServ& server )
{
  for ( std::list<CommandStruct>::iterator it = commands.begin(); it != commands.end(); ++it ) {
    if (findAndExec(*it, server) == UNKNOWN){
      it->errorCode = ERR_UNKNOWNCOMMAND;
      std::cout << "\e[1;31m >> Is UNKNOWN << \e[0m" << std::endl;
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
