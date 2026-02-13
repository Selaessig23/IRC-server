#include <algorithm>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to kill a user (client) from server,
 * privilege of an operator
 *
 * TODO:
 * send a QUIT message to the recipient and all his friends
 * (clients the user is connected through via a channel)
 * 
 * the operator cannot KILL itself
 *
 * @return 0 if succesful, in case of an error it returns error codes:
 * ERR_NOPRIVILEGES
 * ERR_NEEDMOREPARAMS
 * ERR_NOSUCHNICK
 * ERR_CANTKILLSERVER --> not in use
 */
int IrcCommands::kill(Server& base, const struct cmd_obj& cmd) {
  if (cmd.client->get_opertr() == false) {
    send_message(base, cmd, ERR_NOPRIVILEGES, cmd.client, NULL);
    return (ERR_NOPRIVILEGES);
  }

  if (cmd.parameters.empty() || cmd.parameters.size() < 1) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, cmd.client, NULL);
    return (ERR_NEEDMOREPARAMS);
  }

  std::list<Client>::iterator it_client = std::find(
      base._client_list.begin(), base._client_list.end(), cmd.parameters[0]);
  if (it_client == base._client_list.end()) {
    send_message(base, cmd, ERR_NOSUCHNICK, cmd.client, NULL);
    return (ERR_NOSUCHNICK);
  }

  if (it_client->get_nick() == cmd.client->get_nick())
    return (0);
  //to be coded
  std::list<Client*> recipients;
  if (base._irc_commands->get_all_recipients(recipients, base, cmd.client)) {
    std::string out;
    for (std::list<Client*>::iterator it_rec = recipients.begin();
         it_rec != recipients.end(); it_rec++) {
      //send a QUIT-message (has to be created)
      if (!out.empty()) {
        (*it_rec)->add_client_out(out);
        base.set_pollevent((*it_rec)->get_client_fd(), POLLOUT);
      }
    }
  }
  //
  base.remove_client(it_client->get_client_fd());
  return (0);
}
