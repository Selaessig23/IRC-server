#include <algorithm>  //std::find
#include <list>
#include <set>
#include <string>
#include "../Channel/Channel.hpp"
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
 * source: "<client> <nick> <channel> :INVITES YOU"
 *
 * it checks if nick is correct and if bot is not yet member of this channel
 * if correct, channel gets status invited and bot tries to join the channel
 */
int Bot::handle_invitation(cmd_obj& cmd_body, struct pollfd& pfd) {
  if (cmd_body.parameters[1] != _nick)
    return (1);
  std::list<Channel>::iterator it_chan = std::find(
      _channel_list.begin(), _channel_list.end(), cmd_body.parameters[2]);
  if (it_chan != _channel_list.end())
    return (1);
  Channel new_channel(cmd_body.parameters[2]);
  _channel_list.push_back(new_channel);
  std::string out;
  out += "JOIN ";
  out += cmd_body.parameters[2];
  out += "\r\n";
  _output_buffer += out;
  pfd.events |= POLLOUT;
  it_chan = std::find(_channel_list.begin(), _channel_list.end(),
                      cmd_body.parameters[2]);
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
    std::list<Channel>::iterator it_chan = std::find(
        _channel_list.begin(), _channel_list.end(), cmd_body.parameters[0]);
    if (it_chan == _channel_list.end())
      return (1);
    it_chan->set_status(ACCEPTED);
    return (0);
  } else if (cmd_body.parameters[0] == _nick) {
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
 * a swear word. He gets a warning each time he uses a swear word.
 * If bot is registered as an operator, the client gets killed for his 3rd strike
 *
 * TODO: think about removing the client fom channel if he gets killed
 */
void Bot::sanctioning(const std::string& nick, std::string& channel,
                      std::string& out, struct pollfd& pfd) {
  if (nick == _nick)
    return;
  std::list<Channel>::iterator it_chan =
      std::find(_channel_list.begin(), _channel_list.end(), channel);
  if (it_chan == _channel_list.end()) {
    // message was a private message to bot not channel --> could be checked, recipient should than be _nick
    out += nick;
    out += " :This will led to an enormous disadvantage in your further life.";
  } else {
    std::map<std::string, int>& members = it_chan->get_members();
    if (it_chan->get_strikes(nick) == -1)
      members.insert(std::pair<std::string, int>(nick, 1));
    else if (it_chan->get_strikes(nick) < 3) {
      std::map<std::string, int>::iterator chan_member = members.find(nick);
      chan_member->second += 1;
      out += nick;
      out +=
          " :An unwished comment was noticed. This is your first strike. "
          "Please do not confuse other people by spreading falsehoods "
          "about "
          "42.";
    } else if (_operator == true) {
      kill_client(nick, pfd);
      out += nick;
      out += " :Dear members of channel " + channel;
      out += " our not-valued member " + nick +
             " has passed away. May he rest not rest in peace.";
    }
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
 * prefix has to be set to find out the sender
 * otherwise only a general warning gets spread
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
          out += ":" + _nick;
          out += " PRIVMSG ";
          if (cmd_body.prefix.empty() && !cmd_body.parameters.empty()) {
            out += cmd_body.parameters[0];
            out +=
                " :The wise 42 network is going to control everything. "
                "Communicate wisely.";
          } else {
            //cut nick from prefix
            int pos = cmd_body.prefix.find("!");
            std::string nick = cmd_body.prefix.substr(0, pos);
            sanctioning(nick, cmd_body.parameters[0], out, pfd);
          }
          out += "\r\n";
          _output_buffer += out;
          pfd.events |= POLLOUT;
        }
      }
    }
  }
  return (0);
}
