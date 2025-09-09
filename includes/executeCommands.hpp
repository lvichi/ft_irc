#pragma once

#include <iostream>           // std::cout, std::endl
#include <sstream>            // std::stringstream
#include <list>               // std::list
#include <map>                // std::map
#include "parseCommands.hpp"  // CommandStruct


void executeCommands( std::list<CommandStruct>& commands, IrcServ& server );
void printCommands( std::list<CommandStruct>& commands );


// EXAMPLE
//
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