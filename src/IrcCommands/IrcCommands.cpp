#include "IrcCommands.hpp"
#include <algorithm>  // for std::swap
#include <map>
#include "../Server/Server.hpp"
#include "../includes/types.hpp"

IrcCommands::IrcCommands() {
  _irc_commands.insert(std::make_pair("PASS", &IrcCommands::pass));
  _irc_commands.insert(std::make_pair("PING", &IrcCommands::pong));
  _irc_commands.insert(std::make_pair("NICK", &IrcCommands::nick));
  _irc_commands.insert(std::make_pair("CAP", &IrcCommands::cap));
  _irc_commands.insert(std::make_pair("USER", &IrcCommands::user));
  _irc_commands.insert(std::make_pair("JOIN", &IrcCommands::join));
  _irc_commands.insert(std::make_pair("PART", &IrcCommands::part));
  _irc_commands.insert(std::make_pair("PRIVMSG", &IrcCommands::privmsg));
  _irc_commands.insert(std::make_pair("KILL", &IrcCommands::kill));
  _irc_commands.insert(std::make_pair("MODE", &IrcCommands::mode));
  _irc_commands.insert(std::make_pair("INVITE", &IrcCommands::invite));
  _irc_commands.insert(std::make_pair("KICK", &IrcCommands::kick));
  _irc_commands.insert(std::make_pair("OPER", &IrcCommands::oper));
  _irc_commands.insert(std::make_pair("TOPIC", &IrcCommands::topic));
}

IrcCommands::IrcCommands(const IrcCommands& other)
    : _irc_commands(other._irc_commands) {}

IrcCommands IrcCommands::operator=(const IrcCommands& other) {
  IrcCommands temp(other);
  std::swap(*this, temp);
  return (*this);
}

IrcCommands::~IrcCommands() {}

/**
 * @brief class to check the parsed input of client for command
 * (1) it checks which command is enquired by client
 * (2) it runs the commands for parsing
 *
 * @return function returns 1 if command was not found
 */
int IrcCommands::exec_command(Server& base, struct cmd_obj& cmd) {
  function to_execute;
  std::map<std::string, function>::iterator it =
      _irc_commands.find(cmd.command);
  if (it == _irc_commands.end()) {
    send_message(base, cmd, ERR_UNKNOWNCOMMAND, cmd.client, NULL);
    return (ERR_UNKNOWNCOMMAND);
  } else
    to_execute = it->second;
  (this->*to_execute)(base, cmd);
  return (0);
}

