# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mstracke <mstracke@student.42berlin.de>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/09 13:50:36 by mstracke          #+#    #+#              #
#    Updated: 2025/11/20 12:55:53 by mstracke         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
CXX = c++
CXXFLAGS =  -Wall -Werror -Wextra -std=c++98 -Isrc -Iinclude
# -MMD: Generate a .d file listing only your project headers (the ones you #include with quotes or via your -I paths), excluding system headers.
# -MP: Appends dummy rules for every dependency in the .d file. Prevents “No rule to make target …” errors if a header (or .tpp) gets deleted. (maybe not necessary)
DEPFLAGS =	-MMD -MP
# CXXFLAGS += -Wshadow 
# CXXFLAGS += -Wconversion -Wunreachable-code
# CXXFLAGS += -Wno-shadow
# CXXFLAGS += -pedantic
# -O0: Disables optimization to make debugging easier..
# CXXFLAGS += -g3 -O0

#Arguments to test:
ARGS = "8080" "1234abc?"

#styling
RESET =			\033[0m
BOLD =			\033[1m
GREEN =			\033[32m
YELLOW =		\033[33m
RED :=			\033[91m

#sources
SRCS =	main.cpp 
SRCS += Server/Server.cpp
SRCS += Client/Client.cpp
SRCS += Parser/Parser.cpp
SRCS += Commands/pass.cpp
SRCS += Commands/Command_utils.cpp

OBJS = $(SRCS:%.cpp=obj/%.o)

DEPS :=	$(OBJS:.o=.d)

LiBS =
LIBS += src/includes/types.hpp
LIBS += src/includes/CONSTANTS.hpp

all:$(NAME)

$(NAME): $(OBJS) $(LIBS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)
		@echo "-- prog created, try it by using ./ircserv <portno> <password>"

obj/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

# Include generated dependency files
-include $(DEPS)

clean:
	@rm -rf obj
	@echo -- Deleting all .o and .dep files

fclean: clean
	@rm -f $(NAME)
	@echo -- Deleting executables

re: fclean all

run: all
	@echo
	@PATH=".$${PATH:+:$${PATH}}" && $(NAME) $(ARGS)

valrun: all
	@echo
	@PATH=".$${PATH:+:$${PATH}}" && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes $(NAME) $(ARGS)

debug: CXXFLAGS += -DDEBUG -g
debug: fclean $(NAME)
	@echo "$(BOLD)$(YELLOW)Debug build complete.$(RESET)"

.PHONY: all clean fclean re run valrun debug 
