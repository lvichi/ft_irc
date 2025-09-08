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

void execPass(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execNick(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execKick(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execInvite(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execTopic(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execPrivmsg(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);
void execMode(CommandStruct &cmd, std::map<unsigned int, std::string>& outMsg);

void  execPass(CommandStruct &cmd, std::map<unsigned int, std::string> &outMsg)
{
  (void)outMsg;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execNick(CommandStruct &cmd, std::map<unsigned int, std::string> &outMsg)
{
  (void)outMsg;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execKick(CommandStruct &cmd, std::map<unsigned int, std::string> &outMsg)
{
  (void)outMsg;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execInvite(CommandStruct &cmd, std::map<unsigned int, std::string> &outMsg)
{
  (void)outMsg;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execTopic(CommandStruct &cmd, std::map<unsigned int, std::string> &outMsg)
{
  (void)outMsg;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execPrivmsg(CommandStruct &cmd, std::map<unsigned int, std::string> &outMsg)
{
  (void)outMsg;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}

void  execMode(CommandStruct &cmd, std::map<unsigned int, std::string> &outMsg)
{
  (void)outMsg;
  std::cout << "executed: " RED << cmd.command << RST <<std::endl;
  printCmd(cmd);
}
