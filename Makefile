# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lvichi <lvichi@student.42porto.com>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/30 19:57:48 by lvichi            #+#    #+#              #
#    Updated: 2024/04/07 17:45:12 by lvichi         ###   ########.fr          #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CPP = c++
CFLAGS = -Wall -Wextra -Werror -g -std=c++98
RM = rm -fr
FILES = src/*.cpp
OBJS = obj/*.o
INCLUDES = includes

all: $(NAME)

$(NAME): $(OBJS)
	@$(CPP) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS): $(FILES)
	@mkdir -p obj
	@cd obj && $(CPP) $(CFLAGS) -c ../$(FILES) -I ../$(INCLUDES)

clean:
	@$(RM) $(OBJS) obj

fclean:
	@$(RM) $(OBJS) $(NAME) obj

re: fclean all
