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
 * (4) think about adding '~' as user was not considered by name server
 *
 * @return 0, in case of an error it returns error codes:
 *    ERR_NEEDMOREPARAMS
 *    ERR_ALREADYREGISTERED
 */
int IrcCommands::user(Server& base, const struct cmd_obj& cmd) {
  if (cmd.parameters.empty() || cmd.parameters.size() < 4) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, cmd.client, NULL);
    return (ERR_NEEDMOREPARAMS);
  }

  if (!cmd.client->get_user().empty()) {
    send_message(base, cmd, ERR_ALREADYREGISTERED, cmd.client, NULL);
    return (ERR_ALREADYREGISTERED);
  }

  cmd.client->set_register_status(USER);

  std::vector<std::string>::const_iterator it = cmd.parameters.begin();
  cmd.client->set_user(*it);
  it++;
  cmd.client->set_host(*it);
  it++;
  cmd.client->set_servername(*it);
  it++;
  cmd.client->set_realname(*it);
  //   send_message(base, cmd, RPL_INTERN_SETUSER, cmd.client, NULL);

  if (client_register_check(base, *cmd.client)) {
    send_message(base, cmd, RPL_WELCOME, cmd.client, NULL);
    send_message(base, cmd, RPL_YOURHOST, cmd.client, NULL);
    send_message(base, cmd, RPL_CREATED, cmd.client, NULL);
  }
  return (0);
}
