#include "../includes/macros.hpp"
#include "../includes/parseCommands.hpp"
#include "../includes/executeCommands.hpp"
#include "../includes/IrcServ.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"

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
    cmd.errorCode = ERR_ERRONEUSNICKNAME;
    return false;
  }
  else if (cmd.parameters.front().size() > NICK_MAX ||
      cmd.parameters.front().find_first_not_of(ALPHAS DIGITS HYPHEN SPECIAL)
      != cmd.parameters.front().npos){
    cmd.errorCode = ERR_ERRONEUSNICKNAME;
    return false;
  }
  return true;
}   

//KICK <channel>*(,channel) <user>*(,user) [<comment>]

//join need to check for +i and +o;
bool checkJoin(CommandStruct &cmd, IrcServ &serv) {
  (void)serv;
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;

  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  
  std::string channelName = cmd.parameters[0];
  if (channelName.empty() 
		  || std::string(CHANNEL_PREFIX).find_first_of(channelName[0]) == std::string::npos){
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
  Client* clt = serv.getClient(cmd.clientFD);
  if (!clt)
	  return false;
  std::string chan(CHANNEL_PREFIX);
  if(cmd.parameters.empty() || cmd.parameters.size() != 2){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  else if (chan.find_first_of(cmd.parameters.front()[0]) == std::string::npos
      || cmd.parameters.front().find_first_of(',') != std::string::npos){
    cmd.errorCode = ERR_BADCHANMASK;
	return false;
  }
  Channel *chn = serv.getChannel(std::string (chan, chan[1], chan.npos));
  if (!chn){
	  cmd.errorCode = ERR_NOSUCHCHANNEL;
	  return false;
  }
  else if (!chn->isOperator(clt)){
	  cmd.errorCode = ERR_CHANOPRIVSNEEDED;
	  return false;
  }
  std::vector<std::string>::iterator it = cmd.parameters.begin();
  ++it;
  std::string usr = *it;
  (void)usr;
  //std::list<std::string> membs = chn->getMembersList();
  //todo check for user being in previously checked channel;
  return true;
}

//need to implement +i check for it;
bool checkInvite(CommandStruct &cmd, IrcServ &serv){
  std::cout << "checked : " GRN << cmd.command << RST << std::endl;
  if(cmd.parameters.empty() || cmd.parameters.size() != 2){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }

  else if (!cmd.trailing.empty()){
    cmd.errorCode = ERR_INPUTTOOLONG;
    serv.outgoingMessage(cmd.clientFD, "");
  }
  return true;
}

//need to implement +t to it;
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
