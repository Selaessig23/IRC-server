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
 * @brief TOPIC command is used to change or view the topic of channels.
 * Only members of respective channel have the privilege.
 * If topic-restricted (+t) mode is enabled for the channel, only chanops
 * can change the topic.
 * 
 * e.g TOPIC <channel> [<topic>]
 * 
 * TOPIC <channel> : retrieves and shows the topic
 * TOPIC <channel> <topic> : sets the topic to <topic>
 * 
 * If <topic> is an empty string, the topic is cleared.
 * After a successful topic change all the members are informed with RPL_TOPIC.
 *  
 * @return ERR in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS (461)
 * ERR_NOSUCHCHANNEL (403)
 * ERR_NOTONCHANNEL (442)
 * ERR_CHANOPRIVSNEEDED (482)
 * RPL_NOTOPIC (331)
 * RPL_TOPIC (332)
 * RPL_TOPICWHOTIME (333)
 * 
 * @return it returns 1 if command is successfully executed
 */
int IrcCommands::topic(Server& base, const struct cmd_obj& cmd) {
  if (!client_register_check(base, *cmd.client)) {
    send_message(base, cmd, ERR_NOTREGISTERED, cmd.client, NULL);
    return (ERR_NOTREGISTERED);
  }

  if (cmd.parameters.size() == 0) {
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
  if (base._channel_list.empty() || it_chan == base._channel_list.end()) {
    send_message(base, cmd, ERR_NOSUCHCHANNEL, cmd.client, NULL);
    return (ERR_NOSUCHCHANNEL);
  }

  if (cmd.parameters.size() == 1) {
    if (it_chan->get_topic().size()) {
      std::string msg = it_chan->get_topic();
      send_message(base, cmd, RPL_TOPIC, cmd.client, &(*it_chan));
      send_message(base, cmd, RPL_TOPICWHOTIME, cmd.client, &(*it_chan));
    } else
      send_message(base, cmd, RPL_NOTOPIC, cmd.client, &(*it_chan));
    return (1);
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
  if ((it_chan->get_modes() & MODE_TOPIC) && it_chan_mem->second == false) {
    send_message(base, cmd, ERR_CHANOPRIVSNEEDED, cmd.client, &(*it_chan));
    return (ERR_CHANOPRIVSNEEDED);
  }

  cmd_obj tmp_cmd = cmd;
  it_chan->set_topic(cmd.parameters[1], cmd.client->get_nick());
  std::string msg = it_chan->get_topic();
  it_chan_mem = it_chan->get_members().begin();
  for (; it_chan_mem != it_chan->get_members().end(); it_chan_mem++) {
    tmp_cmd.client = it_chan_mem->first;
    send_message(base, tmp_cmd, RPL_TOPIC, it_chan_mem->first, &(*it_chan));
  }

  return (1);
}