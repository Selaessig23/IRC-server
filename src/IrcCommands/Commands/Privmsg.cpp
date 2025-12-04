#include <algorithm>  //std::find
#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to format and send private messages from one client to another or to a channel
 * FORMAT:
 * :<nick>!<user>@<host> PRIVMSG <target_nick> :<message>
 * OR
 * :<nick>!<user>@<host> PRIVMSG <channel_name> :<message>
 * (according to IRC protocol (RFC 1459 / 2812))
 */
int IrcCommands::send_privmsg(Server& base, Client& sender, Client& receiver,
                              const std::string& msg,
                              const std::string& channel) {
  std::string out;
  out += ":" + sender.get_nick();
  out += "!" + sender.get_user();
  out += "@" + sender.get_host();
  out += " PRIVMSG ";
  if (!channel.empty())
    out += channel;
  else
    out += receiver.get_nick();
  out += " :" + msg;
  out += "\r\n";
  receiver.add_client_out(out);
  base.set_pollevent(receiver.get_client_fd(), POLLOUT);
  return (0);
}

/**
 * @brief function to send private messages between users, 
 * as well as to send messages to channels
 * IRC syntax:
 * Command: PRIVMSG
 * Parameters: <target>{,<target>} <text to be sent>
 *
 * TODO
 * (1) check if NOTEXTTOSEND also gets sent if std is empty (current implementation)
 * (2) think about using Channel Membership Prefixes to address messages to certain group of people with a channel
 *     (founder and operator prefix are the only ones of interest)
 * (3) think about using wildcards for identifying users and channels (recipients)
 * (4) what should happen if user sends a message to his own?
 * (5) what should happen if user sends more than 2 parameters? Recently it simply ignoes them
 *
 * @return 0, in case of an error it returns error codes:
 *    ERR_NOTREGISTERED (451)
 *    ERR_NOSUCHNICK (401)
 *    ERR_CANNOTSENDTOCHAN (404)
 *    ERR_NORECIPIENT (411)
 *    ERR_NOTEXTTOSEND (412)
 *
 *
 * reply | error codes from IRC-protocol that are not considered in this implementation:
 *    ERR_NOSUCHSERVER (402) -> multi-server not implemented
 *    ERR_TOOMANYTARGETS (407) -> how to implement?
 *    ERR_NOTOPLEVEL (413) -> USE CASE?
 *    ERR_WILDTOPLEVEL (414) -> USE CASE?
 *    RPL_AWAY (301) -> AWAY-functionality not implemented
 */
int IrcCommands::privmsg(Server& base, const struct cmd_obj& cmd,
                         int fd_curr_client) {
  (void)fd_curr_client;
  if (!client_register_check(base, *cmd.client)) {
    send_message(base, ERR_NOTREGISTERED, true, NULL, *cmd.client);
    return (ERR_NOTREGISTERED);
  }

  if (cmd.parameters.empty()) {
    send_message(base, ERR_NORECIPIENT, true, NULL, *cmd.client);
    return (ERR_NORECIPIENT);
  }

  std::vector<std::string> recipients;
  std::vector<std::string>::const_iterator it_para = cmd.parameters.begin();
  size_t start = 0;
  size_t end = it_para->find(',', start);
  while (end != std::string::npos) {
    recipients.push_back(it_para->substr(start, end - start));
    end = it_para->find(',', start);
    start = end + 1;
  }
  recipients.push_back(it_para->substr(start));
  it_para++;

  if (it_para == cmd.parameters.end() || it_para->empty()) {
    send_message(base, ERR_NOTEXTTOSEND, true, NULL, *cmd.client);
    return (ERR_NOTEXTTOSEND);
  }

  std::string msg = *it_para;

  for (std::vector<std::string>::iterator it_rec = recipients.begin();
       it_rec != recipients.end(); it_rec++) {
    if (*it_rec->begin() == '#' || *it_rec->begin() == '&') {
      std::list<Channel>::iterator it_chan = std::find(
          base._channel_list.begin(), base._channel_list.end(), *it_rec);
      if (it_chan != base._channel_list.end() &&
          std::find(it_chan->get_members_nicks().begin(),
                    it_chan->get_members_nicks().end(),
                    cmd.client->get_nick()) !=
              it_chan->get_members_nicks().end()) {
        std::list<Client*> chan_members = it_chan->get_members();
        for (std::list<Client*>::iterator it_chan_member = chan_members.begin();
             it_chan_member != chan_members.end(); it_chan_member++) {
          send_privmsg(base, *cmd.client, *(*it_chan_member), msg,
                       it_chan->get_name());
        }
      } else {
        send_message(base, ERR_CANNOTSENDTOCHAN, true, NULL, *cmd.client);
        return (ERR_CANNOTSENDTOCHAN);
      }
    } else {
      std::list<Client>::iterator it_nick = std::find(
          base._client_list.begin(), base._client_list.end(), *it_rec);
      if (it_nick->get_client_fd() != cmd.client->get_client_fd() &&
          it_nick != base._client_list.end()) {
        send_privmsg(base, *cmd.client, *it_nick, msg, "");
      } else if (it_nick->get_client_fd() != cmd.client->get_client_fd()) {
        send_message(base, ERR_NOSUCHNICK, true, NULL, *cmd.client);
        return (ERR_NOSUCHNICK);
      }
    }
  }
  return (0);
}
