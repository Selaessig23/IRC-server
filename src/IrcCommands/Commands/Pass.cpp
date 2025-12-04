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
 */
int IrcCommands::pass(Server& base, const struct cmd_obj& cmd,
                      int fd_curr_client) {
  std::list<Client>::iterator it = base._client_list.begin();
  for (; it != base._client_list.end(); it++) {
    if (it->get_client_fd() == fd_curr_client)
      break;
  }
  if (cmd.client->get_register_status() & PASS) {
    send_message(base, ERR_ALREADYREGISTERED, true, NULL, *it);
    return (ERR_ALREADYREGISTERED);
  }
  if (cmd.parameters.empty()) {
    send_message(base, ERR_NEEDMOREPARAMS, true, NULL, *it);
    return (ERR_NEEDMOREPARAMS);
  }
  if (cmd.parameters.size() == 1 &&
      *cmd.parameters.begin() == base._pw) {
    it->set_register_status(PASS);
    return (0);
  } else {
    send_message(base, ERR_PASSWDMISMATCH, true, NULL, *it);
    return (ERR_PASSWDMISMATCH);
  }
}
