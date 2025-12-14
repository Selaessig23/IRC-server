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
 * @brief JOIN command is used to create/join channels
 * If the <channel> already exists and other modes requirements are met
 * caller client can join the channel.
 * If the <channel> isn't existed yet then it gets created.
 * There are various modes for the channels 
 * that works interconnectedly with JOIN command 
 * e.g. +k +i +l: has_key, invite_only, has_limit respectively
 * 
 * TODO:
 * (1) +k has_a_key check,
 * (2) +i invite_only check is,
 * (3) +l has_limit check is gonna be implemented
 * (3) implement respective ERR
 * (4) implement resprective RPL
 *  
 * @return 0, in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS (461)
 * ERR_NOSUCHCHANNEL (403)
 * ERR_BADCHANNELKEY (475)
 * ERR_CHANNELISFULL (471)
 * ERR_INVITEONLYCHAN (473)
 * RPL_TOPIC (332)
 * RPL_NAMREPLY (353)
 * RPL_ENDOFNAMES (366)
 * 
 * @return it returns 1 if command is succesfully executed
 */
int IrcCommands::join(Server& base, const struct cmd_obj& cmd) {
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
    Channel NewChannel(cmd.parameters[0]);
    base._channel_list.push_back(NewChannel);
    base._channel_list.back().new_member(cmd.client, true);
    base._channel_list.back().print_channel_info();
    return (1);
  }

  if ((it_chan->get_modes() & MODE_KEY) &&
      (!cmd.parameters[0].size() ||
       (cmd.parameters[1].size() && cmd.parameters[1] != it_chan->get_key()))) {
    send_message(base, ERR_BADCHANNELKEY, true, NULL, *it_cli);
    return (ERR_BADCHANNELKEY);
  }

  if ((it_chan->get_modes() & MODE_LIMIT) &&
      (it_chan->get_members_size() >= it_chan->get_user_limit())) {
    send_message(base, ERR_CHANNELISFULL, true, NULL, *it_cli);
    return (ERR_CHANNELISFULL);
  }

  if (it_chan->get_modes() & MODE_INVITE) {
    std::list<Client*>::iterator it_inv = it_chan->get_invited().begin();
    for (; it_inv != it_chan->get_invited().end(); it_inv++) {
      if ((*it_inv) == cmd.client) {
        it_chan->remove_from_invited(*it_inv);
        break;
      }
    }
    if (it_inv == it_chan->get_invited().end()) {
      send_message(base, ERR_INVITEONLYCHAN, true, NULL, *it_cli);
      return (ERR_INVITEONLYCHAN);
    }
  }
  it_chan->new_member(cmd.client, false);
  it_chan->print_channel_info();

  return (1);
}
