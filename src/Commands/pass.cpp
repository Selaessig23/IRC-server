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
    Commands::ft_errorprint(ERR_ALREADYREGISTERED, *it);
    return (462);
  }
  if (cmd.parameters.empty()) {
    Commands::ft_errorprint(ERR_NEEDMOREPARAMS, *it);
    return (461);
  }
  if (cmd.command == PASS && cmd.parameters.size() == 1 &&
      *cmd.parameters.begin() == to_check) {
    it->setClientOut(MSG_WELCOME);
    it->addClientOut(MSG_WAITING);
    return (0);
  } else {
    Commands::ft_errorprint(ERR_PASSWDMISMATCH, *it);
    return (464);
  }
}

