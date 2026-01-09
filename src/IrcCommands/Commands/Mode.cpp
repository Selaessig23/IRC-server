#include <cstdlib>
#include <iostream>
#include <list>
#include "../../Channel/Channel.hpp"
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../Utils.hpp"
#include "../../debug.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
  * @brief function that structures MODE reply message and sends it to the target client.
  */
void IrcCommands::send_mode_message(Server& base, const struct cmd_obj& cmd,
                                    Client* target, Channel* chan,
                                    const std::string& update) {
  std::string msg;
  msg += ":" + cmd.client->get_nick();
  msg += "!" + cmd.client->get_user();
  msg += "@" + cmd.client->get_host();
  msg += " MODE " + chan->get_name() + " [" + update + "]";
  msg += "\r\n";
  target->add_client_out(msg);
  base.set_pollevent(target->get_client_fd(), POLLOUT);
}

/**
 * @brief this function sets or unsets the following modes of channels:
 * i: Set/remove Invite-only channel
 * t: Set/remove the restrictions of the TOPIC command to channel
 * k: Set/remove the channel key (password)
 * l: Set/remove the user limit to channel
 * o: Give/take channel operator privilege
 * 
 * After modes updating is done, it broadcasts the MODE reply message
 * that shows the actual changes made by the last command to the channel modes.
 *  
 * @return 0 or ERR_USERNOTINCHANNEL (441) in case of an error
 * 
 * @return it returns 1 if command and password is correct, otherwise it returns 0
 */

int IrcCommands::update_modes(Server& base, const struct cmd_obj& cmd,
                              Channel* chan) {
  const std::string modestring = cmd.parameters[1];
  unsigned long param_ind = 2;
  std::string msg;
  std::string msg_param;
  if (modestring[0] == '-' || modestring[0] == '+') {
    bool sign = (modestring[0] == '+') ? true : false;
    msg = sign ? "+" : "-";
    for (std::string::const_iterator it = modestring.begin() + 1;
         it != modestring.end(); it++) {
      switch (*it) {
        case '+':
          sign = true;
          msg += "+";
          break;
        case '-':
          sign = false;
          msg += "-";
          break;
        case 'i':
          if ((sign && !(chan->get_modes() & MODE_INVITE)) ||
              (!sign && (chan->get_modes() & MODE_INVITE))) {
            chan->adjust_modes(MODE_INVITE, sign);
            msg += "i";
          }
          break;
        case 'k':
          if (!sign && (chan->get_modes() & MODE_KEY)) {
            chan->set_key("");
            chan->adjust_modes(MODE_KEY, sign);
            msg += "k";
          } else if ((param_ind < cmd.parameters.size() && sign &&
                      chan->get_key() != cmd.parameters[param_ind])) {
            chan->set_key(cmd.parameters[param_ind]);
            chan->adjust_modes(MODE_KEY, sign);
            msg += "k";
            msg_param += " " + cmd.parameters[param_ind];
            param_ind++;
          }
          break;
        case 'l':
          if (!sign && (chan->get_modes() & MODE_LIMIT)) {
            chan->set_user_limit(0);
            chan->adjust_modes(MODE_LIMIT, sign);
            msg += "l";
          } else if (sign && param_ind < cmd.parameters.size()) {
            int limit = 0;
            Utils::ft_convert_to_int(limit, cmd.parameters[param_ind]);
            if (limit < 0)
              continue;
            else if (limit != chan->get_user_limit()) {
              chan->set_user_limit(limit);
              chan->adjust_modes(MODE_LIMIT, sign);
              msg += "l";
              msg_param += " " + cmd.parameters[param_ind];
            }
            param_ind++;
          }
          break;
        case 't':
          if ((sign && !(chan->get_modes() & MODE_TOPIC)) ||
              (!sign && (chan->get_modes() & MODE_TOPIC))) {
            chan->adjust_modes(MODE_TOPIC, sign);
            msg += "t";
          }
          break;
        case 'o':
          if (param_ind < cmd.parameters.size()) {
            if (chan->update_chanops_stat(cmd.parameters[param_ind], sign)) {
              msg += "o";
              msg_param += " " + cmd.parameters[param_ind];
            } else
              send_message(base, cmd, ERR_USERNOTINCHANNEL, cmd.client, chan);
            param_ind++;
          }
          break;
      }
    }
  }
  while (msg.size() &&
         (msg[msg.size() - 1] == '-' || msg[msg.size() - 1] == '+'))
    msg.erase(msg.size() - 1);
  while (msg.size() >= 2 &&
         ((msg[0] == '-' && msg[1] == '+') ||
          (msg[0] == '+' && msg[1] == '-') ||
          (msg[0] == '-' && msg[1] == '-') || (msg[0] == '+' && msg[1] == '+')))
    msg.erase(0, 1);
  if (msg.size() < 2)
    return (0);
  msg += msg_param;

  for (std::map<Client*, bool>::iterator it_chan_mem =
           chan->get_members().begin();
       it_chan_mem != chan->get_members().end(); it_chan_mem++)
    send_mode_message(base, cmd, it_chan_mem->first, chan, msg);

  return (1);
}

