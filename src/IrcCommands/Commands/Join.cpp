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
 * @brief join_o is a helper function of JOIN command that executes
 * the special argument of ("0", 0x30). 
 * It makes the issuer client to leave all channels it's a member of 
 * as if it issued PART command for each of them.
 */

int join_0(Server& base, const struct cmd_obj& cmd, IrcCommands& self) {
  std::list<Channel>& chan_list = base.get_channel_list();
  if (!chan_list.empty()) {
    for (std::list<Channel>::iterator it_chan = chan_list.begin();
         it_chan != chan_list.end();) {
      if (it_chan->get_members().find(cmd.client) !=
          it_chan->get_members().end()) {
        cmd_obj tmp_cmd;
        tmp_cmd.command = "PART";
        tmp_cmd.parameters.push_back(it_chan->get_name());
        tmp_cmd.client = cmd.client;
        it_chan++;
        self.exec_command(base, tmp_cmd);
        continue;
      }
      it_chan++;
    }
  }
  return (1);
}

/**
 * @brief JOIN command is used to create/join channels
 * If the <channel> already exists and other requirements are met
 * caller client can join the channel.
 * If the <channel> doesn't exist then it gets created.
 * There are various modes for the channels 
 * that works interconnectedly with JOIN command 
 * e.g. +k +i +l: has_key, invite_only, has_limit respectively
 * 
 * TODO:
 * (1) implement a successful joining message
 * (2) implement resprective RPL messages
 * (3) multi-channel entry at single command call
 * Parameters: <channel>{,<channel>} [<key>{,<key>}]
 * e.g. JOIN #chan,#42,#horizon 
 * (4) JOIN "0" to remove client from all channels at once
 *  
 * @return 0, in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS (461)
 * ERR_NOSUCHCHANNEL (403)
 * ERR_BADCHANNELKEY (475)
 * ERR_CHANNELISFULL (471)
 * ERR_INVITEONLYCHAN (473)
 * 
 * RPL_TOPIC (332)
 * RPL_NAMREPLY (353)
 * RPL_ENDOFNAMES (366)
 * 
 * @return it returns 1 if command is succesfully executed
 */
int IrcCommands::join(Server& base, const struct cmd_obj& cmd) {
  if (!client_register_check(base, *cmd.client)) {
    send_message(base, cmd, ERR_NOTREGISTERED, true, NULL);
    return (ERR_NOTREGISTERED);
  }

  if (cmd.parameters.empty()) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, true, NULL);
    return (ERR_NEEDMOREPARAMS);
  } else if (cmd.parameters[0] == "0")
    return (join_0(base, cmd, *this));
  else if (!(cmd.parameters[0][0] == '#' || cmd.parameters[0][0] == '&'))
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

  std::map<Client*, bool>::iterator it_mem = it_chan->get_members().begin();
  for (; it_mem != it_chan->get_members().end(); it_mem++) {
    if (cmd.client == it_mem->first)
      return (0);
  }

  if ((it_chan->get_modes() & MODE_KEY) &&
      (cmd.parameters.size() < 2 || cmd.parameters[1] != it_chan->get_key())) {
    send_message(base, cmd, ERR_BADCHANNELKEY, true, NULL);
    return (ERR_BADCHANNELKEY);
  }

  if ((it_chan->get_modes() & MODE_LIMIT) &&
      (it_chan->get_members_size() >= it_chan->get_user_limit())) {
    send_message(base, cmd, ERR_CHANNELISFULL, true, NULL);
    return (ERR_CHANNELISFULL);
  }

  std::list<Client*>::iterator it_inv_list = it_chan->get_invited().begin();
  for (; it_inv_list != it_chan->get_invited().end(); it_inv_list++) {
    if (*it_inv_list == cmd.client) {
      it_chan->remove_from_invited(*it_inv_list);
      break;
    }
  }
  if (it_chan->get_modes() & MODE_INVITE &&
      it_inv_list == it_chan->get_invited().end()) {
    send_message(base, cmd, ERR_INVITEONLYCHAN, true, NULL);
    return (ERR_INVITEONLYCHAN);
  }

  it_chan->new_member(cmd.client, false);
  it_chan->print_channel_info();

  return (1);
}
