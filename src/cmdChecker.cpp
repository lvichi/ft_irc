#include "../includes/macros.hpp"
#include "../includes/parseCommands.hpp"
#include "../includes/executeCommands.hpp"
#include "../includes/IrcServ.hpp"

bool checkPass(CommandStruct &cmd, std::map<unsigned int, std::string> &outgoingMessages){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  if(cmd.parameters.empty() || cmd.parameters.size() != 1){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    outgoingMessages[cmd.clientFD].clear();
    return false;
  }
  return true;
}

/*nickname = ( letter / special ) * ( letter / digit / special / "-" )
  letter   = A-Z / a-z
  digit    = 0-9
  special  = "[" | "]" | "\" | "`" | "^" | "{" | "}"
  */

bool checkNick(CommandStruct &cmd, std::map<unsigned int, std::string> &outgoingMessages){
  (void)outgoingMessages;
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
