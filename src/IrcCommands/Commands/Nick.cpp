#include <list>
#include <string>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to confirm the client if nick-change
 * was successful and inform all members of channels the
 * client is member of about the nick change
 *
 * Prefix> <nickname_old>
 * Command: NICK
 * Parameters: <nickname_new>
 */
static void change_nick_msgs(IrcCommands cmd_obj, Server& base, Client* _client,
                             std::string& nick_old) {
  std::string out = ":" + nick_old;
  out += " NICK ";
  out += _client->get_nick();
  out += "\r\n";
  _client->add_client_out(out);
  base.set_pollevent(_client->get_client_fd(), POLLOUT);
  std::list<Client*> recipients;
  // calculate in a seperate function to also enable the use for KILL function
  if (cmd_obj.get_all_recipients(recipients, base, _client)) {
    //end
    for (std::list<Client*>::iterator it_rec = recipients.begin();
         it_rec != recipients.end(); it_rec++) {
      (*it_rec)->add_client_out(out);
      base.set_pollevent((*it_rec)->get_client_fd(), POLLOUT);
    }
  }
}

/**
 * @brief function to create or change the nickname of a certain client
 *
 * client gets a feedback in case of success, esp. for changing nick
 * all other clients of channels the nick changing client is a member of
 * get a feedback msg 
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
    if (client_register_check(base, *cmd.client)) {
      send_message(base, cmd, RPL_WELCOME, cmd.client, NULL);
      send_message(base, cmd, RPL_YOURHOST, cmd.client, NULL);
      send_message(base, cmd, RPL_CREATED, cmd.client, NULL);
    }
  } else {
    change_nick_msgs(*this, base, cmd.client, nick_old);
  }
  return (0);
}
