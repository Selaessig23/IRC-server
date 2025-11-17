#ifndef IRCCOMMANDS_HPP
#define IRCCOMMANDS_HPP

#include <iostream>
#include <list>
#include <vector>
#include "IrcCommands.hpp"
#include "include/Client.hpp"

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
 * @return it returns 0 if command is ready to execute (no syntax errors), otherwise it 
 * returns 1
 */
class IrcCommandParsing {
 private:
  typedef void (IrcCommands::*function)(void);
//   function irccommands[2];
  std::vector<function> irc_commands;

 public:
  static int exec_command(struct parsed_input& to_check,
                          std::list<Client>& cient_list);
};

#endif
