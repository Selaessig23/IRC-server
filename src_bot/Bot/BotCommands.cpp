#include <algorithm>  //std::find
#include <list>
#include <set>
#include <string>
#include "../Channel/Channel.hpp"
#include "../debug.hpp"
#include "../includes/CONSTANTS.hpp"
#include "../includes/types.hpp"
#include "Bot.hpp"

/**
 * @brief this function tries to register to the server by
 * using a server password (if there is one) PASS
 * NICK
 * USER
 */
void Bot::register_at_irc(struct pollfd& pfd) {
  std::string out;
  out += "PASS " + _pw + "\r\n";
  out += "NICK " + _nick + "\r\n";
  out += "USER " + _user + " " + _host + " * " + _realname + "\r\n";
  _output_buffer += out;
  pfd.events |= POLLOUT;
}

/**
 * @brief function to apply for becoming an server's operator
 * on irc-server
 *
 * NAME of this bot is SWEARBOT
 * PW to become operator: pw + "42BOT"
 */
void Bot::become_operator(struct pollfd& pfd) {
  std::string out;
  out += "OPER ";
  out += "SWEARBOT ";
  out += _pw + "42BOT";
  out += "\r\n";
  _output_buffer += out;
  pfd.events |= POLLOUT;
}

/**
 * @brief function to handle an invitation to join a channel
 * 
 * source I: "<client> <nick> <channel> :INVITES YOU" (RPL_INVITING (341))
 * source I: Command: INVITE | Parameters: <nickname> <channel> (INVITE msg)
 *
 * it checks if nick is correct and if bot is not yet member of this channel
 * if correct, channel gets status invited and bot tries to join the channel
 */
int Bot::handle_invitation(cmd_obj& cmd_body, struct pollfd& pfd) {
  if (cmd_body.command == "342" &&
      (cmd_body.parameters.size() < 3 || cmd_body.parameters[1] != _nick))
    return (1);
  else if (cmd_body.command == "INVITE" &&
           (cmd_body.parameters.size() < 2 || cmd_body.parameters[0] != _nick))
    return (1);
  std::string chan;
  if (cmd_body.command == "342")
    chan = cmd_body.parameters[2];
  else if (cmd_body.command == "INVITE")
    chan = cmd_body.parameters[1];
  std::list<Channel>::iterator it_chan =
      std::find(_channel_list.begin(), _channel_list.end(), chan);
  if (it_chan != _channel_list.end())
    return (1);
  Channel new_channel(chan);
  _channel_list.push_back(new_channel);
  std::string out;
  out += "JOIN ";
  out += chan;
  out += "\r\n";
  _output_buffer += out;
  pfd.events |= POLLOUT;
  it_chan = std::find(_channel_list.begin(), _channel_list.end(), chan);
  it_chan->set_status(APPLIED);
  return (0);
}

/**
 * @brief handle reply message of the server for JOIN-request of the bot
 *
 * it handles the following reply messages
 *  - JOIN channel
 *  - RPL_NAMREPLY (353): "<client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}"
 *
 * it sets channel status to ACCEPTED if conditions are correct
 *
 * @return returns 1 in case of channel name or nick name could not be found
 */

int Bot::handle_join(cmd_obj& cmd_body) {
  if (cmd_body.command == "JOIN") {
    if (cmd_body.parameters.size() < 1)
      return (1);
    std::list<Channel>::iterator it_chan = std::find(
        _channel_list.begin(), _channel_list.end(), cmd_body.parameters[0]);
    if (it_chan == _channel_list.end())
      return (1);
    it_chan->set_status(ACCEPTED);
    return (0);
  } else if (cmd_body.parameters[0] == _nick) {
    if (cmd_body.parameters.size() < 3)
      return (1);
    std::list<Channel>::iterator it_chan = std::find(
        _channel_list.begin(), _channel_list.end(), cmd_body.parameters[2]);
    if (it_chan == _channel_list.end())
      return (1);
    it_chan->set_status(ACCEPTED);
    return (0);
  }
  return (1);
}

/**
 * @brief function to KILL a client
 * if client reached 3 strikes
 *
 * Command: KILL
 * Parameters: <nickname> <comment>
 */
void Bot::kill_client(const std::string& nick, struct pollfd& pfd) {
  std::string out;
  out += "KILL ";
  out += nick;
  out +=
      " :We cannot accept creatures like " + nick + " in our lovely network.";
  out += "\r\n";
  _output_buffer += out;
  pfd.events |= POLLOUT;
}

