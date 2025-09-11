#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include <stdexcept>

static void printCmd(CommandStruct &cmd){
    std::cout << "  Client FD: " << cmd.clientFD << std::endl
      << "  Prefix: " << cmd.prefix << std::endl
      << "  Command: " << cmd.command << std::endl
      << "  Parameters: ";

    for ( std::vector<std::string>::iterator param = cmd.parameters.begin(); param != cmd.parameters.end(); ++param )
      std::cout << *param << " ";
    std::cout << std::endl;

    std::cout << "  Trailing: " << cmd.trailing << std::endl;
}

void execPass(CommandStruct &cmd, IrcServ &serv);
void execNick(CommandStruct &cmd, IrcServ &serv);
void execKick(CommandStruct &cmd, IrcServ &serv);
void execInvite(CommandStruct &cmd, IrcServ &serv);
void execTopic(CommandStruct &cmd, IrcServ &serv);
void execPrivmsg(CommandStruct &cmd, IrcServ &serv);
void execMode(CommandStruct &cmd, IrcServ &serv);

void  execPass(CommandStruct &cmd, IrcServ &serv)
{
  (void)serv;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execUser(CommandStruct &cmd, IrcServ &serv)
{
  (void)serv;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execNick(CommandStruct &cmd, IrcServ &serv)
{
  (void)serv;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execKick(CommandStruct &cmd, IrcServ &serv)
{
  (void)serv;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execInvite(CommandStruct &cmd, IrcServ &serv)
{
  (void)serv;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execTopic(CommandStruct &cmd, IrcServ &serv)
{
  (void)serv;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execPrivmsg(CommandStruct &cmd, IrcServ &serv)
{
  (void)serv;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execMode(CommandStruct &cmd, IrcServ &serv)
{
  (void)serv;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}
