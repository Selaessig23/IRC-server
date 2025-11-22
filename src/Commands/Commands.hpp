#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <list>
#include "../Client/Client.hpp"
#include "../includes/types.hpp"

namespace Commands {
  // commands
  int pass(const struct cmd_obj& cmd, std::list<Client>& client_list,
           int fd_curr_client, const std::string& to_check);
  // helper functions
  bool client_register_check(Client& to_check);
  void send_message(int msg, bool error, Client& curr_client);
};  // namespace Commands

#endif  // Commands.hpp
