#include "../includes/parseCommands.hpp"
#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include "../includes/IrcServ.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"

bool isSpecial(unsigned char c){
  std::string specialString(SPECIAL);
  if (specialString.find_first_of(c) != specialString.npos)
    return true;
  return false;
}

bool isNicknameInUse(const std::string& nick, IrcServ& serv) {
    const std::map<int, Client*>& clients = serv.getClients();
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getNickname() == nick)
            return true;
    }
    return false;
}

bool isUsernameInUse(const std::string& user, IrcServ& serv) {
    const std::map<int, Client*>& clients = serv.getClients();
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getUsername() == user)
            return true;
    }
    return false;
}
