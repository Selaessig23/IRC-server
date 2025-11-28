#include <algorithm>  //std::find
#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to to send private messages between users, 
 * as well as to send messages to channels
 * IRC syyntax:
 * Command: PRIVMSG
 * Parameters: <target>{,<target>} <text to be sent>
 *
 * TODO
 * (1) check if NOTEXTTOSEND also gets sent if std is empty (current implementation)
 *
 * numeric replies:
 *    ERR_NOSUCHNICK (401)
 *    ERR_NOSUCHSERVER (402) -> multy-server not implemented
 *    ERR_CANNOTSENDTOCHAN (404)
 *    ERR_TOOMANYTARGETS (407) -> how to implement?
 *    ERR_NORECIPIENT (411)
 *    ERR_NOTEXTTOSEND (412)
 *    ERR_NOTOPLEVEL (413) -> USE CASE?
 *    ERR_WILDTOPLEVEL (414) -> USE CASE?
 *    RPL_AWAY (301) -> AWAY-functionality not implemented
 *
 * @return 0, in case of an error it returns error codes:
 * [...]
 */
int IrcCommands::privmsg(Server& base, const struct cmd_obj& cmd,
                         int fd_curr_client) {
  if (cmd.parameters.empty()) {
    send_message(base, ERR_NORECIPIENT, true, NULL, *cmd.client);
    return (ERR_NORECIPIENT);
  }

  std::vector<std::string> recipients;
  std::vector<std::string>::const_iterator it_para = cmd.parameters.begin();
  for (; it_para != cmd.parameters.end() && *it_para->begin() == ',';
       it_para++) {
    recipients.push_back(*it_para);
  }

  if (it_para == cmd.parameters.end() && it_para->empty()) {
    send_message(base, ERR_NOTEXTTOSEND, true, NULL, *cmd.client);
    return (ERR_NOTEXTTOSEND);
  }

  std::string msg = *it_para;

  for (std::vector<std::string>::const_iterator it_rec = recipients.begin();
       it_rec != recipients.end(); it_rec++) {
    if (*it_rec->begin() == '#' || *it_rec->begin() == '&') {
      std::list<Channel>::iterator it_chan =
          std::find(base._channel_list.begin(), base._channel_list.end(), msg);
      //checks if Channel exists and if current client is member of channel (==allowed to send messages)
      if (it_chan != base._channel_list.end() &&
          std::find(it_chan->get_members().begin(),
                    it_chan->get_members().end(),
                    cmd.client->get_nick()) != it_chan->get_members().end()) {
        //send message to all channel members
        for (std::list<Client*>::const_iterator it_chan_member =
                 it_chan->get_members().begin();
             it_chan_member != it_chan->get_members().end(); it_chan_member++) {
          send_message(base, NO_ERR, true, &msg, it_chan_member);
        }
      } else {
        //send error message
        send_message(base, ERR_CANNOTSENDTOCHAN, true, NULL, *cmd.client);
        return (ERR_CANNOTSENDTOCHAN);
      }
    } else {
      if (std::find(base._client_list.begin(), base._client_list.end(), msg) !=
          base._client_list.end()) {
        //send message to client
      } else {
        //send error message
        send_message(base, ERR_NOSUCHNICK, true, NULL, *cmd.client);
        return (ERR_NOSUCHNICK);
      }
    }
  }
}
