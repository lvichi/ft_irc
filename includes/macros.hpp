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
#define TAG_LIMIT 14
#define CRLF "\r\n"
#define SPECIAL "[]\\`^{}"
#define HYPHEN "-"
#define DIGITS "0123456789"
#define ALPHAS "abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ"
#define CHANNEL_MAX 50
#define CHANNEL_PREFIX "#&!+"

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

//ERROR messages

# define INVAL_CHAR "Error: invalid character input\n" //\a \0
# define TOO_MANY_TAGS "Error: this command has too many tags\n"
# define NO_SUCH_CMD "Error: this command does not exist\n"
# define WRONG_PREFIX "Error: usage of prefix is wrong\n Prefix is \"nick[!user]@host\"\n"

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
  ERR_INPUTTOOLONG	= 417, // more than 14 params or 512 bytes

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
