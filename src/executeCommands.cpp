#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include "../includes/Client.hpp"
#include <stdexcept>
#include <sstream>

static void normalizeInput(std::string &cmd){
  for (int i = 0; cmd[i]; i++)
    cmd[i] = std::toupper(cmd[i]);
}

//note: only check pass being implemented currently, execs are all placeholders
//note: list of clients, server pass, nicklist, channel list, mode list
static int findAndExec(CommandStruct &command, IrcServ &serv){
  const char *cmds[13] = {"PING", "PRIVMSG", "PASS", "NICK", "USER", "JOIN",
    "PART", "QUIT", "KICK", "INVITE", "TOPIC", "MODE", "NOTICE"};
  const checks cf = {checkPing, checkPrivmsg, checkPass, checkNick, checkUser,
    checkJoin, checkPart, checkQuit, checkKick, checkInvite, checkTopic, checkMode, checkNotice};
  const execs ef = {execPing, execPrivmsg, execPass, execNick, execUser, execJoin,
    execPart, execQuit, execKick, execInvite, execTopic, execMode, execNotice};

  normalizeInput(command.command);
  for (int i = 0; i < 13; i++){
    if (command.command == cmds[i]){
      if (cf[i](command, serv) && command.errorCode == 0) {
        std::cout << RED "Command: " << command.command << RST << std::endl;
          ef[i](command, serv);
          return 0;
      } else {
        serv.getClient(command.clientFD)->sendError(serv, (t_error)command.errorCode, command);
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

      server.getClient(it->clientFD)->sendError(server, ERR_UNKNOWNCOMMAND, *it);
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
