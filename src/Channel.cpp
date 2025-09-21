#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include "../includes/IrcServ.hpp"
#include "../includes/macros.hpp"
#include "../includes/IrcServ.hpp"
#include <algorithm>
#include <deque>
#include <sstream>

Channel::Channel() : _inviteOnly(false), _topicProtected(false), _hasKey(false), _hasUserLimit(false), _userLimit(0) {}

Channel::Channel(const std::string& name) : _name(name), _inviteOnly(false), _topicProtected(false), _hasKey(false), _hasUserLimit(false), _userLimit(0) {}

Channel::Channel(const Channel& other) { *this = other; }

Channel& Channel::operator=(const Channel& other) {
    if (this != &other) {
        _name = other._name;
        _topic = other._topic;
        _key = other._key;
        _members = other._members;
        _operators = other._operators;
        _invited = other._invited;
        _inviteOnly = other._inviteOnly;
        _topicProtected = other._topicProtected;
        _hasKey = other._hasKey;
        _hasUserLimit = other._hasUserLimit;
        _userLimit = other._userLimit;
    }
    return *this;
}

Channel::~Channel() {}

// Getters
const std::string& Channel::getName() const { return _name; }
const std::string& Channel::getTopic() const { return _topic; }
const std::string& Channel::getKey() const { return _key; }
const std::set<Client*>& Channel::getMembers() const { return _members; }
const std::set<Client*>& Channel::getOperators() const { return _operators; }
const std::set<Client*>& Channel::getInvited() const { return _invited; }
unsigned int Channel::getUserLimit() const { return _userLimit; }

// Mode getters
bool Channel::isInviteOnly() const { return _inviteOnly; }
bool Channel::isTopicProtected() const { return _topicProtected; }
bool Channel::hasKey() const { return _hasKey; }
bool Channel::hasUserLimit() const { return _hasUserLimit; }

// Setters
void Channel::setTopic(const std::string& topic) { _topic = topic; }
void Channel::setKey(const std::string& key) { 
    _key = key; 
    _hasKey = !key.empty();
}
void Channel::setUserLimit(unsigned int limit) { 
    _userLimit = limit;
    _hasUserLimit = (limit > 0);
}

// Mode setters
void Channel::setInviteOnly(bool mode) { _inviteOnly = mode; }
void Channel::setTopicProtected(bool mode) { _topicProtected = mode; }
void Channel::setHasKey(bool mode) { _hasKey = mode; }
void Channel::setHasUserLimit(bool mode) { _hasUserLimit = mode; }

// Member management
bool Channel::isMember(Client* client) const {
    return _members.find(client) != _members.end();
}

bool Channel::isOperator(Client* client) const {
    return _operators.find(client) != _operators.end();
}

bool Channel::isInvited(Client* client) const {
    return _invited.find(client) != _invited.end();
}

bool Channel::canJoin(Client* client, const std::string& key) const {
    if (_hasUserLimit && _members.size() >= _userLimit)
        return false;
    
    if (_inviteOnly && !isInvited(client))
        return false;
    
    if (_hasKey && key != _key)
        return false;
    
    return true;
}

void Channel::addMember(Client* client) {
    _members.insert(client);
    client->addChannel(_name);
    if (_members.size() == 1) {
        _operators.insert(client);
    }
}

void Channel::removeMember(Client* client) {
    _members.erase(client);
    _operators.erase(client);
    _invited.erase(client);
}

void Channel::addOperator(Client* client) {
    if (isMember(client)) {
        _operators.insert(client);
    }
}

void Channel::removeOperator(Client* client) {
    _operators.erase(client);
}

void Channel::addInvited(Client* client) {
    _invited.insert(client);
}

void Channel::removeInvited(Client* client) {
    _invited.erase(client);
}

// Utility methods
bool Channel::isEmpty() const {
    return _members.empty();
}

unsigned int Channel::getMemberCount() const {
    return _members.size();
}

std::string Channel::getMembersList() const {
    std::string list;
    for (std::set<Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it) {
        if (!list.empty()) list += " ";
        
        if (isOperator(*it)) list += "@";
        
        list += (*it)->getNickname();
    }
    return list;
}

void Channel::broadcast(const std::string& message, IrcServ& server) const {
    for (std::set<Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it) {
        (*it)->send(message, server);
    }
}

