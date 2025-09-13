#pragma once

#include <string>
#include "../includes/IrcServ.hpp"
#include "../includes/macros.hpp"

class Client {
private:
    int _fd;
    std::string _username;
    std::string _nickname;
    std::string _realname;
    std::string _hostname;
    bool _registered;
    bool _passwordAuthenticated;

public:
    Client();
    Client(const Client &other);
    Client &operator=(const Client &original);
    ~Client();

    Client(int fd);

    int getFd() const;
    const std::string &getUsername() const;
    const std::string &getNickname() const;
    const std::string &getRealname() const;
    const std::string &getHostname() const;
    bool isRegistered() const;
    bool isPasswordAuthenticated() const;

    void setUsername(const std::string &username);
    void setNickname(const std::string &nickname);
    void setRealname(const std::string &realname);
    void setHostname(const std::string &hostname);
    void authenticate();
    void setPasswordAuthenticated(bool auth);

    void send(const std::string &message, IrcServ &server) const;
    
    std::string getFullPrefix() const;  // "nick!user@host"
    void sendError(IrcServ &server, t_error errorCode, const std::string& message) const;
    void sendError(IrcServ &server, t_error errorCode) const;
    void sendWelcome(IrcServ &server) const;
    bool canRegister() const;
};