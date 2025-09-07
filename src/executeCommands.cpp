#include "../includes/executeCommands.hpp"
#include "../includes/macros.hpp"
#include <stdexcept>

//insert cmd distribution in this part, easy to test out.. 
//
static void normalizeInput(std::string &cmd){
  for (int i = 0; cmd[i]; i++)
    cmd[i] = std::toupper(cmd[i]);
}

static void findCommand(CommandStruct &command){
  const char *cmds[8] = {"PASS", "NICK", "KICK", "INVITE", "TOPIC", "PRIVMSG", "MODE", NULL};
  normalizeInput(command.command);
  for (int i = 0; cmds[i] != NULL; i++){
    if (command.command == cmds[i]){
      command.type = i;
      return;
    }
  }
  command.type = UNKNOWN;
}

void executeCommands( std::list<CommandStruct>& commands, std::map<unsigned int, std::string>& outgoingMessages )
{

  for ( std::list<CommandStruct>::iterator it = commands.begin(); it != commands.end(); ++it ) {
    std::stringstream   ss;
    findCommand(*it);
    switch (it->type){
      case UNKNOWN:
        it->errorCode = ERR_UNKNOWNCOMMAND;
        std::cout << "\e[1;31m >> Is UNKNOWN << \e[0m" << std::endl;
        ss << "Command sent is Unknown by this server" << "\r\n";
        outgoingMessages[ it->clientFD ] += ss.str();
        continue ;
      case PASS:
        std::cout << "\e[1;31m >> Is PASS << \e[0m" << std::endl;
        printCommands( commands );
        break;
      case NICK:
        std::cout << "\e[1;31m >> Is NICK << \e[0m" << std::endl;
        printCommands( commands );
        break;
      case KICK:
        std::cout << "\e[1;31m >> Is KICK << \e[0m" << std::endl;
        printCommands( commands );
        break;
      case INVITE:
        std::cout << "\e[1;31m >> Is INVITE << \e[0m" << std::endl;
        printCommands( commands );
        break;
      case TOPIC:
        std::cout << "\e[1;31m >> Is TOPIC << \e[0m" << std::endl;
        printCommands( commands );
        break;
      case PRIVMSG:
        std::cout << "\e[1;31m >> Is PRIVMSG << \e[0m" << std::endl;
        printCommands( commands );
        break;
      case MODE:
        std::cout << "\e[1;31m >> Is MODE << \e[0m" << std::endl;
        printCommands( commands );
        break;
    }
    ss << "Responding message to client FD: " << it->clientFD
      << " Message: " << it->trailing
      << "\r\n";
    outgoingMessages[ it->clientFD ] += ss.str();
  }

  commands.clear();
}

void printCommands( std::list<CommandStruct>& commands )
{
  int i = 0;

  for ( std::list<CommandStruct>::iterator it = commands.begin(); it != commands.end(); ++it ) {

    CommandStruct& cmd = *it;
    std::cout << "Commands[" << i++ << "] :" << std::endl
      << "  Client FD: " << cmd.clientFD << std::endl
      << "  Prefix: " << cmd.prefix << std::endl
      << "  Command: " << cmd.command << std::endl
      << "  Parameters: ";

    for ( std::vector<std::string>::iterator param = cmd.parameters.begin(); param != cmd.parameters.end(); ++param )
      std::cout << *param << " ";
    std::cout << std::endl;

    std::cout << "  Trailing: " << cmd.trailing << std::endl;
  }
}
