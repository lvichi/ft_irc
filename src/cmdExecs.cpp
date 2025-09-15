#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/parseCommands.hpp"
#include <stdexcept>
#include <sstream>
#include <set>

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
   /* if (!channel->isOperator(client)) {
        client->sendError(serv, ERR_CHANOPRIVSNEEDED);
        return;
    }*/
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

  if (channel->isEmpty()) {
    serv.removeChannel(channelName);
  }
}

void execQuit(CommandStruct &cmd, IrcServ &serv)
{
    Client* client = serv.getClient(cmd.clientFD);
    if (!client) return;
    std::string quitMessage = cmd.trailing.empty() ? "Client Quit" : cmd.trailing;
    const std::set<std::string>& channels = client->getChannels();
    std::string quitReply = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " QUIT :" + quitMessage + "\r\n";
    for (std::set<std::string>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel* channel = serv.getChannel(*it);
        if (channel) {
            const std::set<Client*>& members = channel->getMembers();
            for (std::set<Client*>::const_iterator mit = members.begin(); mit != members.end(); ++mit) {
                if (*mit != client) {
                    serv.outgoingMessage((*mit)->getFd(), quitReply);
                }
            }
            channel->removeMember(client);
            if (channel->isEmpty()) serv.removeChannel(*it);
        }
    }
    std::cout << "Client " << (client->getNickname().empty() ? "unknown" : client->getNickname()) << " quit: " << quitMessage << std::endl;
    serv.removeClient(cmd.clientFD);
}

void execMode(CommandStruct &cmd, IrcServ &serv)
{
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
    if (target[0] == '#') {
        Channel* channel = serv.getChannel(target);
        if (!channel) {
            client->sendError(serv, ERR_NOSUCHCHANNEL);
            return;
        }
        channel->handleMode(client, cmd, serv);
    } else {
        client->handleUserMode(cmd, serv);
    }
}
