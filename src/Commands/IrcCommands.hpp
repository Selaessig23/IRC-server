#ifndef IRCCOMMANDS_HPP
#define IRCCOMMANDS_HPP

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include "../Client/Client.hpp"
#include "Commands.hpp"

struct parsed_input {
  std::string prefix;
  std::string command;
  std::vector<std::string> parameters;
};

/**
 * TODO: 
 * think about if it makes sense to have the command-functions as
 * private member functions within this class
 * and making the class a friend of Server-class
 * PRO would be that the execute functin would have access to all private
 * member functin of Server class in that case
 *
 */
class IrcCommands {
 private:
//   typedef void (Commands::*function)(void);
  typedef int (*function) (const struct cmd_obj& cmd, std::list<Client>& client_list,
           int fd_curr_client, const std::string& to_check);
  static std::map<enum CMD_TYPE, function> _irc_commands;

 public:
  IrcCommands(); // inits std::map
  // OCF missing
  static int exec_command(struct cmd_obj &cmd, std::list<Client> &client_list, int fd_curr_client, const std::string &to_check);
} ;

# endif
