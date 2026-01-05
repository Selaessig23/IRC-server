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
  * @brief function that structures KICK message and sends it to the target client.
  */
void IrcCommands::send_kick_message(Server& base, const struct cmd_obj& cmd,
                                    Client* target, Channel* chan) {
  std::string msg;
  msg += ":" + cmd.client->get_nick();
  msg += "!" + cmd.client->get_user();
  msg += "@" + cmd.client->get_host();
  msg += " kicked " + target->get_nick();
  msg += " from " + chan->get_name();
  if (cmd.parameters.size() > 2)
    msg += ". Reason: " + cmd.parameters[2];
  msg += "\r\n";
  target->add_client_out(msg);
  base.set_pollevent(target->get_client_fd(), POLLOUT);
}

/**
 * @brief KICK command is used to remove members from channels
 * Only operaators of the channel can remove other members
 * KICK <channel> <user> *( "," <user> ) [<comment>]
 * 
 * TODO:
 * (1) implement multi-kick at a single call
 *  
 * @return 0, in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS (461)
 * ERR_NOSUCHCHANNEL (403)
 * ERR_CHANOPRIVSNEEDED (482)
 * ERR_USERNOTINCHANNEL (441)
 * ERR_NOTONCHANNEL (442)
 * 
 * @return it returns 1 if command is successfully executed
 */
int IrcCommands::kick(Server& base, const struct cmd_obj& cmd) {
  if (!client_register_check(base, *cmd.client)) {
    send_message(base, cmd, ERR_NOTREGISTERED, cmd.client, NULL);
    return (ERR_NOTREGISTERED);
  }

  if (cmd.parameters.size() < 2) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, cmd.client, NULL);
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
    send_message(base, cmd, ERR_NOSUCHCHANNEL, cmd.client, NULL);
    return (ERR_NOSUCHCHANNEL);
  }

  std::map<Client*, bool>::iterator it_chan_mem =
      it_chan->get_members().begin();
  for (; it_chan_mem != it_chan->get_members().end(); it_chan_mem++) {
    if (it_chan_mem->first == cmd.client)
      break;
  }
  if (it_chan_mem == it_chan->get_members().end()) {
    send_message(base, cmd, ERR_NOTONCHANNEL, cmd.client, &(*it_chan));
    return (ERR_NOTONCHANNEL);
  }

  if (it_chan_mem->second == false) {
    send_message(base, cmd, ERR_CHANOPRIVSNEEDED, cmd.client, &(*it_chan));
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
    send_message(base, cmd, ERR_USERNOTINCHANNEL, cmd.client, &(*it_chan));
    return (ERR_USERNOTINCHANNEL);
  }

  it_chan->remove_from_members(&(*it_kick_nick));

  send_kick_message(base, cmd, cmd.client, &(*it_chan));
  send_kick_message(base, cmd, it_kick_mem->first, &(*it_chan));

  return (1);
}