/**
 * @brief function that organises the sanctioning of a channel member when using
 * a swear word by creating the reply message. 
 * The client gets a warning each time he uses a swear word.
 *
 * function distinguishes between private messages to the bot
 * and messages to a channel the bot is member of
 */
void Bot::sanctioning(const std::string& nick, std::string& channel,
                      std::string& out) {
  if (nick == _nick)
    return;
  std::list<Channel>::iterator it_chan =
      std::find(_channel_list.begin(), _channel_list.end(), channel);
  if (it_chan == _channel_list.end()) {
    // message was a private message to bot not channel --> could be checked, recipient should than be _nick
    out += nick;
    out += " :This will lead to an enormous disadvantage in your future life.";
  } else {
    std::string amount;
    if (get_strikes(nick) == -1) {
      _warned_members.insert(std::pair<std::string, int>(nick, 1));
      amount = "first";
    } else if (get_strikes(nick) < 3) {
      std::map<std::string, int>::iterator chan_member =
          _warned_members.find(nick);
      chan_member->second += 1;
      if (get_strikes(nick) == 2) {
        amount = "second";
        if (_operator == true)
          amount += " (penultimate)";
      } else if (get_strikes(nick) == 3) {
        amount = "third";
        if (_operator == true)
          amount += " (ultimate)";
      } else {
        amount = get_strikes(nick);
        amount += "th";
      }
    }
    out += nick;
    out += " :An inappropriate has been detected. This is your ";
    out += amount;
    out +=
        " strike. "
        "Please do not confuse other people by spreading falsehoods "
        "about "
        "42.";
  }
}

/**
 * @brief function to check for swear words in PRIVMSGs of channels
 * the bot is member of
 *
 * PRIVMSGs have the following format:
 * Command: PRIVMSG
 * Parameters: <target>{,<target>} <text to be sent>
 *
 * in case the message was addressed to a channel:
 * prefix has to be set to find out the sender (and give him a strike)
 * otherwise only a general warning gets spread to the channel
 *
 * in case of private message was addressed directly to the bot: 
 * the sender can be identified and
 * receives a return message (but no official strike)
 *
 * if a sender gets a strike for 3rd time and bot is an operator
 * the sender gets killed and the channel gets an information
 * (information message should normally be part of irc-server)
 * client gets removed from _warned_member list afterwards
 *
 */
int Bot::check_for_swears(cmd_obj& cmd_body, struct pollfd& pfd) {
  if (cmd_body.parameters.size() >= 2) {
    std::string to_check = cmd_body.parameters[1];
    size_t delimiter = to_check.find("42");
    if (delimiter != std::string::npos) {
      for (std::set<std::string>::iterator it_swear = _swear_words.begin();
           it_swear != _swear_words.end(); it_swear++) {
        delimiter = to_check.find(*it_swear);
        if (delimiter != std::string::npos) {
          std::string out;
          std::string nick;
          std::string channel = cmd_body.parameters[0];

          out += ":" + _nick;
          out += " PRIVMSG ";
          if (cmd_body.prefix.empty() && !cmd_body.parameters.empty()) {
            out += channel;
            out +=
                " :The wise and intelligent 42 network will control "
                "everything "
                "in the near future. "
                "Communicate with caution.";
          } else {
            //cut nick from prefix
            int pos = cmd_body.prefix.find("!");
            nick = cmd_body.prefix.substr(0, pos);
            sanctioning(nick, cmd_body.parameters[0], out);
          }
          out += "\r\n";
          _output_buffer += out;
          pfd.events |= POLLOUT;
          if (_operator == true && get_strikes(nick) >= 3) {
            DEBUG_PRINT("Strikes " << get_strikes(nick));
            DEBUG_PRINT("Operator status:" << _operator);
            kill_client(nick, pfd);
            std::map<std::string, int>::iterator to_kill =
                _warned_members.find(nick);
            _warned_members.erase(to_kill);
            out += ":" + _nick;
            out += " PRIVMSG ";
            out += channel;
            out += " :Dear members of channel " + channel;
            out += " our not-valued member " + nick +
                   " has passed away. He hasn't treated the 42 network in a "
                   "fair way. May he not rest in peace.";
            out += "\r\n";
            _output_buffer += out;
            pfd.events |= POLLOUT;
          }
          break;
        }
      }
    }
  }
  return (0);
}
