

#include "IrcServ.hpp"


//struct ClientStruct
//{
//  int                                 fd;
//  std::string                         nickName;
//  std::string                         userName;
//  bool                                authenticated;
//  std::vector<std::string>            joinedChannels;
//};
//std::list<ClientStruct>               clients;
//
//struct ChannelStruct
//{
//  std::string                         name;
//  std::string                         topic;
//  std::vector<std::string>            members;
//  std::vector<std::string>            operators;
//  std::string                         channelKey;
//  int                                 userLimit;
//  bool                                inviteOnly;
//  bool                                topicRestricted;
//};
//std::list<ChannelStruct>              channels;


void IrcServ::executeCommands()
{
  printCommands();

  for ( std::deque<CommandStruct>::iterator it = commands.begin(); it != commands.end(); ++it ) {
    std::stringstream   ss;
    ss << "Responding message to client FD: " << it->clientFD
       << " Message: " << it->trailing
       << "\r\n";
    outgoingMessages[ it->clientFD ] += ss.str();
  }

  commands.clear();
}
