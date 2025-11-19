#ifndef IRCCOMMANDS_HPP
#define IRCCOMMANDS_HPP

#include <iostream>
#include <list>
#include <vector>
#include "Client/Client.hpp"

struct parsed_input {
  std::string prefix;
  std::string command;
  std::vector<std::string> parameters;
};

/**
 * @brief class to check the parsed input of client for command
 * (1) it checks which command is enquired by client
 * (2) it runs the commands for parsing
 *
 * TODO: think about if it makes sense to have the command-functions as
 * private member functions or a vector of functions that refer to function pointers 
 * of a namespace called IrcCommands
 *
 */
class IrcCommands {
 private:
  typedef void (IrcCommands::*function)(void);
  std::map<std::string, function> irc_commands;

  //commands
  int PASS(struct parsed_input& input, std::string& to_compare) const;

 public:
  IrcCommands(); // inits std::map
  int exec_command(struct parsed_input& to_check,
                          std::list<Client>& cient_list) const;
  int check_pw(struct parsed_input& input,
                      std::string& to_compare) const; //if we want to seperate this check
};

#endif
