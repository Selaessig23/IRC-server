#ifndef IRCCOMMANDS_HPP
#define IRCCOMMANDS_HPP

#include <iostream>
#include <list>
#include <map>
#include <vector>
#include "../Client/Client.hpp"
#include "../includes/types.hpp"

class Server;  //forward declaration

class IrcCommands {
 private:
  typedef int (IrcCommands::*function)(Server& base, const struct cmd_obj& cmd);

  std::map<std::string, function> _irc_commands;

  // Commands
  int pass(Server& base, const struct cmd_obj& cmd);
  int pong(Server& base, const struct cmd_obj& cmd);
  int nick(Server& base, const struct cmd_obj& cmd);
  int cap(Server& base, const struct cmd_obj& cmd);
  int user(Server& base, const struct cmd_obj& cmd);
  int join(Server& base, const struct cmd_obj& cmd);
  int part(Server& base, const struct cmd_obj& cmd);
  int privmsg(Server& base, const struct cmd_obj& cmd);
  int kill(Server& base, const struct cmd_obj& cmd);
  int mode(Server& base, const struct cmd_obj& cmd);
  int invite(Server& base, const struct cmd_obj& cmd);
  int kick(Server& base, const struct cmd_obj& cmd);
  // helper functions
  bool client_register_check(Server& base, Client& to_check);
  std::string get_error(Server& base, const cmd_obj& cmd, enum PARSE_ERR err);
  std::string get_rpl(Server& base, const cmd_obj& cmd, enum RPL_MSG rpl);
  int send_privmsg(Server& base, Client& sender, Client& receiver,
                   const std::string& msg, const std::string& channel);

 public:
  IrcCommands();
  IrcCommands(const IrcCommands& other);
  IrcCommands operator=(const IrcCommands& other);
  ~IrcCommands();
  int exec_command(Server& base, struct cmd_obj& cmd);
  void send_message(Server& base, const cmd_obj& cmd, int numeric_msg_code,
                    bool error, std::string* msg);
};

#endif  //IRCCOMMANDS_HPP