/**
 * @brief MODE command to set channel's following modes:
 * In the following all necessary checks are getting done before the actual updating
 * takes place. If all the requirements are made update_modes() is called.
 * Otherwise an error code is returned. 
 * 
 * Command Usage: 
 * MODE <channel> [<modestring> [<mode arguments>...]]
 * e.g. MODE #chan42 +iklo <key> <limit> <nickname>
 *  
 * @return 0 or error codes:
 * ERR_NOSUCHCHANNEL (403)
 * ERR_USERNOTINCHANNEL (441)
 * ERR_NEEDMOREPARAMS (461)
 * ERR_CHANOPRIVSNEEDED (482)
 * RPL_CHANNELMODEIS (324)
 * RPL_CREATIONTIME (329)
 * in case of an error
 * 
 * @return it returns 1 if command and password is correct, otherwise it returns 0
 */

int IrcCommands::mode(Server& base, const struct cmd_obj& cmd) {
  if (!client_register_check(base, *cmd.client)) {
    send_message(base, cmd, ERR_NOTREGISTERED, cmd.client, NULL);
    return (ERR_NOTREGISTERED);
  }

  if (cmd.parameters.empty()) {
    send_message(base, cmd, ERR_NEEDMOREPARAMS, cmd.client, NULL);
    return (ERR_NEEDMOREPARAMS);

  } else if (!(cmd.parameters[0][0] == '#' || cmd.parameters[0][0] == '&')) {
    send_message(base, cmd, ERR_NOSUCHCHANNEL, cmd.client, NULL);
    return (ERR_NOSUCHCHANNEL);
  }

  std::list<Channel>::iterator it_chan = base._channel_list.begin();
  for (; it_chan != base._channel_list.end(); it_chan++) {
    if (it_chan->get_name() == cmd.parameters[0])
      break;
  }
  if (it_chan == base._channel_list.end()) {
    send_message(base, cmd, ERR_NOSUCHCHANNEL, cmd.client, NULL);
    return (ERR_NOSUCHCHANNEL);
  }

  if (cmd.parameters.size() == 1) {
    std::string msg =
        it_chan->get_name() + " Modes: [" + it_chan->get_modes_string() + "]";
    send_message(base, cmd, RPL_CHANNELMODEIS, cmd.client, &(*it_chan));
    send_message(base, cmd, RPL_CREATIONTIME, cmd.client, &(*it_chan));
    return (0);
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
  if (!it_chan_mem->second) {
    send_message(base, cmd, ERR_CHANOPRIVSNEEDED, cmd.client, &(*it_chan));
    return (ERR_CHANOPRIVSNEEDED);
  }

  return (update_modes(base, cmd, &(*it_chan)));
}
