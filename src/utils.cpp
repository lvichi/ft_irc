#include "../includes/parseCommands.hpp"
#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"

bool isSpecial(unsigned char c){
  std::string specialString(SPECIAL);
  if (specialString.find_first_of(c) != specialString.npos)
    return true;
  return false;
}
