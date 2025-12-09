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
int IrcCommands::join(Server& base, const struct cmd_obj& cmd,
                      int fd_curr_client) {
  std::list<Client>::iterator it = base._client_list.begin();
  for (; it != base._client_list.end(); it++) {
    if (it->get_client_fd() == fd_curr_client)
      break;
  }
  if (cmd.parameters.empty()) {
    send_message(base, ERR_NEEDMOREPARAMS, true, NULL, *it);
    return (ERR_NEEDMOREPARAMS);
  } else if (!(cmd.parameters[0][0] == '#' || cmd.parameters[0][0] == '&'))
    return (0);
  if (base._channel_list.empty()) {
    Channel NewChannel(cmd.parameters[0]);
    base._channel_list.push_back(NewChannel);
    base._channel_list.back().new_member(cmd.client, true);
    base._channel_list.back().print_channel_info();
  } else {
    std::list<Channel>::iterator iter = base._channel_list.begin();
    for (; iter != base._channel_list.end(); iter++) {
      if (iter->get_name() == cmd.parameters[0])
        break;
    }
    if (iter == base._channel_list.end()) {
      Channel NewChannel(cmd.parameters[0]);
      base._channel_list.push_back(NewChannel);
      base._channel_list.back().new_member(cmd.client, true);
      base._channel_list.back().print_channel_info();
    } else {
      iter->new_member(cmd.client, false);
      iter->print_channel_info();
    }
  }
  return (1);
}