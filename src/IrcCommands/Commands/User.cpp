#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to add all the user's information to client
 *
 * TODO:
 * (1) think about client feedback in case of success
 * (2) think about if all other clients should get a feedback in case
 * of the client has changed his nick (maybe only as a channel notice)
 *
 * @return 0, in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS
 * ERR_ALREADYREGISTRED
 */
int IrcCommands::user(Server& base, const struct cmd_obj& cmd,
                      int fd_curr_client) {
  if (cmd.parameters.empty() || cmd.parameters.size() < 4) {
    send_message(base, ERR_NEEDMOREPARAMS, true, NULL, *cmd.client);
    return (ERR_NEEDMOREPARAMS);
  }
  if (cmd.client->

  cmd.client->set_user(*cmd.parameters.begin());
  cmd.client->set_host(*cmd.parameters.begin());
  cmd.client->set_realname(*cmd.parameters.begin());
  if (nick_old.empty())
    send_message(base, RPL_INTERN_SETNICK, false, NULL, *it_client);
  else
    send_message(base, RPL_INTERN_CHANGENICK, false, &nick_old, *it_client);
  return (0);
}
