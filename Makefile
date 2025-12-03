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
SRCS += Server/ServerUtils.cpp
SRCS += Server/Server.cpp
SRCS += Client/Client.cpp
SRCS += Channel/Channel.cpp
SRCS += Parser/Parser.cpp
SRCS += IrcCommands/Commands/Pass.cpp
SRCS += IrcCommands/Commands/Pong.cpp
SRCS += IrcCommands/Commands/Nick.cpp
SRCS += IrcCommands/Commands/Cap.cpp
SRCS += IrcCommands/Commands/User.cpp
SRCS += IrcCommands/Commands/Privmsg.cpp
SRCS += IrcCommands/IrcCommandsUtils.cpp
SRCS += IrcCommands/IrcCommands.cpp

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

debugvalrun: CXXFLAGS += -DDEBUG -g
debugvalrun: fclean $(NAME)
	@echo "$(BOLD)$(YELLOW)Debug build complete.$(RESET)"
	@echo
	@PATH=".$${PATH:+:$${PATH}}" && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes $(NAME) $(ARGS)


.PHONY: all clean fclean re run valrun debug 
