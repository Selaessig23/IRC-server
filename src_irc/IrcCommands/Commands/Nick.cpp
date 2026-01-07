#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to create or change the nickname of a certain client
 *
 * TODO:
 * (1) think about client feedback in case of success, esp. for changing nick
 * (2) think about if all other clients should get a feedback in case
 * of the client has changed his nick (maybe only as a channel notice)
 * (3) maybe choose another errmsg for pw not set yet (or just do not return anything, simply ignore)
 *
 * @return 0, in case of an error it returns error codes:
 * ERR_NOTREGISTERED (451) --> if pw was not set
 * ERR_NONICKNAMEGIVEN (431)
 * ERR_ERRONEUSNICKNAME (432)
 * ERR_NICKNAMEINUSE (433)
 *  ->chars that introduce ambiguity in other commands 
 *    (prefix, tags, channels, spaces)
 *
 * not in use: 
 * ERR_NICKCOLLISION (436) ->only relevant for multi-server setup
 */
int IrcCommands::nick(Server& base, const struct cmd_obj& cmd) {
  if (!(cmd.client->get_register_status() & PASS)) {
    send_message(base, cmd, ERR_NOTREGISTERED, cmd.client, NULL);
    return (ERR_NOTREGISTERED);
  }

  if (cmd.parameters.empty()) {
    send_message(base, cmd, ERR_NONICKNAMEGIVEN, cmd.client, NULL);
    return (ERR_NONICKNAMEGIVEN);
  }

  for (std::string::const_iterator it = cmd.parameters[0].begin();
       it != cmd.parameters[0].end(); it++) {
    if (*it == ' ' || *it == ':' || *it == '#' || *it == '&' || *it == '@') {
      send_message(base, cmd, ERR_ERRONEUSNICKNAME, cmd.client, NULL);
      return (ERR_ERRONEUSNICKNAME);
    }
  }

  for (std::list<Client>::iterator it = base._client_list.begin();
       it != base._client_list.end(); it++) {
    if (it->get_client_fd() != cmd.client->get_client_fd() &&
        !it->get_nick().empty() && it->get_nick() == cmd.parameters[0]) {
      send_message(base, cmd, ERR_NICKNAMEINUSE, cmd.client, NULL);
      return (ERR_NICKNAMEINUSE);
    }
  }

  std::string nick_old = cmd.client->get_nick();
  cmd.client->set_nick(*cmd.parameters.begin());
  cmd.client->set_register_status(NICK);
  if (nick_old.empty()) {
    //     send_message(base, cmd, RPL_INTERN_SETNICK, cmd.client, NULL);
    if (client_register_check(base, *cmd.client)) {
      send_message(base, cmd, RPL_WELCOME, cmd.client, NULL);
      send_message(base, cmd, RPL_YOURHOST, cmd.client, NULL);
      send_message(base, cmd, RPL_CREATED, cmd.client, NULL);
    }
  } else {
    //     send_message(base, cmd, RPL_INTERN_CHANGENICK, cmd.client, NULL);
  }
  return (0);
}
