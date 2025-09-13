#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/parseCommands.hpp"
#include <stdexcept>
#include <sstream>
#include <set>

static bool isNicknameInUse(const std::string& nick, IrcServ& serv) {
    const std::map<int, Client*>& clients = serv.getClients();
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getNickname() == nick)
            return true;
    }
    return false;
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
    client->sendError(serv, ERR_NICKNAMEINUSE, newNick + " " + CMT_NICKNAMEINUSE);
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

void  execKick(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  if (!client->isRegistered()) {
    client->sendError(serv, ERR_NOTREGISTERED, ":You have not registered");
    return;
  }
  
  // Comando KICK básico - implementação simplificada
  std::cout << "KICK command executed (simplified)" << std::endl;
}

void  execInvite(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;

  if (!client->isRegistered()) {
    client->sendError(serv, ERR_NOTREGISTERED, ":You have not registered");
    return;
  }
  
  // Comando INVITE básico - implementação simplificada
  std::cout << "INVITE command executed (simplified)" << std::endl;
}

void  execTopic(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;

  if (!client->isRegistered()) {
    client->sendError(serv, ERR_NOTREGISTERED, ":You have not registered");
    return;
  }
  
  // Comando TOPIC básico - implementação simplificada  
  std::cout << "TOPIC command executed (simplified)" << std::endl;
}

void  execPrivmsg(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;

  if (!client->isRegistered()) {
    client->sendError(serv, ERR_NOTREGISTERED, ":You have not registered");
    return;
  }
  
  // Comando PRIVMSG básico - implementação simplificada
  std::cout << "PRIVMSG command executed (simplified)" << std::endl;
}

void  execJoin(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;

  if (!client->isRegistered()) {
    client->sendError(serv, ERR_NOTREGISTERED, ":You have not registered");
    return;
  }
  
  std::string channelName = cmd.parameters[0];
  std::string key = (cmd.parameters.size() > 1) ? cmd.parameters[1] : "";

  Channel* channel = serv.getChannel(channelName);
  if (!channel) {
    channel = serv.createChannel(channelName);
  }

  if (!channel->canJoin(client, key)) {
    client->sendError(serv, ERR_CHANNELISFULL, channelName + " :Cannot join channel");
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
    client->sendError(serv, ERR_NOTREGISTERED, ":You have not registered");
    return;
  }
  
  std::string channelName = cmd.parameters[0];
  std::string partMessage = cmd.trailing.empty() ? "Leaving" : cmd.trailing;
  
  Channel* channel = serv.getChannel(channelName);
  if (!channel) {
    client->sendError(serv, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
    return;
  }

  if (!channel->isMember(client)) {
    client->sendError(serv, ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
    return;
  }

  channel->sendPartMessages(client, partMessage, serv);
  channel->removeMember(client);

  if (channel->isEmpty()) {
    serv.removeChannel(channelName);
  }
}

void  execQuit(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  std::string quitMessage = cmd.trailing.empty() ? "Client Quit" : cmd.trailing;
  
  // Remove client from all channels and notify members
  // const std::set<std::string>& channels = client->getChannels();
  // std::string quitReply = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " QUIT :" + quitMessage + "\r\n";
  // for (std::set<std::string>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
  //   Channel* channel = serv.getChannel(*it);
  //   if (channel) {
  //     // Notify all other members in the channel
  //     const std::set<int>& members = channel->getMembers();
  //     for (std::set<int>::const_iterator mit = members.begin(); mit != members.end(); ++mit) {
  //       if (*mit != cmd.clientFD) {
  //         serv.outgoingMessage(*mit, quitReply);
  //       }
  //     }
  //     // Remove client from channel
  //     channel->removeMember(cmd.clientFD);
  //     // If channel is empty, remove it from server
  //     if (channel->isEmpty()) {
  //       serv.removeChannel(*it);
  //     }
  //   }
  // }
  // Optionally, log the quit event
  std::cout << "Client " << (client->getNickname().empty() ? "unknown" : client->getNickname()) << " quit: " << quitMessage << std::endl;
}

void  execMode(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  if (!client->isRegistered()) {
    client->sendError(serv, ERR_NOTREGISTERED, ":You have not registered");
    return;
  }
  
  // Comando MODE básico - implementação simplificada
  std::cout << "MODE command executed (simplified)" << std::endl;
}
