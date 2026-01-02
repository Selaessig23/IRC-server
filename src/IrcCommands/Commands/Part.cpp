#include <iostream>
#include <list>
#include "../../Channel/Channel.hpp"
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../debug.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief PART command is used to leave channels
 * After succesful call client gets removed from the <channel>
 * e.g. PART <channel>
 * 
 * TODO:
 * (1) Handling multiple channel entry at single command call
 * e.g. PART #chan,#42
 *  
 * @return 0 or in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS (461)
 * ERR_NOSUCHCHANNEL (403)
 * ERR_NOTONCHANNEL (442)
 * 
 * @return it returns 1 if command is succesfully executed
 */

int IrcCommands::part(Server& base, const struct cmd_obj& cmd) {
  // if (!client_register_check(base, *cmd.client)) {
  //   send_message(base, cmd, ERR_NOTREGISTERED, true, NULL);
  //   return (ERR_NOTREGISTERED);
  // }

  if (cmd.parameters.empty()) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, true, NULL);
    return (ERR_NEEDMOREPARAMS);
  } else if (!(cmd.parameters[0][0] == '#' || cmd.parameters[0][0] == '&'))
    return (0);

  std::list<Channel>::iterator it_chan = base._channel_list.begin();
  if (!base._channel_list.empty()) {
    for (; it_chan != base._channel_list.end(); it_chan++) {
      if (it_chan->get_name() == cmd.parameters[0])
        break;
    }
  }
  if (base._channel_list.empty() || it_chan == base._channel_list.end()) {
    send_message(base, cmd, ERR_NOSUCHCHANNEL, true, NULL);
    return (ERR_NOSUCHCHANNEL);
  }

  std::map<Client*, bool>::iterator it_chan_mem =
      it_chan->get_members().begin();
  for (; it_chan_mem != it_chan->get_members().end(); it_chan_mem++) {
    if (it_chan_mem->first == cmd.client)
      break;
  }
  if (it_chan_mem == it_chan->get_members().end()) {
    send_message(base, cmd, ERR_NOTONCHANNEL, true, NULL);
    return (ERR_NOTONCHANNEL);
  }

  it_chan->remove_from_members(cmd.client);

  std::string msg;
  msg += ":" + cmd.client->get_nick();
  msg += "!" + cmd.client->get_user();
  msg += "@" + cmd.client->get_host();
  msg += " has left the channel " + it_chan->get_name();
  if (cmd.parameters[1].size())
    msg += " :" + cmd.parameters[1];
  msg += "\r\n";
  cmd.client->add_client_out(msg);
  base.set_pollevent(cmd.client->get_client_fd(), POLLOUT);

  it_chan_mem = it_chan->get_members().begin();
  for (; it_chan_mem != it_chan->get_members().end(); it_chan_mem++) {
    it_chan_mem->first->add_client_out(msg);
    base.set_pollevent(it_chan_mem->first->get_client_fd(), POLLOUT);
  }

  if (it_chan->get_members_size() == 0)
    base._channel_list.erase(it_chan);

  return (1);
}
