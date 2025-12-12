NAME_IRC = ircserv
NAME_BONUS = bot
CXX = c++
CXXFLAGS =  -Wall -Werror -Wextra -std=c++98 -Isrc -Isrc_bonus -Iinclude
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
ARGS_IRS = "8080" "1234abc?"
ARGS_BOT = "localhost" "8080" "1234abc?" "swear_words.csv"

#styling
RESET =			\033[0m
BOLD =			\033[1m
GREEN =			\033[32m
YELLOW =		\033[33m
RED :=			\033[91m

#sources irc
SRCS_IRC = src_irc/main.cpp 
SRCS_IRC += src_irc/Server/ServerUtils.cpp
SRCS_IRC += src_irc/Server/Server.cpp
SRCS_IRC += src_irc/Client/Client.cpp
SRCS_IRC += src_irc/Channel/Channel.cpp
SRCS_IRC += src_irc/Parser/Parser.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Join.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Pass.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Pong.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Nick.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Cap.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/User.cpp
SRCS_IRC += src_irc/IrcCommands/Commands/Privmsg.cpp
SRCS_IRC += src_irc/IrcCommands/IrcCommandsUtils.cpp
SRCS_IRC += src_irc/IrcCommands/IrcCommands.cpp

#sources bot
SRCS_BOT = src_bot/main.cpp

OBJS_IRC = $(SRCS_IRC:%.cpp=obj_irc/%.o)

OBJS_BOT= $(SRCS_BOT:%.cpp=obj_bot/%.o)

DEPS :=	$(OBJS:.o=.d)

DEPS_BONUS := $(OBJS_BONUS:.o=.d)

LiBS =
LIBS += src/includes/types.hpp
LIBS += src/includes/CONSTANTS.hpp

LiBS_BOT =
LIBS_BOT += src/includes/types.hpp
LIBS_BOT += src/includes/CONSTANTS.hpp

all:$(NAME_IRC)

$(NAME_IRC): $(OBJS_IRC) $(LIBS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS_IRC)
		@echo "-- irc prog created, try it by using ./ircserv <portno> <password>"

obj_irc/%.o: src_irc/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

bot: $(NAME_BOT)

$(NAME_BOT): $(OBJS_BOT) $(LIBS_BOT)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS_BOT)
		@echo "-- bot prog created, try it by using ./bot>"

obj_bot/%.o: src_bot/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

# Include generated dependency files
-include $(DEPS)

# Include generated dependency files
-include $(DEPS_BONUS)

clean:
	@rm -rf obj_irc
	@rm -rf obj_bot
	@echo -- Deleting all .o and .dep files

fclean: clean
	@rm -f $(NAME_IRC)
	@rm -f $(NAME_BOT)
	@echo -- Deleting executables

re: fclean all

bonus: all bot

run: all
	@echo
	@PATH=".$${PATH:+:$${PATH}}" && $(NAME_IRC) $(ARGS)

# to run both program simultanously
runbonus: bonus
	@echo
	@echo "Running irc-server and bot"
	@PATH=".$${PATH:+:$${PATH}}" && $(NAME_IRC) $(ARGS) & \
	PATH=".$${PATH:+:$${PATH}}" && $(NAME_BOT) & \
	wait

valrun: all
	@echo
	@PATH=".$${PATH:+:$${PATH}}" && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes $(NAME_IRC) $(ARGS)

debug: CXXFLAGS += -DDEBUG -g
debug: fclean $(NAME_IRC)
	@echo "$(BOLD)$(YELLOW)Debug build complete.$(RESET)"

debugvalrun: CXXFLAGS += -DDEBUG -g
debugvalrun: fclean $(NAME_IRC)
	@echo "$(BOLD)$(YELLOW)Debug build complete.$(RESET)"
	@echo
	@PATH=".$${PATH:+:$${PATH}}" && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes $(NAME_IRC) $(ARGS)


.PHONY: all clean fclean re run valrun debug 