void Channel::sendJoinMessages(Client* client, IrcServ& server) const {
    std::string joinMsg = ":" + client->getFullPrefix() + " JOIN " + _name;
    broadcast(joinMsg, server);
    
    if (!_topic.empty()) {
        std::string topicReply = ":" + std::string(SERVER_NAME) + " " + RPL_TOPIC + " " + 
                               client->getNickname() + " " + _name + " :" + _topic;
        client->send(topicReply, server);
    } else {
        std::string noTopicReply = ":" + std::string(SERVER_NAME) + " " + RPL_NOTOPIC + " " + 
                                 client->getNickname() + " " + _name + " :No topic is set";
        client->send(noTopicReply, server);
    }
    
    std::string namesList = getMembersList();
    std::string namesReply = ":" + std::string(SERVER_NAME) + " " + RPL_NAMREPLY + " " + 
                           client->getNickname() + " = " + _name + " :" + namesList;
    std::string endNamesReply = ":" + std::string(SERVER_NAME) + " " + RPL_ENDOFNAMES + " " + 
                              client->getNickname() + " " + _name + " :End of /NAMES list";
    client->send(namesReply, server);
    client->send(endNamesReply, server);
}

void Channel::sendPartMessages(Client* client, const std::string& reason, IrcServ& server) const {
    std::string partMsg = ":" + client->getFullPrefix() + " PART " + _name + " :" + reason;
    broadcast(partMsg, server);
}

