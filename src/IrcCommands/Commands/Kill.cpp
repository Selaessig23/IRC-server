#include <algorithm>
#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to add all the user's information to client
 * it sets user information according to the parameters sent
 * Command: USER
 * Parameters: <username> <hostname> <servername> <realname>
 *
 * TODO:
 * (1) think about client feedback in case of success
 * (2) think about if not only user should be checked for error
 * ERR_ALREADYREGISTERED but also host and realname as well
 * (3) maybe choose another errmsg for pw not set yet (or just do not return anything, simply ignore)
 * (4) think about adding '~' as user was not considered my nameserver
 *
 * @return 0, in case of an error it returns error codes:
 * ERR_NOPRIVILEGES
 * ERR_NEEDMOREPARAMS
 * ERR_NOSUCHNICK
 * ERR_CANTKILLSERVER --> not in use
 */
int IrcCommands::kill(Server& base, const struct cmd_obj& cmd) {
  if (cmd.client->get_opertr() == false) {
    send_message(base, cmd, ERR_NOPRIVILEGES, true, NULL);
    return (ERR_NOPRIVILEGES);
  }

  if (cmd.parameters.empty() || cmd.parameters.size() < 4) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, true, NULL);
    return (ERR_NEEDMOREPARAMS);
  }

  std::list<Client>::iterator it_client =
      std::find(base._client_list.begin(), base._client_list.end(),
                cmd.client->get_nick());
  if (it_client == base._client_list.end()) {
    send_message(base, cmd, ERR_NOSUCHNICK, true, NULL);
    return (ERR_NOSUCHNICK);
  }

  base.remove_client(cmd.client->get_client_fd());
  return (0);
}

