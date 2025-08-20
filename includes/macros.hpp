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


//typedef bool (*cmds[8])(std::list<CommandStruct> cmd);

typedef enum e_cmd{
	PASS,
	NICK,
	KICK,
	INVITE,
	TOPIC,
	PRIVMSG,
	MODE
} t_cmd;

typedef enum e_mode{
	I_MODE,
	T_MODE,
	K_MODE,
	O_MODE,
	L_MODE
} t_mode;
