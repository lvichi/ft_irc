#include "../includes/Client.hpp"
#include <cstring>

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
void Client::setPasswordAuthenticated(bool auth) {_passwordAuthenticated = auth;}

// Methods
void Client::send(const std::string &message, IrcServ &server) const {
    if (_fd != -1) {
        server.outgoingMessage(_fd, message + "\r\n");
    }
}