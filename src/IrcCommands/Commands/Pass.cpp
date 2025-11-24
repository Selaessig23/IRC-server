#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

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
int IrcCommands::pass(Server& base, const struct cmd_obj& cmd,
                      int fd_curr_client) {
  std::list<Client>::iterator it = base._client_list.begin();
  for (; it != base._client_list.end(); it++) {
    if (it->getClientFd() == fd_curr_client)
      break;
  }
  if (it->getRegisterStatus() == 1) {
    send_message(base, ERR_ALREADYREGISTERED, true, *it);
    return (462);
  }
  if (cmd.parameters.empty()) {
    send_message(base, ERR_NEEDMOREPARAMS, true, *it);
    return (461);
  }
  if (cmd.command == PASS && cmd.parameters.size() == 1 &&
      *cmd.parameters.begin() == base._pw) {
    send_message(base, RPL_WELCOME, false, *it);
    send_message(base, RPL_YOURHOST, false, *it);
    send_message(base, RPL_CREATED, false, *it);
    return (0);
  } else {
    send_message(base, ERR_PASSWDMISMATCH, true, *it);
    return (464);
  }
}

