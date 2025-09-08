#include "../includes/macros.hpp"
#include "../includes/parseCommands.hpp"
#include "../includes/executeCommands.hpp"
#include "../includes/IrcServ.hpp"

bool checkPass(CommandStruct &cmd, std::map<unsigned int, std::string> &outgoingMessages){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    outgoingMessages[cmd.clientFD].clear();
    return false;
  }
/*  else if (server.pass != cmd.parameters.front()){
    cmd.errorCode = ERR_PASSWDMISMATCH;
    outgoingMessages[cmd.clientFD].clear();
  }*/
  else if (cmd.parameters.size() != 1){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    outgoingMessages[cmd.clientFD].clear();
    return false;
  }
  return true;
}

bool checkNick(CommandStruct &cmd, std::map<unsigned int, std::string> &outgoingMessages){
  (void)outgoingMessages;
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  return true;
}

bool checkKick(CommandStruct &cmd, std::map<unsigned int, std::string> &outgoingMessages){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)outgoingMessages;
  return true;
}

bool checkInvite(CommandStruct &cmd, std::map<unsigned int, std::string> &outgoingMessages){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)outgoingMessages;
  return true;
}

bool checkTopic(CommandStruct &cmd, std::map<unsigned int, std::string> &outgoingMessages){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)outgoingMessages;
  return true;
}

bool checkPrivmsg(CommandStruct &cmd, std::map<unsigned int, std::string> &outgoingMessages){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)outgoingMessages;
  return true;
}

bool checkMode(CommandStruct &cmd, std::map<unsigned int, std::string> &outgoingMessages){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)outgoingMessages;
  return true;
}
