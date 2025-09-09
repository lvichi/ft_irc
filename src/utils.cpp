#include "../includes/parseCommands.hpp"
#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include "../includes/IrcServ.hpp"

bool isSpecial(unsigned char c){
  std::string specialString(SPECIAL);
  if (specialString.find_first_of(c) != specialString.npos)
    return true;
  return false;
}

t_serverInfo *server(void){
  static t_serverInfo serv = {0};
  return &serv;
}
