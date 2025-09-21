#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/parseCommands.hpp"
#include <stdexcept>
#include <sstream>
#include <set>
#include <string>
#include <deque>

void    execPing(CommandStruct &cmd, IrcServ &serv){
  Client* client = serv.getClient(cmd.clientFD);
  if (!client)
      return;

  std::string pongMsg = ":" + std::string (SERVER_NAME) + " "
    + "PONG" + " " + cmd.parameters[0];
  client->send(pongMsg, serv);
}

void  execPass(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;

  if (client->isRegistered()) {
    client->sendError(serv, ERR_ALREADYREGISTERED);
    return;
  }

  if (serv.isPasswordValid(cmd.parameters[0])) {
    client->authenticatePassword();
  } else {
    client->sendError(serv, ERR_PASSWDMISMATCH);
  }
}

void  execNick(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  std::string newNick = cmd.parameters[0];
  
  if (isNicknameInUse(newNick, serv)) {
    client->sendError(serv, ERR_NICKNAMEINUSE);
    return;
  }

  client->setNickname(newNick);

  if (client->canRegister()) {
    client->authenticate();
    client->sendWelcome(serv);
  }
}

void  execUser(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  if (client->isRegistered()) {
    client->sendError(serv, ERR_ALREADYREGISTERED);
    return;
  }
  
  client->setUsername(cmd.parameters[0]);
  client->setHostname(cmd.parameters[1]);
  client->setRealname(cmd.trailing);

  if (client->canRegister()) {
    client->authenticate();
    client->sendWelcome(serv);
  }
}

void execKick(CommandStruct &cmd, IrcServ &serv)
{
    Client* client = serv.getClient(cmd.clientFD);
    if (!client) return;
    if (!client->isRegistered()) {
        client->sendError(serv, ERR_NOTREGISTERED);
        return;
    }
    std::string channelName = cmd.parameters[0];
    std::string targetNick = cmd.parameters[1];
    Channel* channel = serv.getChannel(channelName);
    if (!channel) {
        client->sendError(serv, ERR_NOSUCHCHANNEL);
        return;
    }
    if (!channel->isOperator(client)) {
        client->sendError(serv, ERR_CHANOPRIVSNEEDED);
        return;
    }
    Client* target = serv.getClientByNick(targetNick);
    if (!target || !channel->isMember(target)) {
        client->sendError(serv, ERR_USERNOTINCHANNEL);
        return;
    }
    std::string kickMsg = cmd.trailing.empty() ? targetNick : cmd.trailing;
    channel->sendKickMessage(client, target, kickMsg, serv);
    channel->removeMember(target);
    target->removeChannel(channelName);
    if (channel->isEmpty()) serv.removeChannel(channelName);
}

void execInvite(CommandStruct &cmd, IrcServ &serv)
{
    Client* client = serv.getClient(cmd.clientFD);
    if (!client) return;
    if (!client->isRegistered()) {
        client->sendError(serv, ERR_NOTREGISTERED);
        return;
    }
    std::string targetNick = cmd.parameters[1];
    std::string channelName = cmd.parameters[0];
    Channel* channel = serv.getChannel(channelName);
    if (!channel) {
        client->sendError(serv, ERR_NOSUCHCHANNEL);
        return;
    }
    if (!channel->isOperator(client) && channel->isInviteOnly()) {
        client->sendError(serv, ERR_CHANOPRIVSNEEDED);
        return;
    }    
    Client* target = serv.getClientByNick(targetNick);
    if (!target) {
        client->sendError(serv, ERR_NOSUCHNICK);
        return;
    }
    if (channel->isMember(target)) {
        client->sendError(serv, ERR_USERONCHANNEL);
        return;
    }
    target->sendInvite(serv, channelName);
}

void execTopic(CommandStruct &cmd, IrcServ &serv)
{
    Client* client = serv.getClient(cmd.clientFD);
    if (!client) return;
    if (!client->isRegistered()) {
        client->sendError(serv, ERR_NOTREGISTERED);
        return;
    }
    std::string channelName = cmd.parameters[0];
    Channel* channel = serv.getChannel(channelName);
    if (!channel) {
        client->sendError(serv, ERR_NOSUCHCHANNEL);
        return;
    }
    if (cmd.trailing.empty()) {
        channel->sendTopic(client, serv);
        return;
    }
    if (channel->isTopicProtected() && !channel->isOperator(client)) {
        client->sendError(serv, ERR_CHANOPRIVSNEEDED);
        return;
    }
    if (cmd.trailing == "\"\"")
        channel->setTopic("");
    else
        channel->setTopic(cmd.trailing);
    channel->broadcastTopic(client, serv);
}

