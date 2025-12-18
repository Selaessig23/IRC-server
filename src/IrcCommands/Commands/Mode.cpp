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
 * ERR_USERNOTINCHANNEL (441)
 * ERR_CHANOPRIVSNEEDED (482)
 * RPL_CHANNELMODEIS (324)
 * RPL_CREATIONTIME (329)
 * 
 * @return it returns 1 if command and password is correct, otherwise it returns 0
 */
int IrcCommands::mode(Server& base, const struct cmd_obj& cmd) {

  if (cmd.parameters.empty()) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, true, NULL);
    return (ERR_NEEDMOREPARAMS);

  } else if (!(cmd.parameters[0][0] == '#' || cmd.parameters[0][0] == '&')) {
    send_message(base, cmd, ERR_NOSUCHCHANNEL, true, NULL);
    return (ERR_NOSUCHCHANNEL);
  }

  std::list<Channel>::iterator it_chan = base._channel_list.begin();
  for (; it_chan != base._channel_list.end(); it_chan++) {
    if (it_chan->get_name() == cmd.parameters[0])
      break;
  }
  if (it_chan == base._channel_list.end())
    return (ERR_NOSUCHCHANNEL);

  if (cmd.parameters.size() == 1) {
    std::cout << it_chan->get_name() << " Modes: ["
              << it_chan->get_modes_string() << "]" << std::endl;
    it_chan->print_channel_info();
    return (0);
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
  if (!it_chan_mem->second) {
    send_message(base, cmd, ERR_CHANOPRIVSNEEDED, true, NULL);
    return (ERR_CHANOPRIVSNEEDED);
  }

  const std::string modes = cmd.parameters[1];
  int param_ind = 2;
  if (modes[0] == '-' || modes[0] == '+') {
    bool sign = (modes[0] == '+');
    std::string::const_iterator it = modes.begin() + 1;
    for (; it != modes.end(); it++) {
      switch (*it) {
        case 'i':
          it_chan->set_mode(MODE_INVITE, sign);
          break;
        case 'k':
          if (!sign) {
            it_chan->set_key("");
            it_chan->set_mode(MODE_KEY, sign);
          } else if (sign && cmd.parameters.size() >= 3) {
            it_chan->set_key(cmd.parameters[param_ind]);
            it_chan->set_mode(MODE_KEY, sign);
            param_ind++;
          }
          break;
        case 'l':
          if (!sign) {
            it_chan->set_user_limit(0);
            it_chan->set_mode(MODE_LIMIT, sign);
          } else if (sign && cmd.parameters.size() >= 3) {
            it_chan->set_user_limit(atoi(cmd.parameters[param_ind].c_str()));
            it_chan->set_mode(MODE_LIMIT, sign);
            param_ind++;
          }
          break;
        case 't':
          it_chan->set_mode(MODE_TOPIC, sign);
          break;
        case 'o':
          if (cmd.parameters.size() >= 3) {
            if (it_chan->update_chanops_stat(cmd.parameters[param_ind], sign))
              send_message(base, cmd, NO_ERR, false, NULL);
            else
              send_message(base, cmd, ERR_USERNOTINCHANNEL, true, NULL);
          }
          break;
      }
    }
  }
  std::cout << it_chan->get_name() << " Modes: [" << it_chan->get_modes_string()
            << "]" << std::endl;
  return (0);
}
