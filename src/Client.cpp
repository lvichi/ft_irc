#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/macros.hpp"
#include <cstring>
#include <sstream>

Client::Client() : _fd(-1), _registered(false), _passwordAuthenticated(false) {}
Client::Client(const Client &other) {*this = other;}
Client &Client::operator=(const Client &other) {
    if (this != &other) {
        _fd = other._fd;
        _nickname = other._nickname;
        _username = other._username;
        _realname = other._realname;
        _hostname = other._hostname;
        _registered = other._registered;
        _passwordAuthenticated = other._passwordAuthenticated;
        _channels = other._channels;
    }
    return *this;
}
Client::~Client() {}
Client::Client(int fd) : _fd(fd), _registered(false), _passwordAuthenticated(false) {}

// Getter
int Client::getFd() const {return _fd;}
const std::string &Client::getUsername() const {return _username;}
const std::string &Client::getNickname() const {return _nickname;}
const std::string &Client::getRealname() const {return _realname;}
const std::string &Client::getHostname() const {return _hostname;}
bool Client::isRegistered() const {return _registered;}
bool Client::isPasswordAuthenticated() const {return _passwordAuthenticated;}

// Setter
void Client::setUsername(const std::string &username) {_username = username;}
void Client::setNickname(const std::string &nickname) {_nickname = nickname;}
void Client::setRealname(const std::string &realname) {_realname = realname;}
void Client::setHostname(const std::string &hostname) {_hostname = hostname;}
void Client::authenticate() {_registered = true;}
void Client::authenticatePassword() {_passwordAuthenticated = true;}

// Methods
void Client::send(const std::string &message, IrcServ &server) const {
    if (_fd != -1) {
        server.outgoingMessage(_fd, message + "\r\n");
    }
}

std::string Client::getFullPrefix() const {
    return _nickname + "!" + _username + "@" + _hostname;
}

void Client::sendError(IrcServ &server, t_error errorCode, const std::string& message) const {
    std::ostringstream oss;
    oss << errorCode;
    std::string reply = ":" + std::string(SERVER_NAME) + " " + oss.str() + " " + 
                       (_nickname.empty() ? "*" : _nickname) + " " + message;
    send(reply, server);
}

void Client::sendError(IrcServ &server, t_error errorCode) const {
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
    
    std::string reply = ":" + std::string(SERVER_NAME) + " " + oss.str() + " " + 
                       (_nickname.empty() ? "*" : _nickname) + " " + message;
    send(reply, server);
}

void Client::sendWelcome(IrcServ &server) const {
    std::string welcome = ":" + std::string(SERVER_NAME) + " " + RPL_WELCOME + " " + _nickname + 
                          " :Welcome to the Internet Relay Network " + _nickname;
    std::string yourhost = ":" + std::string(SERVER_NAME) + " " + RPL_YOURHOST + " " + _nickname + 
                           " :Your host is " + std::string(SERVER_NAME) + ", running version " + std::string(SERVER_VERSION);
    send(welcome, server);
    send(yourhost, server);
}

bool Client::canRegister() const {
    return _passwordAuthenticated && !_nickname.empty() && !_username.empty() && !_registered;
}

void Client::sendInvite(IrcServ& serv, const std::string& channelName) {
    std::string msg = ":" + std::string(SERVER_NAME) + " 341 " + getNickname() + " " + channelName + "\r\n";
    serv.outgoingMessage(getFd(), msg);
}

void Client::sendPrivmsg(Client* sender, const std::string& msg, IrcServ& serv) {
    std::string privMsg = ":" + sender->getFullPrefix() + " PRIVMSG " + getNickname() + " :" + msg + "\r\n";
    serv.outgoingMessage(getFd(), privMsg);
}

void Client::sendQuit(IrcServ& serv, const std::string& quitMessage) {
    std::string quitMsg = ":" + getFullPrefix() + " QUIT :" + quitMessage + "\r\n";

    std::set<std::string> channels = _channels;    
    for (std::set<std::string>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel* channel = serv.getChannel(*it);
        if (channel) {
            const std::set<Client*>& members = channel->getMembers();
            for (std::set<Client*>::const_iterator mit = members.begin(); mit != members.end(); ++mit) {
                if (*mit != this) {
                    serv.outgoingMessage((*mit)->getFd(), quitMsg);
                }
            }
            channel->removeMember(this);
            if (channel->isEmpty()) serv.removeChannel(*it);
        }
    }
    _channels.clear();
}

std::set<std::string> Client::getChannels() const {
    return _channels;
}

void Client::addChannel(const std::string& channelName) {
    _channels.insert(channelName);
}

void Client::removeChannel(const std::string& channelName) {
    _channels.erase(channelName);
}

void Client::handleUserMode(CommandStruct& cmd, IrcServ& serv) {
    (void)cmd; // Suppress unused parameter warning
    serv.outgoingMessage(getFd(), ":USERMODE command received\r\n");
}