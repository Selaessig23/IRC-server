NAME_IRC = ircserv
NAME_BOT = bot
CXX = c++
CXXFLAGS =  -Wall -Werror -Wextra -std=c++98 -Isrc_irc -Isrc_bot -Iinclude
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
ARGS_IRC = "8080" "1234abc?"
ARGS_BOT = "8080" "1234abc?" "swear_words.txt"

#styling
RESET =			\033[0m
BOLD =			\033[1m
GREEN =			\033[32m
YELLOW =		\033[33m
RED :=			\033[91m

#sources
SRCS_IRC =	src_irc/main.cpp
SRCS_IRC += src_irc/Utils/Utils.cpp
SRCS_IRC += src_irc/Server/Server.cpp
SRCS_IRC += src_irc/Server/ServerPoll.cpp
SRCS_IRC += src_irc/Server/ServerUtils.cpp
SRCS_IRC += src_irc/Client/Client.cpp
SRCS_IRC += src_irc/Channel/Channel.cpp
SRCS_IRC += src_irc/Parser/Parser.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Join.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Part.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Nick.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Mode.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Invite.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Kick.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Kill.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Pass.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Pong.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Cap.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/User.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Privmsg.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Oper.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Topic.cpp
SRCS_IRC += src_irc/IrcCommands/IrcCommandsUtils.cpp
SRCS_IRC += src_irc/IrcCommands/IrcCommands.cpp

#sources bot
SRCS_BOT = src_bot/main.cpp
SRCS_BOT += src_bot/Bot/Bot.cpp
SRCS_BOT += src_bot/Bot/BotCommands.cpp
SRCS_BOT += src_bot/Parser/Parser.cpp
SRCS_BOT += src_bot/Channel/Channel.cpp
SRCS_BOT += src_bot/Utils/Utils.cpp

OBJS_IRC = $(SRCS_IRC:src_irc/%.cpp=obj_irc/%.o)

OBJS_BOT = $(SRCS_BOT:src_bot/%.cpp=obj_bot/%.o)

DEPS_IRC := $(OBJS_IRC:.o=.d)

DEPS_BOT := $(OBJS_BOT:.o=.d)

LIBS = src_irc/includes/types.hpp
LIBS += src_irc/includes/CONSTANTS.hpp

LIBS_BOT = src_bot/includes/types.hpp
LIBS_BOT += src_bot/includes/CONSTANTS.hpp

irc: $(NAME_IRC)

$(NAME_IRC): $(OBJS_IRC) $(LIBS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS_IRC)
	@echo "-- irc prog created, try it by using ./ircserv <portno> <password>"

obj_irc/%.o: src_irc/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

bot: $(NAME_BOT)

$(NAME_BOT): $(OBJS_BOT) $(LIBS_BOT)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS_BOT)
	@echo "-- bot prog created, try it by using ./bot <portno> <password> <path of swear-word-file>"

obj_bot/%.o: src_bot/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

# Include generated dependency files
-include $(DEPS_IRC)

# Include generated dependency files
-include $(DEPS_BOT)

clean:
	@rm -rf obj_irc
	@rm -rf obj_bot
	@echo -- Deleting all .o and .dep files

cleanirc:
	@rm -rf obj_irc
	@echo -- Deleting all irc .o and .dep files

cleanbot:
	@rm -rf obj_bot
	@echo -- Deleting all bot .o and .dep files

fclean: clean
	@rm -f $(NAME_IRC)
	@rm -f $(NAME_BOT)
	@echo -- Deleting executables

fcleanirc: cleanirc
	@rm -f $(NAME_IRC)
	@echo -- Deleting irc executables

fcleanbot: cleanbot
	@rm -f $(NAME_BOT)
	@echo -- Deleting bot executables

re: fclean all

all: irc 

bonus: irc bot

runirc: irc 
	@echo
	@echo "Running irc-server"
	@PATH=".$${PATH:+:$${PATH}}" && $(NAME_IRC) $(ARGS_IRC)

runbot: bot
	@echo
	@echo "Running bot-server"
	@PATH=".$${PATH:+:$${PATH}}" && $(NAME_BOT) $(ARGS_BOT)

# to run both program simultanously
runbonus: bonus
	@echo
	@echo "Running irc-server and bot"
	@PATH=".$${PATH:+:$${PATH}}" && $(NAME_IRC) $(ARGS_IRC) & \
	PATH=".$${PATH:+:$${PATH}}" && $(NAME_BOT) $(ARGS_BOT) & \
	wait

valrunirc: all
	@echo
	@echo "Running irc-server with valgrind"
	@PATH=".$${PATH:+:$${PATH}}" && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes $(NAME_IRC) $(ARGS_IRC)

valrunbot: bot
	@echo
	@echo "Running bot with valgrind"
	@PATH=".$${PATH:+:$${PATH}}" && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes $(NAME_BOT) $(ARGS_BOT)

debugirc: CXXFLAGS += -DDEBUG -g
debugirc: fcleanirc $(NAME_IRC)
	@echo "$(BOLD)$(YELLOW)Debug build IRC complete.$(RESET)"

debugbot: CXXFLAGS += -DDEBUG -g
debugbot: fcleanbot $(NAME_BOT)
	@echo "$(BOLD)$(YELLOW)Debug build BOT complete.$(RESET)"

debugvalrunirc: CXXFLAGS += -DDEBUG -g
debugvalrunirc: fcleanirc $(NAME_IRC)
	@echo "$(BOLD)$(YELLOW)Debug build IRC complete.$(RESET)"
	@echo
	@echo "Running irc-server with valgrind"
	@PATH=".$${PATH:+:$${PATH}}" && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes $(NAME_IRC) $(ARGS_IRC)

debugvalrunbot: CXXFLAGS += -DDEBUG -g
debugvalrunbot: fcleanbot $(NAME_BOT)
	@echo "$(BOLD)$(YELLOW)Debug build BOT complete.$(RESET)"
	@echo
	@echo "Running BOT with valgrind"
	@PATH=".$${PATH:+:$${PATH}}" && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes $(NAME_BOT) $(ARGS_BOT)

debugvalrunbonus: CXXFLAGS += -DDEBUG -g
debugvalrunbonus: fclean bonus
	@echo "$(BOLD)$(YELLOW)Debug build complete.$(RESET)"
	@echo
	@echo "Running irc-server and bot"
	@PATH=".$${PATH:+:$${PATH}}" && $(NAME_IRC) $(ARGS_IRC) & \
	PATH=".$${PATH:+:$${PATH}}" && $(NAME_BOT) $(ARGS_BOT) & \
	wait


.PHONY: all clean fclean re run valrun debug 
