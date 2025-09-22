#include "../includes/macros.hpp"
#include "../includes/parseCommands.hpp"
#include "../includes/executeCommands.hpp"
#include "../includes/IrcServ.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"

bool checkPing(CommandStruct &cmd, IrcServ &serv){
  (void)serv;
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NOORIGIN;
    return false;
  }
  return true;
}

bool checkPass(CommandStruct &cmd, IrcServ &serv){
  (void)serv;
  if(cmd.parameters.empty() || cmd.parameters.size() != 1){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  return true;
}

bool checkUser(CommandStruct &cmd, IrcServ &serv){
  (void)serv;
  if(cmd.parameters.empty() || cmd.parameters.size() != 3 
      || cmd.trailing.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  if (cmd.parameters.front().find_first_of(CRLF"@ \0")
      != cmd.parameters.front().npos){
    cmd.errorCode = ERR_ERRONEUSNICKNAME;
    return false;
  }
  std::string usr(cmd.parameters[0]);
  if (isUsernameInUse(usr, serv)){
    cmd.errorCode = ERR_ALREADYREGISTERED;
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

//join need to check for +i and +k;
bool checkJoin(CommandStruct &cmd, IrcServ &serv) {
 Client *client = serv.getClient(cmd.clientFD); 
  if (!client){
    cmd.errorCode = ERR_NOORIGIN;
    return false;
  }
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
  cmd.channelName = channelName;
  Channel *chn = serv.getChannel(channelName);
  if (!chn){
    return true;
  }
  else if (chn->isInviteOnly() && !chn->isInvited(client)){ //+i
    cmd.errorCode = ERR_INVITEONLYCHAN;
    return false;
  }
  else if (chn->hasKey()){ // +k
    if (cmd.parameters.size() < 2){
      cmd.errorCode = ERR_BADCHANNELKEY;
      return false;
    }
    std::vector<std::string>::iterator key(cmd.parameters.begin());
    key++;
    if (chn->getKey() != *key){
      cmd.errorCode = ERR_BADCHANNELKEY;
      return false;
    }
  }
  return true;
}

bool checkPart(CommandStruct &cmd, IrcServ &serv){
  Client* client = serv.getClient(cmd.clientFD);
  if (!client){
    cmd.errorCode = ERR_NOORIGIN;
    return false;
  }
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  std::string channelName = cmd.parameters[0];
  if (channelName.empty() 
		  || std::string(CHANNEL_PREFIX).find_first_of(channelName[0]) == std::string::npos){
    cmd.errorCode = ERR_BADCHANMASK;
    return false;
  }
  Channel* chn = serv.getChannel(channelName);
  if (!chn) {
    cmd.errorCode = ERR_NOSUCHCHANNEL;
    return false;
  }
  cmd.channelName = channelName;
  if (!chn->isMember(client)) {
    cmd.errorCode = ERR_NOTONCHANNEL;
    return false;
  }
  return true;
}

bool checkQuit(CommandStruct &cmd, IrcServ &serv){
  (void)serv;
  (void)cmd;
  return true;
}

bool checkKick(CommandStruct &cmd,  IrcServ&serv){
  Client* clt = serv.getClient(cmd.clientFD);
  if (!clt)
	  return false;
  std::string chan(CHANNEL_PREFIX);
  if(cmd.parameters.empty() || cmd.parameters.size() != 2){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  std::string cName = cmd.parameters[0];
  if (chan.find_first_of(cName[0]) == std::string::npos
      || cmd.parameters.front().find_first_of(',') != std::string::npos){
    cmd.errorCode = ERR_BADCHANMASK;
	return false;
  }
  Channel *chn = serv.getChannel(cName);
  if (!chn){
	  cmd.errorCode = ERR_NOSUCHCHANNEL;
	  return false;
  }
  cmd.channelName = cName;
  if (!chn->isOperator(clt)){
    cmd.errorCode = ERR_CHANOPRIVSNEEDED;
    return false;
  }
  std::vector<std::string>::iterator it = cmd.parameters.begin();
  ++it;
  std::string membs = chn->getMembersList();
  if (membs.find(*it) == std::string::npos){
    cmd.errorCode = ERR_NOSUCHNICK;
    return false;
  }
  return true;
}

//need to implement +i check for it;
bool checkInvite(CommandStruct &cmd, IrcServ &serv) {
  Client *client = serv.getClient(cmd.clientFD);
  if (!client){
    cmd.errorCode = ERR_NOORIGIN;
    return false;
  }
  if(cmd.parameters.empty() || cmd.parameters.size() != 2){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  else if (!cmd.trailing.empty()){
    cmd.errorCode = ERR_INPUTTOOLONG;
    return false;
  }
  std::string user(cmd.parameters[0]);
  Channel *chan = serv.getChannel(cmd.parameters[1]);
  if (!chan){
    cmd.errorCode = ERR_NOSUCHCHANNEL;
    return false;
  }
  cmd.channelName = chan->getName();
  if (!chan->isMember(client)){
    cmd.errorCode = ERR_NOTONCHANNEL;
    return false;
  }
  if (!isNicknameInUse(user, serv)){
    cmd.errorCode = ERR_NOSUCHNICK;
    return false;
  }
  if (chan->isInviteOnly() && !chan->isOperator(client)){
    cmd.errorCode = ERR_CHANOPRIVSNEEDED;
    return false;
  }
  std::cout << "this is ok" << std::endl;
  return true;
}

//need to implement +t to it;
bool checkTopic(CommandStruct &cmd, IrcServ &serv){
  Client *client = serv.getClient(cmd.clientFD);
  if (!client){
    cmd.errorCode = ERR_NOORIGIN;
    return false;
  }
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  Channel *channel = serv.getChannel(cmd.parameters[0]);
  if (!channel){
    cmd.errorCode = ERR_NOSUCHCHANNEL;
    return false;
  }
  cmd.channelName = channel->getName();
  if (!channel->isMember(client)){
    cmd.errorCode = ERR_NOTONCHANNEL;
    return false;
  }
  return true;
}

bool checkNotice(CommandStruct &cmd, IrcServ &serv){
  (void)serv;
  (void)cmd;
  return true;
}

bool checkPrivmsg(CommandStruct &cmd, IrcServ &serv){
  (void)serv;
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NORECIPIENT;
    return false;
  }
  else if (cmd.trailing.empty()){
    cmd.errorCode = ERR_NOTEXTTOSEND;
    return false;
  }
  return true;
}

//MODE: +i, +t, +k, +o, +l;
bool checkMode(CommandStruct &cmd, IrcServ &serv){
  Client *client = serv.getClient(cmd.clientFD);
  if (!client){
    cmd.errorCode = ERR_NOORIGIN;
    return false;
  }
  if(cmd.parameters.empty()){
    cmd.errorCode = ERR_NEEDMOREPARAMS;
    return false;
  }
  std::string cName = cmd.parameters[0];
  if (std::string(CHANNEL_PREFIX).find(cName[0]) == std::string::npos){
    cmd.errorCode = ERR_BADCHANMASK;
    return false;
  }
  Channel *channel = serv.getChannel(cmd.parameters[0]);
  if (!channel){
    cmd.errorCode = ERR_NOSUCHCHANNEL;
    return false;
  }
  cmd.channelName = cName;
  if (!channel->isMember(client)){
    cmd.errorCode = ERR_NOTONCHANNEL;
    return false;
  }
  if (cmd.parameters.size() > 1){
    if (!channel->isOperator(client)){
      cmd.errorCode = ERR_CHANOPRIVSNEEDED;
      return false;
    }
    std::vector<std::string>::iterator modeString = cmd.parameters.begin();
    modeString++;
    if (modeString->find_first_of(MOD_CHANGE) == std::string::npos){
	cmd.errorCode = ERR_UNKNOWNMODE;
	return false;
    }
    for (; modeString != cmd.parameters.end(); modeString++){
      if (modeString->find_first_of(MOD_CHANGE) == std::string::npos)
	continue;
      if (modeString->find_first_of(MOD_CHANGE) != modeString->npos
	  && modeString->find_first_of(ALLOWED_MODES) == modeString->npos){
	cmd.errorCode = ERR_UNKNOWNMODE;
	return false;
      }
    }
  }
  return true;
}
