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
 *
 * @return 0, in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS
 * ERR_ALREADYREGISTERED
 */
int IrcCommands::user(Server& base, const struct cmd_obj& cmd,
                      int fd_curr_client) {
  (void)fd_curr_client;
  if (cmd.parameters.empty() || cmd.parameters.size() < 4) {
    send_message(base, ERR_NEEDMOREPARAMS, true, NULL, *cmd.client);
    return (ERR_NEEDMOREPARAMS);
  }
  else if (!cmd.client->get_user().empty()) {
    send_message(base, ERR_ALREADYREGISTERED, true, NULL, *cmd.client);
    return (ERR_ALREADYREGISTERED);
  }

  std::vector<std::string>::const_iterator it = cmd.parameters.begin();
  cmd.client->set_user("~" + *it);
  it++;
  cmd.client->set_host(*it);
  it++;
  cmd.client->set_realname(*cmd.parameters.begin());
  send_message(base, RPL_INTERN_SETUSER, false, NULL, *cmd.client);
  return (0);
}
