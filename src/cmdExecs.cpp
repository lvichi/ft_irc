#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include "../includes/replies.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <stdexcept>
#include <sstream>

static std::string makeReply(const std::string& code, const std::string& nick, const std::string& message) {
    return ":" + std::string(SERVER_NAME) + " " + code + " " + nick + " " + message;
}

static bool isValidNickname(const std::string& nick) {
    if (nick.empty() || nick.length() > 9)
        return false;

    // Primeiro caractere deve ser letra ou caractere especial
    if (!std::isalpha(nick[0]) && nick[0] != '[' && nick[0] != ']' &&
        nick[0] != '{' && nick[0] != '}' && nick[0] != '\\' && nick[0] != '|')
        return false;

    // O restante pode ser alfanumérico ou caracteres especiais
    for (size_t i = 1; i < nick.length(); ++i) {
        if (!std::isalnum(nick[i]) && nick[i] != '-' && nick[i] != '[' && 
            nick[i] != ']' && nick[i] != '{' && nick[i] != '}' && 
            nick[i] != '\\' && nick[i] != '|')
            return false;
    }
    return true;
}

static bool isNicknameInUse(const std::string& nick, IrcServ& serv) {
    const std::map<int, Client*>& clients = serv.getClients();
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getNickname() == nick)
            return true;
    }
    return false;
}

void execPass(CommandStruct &cmd, IrcServ &serv);
void execNick(CommandStruct &cmd, IrcServ &serv);
void execUser(CommandStruct &cmd, IrcServ &serv);
void execJoin(CommandStruct &cmd, IrcServ &serv);
void execPart(CommandStruct &cmd, IrcServ &serv);
void execQuit(CommandStruct &cmd, IrcServ &serv);
void execKick(CommandStruct &cmd, IrcServ &serv);
void execInvite(CommandStruct &cmd, IrcServ &serv);
void execTopic(CommandStruct &cmd, IrcServ &serv);
void execPrivmsg(CommandStruct &cmd, IrcServ &serv);
void execMode(CommandStruct &cmd, IrcServ &serv);

