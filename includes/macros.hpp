/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macros.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rapcampo <rapcampo@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 19:02:25 by rapcampo          #+#    #+#             */
/*   Updated: 2025/08/20 20:02:33 by rapcampo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "parseCommands.hpp"

#define BELL "\a"
#define PARAM_MAX 14
#define NICK_MAX 9
#define CRLF "\r\n"
#define SPECIAL "[]\\`^{}"
#define HYPHEN "-"
#define DIGITS "0123456789"
#define ALPHAS "abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ"
#define CHANNEL_MAX 50
#define CHANNEL_PREFIX "#&!+"
#define MOD_CHANGE "-+"
#define ALLOWED_MODES "itkol"

//colour prints

# define RST "\e[0m"
# define ULI "\e[4m"
# define BLN "\e[5m"
# define BLK "\e[1;30m"
# define RED "\e[1;31m"
# define GRN "\e[1;32m"
# define YLW "\e[1;33m"
# define BLU "\e[1;34m"
# define PRP "\e[1;35m"
# define CYN "\e[1;36m"
# define WHT "\e[1;37m"
# define CLR "\e[0;39m"

//ERROR status messages

# define INVAL_CHAR "Error: invalid character input\n" //\a \0
# define TOO_MANY_TAGS "Error: this command has too many tags\n"
# define NO_SUCH_CMD "Error: this command does not exist\n"

// IRC Reply codes (RFC 1459)
// Welcome messages
#define RPL_WELCOME             "001"
#define RPL_YOURHOST            "002"
#define RPL_CREATED             "003"
#define RPL_MYINFO              "004"

// Channel replies
#define RPL_NOTOPIC             "331"
#define RPL_TOPIC               "332"
#define RPL_NAMREPLY            "353"
#define RPL_ENDOFNAMES          "366"

// Server settings
#define SERVER_NAME             "ft_irc" // Spaces here can cause issues
#define SERVER_VERSION          "0.42"

typedef enum e_error {
  ERR_NOSUCHNICK        = 401, // target nick does not exist
  ERR_NOSUCHCHANNEL     = 403, // no such channel
  ERR_CANNOTSENDTOCHAN  = 404, // cannot send to channel (e.g., +n, +m, banned)
  ERR_TOOMANYCHANNELS   = 405, // too many channels joined

  ERR_NOORIGIN          = 409, // no origin specified (e.g., bad PING/PONG)

  ERR_NORECIPIENT       = 411, // no recipient given
  ERR_NOTEXTTOSEND      = 412, // no text to send
  ERR_NOTOPLEVEL        = 413, // no top-level domain specified
  ERR_WILDTOPLEVEL      = 414, // wildcard in top-level domain
  ERR_BADMASK           = 415, // bad server/host mask
  ERR_INPUTTOOLONG	    = 417, // more than 14 params or 512 bytes

  ERR_UNKNOWNCOMMAND    = 421, // unknown/unsupported command
  ERR_NOMOTD            = 422, // MOTD is missing

  ERR_NONICKNAMEGIVEN   = 431, // no nickname provided
  ERR_ERRONEUSNICKNAME  = 432, // invalid nickname
  ERR_NICKNAMEINUSE     = 433, // nickname already in use

  ERR_USERNOTINCHANNEL  = 441, // target user not in channel
  ERR_NOTONCHANNEL      = 442, // you are not on that channel
  ERR_USERONCHANNEL     = 443, // user already on that channel

  ERR_NOTREGISTERED     = 451, // command before registration

  ERR_NEEDMOREPARAMS    = 461, // not enough parameters
  ERR_ALREADYREGISTERED = 462, // already registered (aka ERR_ALREADYREGISTRED)
  ERR_PASSWDMISMATCH    = 464, // incorrect or missing PASS
  ERR_YOUREBANNEDCREEP  = 465, // you are banned from this server

  ERR_CHANNELISFULL     = 471, // channel is full (+l)
  ERR_UNKNOWNMODE       = 472, // unknown mode flag
  ERR_INVITEONLYCHAN    = 473, // invite-only channel (+i)
  ERR_BANNEDFROMCHAN    = 474, // banned from channel (+b)
  ERR_BADCHANNELKEY     = 475, // bad channel key (+k)
  ERR_BADCHANMASK       = 476, // bad channel mask
  ERR_NOCHANMODES       = 477, // channel does not support modes
  ERR_BANLISTFULL       = 478, // ban list is full

  ERR_NOPRIVILEGES      = 481, // need IRC operator privileges
  ERR_CHANOPRIVSNEEDED  = 482, // need channel operator privileges

  ERR_NOOPERHOST        = 491, // OPER not allowed from this host

  ERR_UMODEUNKNOWNFLAG  = 501, // unknown user mode flag
  ERR_USERSDONTMATCH    = 502  // cannot change modes for other users
} t_error;

// Error messages for IRC protocol
#define CMT_NOSUCHNICK          ":No such nick/channel"
#define CMT_NOSUCHCHANNEL       ":No such channel"
#define CMT_CANNOTSENDTOCHAN    ":Cannot send to channel"
#define CMT_TOOMANYCHANNELS     ":You have joined too many channels"
#define CMT_NOORIGIN            ":No origin specified"
#define CMT_NORECIPIENT         ":No recipient given"
#define CMT_NOTEXTTOSEND        ":No text to send"
#define CMT_NOTOPLEVEL          ":No toplevel domain specified"
#define CMT_WILDTOPLEVEL        ":Wildcard in toplevel domain"
#define CMT_BADMASK             ":Bad Server/host mask"
#define CMT_INPUTTOOLONG        ":Input line was too long"
#define CMT_UNKNOWNCOMMAND      ":Unknown command"
#define CMT_NOMOTD              ":MOTD File is missing"
#define CMT_NONICKNAMEGIVEN     ":No nickname given"
#define CMT_ERRONEUSNICKNAME    ":Erroneous nickname"
#define CMT_NICKNAMEINUSE       ":Nickname is already in use"
#define CMT_USERNOTINCHANNEL    ":They aren't on that channel"
#define CMT_NOTONCHANNEL        ":You're not on that channel"
#define CMT_USERONCHANNEL       ":is already on channel"
#define CMT_NOTREGISTERED       ":You have not registered"
#define CMT_NEEDMOREPARAMS      ":Not enough parameters"
#define CMT_ALREADYREGISTERED   ":You may not reregister"
#define CMT_PASSWDMISMATCH      ":Password incorrect"
#define CMT_YOUREBANNEDCREEP    ":You are banned from this server"
#define CMT_CHANNELISFULL       ":Cannot join channel (+l)"
#define CMT_UNKNOWNMODE         ":is unknown mode char to me"
#define CMT_INVITEONLYCHAN      ":Cannot join channel (+i)"
#define CMT_BANNEDFROMCHAN      ":Cannot join channel (+b)"
#define CMT_BADCHANNELKEY       ":Cannot join channel (+k)"
#define CMT_BADCHANMASK         ":Bad Channel Mask"
#define CMT_NOCHANMODES         ":Channel doesn't support modes"
#define CMT_BANLISTFULL         ":Channel banlist is full"
#define CMT_NOPRIVILEGES        ":Permission Denied- You're not an IRC operator"
#define CMT_CHANOPRIVSNEEDED    ":You're not channel operator"
#define CMT_NOOPERHOST          ":No O-lines for your host"
#define CMT_UMODEUNKNOWNFLAG    ":Unknown MODE flag"
#define CMT_USERSDONTMATCH      ":Cant change mode for other users"

//typedef bool (*cmds[8])(std::list<CommandStruct> cmd);

typedef enum e_cmd{
	PASS,
	USER,
	NICK,
	KICK,
	INVITE,
	TOPIC,
	PRIVMSG,
	MODE,
	UNKNOWN = -1
} t_cmd;

typedef enum e_mode{
	I_MODE,
	T_MODE,
	K_MODE,
	O_MODE,
	L_MODE
} t_mode;

bool isSpecial(unsigned char c);
