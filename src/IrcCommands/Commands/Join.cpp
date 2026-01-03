#include <iostream>
#include <list>
#include <sstream>
#include "../../Channel/Channel.hpp"
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../debug.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * TODO:
 * (1) All message sending functionalities in the file works now 
 * but are going to be adapted to last send_message() version
 */

/**
  * @brief function that structures JOIN reply message and sends it to the target client.
  *  It is called from new_channel and join_existing_channel functions 
  */
void IrcCommands::send_join_message(Server& base, const struct cmd_obj& cmd,
                                    Client* target, Channel* chan) {
  std::string msg;
  msg += ":" + cmd.client->get_nick();
  msg += "!" + cmd.client->get_user();
  msg += "@" + cmd.client->get_host();
  msg += " has joined the " + chan->get_name();
  msg += "\r\n";
  target->add_client_out(msg);
  base.set_pollevent(target->get_client_fd(), POLLOUT);
}

/**
 * @brief function that add cmd.client to the <channel> after checking if
 * MODE requirements of chnannel are set and met
 * e.g. MODE_KEY (+k), MODE_INVITE (+i), MODE_LIMIT (+l)
 */
int IrcCommands::join_existing_channel(Server& base, const struct cmd_obj& cmd,
                                       Channel* chan, std::string cmd_key) {
  std::map<Client*, bool>::iterator it_chan_mem = chan->get_members().begin();
  for (; it_chan_mem != chan->get_members().end(); it_chan_mem++) {
    if (cmd.client == it_chan_mem->first)
      return (0);
  }

  if ((chan->get_modes() & MODE_KEY) &&
      (cmd_key.empty() || cmd_key != chan->get_key())) {
    send_message(base, cmd, ERR_BADCHANNELKEY, true, NULL);
    return (ERR_BADCHANNELKEY);
  }

  if ((chan->get_modes() & MODE_LIMIT) &&
      (chan->get_members_size() >= chan->get_user_limit())) {
    send_message(base, cmd, ERR_CHANNELISFULL, true, NULL);
    return (ERR_CHANNELISFULL);
  }

  std::list<Client*>::iterator it_inv_list = chan->get_invited().begin();
  for (; it_inv_list != chan->get_invited().end(); it_inv_list++) {
    if (*it_inv_list == cmd.client) {
      chan->remove_from_invited(*it_inv_list);
      break;
    }
  }
  if (chan->get_modes() & MODE_INVITE &&
      it_inv_list == chan->get_invited().end()) {
    send_message(base, cmd, ERR_INVITEONLYCHAN, true, NULL);
    return (ERR_INVITEONLYCHAN);
  }

  chan->new_member(cmd.client, false);
  for (it_chan_mem = chan->get_members().begin();
       it_chan_mem != chan->get_members().end(); it_chan_mem++)
    send_join_message(base, cmd, it_chan_mem->first, chan);
  if (chan->get_topic().size()) {
    std::string msg = chan->get_topic();
    send_message(base, cmd, RPL_TOPIC, false, &msg);
    msg = chan->get_topic_who() + " set the topic on " + chan->get_topic_time();
    send_message(base, cmd, RPL_TOPICWHOTIME, false, &msg);
  } else
    send_message(base, cmd, RPL_NOTOPIC, false, NULL);
  send_message(base, cmd, RPL_NAMREPLY, false, NULL);
  std::string msg = chan->get_nicks_string();
  send_message(base, cmd, 000, false, &msg);
  send_message(base, cmd, RPL_ENDOFNAMES, false, NULL);

  return (1);
}

/**
 * @brief Creates a new channel object and adds the cmd.client to it as its first member
 * by assigning it as an operator of the channel. 
 * After creating the channel, pushes it to the end of the list of _channels of _Server. 
 */
int IrcCommands::new_channel(Server& base, const struct cmd_obj& cmd,
                             std::string chan_name) {
  Channel NewChannel(chan_name);
  base._channel_list.push_back(NewChannel);
  base._channel_list.back().new_member(cmd.client, true);
  base._channel_list.back();
  send_join_message(base, cmd, cmd.client, &base._channel_list.back());
  send_message(base, cmd, RPL_NOTOPIC, false, NULL);
  send_message(base, cmd, RPL_ENDOFNAMES, false, NULL);
  return (1);
}

/**
 * @brief join_o is a helper function of JOIN command that executes
 * the special argument of ("0", 0x30). 
 * It makes the cmd.client to leave all channels that 
 * it's a member of by calling PART command for each of them.
 */
int IrcCommands::join_0(Server& base, const struct cmd_obj& cmd) {
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
        this->exec_command(base, tmp_cmd);
        continue;
      }
      it_chan++;
    }
  }
  return (1);
}

/**
 * @brief JOIN command is used to create/join channels
 * If the <channel> already exists and other requirements are met,
 * command issuer client can join the channel.
 * If the <channel> doesn't exist then it gets created.
 * 
 * Executes checks if cmd.client is registered and if there are enough parameters in command call.
 * After these checks it parses multiple <channel> and <key> values that are separated by comma ','
 * and stores them in separate vectors. 
 * 
 * Flow is continued with one of these helper functions:
 * join_0(), new_channel(), join_existing_channel()
 *
 * Usage: JOIN <channel>{,<channel>} [<key>{,<key>}]
 * e.g. JOIN #chan,#42,#horizon 
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
  if (!client_register_check(base, *cmd.client)) {
    send_message(base, cmd, ERR_NOTREGISTERED, true, NULL);
    return (ERR_NOTREGISTERED);
  }

  if (cmd.parameters.empty()) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, true, NULL);
    return (ERR_NEEDMOREPARAMS);
  } else if (cmd.parameters[0] == "0")
    return (join_0(base, cmd));

  std::vector<std::string> chans;
  if (cmd.parameters.size() > 0) {
    std::stringstream ss(cmd.parameters[0]);
    std::string elem;
    while (std::getline(ss, elem, ','))
      chans.push_back(elem);
  }
  std::vector<std::string> keys;
  if (cmd.parameters.size() > 1) {
    std::stringstream ss(cmd.parameters[1]);
    std::string elem;
    while (std::getline(ss, elem, ','))
      keys.push_back(elem);
  }

  for (size_t i = 0; i < chans.size(); i++) {
    if (!(chans[i][0] == '#' || chans[i][0] == '&'))
      continue;

    std::string key = (i < keys.size()) ? keys[i] : "";
    std::list<Channel>::iterator it_chan = base._channel_list.begin();
    if (!base._channel_list.empty()) {
      for (; it_chan != base._channel_list.end(); ++it_chan) {
        if (it_chan->get_name() == chans[i])
          break;
      }
    }
    if (base._channel_list.empty() || it_chan == base._channel_list.end())
      new_channel(base, cmd, chans[i]);
    else
      join_existing_channel(base, cmd, &(*it_chan), key);
  }
  return (1);
}
