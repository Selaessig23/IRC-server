#include <iostream>
#include <list>
#include "../Client/Client.hpp"
#include "../includes/CONSTANTS.hpp"
#include "../includes/types.hpp"
#include "Commands.hpp"

/**
 * @brief function to check if client enters the correct password
 * while trying to connect to server 
 *
 * @return 0, in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS (461)
 * ERR_ALREADYREGISTERED (462)
 * ERR_PASSWDMISMATCH (464)
 *
 * @return it returns 1 if command and password is correct, otherwise it returns 0
 */
int Commands::pass(const struct cmd_obj& cmd, std::list<Client>& clients,
                   int fd_curr_client, const std::string& to_check) {
  std::list<Client>::iterator it = clients.begin();
  for (; it != clients.end(); it++) {
    if (it->getClientFd() == fd_curr_client)
      break;
  }
  if (it->getRegisterStatus() == 1) {
    Commands::send_message(ERR_ALREADYREGISTERED, true, *it);
    return (462);
  }
  if (cmd.parameters.empty()) {
    Commands::send_message(ERR_NEEDMOREPARAMS, true, *it);
    return (461);
  }
  if (cmd.command == PASS && cmd.parameters.size() == 1 &&
      *cmd.parameters.begin() == to_check) {
    Commands::send_message(RPL_WELCOME, false, *it);
    Commands::send_message(RPL_YOURHOST, false, *it);
    Commands::send_message(RPL_CREATED, false, *it);
    return (0);
  } else {
    Commands::send_message(ERR_PASSWDMISMATCH, true, *it);
    return (464);
  }
}

