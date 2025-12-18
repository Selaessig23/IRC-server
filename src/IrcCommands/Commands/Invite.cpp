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
 * can succesfully call the command.
 * After a suceesful call invited client is getting added to _invited list
 * of the channel. Caller client receives RPL_INVITING and invited client
 * receives an custom INVITE message from the server.
 * e.g INVITE <nickname> <channel>
 * 
 * TODO:
 * (1) implement and send RPL_341 to caller after a succesful call 
 * (2) implement and send an INVITE message to the invited client
 *  
 * @return 0, in case of an error it returns error codes:
 * RPL_INVITING (341)
 * ERR_NEEDMOREPARAMS (461)
 * ERR_NOSUCHCHANNEL (403)
 * ERR_CHANOPRIVSNEEDED (482)
 * ERR_USERNOTINCHANNEL (441)
 * ERR_NOTONCHANNEL (442)
 * 
 * @return it returns 1 if command is succesfully executed
 */
int IrcCommands::invite(Server& base, const struct cmd_obj& cmd) {
  if (cmd.parameters.empty()) {
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
  if (it_chan == base._channel_list.end() || base._channel_list.empty()) {
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

  return (1);
}