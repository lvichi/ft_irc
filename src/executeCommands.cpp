#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include <stdexcept>

static void normalizeInput(std::string &cmd){
  for (int i = 0; cmd[i]; i++)
    cmd[i] = std::toupper(cmd[i]);
}

//note: only check pass being implemented currently, execs are all placeholders
//note: list of clients, server pass, nicklist, channel list, mode list
static int findAndExec(CommandStruct &command, std::map<unsigned int, std::string> &msg){
  const char *cmds[8] = {"PASS", "NICK", "KICK", "INVITE", "TOPIC", "PRIVMSG", "MODE", NULL};
  const checks cf = {checkPass, checkNick, checkKick, checkInvite, checkTopic, checkPrivmsg, checkMode, NULL};
  const execs ef = {execPass, execNick, execKick, execInvite, execTopic, execPrivmsg, execMode, NULL};

  normalizeInput(command.command);
  for (int i = 0; cmds[i] != NULL; i++){
    if (command.command == cmds[i]){
      if (cf[i](command, msg))
        try{
          ef[i](command, msg);
          return 0;
        }catch(std::exception &e){
          std::cout << e.what() << std::endl;
          return -2;
        }
    }
  }
  return UNKNOWN;
}

void executeCommands( std::list<CommandStruct>& commands, std::map<unsigned int, std::string>& outgoingMessages )
{
  for ( std::list<CommandStruct>::iterator it = commands.begin(); it != commands.end(); ++it ) {
    std::stringstream   ss;
    if (findAndExec(*it, outgoingMessages) == UNKNOWN){
      it->errorCode = ERR_UNKNOWNCOMMAND;
      std::cout << "\e[1;31m >> Is UNKNOWN << \e[0m" << std::endl;
      ss << "Command sent is unknown by this server" << CRLF;
      outgoingMessages[ it->clientFD ] += ss.str();
      continue ;
    }
    ss << "Responding message to client FD: " << it->clientFD
      << " Message: " << it->trailing
      << "\r\n";
    outgoingMessages[ it->clientFD ] += ss.str();
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
