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
 * (1) implement respective ERR
 * (2) implement resprective RPL
 *  
 * @return 0, in case of an error it returns error codes:
 * RPL_INVITING (341)
 * ERR_NEEDMOREPARAMS (461)
 * ERR_NOSUCHCHANNEL (403)
 * ERR_NOTONCHANNEL (442)
 * ERR_USERONCHANNEL (443)
 * ERR_CHANOPRIVSNEEDED (482)
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
  if (base._channel_list.empty() || it_chan == base._channel_list.end()) {
    send_message(base, cmd, ERR_NOSUCHCHANNEL, true, NULL);
    return (ERR_NOSUCHCHANNEL);
  }

  /**
   * ERR_NOTONCHANNEL (442) 
   *   "<client> <channel> :You're not on that channel"
   *   Returned when a client tries to perform a channel-affecting command 
   *   on a channel which the client isn’t a part of.
   */
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
  /**ERR_CHANOPRIVSNEEDED (482) 
   *   "<client> <channel> :You're not channel operator"
   * Indicates that a command failed because the client does not have the appropriate channel privileges. 
   * This numeric can apply for different prefixes such as halfop, operator, etc. 
   * The text used in the last param of this message may vary.
   */
  if (!it_chan_mem->second) {
    send_message(base, cmd, ERR_CHANOPRIVSNEEDED, true, NULL);
    return (ERR_CHANOPRIVSNEEDED);
  }

  std::list<Client>::iterator it_inv = base._client_list.begin();
  if (!base._client_list.empty()) {
    for (; it_inv != base._client_list.end(); it_inv++) {
      if (it_inv->get_nick() == cmd.parameters[0])
        break;
    }
  }
  if (it_inv == base._client_list.end())
    return (0);

  /** ERR_USERONCHANNEL (443) 
   * "<client> <nick> <channel> :is already on channel"
   * Returned when a client tries to invite <nick> to a channel they’re already joined to.
  */
  it_chan_mem = it_chan->get_members().begin();
  for (; it_chan_mem != it_chan->get_members().end(); it_chan_mem++) {
    // std::cout << it_chan_mem->first  << std::endl << &(*it_inv) << std::endl;
    if (it_chan_mem->first == &(*it_inv))
      break;
  }
  if (it_chan_mem == it_chan->get_members().end()) {
    send_message(base, cmd, ERR_USERONCHANNEL, true, NULL);
    return (ERR_USERONCHANNEL);
  }

  it_chan->new_invited(&(*it_inv));
  send_message(base, cmd, RPL_INVITING, false, NULL);

  return (1);
}