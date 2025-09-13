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

bool checkUser(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;

  if(cmd.parameters.empty() || cmd.parameters.size() != 3 
      || cmd.trailing.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    serv.outgoingMessage(cmd.clientFD, "");
    return false;
  }

  if (cmd.parameters.front().find_first_of(CRLF"@ \0")
      != cmd.parameters.front().npos)
    return false;
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
  else if (cmd.parameters.front().size() > NICK_MAX ||
      cmd.parameters.front().find_first_not_of(ALPHAS DIGITS HYPHEN SPECIAL)
      != cmd.parameters.front().npos){
    std::cout << "problem is input sanitizer" << std::endl;
    cmd.errorCode = ERR_ERRONEUSNICKNAME;
    return false;
  }
  return true;
}   

//KICK <channel>*(,channel) <user>*(,user) [<comment>]

bool checkJoin(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  
  std::string channelName = cmd.parameters[0];
  if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&')) {
    cmd.errorCode = ERR_NOSUCHCHANNEL;
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
  std::string chan(CHANNEL_PREFIX);
  if(cmd.parameters.empty() || cmd.parameters.size() != 2){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    serv.outgoingMessage(cmd.clientFD, "");
    return false;
  }
  else if (chan.find_first_of(cmd.parameters.front()[0]) == std::string::npos
      || cmd.parameters.front().find_first_of(',') != std::string::npos){
    cmd.errorCode = ERR_BADCHANMASK;
    serv.outgoingMessage(cmd.clientFD, "");
  }
  return true;
}

bool checkInvite(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  if(cmd.parameters.empty() || cmd.parameters.size() != 2){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    serv.outgoingMessage(cmd.clientFD, "");
    return false;
  }

  else if (!cmd.trailing.empty()){
    cmd.errorCode = ERR_INPUTTOOLONG;
    serv.outgoingMessage(cmd.clientFD, "");
  }
  return true;
}

bool checkTopic(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  return true;
}

bool checkPrivmsg(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  if(cmd.parameters.empty() || cmd.trailing.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  return true;
}

bool checkMode(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  (void)serv;
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  return true;
}