void  execPass(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;

  // Verifica se já foi registrado
  if (client->isRegistered()) {
    std::string reply = makeReply(ERR_ALREADYREGISTRED, client->getNickname(), ":You may not reregister");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  // Verifica se a senha existe
  if (cmd.parameters.empty()) {
    std::string reply = makeReply(ERR_NEEDMOREPARAMS, "*", "PASS :Not enough parameters");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica a senha
  if (serv.isPasswordValid(cmd.parameters[0])) {
    client->setPasswordAuthenticated(true);
  } else {
    std::string reply = makeReply(ERR_PASSWDMISMATCH, "*", ":Password incorrect");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
  }
}

void  execNick(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  // Verifica se o nick existe
  if (cmd.parameters.empty()) {
    std::string reply = makeReply(ERR_NONICKNAMEGIVEN, "*", ":No nickname given");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  std::string newNick = cmd.parameters[0];
  
  // Verifica se o nick é válido
  if (!isValidNickname(newNick)) {
    std::string reply = makeReply(ERR_ERRONEUSNICKNAME, "*", newNick + " :Erroneous nickname");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  // Verifica se o nick já está em uso
  if (isNicknameInUse(newNick, serv)) {
    std::string reply = makeReply(ERR_NICKNAMEINUSE, "*", newNick + " :Nickname is already in use");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Define o novo nick
  client->setNickname(newNick);

  // Verifica se o cliente está agora totalmente registrado (tem senha, nick e usuário)
  if (client->isPasswordAuthenticated() && !client->getNickname().empty() &&
      !client->getUsername().empty() && !client->isRegistered()) {
    client->authenticate();
    
    // Envia mensagens de boas-vindas
    std::string welcome = makeReply(RPL_WELCOME, newNick, ":Welcome to the Internet Relay Network " + newNick);
    std::string yourhost = makeReply(RPL_YOURHOST, newNick, ":Your host is " + std::string(SERVER_NAME) + ", running version " + std::string(SERVER_VERSION));
    
    serv.outgoingMessage(cmd.clientFD, welcome + "\r\n");
    serv.outgoingMessage(cmd.clientFD, yourhost + "\r\n");
  }
}

void  execUser(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  // Verifica se já foi registrado
  if (client->isRegistered()) {
    std::string reply = makeReply(ERR_ALREADYREGISTRED, client->getNickname(), ":You may not reregister");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica se há parâmetros suficientes (USER <username> <hostname> <servername> <realname>)
  if (cmd.parameters.size() < 3 || cmd.trailing.empty()) {
    std::string reply = makeReply(ERR_NEEDMOREPARAMS, "*", "USER :Not enough parameters");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  // Define os detalhes do usuário
  client->setUsername(cmd.parameters[0]);
  client->setHostname(cmd.parameters[1]);
  client->setRealname(cmd.trailing);

  // Verifica se o cliente está agora totalmente registrado (tem senha, nick e usuário)
  if (client->isPasswordAuthenticated() && !client->getNickname().empty() && 
      !client->getUsername().empty() && !client->isRegistered()) {
    client->authenticate();

    // Envia mensagens de boas-vindas
    std::string nick = client->getNickname();
    std::string welcome = makeReply(RPL_WELCOME, nick, ":Welcome to the Internet Relay Network " + nick);
    std::string yourhost = makeReply(RPL_YOURHOST, nick, ":Your host is " + std::string(SERVER_NAME) + ", running version " + std::string(SERVER_VERSION));
    
    serv.outgoingMessage(cmd.clientFD, welcome + "\r\n");
    serv.outgoingMessage(cmd.clientFD, yourhost + "\r\n");
  }
}

void  execKick(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  // Verifica se o cliente está registrado
  if (!client->isRegistered()) {
    std::string reply = makeReply(ERR_NOTREGISTERED, "*", ":You have not registered");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  // Verifica se há parâmetros suficientes (KICK <channel> <user> [<reason>])
  if (cmd.parameters.size() < 2) {
    std::string reply = makeReply(ERR_NEEDMOREPARAMS, client->getNickname(), "KICK :Not enough parameters");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  std::string channelName = cmd.parameters[0];
  std::string targetNick = cmd.parameters[1];
  std::string kickReason = cmd.trailing.empty() ? targetNick : cmd.trailing;
  
  // Pega o canal
  Channel* channel = serv.getChannel(channelName);
  if (!channel) {
    std::string reply = makeReply(ERR_NOSUCHCHANNEL, client->getNickname(), channelName + " :No such channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica se o cliente é operador do canal
  if (!channel->isOperator(client)) {
    std::string reply = makeReply(ERR_CHANOPRIVSNEEDED, client->getNickname(), channelName + " :You're not channel operator");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Find the target client
  Client* targetClient = NULL;
  const std::map<int, Client*>& clients = serv.getClients();
  for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
    if (it->second->getNickname() == targetNick) {
      targetClient = it->second;
      break;
    }
  }
  
  if (!targetClient) {
    std::string reply = makeReply(ERR_NOSUCHNICK, client->getNickname(), targetNick + " :No such nick/channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica se o cliente está no canal
  if (!channel->isMember(targetClient)) {
    std::string reply = makeReply(ERR_USERNOTINCHANNEL, client->getNickname(), targetNick + " " + channelName + " :They aren't on that channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Envia a mensagem KICK para todos os membros do canal
  std::string kickMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " KICK " + channelName + " " + targetNick + " :" + kickReason;
  const std::set<Client*>& members = channel->getMembers();
  for (std::set<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
    serv.outgoingMessage((*it)->getFd(), kickMsg + "\r\n");
  }

  // Remove o cliente do canal
  channel->removeMember(targetClient);
}

void  execInvite(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;

  // Verifica se o cliente está registrado
  if (!client->isRegistered()) {
    std::string reply = makeReply(ERR_NOTREGISTERED, "*", ":You have not registered");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica se há parâmetros suficientes (INVITE <nickname> <channel>)
  if (cmd.parameters.size() < 2) {
    std::string reply = makeReply(ERR_NEEDMOREPARAMS, client->getNickname(), "INVITE :Not enough parameters");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  std::string targetNick = cmd.parameters[0];
  std::string channelName = cmd.parameters[1];

  // Encontra o cliente alvo
  Client* targetClient = NULL;
  const std::map<int, Client*>& clients = serv.getClients();
  for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
    if (it->second->getNickname() == targetNick) {
      targetClient = it->second;
      break;
    }
  }
  
  if (!targetClient) {
    std::string reply = makeReply(ERR_NOSUCHNICK, client->getNickname(), targetNick + " :No such nick/channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Pega o canal
  Channel* channel = serv.getChannel(channelName);
  if (!channel) {
    std::string reply = makeReply(ERR_NOSUCHCHANNEL, client->getNickname(), channelName + " :No such channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica se o convidador está no canal
  if (!channel->isMember(client)) {
    std::string reply = makeReply(ERR_NOTONCHANNEL, client->getNickname(), channelName + " :You're not on that channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica se o cliente alvo já está no canal
  if (channel->isMember(targetClient)) {
    std::string reply = makeReply(ERR_USERONCHANNEL, client->getNickname(), targetNick + " " + channelName + " :is already on channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica se o modo de convite requer privilégios de operador
  if (channel->isInviteOnly() && !channel->isOperator(client)) {
    std::string reply = makeReply(ERR_CHANOPRIVSNEEDED, client->getNickname(), channelName + " :You're not channel operator");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Adiciona o cliente alvo à lista de convidados
  channel->addInvited(targetClient);

  // Envia a mensagem de convite para o cliente alvo
  std::string inviteMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " INVITE " + targetNick + " " + channelName;
  serv.outgoingMessage(targetClient->getFd(), inviteMsg + "\r\n");

  // Confirma para quem convidou (não está no RFC)
  std::cout << "User " << client->getNickname() << " invited " << targetNick << " to " << channelName << std::endl;
}

void  execTopic(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;

  // Verifica se o cliente está registrado
  if (!client->isRegistered()) {
    std::string reply = makeReply(ERR_NOTREGISTERED, "*", ":You have not registered");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica se há parâmetros suficientes (TOPIC <channel> [<topic>])
  if (cmd.parameters.empty()) {
    std::string reply = makeReply(ERR_NEEDMOREPARAMS, client->getNickname(), "TOPIC :Not enough parameters");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  std::string channelName = cmd.parameters[0];
  
  // Pega o canal
  Channel* channel = serv.getChannel(channelName);
  if (!channel) {
    std::string reply = makeReply(ERR_NOSUCHCHANNEL, client->getNickname(), channelName + " :No such channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica se o cliente está no canal
  if (!channel->isMember(client)) {
    std::string reply = makeReply(ERR_NOTONCHANNEL, client->getNickname(), channelName + " :You're not on that channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Mostra o tópico atual
  if (cmd.trailing.empty()) {
    if (channel->getTopic().empty()) {
      std::string reply = makeReply(RPL_NOTOPIC, client->getNickname(), channelName + " :No topic is set");
      serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    } else {
      std::string reply = makeReply(RPL_TOPIC, client->getNickname(), channelName + " :" + channel->getTopic());
      serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    }
  } else {
    // Configurando tópico - verifica se a proteção de tópico está habilitada
    if (channel->isTopicProtected() && !channel->isOperator(client)) {
      std::string reply = makeReply(ERR_CHANOPRIVSNEEDED, client->getNickname(), channelName + " :You're not channel operator");
      serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
      return;
    }
    
    // Define o novo tópico
    channel->setTopic(cmd.trailing);

    // Notifica todos os membros do canal
    std::string topicMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " TOPIC " + channelName + " :" + cmd.trailing;
    const std::set<Client*>& members = channel->getMembers();
    for (std::set<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
      serv.outgoingMessage((*it)->getFd(), topicMsg + "\r\n");
    }
  }
}

void  execPrivmsg(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;

  // Verifica se o cliente está registrado
  if (!client->isRegistered()) {
    std::string reply = makeReply(ERR_NOTREGISTERED, "*", ":You have not registered");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  // Verifica se há parâmetros suficientes (PRIVMSG <target> :<message>)
  if (cmd.parameters.empty() || cmd.trailing.empty()) {
    std::string reply = makeReply(ERR_NEEDMOREPARAMS, client->getNickname(), "PRIVMSG :Not enough parameters");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  std::string target = cmd.parameters[0];
  std::string message = cmd.trailing;
  
  // Verifica se é um canal ou um usuário
  if (target[0] == '#' || target[0] == '&') {
    Channel* channel = serv.getChannel(target);
    if (!channel) {
      std::string reply = makeReply(ERR_NOSUCHCHANNEL, client->getNickname(), target + " :No such channel");
      serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
      return;
    }

    // Verifica se o cliente está no canal
    if (!channel->isMember(client)) {
      std::string reply = makeReply(ERR_CANNOTSENDTOCHAN, client->getNickname(), target + " :Cannot send to channel");
      serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
      return;
    }
    
    // Envia a mensagem para todos os membros do canal
    std::string broadcastMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " PRIVMSG " + target + " :" + message;
    const std::set<Client*>& members = channel->getMembers();
    for (std::set<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
      if (*it != client) { // Don't send to sender
        serv.outgoingMessage((*it)->getFd(), broadcastMsg + "\r\n");
      }
    }
  }
  else {
    // Mensagem privada para um usuário
    Client* targetClient = NULL;
    const std::map<int, Client*>& clients = serv.getClients();
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
      if (it->second->getNickname() == target) {
        targetClient = it->second;
        break;
      }
    }
    
    if (!targetClient) {
      std::string reply = makeReply(ERR_NOSUCHNICK, client->getNickname(), target + " :No such nick/channel");
      serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
      return;
    }
    
    // Envia a mensagem privada
    std::string privateMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " PRIVMSG " + target + " :" + message;
    serv.outgoingMessage(targetClient->getFd(), privateMsg + "\r\n");
  }
}

void  execJoin(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;

  // Verifica se o cliente está registrado
  if (!client->isRegistered()) {
    std::string reply = makeReply(ERR_NOTREGISTERED, "*", ":You have not registered");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  // Verifica se o nome do canal foi fornecido
  if (cmd.parameters.empty()) {
    std::string reply = makeReply(ERR_NEEDMOREPARAMS, client->getNickname(), "JOIN :Not enough parameters");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  std::string channelName = cmd.parameters[0];
  std::string key = (cmd.parameters.size() > 1) ? cmd.parameters[1] : "";

  // Valida o nome do canal
  if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&')) {
    std::string reply = makeReply(ERR_NOSUCHCHANNEL, client->getNickname(), channelName + " :No such channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Pega ou cria o canal
  Channel* channel = serv.getChannel(channelName);
  if (!channel) {
    channel = serv.createChannel(channelName);
  }

  // Verifica se o cliente pode entrar no canal
  if (!channel->canJoin(client, key)) {
    if (channel->hasUserLimit() && channel->getMemberCount() >= channel->getUserLimit()) {
      std::string reply = makeReply(ERR_CHANNELISFULL, client->getNickname(), channelName + " :Cannot join channel (+l)");
      serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    }
    else if (channel->isInviteOnly() && !channel->isInvited(client)) {
      std::string reply = makeReply(ERR_INVITEONLYCHAN, client->getNickname(), channelName + " :Cannot join channel (+i)");
      serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    }
    else if (channel->hasKey() && key != channel->getKey()) {
      std::string reply = makeReply(ERR_BADCHANNELKEY, client->getNickname(), channelName + " :Cannot join channel (+k)");
      serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    }
    return;
  }

  // Adiciona o cliente ao canal
  channel->addMember(client);

  // Envia confirmação de JOIN para o cliente
  std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " JOIN " + channelName;
  serv.outgoingMessage(cmd.clientFD, joinMsg + "\r\n");

  // Notifica outros membros do canal
  const std::set<Client*>& members = channel->getMembers();
  for (std::set<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
    if (*it != client) {
      serv.outgoingMessage((*it)->getFd(), joinMsg + "\r\n");
    }
  }

  // Se o tópico existir, envia o tópico atual
  if (!channel->getTopic().empty()) {
    std::string topicReply = makeReply(RPL_TOPIC, client->getNickname(), channelName + " :" + channel->getTopic());
    serv.outgoingMessage(cmd.clientFD, topicReply + "\r\n");
  } else {
    std::string noTopicReply = makeReply(RPL_NOTOPIC, client->getNickname(), channelName + " :No topic is set");
    serv.outgoingMessage(cmd.clientFD, noTopicReply + "\r\n");
  }

  // Se a lista de nomes for solicitada, envia a lista de nomes
  std::string namesList = channel->getMembersList();
  std::string namesReply = makeReply(RPL_NAMREPLY, client->getNickname(), "= " + channelName + " :" + namesList);
  std::string endNamesReply = makeReply(RPL_ENDOFNAMES, client->getNickname(), channelName + " :End of /NAMES list");
  
  serv.outgoingMessage(cmd.clientFD, namesReply + "\r\n");
  serv.outgoingMessage(cmd.clientFD, endNamesReply + "\r\n");
}

void  execPart(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  // Verifica se o cliente está registrado
  if (!client->isRegistered()) {
    std::string reply = makeReply(ERR_NOTREGISTERED, "*", ":You have not registered");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica se o nome do canal foi fornecido
  if (cmd.parameters.empty()) {
    std::string reply = makeReply(ERR_NEEDMOREPARAMS, client->getNickname(), "PART :Not enough parameters");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  std::string channelName = cmd.parameters[0];
  std::string partMessage = cmd.trailing.empty() ? "Leaving" : cmd.trailing;
  
  // Pega o canal
  Channel* channel = serv.getChannel(channelName);
  if (!channel) {
    std::string reply = makeReply(ERR_NOSUCHCHANNEL, client->getNickname(), channelName + " :No such channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica se o cliente está no canal
  if (!channel->isMember(client)) {
    std::string reply = makeReply(ERR_NOTONCHANNEL, client->getNickname(), channelName + " :You're not on that channel");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Envia a mensagem PART para todos os membros do canal (incluindo o remetente)
  std::string partMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " PART " + channelName + " :" + partMessage;
  const std::set<Client*>& members = channel->getMembers();
  for (std::set<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
    serv.outgoingMessage((*it)->getFd(), partMsg + "\r\n");
  }
  
  // Remove o cliente do canal
  channel->removeMember(client);

  // Se o canal estiver vazio, remove o canal do servidor
  if (channel->isEmpty()) {
    serv.removeChannel(channelName);
  }
}

void  execQuit(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  std::string quitMessage = cmd.trailing.empty() ? "Client Quit" : cmd.trailing;
  
  // Nota: Por agora só imprime a mensagem de quit no console
  // A implementação completa removeria o cliente de todos os canais e notificaria os membros
  std::cout << "Client " << (client->getNickname().empty() ? "unknown" : client->getNickname()) << " quit: " << quitMessage << std::endl;
}

void  execMode(CommandStruct &cmd, IrcServ &serv)
{
  Client* client = serv.getClient(cmd.clientFD);
  if (!client) return;
  
  // Verifica se o cliente está registrado
  if (!client->isRegistered()) {
    std::string reply = makeReply(ERR_NOTREGISTERED, "*", ":You have not registered");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }

  // Verifica os parâmetros
  if (cmd.parameters.empty()) {
    std::string reply = makeReply(ERR_NEEDMOREPARAMS, client->getNickname(), "MODE :Not enough parameters");
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
    return;
  }
  
  std::string target = cmd.parameters[0];
  
  // Por agora, apenas mostra o modo do canal ou usuário
  if (target[0] == '#' || target[0] == '&') {
    // Channel mode
    Channel* channel = serv.getChannel(target);
    if (!channel) {
      std::string reply = makeReply(ERR_NOSUCHCHANNEL, client->getNickname(), target + " :No such channel");
      serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
      return;
    }
    
    // Por agora, apenas retorna um modo fixo (+nt)
    std::cout << "MODE request for channel " << target << " from " << client->getNickname() << std::endl;
    std::string reply = ":" + std::string(SERVER_NAME) + " 324 " + client->getNickname() + " " + target + " +nt";
    serv.outgoingMessage(cmd.clientFD, reply + "\r\n");
  } else {
    // Modo de usuário - não implementado por enquanto
    std::cout << "User MODE request (not implemented)" << std::endl;
  }
}
