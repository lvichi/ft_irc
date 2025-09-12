#include "../includes/macros.hpp"
#include "../includes/parseCommands.hpp"
#include "../includes/executeCommands.hpp"
#include "../includes/IrcServ.hpp"

bool checkPass(CommandStruct &cmd, IrcServ &serv){
  (void)serv;
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  if(cmd.parameters.empty() || cmd.parameters.size() != 1){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  return true;
}

/*nickname = ( letter / special ) * ( letter / digit / special / "-" )
  letter   = A-Z / a-z
  digit    = 0-9
  special  = "[" | "]" | "\" | "`" | "^" | "{" | "}"
  */

bool checkNick(CommandStruct &cmd, IrcServ &serv){
  (void)serv;
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NONICKNAMEGIVEN;
    return false;
  }
  else if (cmd.parameters.size() != 1
      || !(std::isalpha(cmd.parameters.front()[0])
        || isSpecial(cmd.parameters.front()[0]))){
    std::cout << "problem is isalpha or isspecial" << std::endl;
    cmd.errorCode = ERR_ERRONEUSNICKNAME;
    return false;
  }
  else if (cmd.parameters.front().size() > 9 ||
      cmd.parameters.front().find_first_not_of(ALPHAS DIGITS HYPHEN SPECIAL)
      != cmd.parameters.front().npos){
    std::cout << "problem is input sanitizer" << std::endl;
    cmd.errorCode = ERR_ERRONEUSNICKNAME;
    return false;
  }
  return true;
}   

bool checkUser(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  if(cmd.parameters.size() < 3 || cmd.trailing.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  return true;
}

bool checkJoin(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  return true;
}

bool checkPart(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  return true;
}

bool checkQuit(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  (void)cmd;
  return true;
}

bool checkKick(CommandStruct &cmd,  IrcServ&serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  return true;
}

bool checkInvite(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  return true;
}

bool checkTopic(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  return true;
}

bool checkPrivmsg(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  return true;
}

bool checkMode(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  return true;
}
