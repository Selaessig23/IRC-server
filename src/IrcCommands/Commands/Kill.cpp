#include <algorithm>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to kill a user (client) form server,
 * privilege of an operator
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

  if (cmd.parameters.empty() || cmd.parameters.size() < 1) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, true, NULL);
    return (ERR_NEEDMOREPARAMS);
  }

  std::list<Client>::iterator it_client = std::find(
      base._client_list.begin(), base._client_list.end(), cmd.parameters[0]);
  if (it_client == base._client_list.end()) {
    send_message(base, cmd, ERR_NOSUCHNICK, true, NULL);
    return (ERR_NOSUCHNICK);
  }

  base.remove_client(it_client->get_client_fd());
  return (0);
}

