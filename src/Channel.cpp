#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include "../includes/IrcServ.hpp"
#include "../includes/macros.hpp"
#include "../includes/IrcServ.hpp"
#include <algorithm>

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