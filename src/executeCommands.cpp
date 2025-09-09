#include "executeCommands.hpp"


void executeCommands( std::list<CommandStruct>& commands, IrcServ& server )
{
  printCommands( commands );

  for ( std::list<CommandStruct>::iterator it = commands.begin(); it != commands.end(); ++it ) {
    std::stringstream   ss;
    ss << "Responding message to client FD: " << it->clientFD
       << " Message: " << it->trailing
       << "\r\n";
    server.outgoingMessage( it->clientFD, ss.str() );
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
