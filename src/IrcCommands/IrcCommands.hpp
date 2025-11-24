#ifndef IRCCOMMANDS_HPP
#define IRCCOMMANDS_HPP

#include <iostream>
#include <list>
#include <map>
#include <vector>
#include "../Client/Client.hpp"
// #include "../Server/Server.hpp"
#include "../includes/types.hpp"

class Server;  //forward declaration

class IrcCommands {
 private:
  typedef int (IrcCommands::*function)(Server& base, const struct cmd_obj& cmd,
                                       int fd_curr_client);

  std::map<enum CMD_TYPE, function> _irc_commands;

  // Commands
  int pass(Server& base, const struct cmd_obj& cmd, int fd_curr_client);
  // helper functions
  bool client_register_check(Server& base, Client& to_check);
  std::string get_error(Server& base, enum PARSE_ERR err);
  std::string get_rpl(Server& base, enum RPL_MSG rpl);

 public:
  IrcCommands();
  IrcCommands(const IrcCommands& other);
  IrcCommands operator=(const IrcCommands& other);
  ~IrcCommands();
  int exec_command(Server& base, struct cmd_obj& cmd, int fd_curr_client);
  void send_message(Server& base, int msg, bool error, Client& curr_client);
};

#endif
