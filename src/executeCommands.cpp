

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

  commands.clear();
}

