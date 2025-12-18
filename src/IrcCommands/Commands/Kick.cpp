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
 * @brief KICK command is used to remove members from channels
 * Only operators of the channel can remove other members
 * KICK <channel> <user> *( "," <user> ) [<comment>]
 * 
 * TODO:
 * (1) implement message sending with [<comment>]
 *      if [<comment>] isn't passed then a default message
 * (2) implement multi-kick at a single call
 *  
 * @return 0, in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS (461)
 * ERR_NOSUCHCHANNEL (403)
 * ERR_CHANOPRIVSNEEDED (482)
 * ERR_USERNOTINCHANNEL (441)
 * ERR_NOTONCHANNEL (442)
 * 
 * @return it returns 1 if command is succesfully executed
 */
int IrcCommands::kick(Server& base, const struct cmd_obj& cmd) {
  if (cmd.parameters.empty()) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, true, NULL);
    return (ERR_NEEDMOREPARAMS);
  }
  std::list<Channel>::iterator it_chan = base._channel_list.begin();
  if (!base._channel_list.empty()) {
    for (; it_chan != base._channel_list.end(); it_chan++) {
      if (it_chan->get_name() == cmd.parameters[0])
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

  std::list<Client>::iterator it_kick_nick = base._client_list.begin();
  if (!base._client_list.empty()) {
    for (; it_kick_nick != base._client_list.end(); it_kick_nick++) {
      if (it_kick_nick->get_nick() == cmd.parameters[1])
        break;
    }
  }
  if (it_kick_nick == base._client_list.end())
    return (0);

  std::map<Client*, bool>::iterator it_kick_mem =
      it_chan->get_members().begin();
  for (; it_kick_mem != it_chan->get_members().end(); it_kick_mem++) {
    if (it_kick_mem->first == &(*it_kick_nick))
      break;
  }
  if (it_kick_mem == it_chan->get_members().end()) {
    send_message(base, cmd, ERR_USERNOTINCHANNEL, true, NULL);
    return (ERR_USERNOTINCHANNEL);
  }

  it_chan->remove_from_members(&(*it_kick_nick));
  send_message(base, cmd, RPL_INVITING, false, NULL);

  return (1);
}