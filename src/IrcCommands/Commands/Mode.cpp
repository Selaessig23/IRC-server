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

  if (cmd.parameters.size() == 1) {
    std::cout << iter_chan->get_name() << " Modes: " << iter_chan->get_modes()
              << std::endl;
    send_message(base, 000, false, &(iter_chan->get_name()), *it);
  }
  if (cmd.parameters.size() == 2 &&
      (cmd.parameters[1][0] == '-' || cmd.parameters[1][0] == '+')) {
    bool flag = (cmd.parameters[1][0] == '+');
    std::string::const_iterator it = cmd.parameters[1].begin();
    for (; it != cmd.parameters[1].end(); ++it)
      switch (*it) {
        case 'i':
          iter_chan->set_mode(MODE_INVITE, flag);
          break;
        case 'k':
          iter_chan->set_mode(MODE_KEY, flag);
          break;
        case 'l':
          iter_chan->set_mode(MODE_LIMIT, flag);
          break;
        case 't':
          iter_chan->set_mode(MODE_TOPIC, flag);
          break;
      }
  }
  return (0);
}
