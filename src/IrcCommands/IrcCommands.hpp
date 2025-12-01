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
  typedef int (IrcCommands::*function)(Server& base, const struct cmd_obj& cmd,
                                       int fd_curr_client);

  std::map<std::string, function> _irc_commands;

  // Commands
  int pass(Server& base, const struct cmd_obj& cmd, int fd_curr_client);
  int pong(Server& base, const struct cmd_obj& cmd, int fd_curr_client);
  int nick(Server& base, const struct cmd_obj& cmd, int fd_curr_client);
  int user(Server& base, const struct cmd_obj& cmd, int fd_curr_client);
  int privmsg(Server& base, const struct cmd_obj& cmd, int fd_curr_client);
  // helper functions
  bool client_register_check(Server& base, Client& to_check);
  std::string get_error(Server& base, enum PARSE_ERR err);
  std::string get_rpl(Server& base, enum RPL_MSG rpl);
  int send_privmsg(Server &base, Client& sender, Client& receiver, const std::string& msg,
                        const std::string& channel);

 public:
  IrcCommands();
  IrcCommands(const IrcCommands& other);
  IrcCommands operator=(const IrcCommands& other);
  ~IrcCommands();
  int exec_command(Server& base, struct cmd_obj& cmd, int fd_curr_client);
  void send_message(Server& base, int numeric_msg_code, bool error,
                    std::string* msg, Client& curr_client);
};

#endif  //IRCCOMMANDS_HPP
