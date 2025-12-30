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
 * @brief INVITE command is used to invite clients to channels
 * Only members of that channel can invite non-members
 * If invite_only (+i) mode is enabled for the channel, only chanops
 * can successfully call the command.
 * After a successful call invited client is getting added to _invited list
 * of the channel. Caller client receives RPL_INVITING and invited client
 * receives an custom INVITE message from the server.
 * e.g INVITE <nickname> <channel>
 * 
 * To comply with IRC protocol:
 * (1) If <nickname> is not existed, 0 is returned but
 * ERR_NOSUCHNICK can be returned instead.
 * (2) If client is already invited before, 0 is returned but
 * a custom message can be returned instead.
 * 
 * @returns 0 or ERR_ in case of an error:
 * ERR_NEEDMOREPARAMS (461)
 * ERR_NOSUCHCHANNEL (403)
 * ERR_CHANOPRIVSNEEDED (482)
 * ERR_USERNOTINCHANNEL (441)
 * ERR_NOTONCHANNEL (442)
 * 
 * @returns 1 in case of successful execution after
 * sending RPL_INVITING (341) to command issuer,
 * sending an INVITE message from issuer to invited client.
 */

int IrcCommands::invite(Server& base, const struct cmd_obj& cmd) {
  if (!client_register_check(base, *cmd.client)) {
    send_message(base, cmd, ERR_NOTREGISTERED, true, NULL);
    return (ERR_NOTREGISTERED);
  }
  if (cmd.parameters.size() < 2) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, true, NULL);
    return (ERR_NEEDMOREPARAMS);
  }
  std::list<Channel>::iterator it_chan = base._channel_list.begin();
  if (!base._channel_list.empty()) {
    for (; it_chan != base._channel_list.end(); it_chan++) {
      if (it_chan->get_name() == cmd.parameters[1])
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

  if (it_chan_mem->second == false) {
    send_message(base, cmd, ERR_CHANOPRIVSNEEDED, true, NULL);
    return (ERR_CHANOPRIVSNEEDED);
  }

  std::list<Client>::iterator it_inv_cli = base._client_list.begin();
  if (!base._client_list.empty()) {
    for (; it_inv_cli != base._client_list.end(); it_inv_cli++) {
      if (it_inv_cli->get_nick() == cmd.parameters[0])
        break;
    }
  }
  if (it_inv_cli == base._client_list.end())
    return (0);

  std::list<Client*>::iterator it_inv_list = it_chan->get_invited().begin();
  for (; it_inv_list != it_chan->get_invited().end(); it_inv_list++) {
    if (*it_inv_list == &(*it_inv_cli)) {
      return (0);
    }
  }

  it_chan_mem = it_chan->get_members().begin();
  for (; it_chan_mem != it_chan->get_members().end(); it_chan_mem++) {
    if (it_chan_mem->first == &(*it_inv_cli)) {
      send_message(base, cmd, ERR_USERONCHANNEL, true, NULL);
      return (ERR_USERONCHANNEL);
    }
  }

  it_chan->new_invited(&(*it_inv_cli));
  send_message(base, cmd, RPL_INVITING, false, NULL);

  std::string msg;
  msg += ":" + cmd.client->get_nick();
  msg += "!" + cmd.client->get_user();
  msg += "@" + cmd.client->get_host();
  msg += " invites " + it_inv_cli->get_nick();
  msg += " to the channel " + it_chan->get_name();
  msg += "\r\n";
  it_inv_cli->add_client_out(msg);
  base.set_pollevent(it_inv_cli->get_client_fd(), POLLOUT);

  return (1);
}