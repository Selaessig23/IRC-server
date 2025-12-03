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
 * TODO:
 * (1) check if there is enough parameters
 * (2) check for parameter prefix
 * (3) implement respective ERR
 * (4) implement resprective RPL
 *  
 * @return 0, in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS (461)
 * ERR_NOSUCHCHANNEL (403)
 * ERR_TOOMANYCHANNELS (405)
 * ERR_BADCHANNELKEY (475)
 * ERR_BANNEDFROMCHAN (474)
 * ERR_CHANNELISFULL (471)
 * ERR_INVITEONLYCHAN (473)
 * ERR_BADCHANMASK (476)
 * RPL_TOPIC (332)
 * RPL_TOPICWHOTIME (333)
 * RPL_NAMREPLY (353)
 * RPL_ENDOFNAMES (366)
 * 
 * @return it returns 1 if command and password is correct, otherwise it returns 0
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
  } else if (cmd.parameters[0][0] != '#' || cmd.parameters[0][0] != '&')
    return (0);
  if (base._channel_list.empty()) {
    Channel NewChannel(cmd.parameters[0]);
    base._channel_list.push_back(NewChannel);
    base._channel_list.back().new_member(cmd.client);
    base._channel_list.back().new_operator(cmd.client);
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
      base._channel_list.back().new_member(cmd.client);
      base._channel_list.back().new_operator(cmd.client);
      base._channel_list.back().print_channel_info();
    } else {
      iter->new_member(cmd.client);
      iter->print_channel_info();
    }
  }
  return (0);
}