void execPrivmsg(CommandStruct &cmd, IrcServ &serv)
{
    Client* client = serv.getClient(cmd.clientFD);
    if (!client) return;
    if (!client->isRegistered()) {
        client->sendError(serv, ERR_NOTREGISTERED);
        return;
    }
    std::string target = cmd.parameters[0];
    std::string message = cmd.trailing;
/*    if (target.empty() || message.empty()) {
        client->sendError(serv, ERR_NORECIPIENT);
        return;
    }*/
    if (target[0] == '#') {
        Channel* channel = serv.getChannel(target);
        if (!channel) {
            client->sendError(serv, ERR_NOSUCHCHANNEL);
            return;
        }
        if (!channel->isMember(client)) {
            client->sendError(serv, ERR_CANNOTSENDTOCHAN);
            return;
        }
        channel->broadcastPrivmsg(client, message, serv);
    } else {
        Client* dest = serv.getClientByNick(target);
        if (!dest) {
            client->sendError(serv, ERR_NOSUCHNICK);
            return;
        }
        dest->sendPrivmsg(client, message, serv);
    }
}

void  execJoin(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;

  if (!client->isRegistered()) {
    client->sendError(serv, ERR_NOTREGISTERED);
    return;
  }
  
  std::string channelName = cmd.parameters[0];
  std::string key = (cmd.parameters.size() > 1) ? cmd.parameters[1] : "";

  Channel* channel = serv.getChannel(channelName);
  if (!channel) {
    channel = serv.createChannel(channelName);
  }

  if (!channel->canJoin(client, key)) {
    client->sendError(serv, ERR_CHANNELISFULL);
    return;
  }

  channel->addMember(client);
  channel->sendJoinMessages(client, serv);
}

void  execPart(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  if (!client->isRegistered()) {
    client->sendError(serv, ERR_NOTREGISTERED);
    return;
  }
  
  std::string channelName = cmd.parameters[0];
  std::string partMessage = cmd.trailing.empty() ? "Leaving" : cmd.trailing;
  
  Channel* channel = serv.getChannel(channelName);
  channel->sendPartMessages(client, partMessage, serv);
  channel->removeMember(client);
  client->removeChannel(channelName);  // Remover o canal da lista do cliente explicitamente

  if (channel->isEmpty()) {
    serv.removeChannel(channelName);
  }
}

void execQuit(CommandStruct &cmd, IrcServ &serv)
{
    Client* client = serv.getClient(cmd.clientFD);
    if (!client) return;
    std::string quitMessage = cmd.trailing.empty() ? "Client Quit" : cmd.trailing;
    client->sendQuit(serv, quitMessage);
    std::cout << "Client " << (client->getNickname().empty() ? "unknown" : client->getNickname()) << " quit: " << quitMessage << std::endl;
}

//static ssize_t getModeChange(std::string &modeString){
//}

void execMode(CommandStruct &cmd, IrcServ &serv)
{
    char modifier = 0;
    unsigned short operations = 0;
    Client* client = serv.getClient(cmd.clientFD);
    if (!client) return;
    if (!client->isRegistered()) {
        client->sendError(serv, ERR_NOTREGISTERED);
        return;
    }
    std::string target = cmd.parameters[0];
    if (target.empty()) {
        client->sendError(serv, ERR_NEEDMOREPARAMS);
        return;
    }
    Channel* channel = serv.getChannel(target);
    if (!channel) {
        client->sendError(serv, ERR_NOSUCHCHANNEL);
        return;
    }
    if (cmd.parameters.size() == 1){
        channel->sendMode(client, serv);
        return;
    }
    std::vector<std::string>::iterator vIterStr = cmd.parameters.begin();
    vIterStr++;
    std::deque<std::string> mode;
    std::deque<std::string> param;
    for(; vIterStr != cmd.parameters.end(); ++vIterStr){
        if (vIterStr->find_first_of(MOD_CHANGE) == std::string::npos){
            param.push_back(*vIterStr);
            continue;
        }
        for (unsigned int i = 0; i < vIterStr->length(); i++){
            char mod = (*vIterStr)[i];
            if (mod == '+' || mod == '-')
                modifier = mod;
            else if (modifier == '-' || modifier == '+'){
                operations++;
                if (operations > MAX_OPERATIONS){
                    client->sendError(serv, ERR_INPUTTOOLONG);
                    return;
                }
                mode.push_back(std::string(1, modifier) + mod);
            }
        } 
    }
    channel->handleMode(cmd, serv, mode, param);
}
