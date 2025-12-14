#include <stdlib.h>
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
 * @brief MODE command to set channel's following modes:
 * i: Set/remove Invite-only channel
 * t: Set/remove the restrictions of the TOPIC command to channel
 * k: Set/remove the channel key (password)
 * o: Give/take channel operator privilege
 * l: Set/remove the user limit to channel
 * Usage: MODE <#channel> <mode>
 * e.g. MODE #chan42 +ikl <key> <limit>
 * 
 * TODO:
 * (1) send_channel_message is going to be implemented
 * (2) implement respective ERR
 * (3) implement resprective RPL
 *  
 * @return 0, in case of an error it returns error codes:
 * ERR_NOSUCHCHANNEL (403)
 * RPL_CHANNELMODEIS (324)
 * RPL_CREATIONTIME (329)
 * ERR_CHANOPRIVSNEEDED (482)
 * 
 * @return it returns 1 if command and password is correct, otherwise it returns 0
 */
int IrcCommands::mode(Server& base, const struct cmd_obj& cmd,
                      int fd_curr_client) {
  std::list<Client>::iterator it = base._client_list.begin();
  for (; it != base._client_list.end(); it++) {
    if (it->get_client_fd() == fd_curr_client)
      break;
  }
  if (cmd.parameters.empty()) {
    send_message(base, ERR_NEEDMOREPARAMS, true, NULL, *it);
    return (ERR_NEEDMOREPARAMS);

  } else if (!(cmd.parameters[0][0] == '#' || cmd.parameters[0][0] == '&')) {
    send_message(base, ERR_NOSUCHCHANNEL, true, NULL, *it);
    return (ERR_NOSUCHCHANNEL);
  }

  std::list<Channel>::iterator iter_chan = base._channel_list.begin();
  for (; iter_chan != base._channel_list.end(); iter_chan++) {
    if (iter_chan->get_name() == cmd.parameters[0])
      break;
  }
  if (iter_chan == base._channel_list.end())
    return (ERR_NOSUCHCHANNEL);

  /**
     * @brief sends current modes of to the client
     */
  if (cmd.parameters.size() == 1) {
    std::cout << iter_chan->get_name() << " Modes: ["
              << iter_chan->get_modes_string() << "]" << std::endl;
    return (0);
  }
  /**
   * @brief function for parsing modes and parameters 
   * that passed right after call of MODE command.
   * it parses multiple signs and also parameters 
   * for key and limit values
   * e.g. MODE #<channel> +ikl <key_value> <limit_value>
   */
  const std::string modes = cmd.parameters[1];
  int param_ind = 2;
  if (modes[0] == '-' || modes[0] == '+') {
    bool sign = (modes[0] == '+');
    std::string::const_iterator it = modes.begin();
    for (; it != modes.end(); ++it) {
      if (*it == '+')
        sign = true;
      else if (*it == '-')
        sign = false;
      switch (*it) {
        case 'i':
          iter_chan->set_mode(MODE_INVITE, sign);
          break;
        case 'k':
          if (!sign) {
            iter_chan->set_key("");
            iter_chan->set_mode(MODE_KEY, sign);
          } else if (sign && cmd.parameters.size() >= 3) {
            iter_chan->set_key(cmd.parameters[param_ind]);
            iter_chan->set_mode(MODE_KEY, sign);
            param_ind++;
          }
          break;
        case 'l':
          if (!sign) {
            iter_chan->set_user_limit(0);
            iter_chan->set_mode(MODE_KEY, sign);
          } else if (sign && cmd.parameters.size() >= 3) {
            iter_chan->set_user_limit(atoi(cmd.parameters[param_ind].c_str()));
            iter_chan->set_mode(MODE_LIMIT, sign);
            param_ind++;
          }
          break;
        case 't':
          iter_chan->set_mode(MODE_TOPIC, sign);
          break;
      }
    }
  }
  return (0);
}