void Channel::sendKickMessage(Client* sender, Client* target, const std::string& msg, IrcServ& serv) {
    std::string kickMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname() + " KICK " + getName() + " " + target->getNickname() + " :" + msg + "\r\n";
    for (std::set<Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
        serv.outgoingMessage((*it)->getFd(), kickMsg);
}

void Channel::sendTopic(Client* client, IrcServ& serv) {
    std::string topicMsg = ":" + std::string(SERVER_NAME) + " 332 " + client->getNickname() + " " + getName() + " :" + getTopic() + "\r\n";
    serv.outgoingMessage(client->getFd(), topicMsg);
}

void Channel::sendMode(Client* client, IrcServ& serv) {
    std::stringstream topicMsg;
    topicMsg << ":" + std::string(SERVER_NAME) + " 324 " + client->getNickname() + " " + getName() + " " + getModes();
    if (hasKey()){
        topicMsg << " ";
        if (isOperator(client))
            topicMsg << getKey();
        else
            topicMsg << "***";
    }
    if (hasUserLimit()){
        topicMsg << " " << getUserLimit();
    }
    topicMsg << "\r\n";
    serv.outgoingMessage(client->getFd(), topicMsg.str());
}

void Channel::broadcastTopic(Client* client, IrcServ& serv) {
    std::string topicMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " TOPIC " + getName() + " :" + getTopic() + "\r\n";
    for (std::set<Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
        serv.outgoingMessage((*it)->getFd(), topicMsg);
}

void Channel::broadcastPrivmsg(Client* sender, const std::string& msg, IrcServ& serv) {
    std::string privMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname() + " PRIVMSG " + getName() + " :" + msg + "\r\n";
    for (std::set<Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
        if (*it != sender)
            serv.outgoingMessage((*it)->getFd(), privMsg);
}

void Channel::handleMode(CommandStruct& cmd, IrcServ& serv,
        std::deque<std::string> &mode, std::deque<std::string> &param) {
    Client* client = serv.getClient(cmd.clientFD);
    if (!client) return;
	std::string modeChange;
	std::string mStr(ALLOWED_MODES);
    for (std::deque<std::string>::iterator it = mode.begin(); it != mode.end(); it++){
		modeChange = ":" + std::string(SERVER_NAME) + client->getNickname() + getName() + " MODE " + getName() + " ";
        switch(mStr.find_first_of((*it)[1])){
            case (I_MODE):{
                if ((*it)[0] == '+' && !isInviteOnly()){
                    setInviteOnly(true);
					modeChange += *it;
				}
                else if ((*it)[0] == '-'){
					modeChange += *it;
                    setInviteOnly(false);
				}
                break;
            }
            case (T_MODE):{
                if ((*it)[0] == '+' && !isTopicProtected()){
					modeChange += *it;
                    setTopicProtected(true);
				}
                else if ((*it)[0] == '-'){
					modeChange += *it;
                    setTopicProtected(false);
				}
                break;
            }
            case (K_MODE):{
                if (param.empty()){
                    sendError(client, serv, ERR_NEEDMOREPARAMS);
                    return;
                }
                if ((*it)[0] == '+' && !hasKey()){
                    setKey(param.front());
					modeChange += *it + " " + param[0];
                    param.pop_front();
                    setHasKey(true);
                }
                else if ((*it)[0] == '-' && hasKey()){
                    if (param.front() == getKey()){
                        param.pop_front();
                        setKey(std::string(""));
						modeChange += *it;
                        setHasKey(false);
                    }
                    else{
                        sendError(client, serv, ERR_BADCHANNELKEY);
                        return;
                    }
                }
                break;
            }
            case (O_MODE):{
                if (param.empty()){
                    sendError(client, serv, ERR_NEEDMOREPARAMS);
                    return;
                }
                Client *opToBe = serv.getClientByNick(param.front());
                if (!opToBe){
                    sendError(client, serv, ERR_NOSUCHNICK);
                    return;
                }
                if (!isMember(opToBe)){
                    sendError(client, serv, ERR_NOTONCHANNEL);
                    return;
                }
                param.pop_front();
                if ((*it)[0] == '+' && !isOperator(opToBe)){
					modeChange += *it + " " + opToBe->getNickname();
                    addOperator(opToBe);
				}
                else if ((*it)[0] == '-' && isOperator(opToBe)){
					modeChange += *it + " " + opToBe->getNickname();
                    removeOperator(opToBe);
				}
                break;
            }
            case (L_MODE):{
                if ((*it)[0] == '+'){
                    if (param.empty() 
                            || param.front().find_first_not_of(DIGITS) != std::string::npos){
                        sendError(client, serv, ERR_NEEDMOREPARAMS);
                        return;
                    }
                    unsigned long limit = std::strtoul(param.front().c_str(), 0, 10);
                    if (limit > CHANNEL_MAX || limit == 0){
                        sendError(client, serv, ERR_INPUTTOOLONG);
                        return;
                    }
                    setUserLimit(limit);
                    setHasUserLimit(true);
					modeChange += *it + " " + param[0];
                    param.pop_front();
                }
                else if ((*it)[0] == '-'){
                    setUserLimit(CHANNEL_MAX);
                    setHasUserLimit(false);
					modeChange += *it;
                }
                break;
            }
            default:{
                sendError(client, serv, ERR_UNKNOWNMODE);
                break;
            }
        }
		modeChange += CRLF;
		broadcast(modeChange, serv);
    }
}
//itkol
const std::string Channel::getModes() const{
    std::string activeModes = "";
    if (isInviteOnly())
        activeModes += "i";
    if (isTopicProtected())
        activeModes += "t";
    if (hasKey())
        activeModes += "k";
    if (hasUserLimit())
        activeModes += "l";
    if (!activeModes.empty())
        activeModes.insert(0, "+");
    return activeModes;
}

void Channel::sendError(Client* client, IrcServ& serv, t_error errorCode) const {
    std::ostringstream oss;
    oss << errorCode;
    
    const t_error errorCodes[] = {
        ERR_NOSUCHNICK, ERR_NOSUCHCHANNEL, ERR_CANNOTSENDTOCHAN, ERR_TOOMANYCHANNELS,
        ERR_UNKNOWNCOMMAND, ERR_NONICKNAMEGIVEN, ERR_ERRONEUSNICKNAME, ERR_NICKNAMEINUSE,
        ERR_USERNOTINCHANNEL, ERR_NOTONCHANNEL, ERR_USERONCHANNEL, ERR_NOTREGISTERED,
        ERR_NEEDMOREPARAMS, ERR_ALREADYREGISTERED, ERR_PASSWDMISMATCH, ERR_CHANNELISFULL,
        ERR_UNKNOWNMODE, ERR_INVITEONLYCHAN, ERR_BANNEDFROMCHAN, ERR_BADCHANNELKEY,
        ERR_CHANOPRIVSNEEDED
    };
    
    const char* errorMessages[] = {
        CMT_NOSUCHNICK, CMT_NOSUCHCHANNEL, CMT_CANNOTSENDTOCHAN, CMT_TOOMANYCHANNELS,
        CMT_UNKNOWNCOMMAND, CMT_NONICKNAMEGIVEN, CMT_ERRONEUSNICKNAME, CMT_NICKNAMEINUSE,
        CMT_USERNOTINCHANNEL, CMT_NOTONCHANNEL, CMT_USERONCHANNEL, CMT_NOTREGISTERED,
        CMT_NEEDMOREPARAMS, CMT_ALREADYREGISTERED, CMT_PASSWDMISMATCH, CMT_CHANNELISFULL,
        CMT_UNKNOWNMODE, CMT_INVITEONLYCHAN, CMT_BANNEDFROMCHAN, CMT_BADCHANNELKEY,
        CMT_CHANOPRIVSNEEDED
    };
    
    const int numErrors = sizeof(errorCodes) / sizeof(errorCodes[0]);
    
    std::string message = ":Unknown error";
    for (int i = 0; i < numErrors; i++) {
        if (errorCodes[i] == errorCode) {
            message = errorMessages[i];
            break;
        }
    }

    // Para erros relacionados a canal, incluir o nome do canal na mensagem
    std::string reply = ":" + std::string(SERVER_NAME) + " " + oss.str() + " " + 
                       (client->getNickname().empty() ? "*" : client->getNickname()) + " " + 
                       _name + " " + message;
    client->send(reply, serv);
}

void Channel::sendError(Client* client, IrcServ& serv, t_error errorCode, const std::string& message) const {
    std::ostringstream oss;
    oss << errorCode;
    
    // Para erros relacionados a canal, incluir o nome do canal na mensagem
    std::string reply = ":" + std::string(SERVER_NAME) + " " + oss.str() + " " + 
                       (client->getNickname().empty() ? "*" : client->getNickname()) + " " + 
                       _name + " " + message;
    client->send(reply, serv);
}
