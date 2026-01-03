#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief is used by a normal user to obtain IRC operator privileges.
 * Parameters: <name> <password>
 * 
 * only the swearbot is allowed to act as an operator
 * <name> has to be SWEARBOT 
 * <password> has to be server password + 42BOT
 *
 * if client has already been operator before: nothing happens
 *  
 * @return 0, in case of an error it returns error codes:
 * ERR_NOTREGISTERED (451)
 * ERR_NEEDMOREPARAMS (461)
 * ERR_PASSWDMISMATCH (464)
 * ERR_NOOPERHOST (491) --> not in use
 */
int IrcCommands::oper(Server& base, const struct cmd_obj& cmd) {
    if (!client_register_check(base, *cmd.client)) {
    send_message(base, cmd, ERR_NOTREGISTERED, true, NULL);
    return (ERR_NOTREGISTERED);
  }

  if (cmd.parameters.size() < 2) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, true, NULL);
    return (ERR_NEEDMOREPARAMS);
  }
  if (cmd.parameters.size() >= 2 &&
      *cmd.parameters.begin() == ("SWEARBOT") &&
      *(cmd.parameters.begin() + 1) == (base._pw + "42BOT")) {
    cmd.client->set_opertr(true);
    send_message(base, cmd, RPL_YOUREOPER, false, NULL);
    return (0);
  } else {
    send_message(base, cmd, ERR_PASSWDMISMATCH, true, NULL);
    return (ERR_PASSWDMISMATCH);
  }
}
