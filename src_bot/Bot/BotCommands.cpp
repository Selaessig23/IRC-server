#include <algorithm>  //std::find
#include <string>
#include "../Channel/Channel.hpp"
#include "../includes/types.hpp"
#include "../includes/CONSTANTS.hpp"
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
  if (it_chan == _channel_list.end())
    return (1);
  Channel new_channel(cmd_body.parameters[2]);
  _channel_list.push_back(new_channel);
  std::string out;
  out += "JOIN ";
  out += cmd_body.parameters[2];
  _output_buffer += out;
  pfd.events |= POLLOUT;
  it_chan = std::find(
        _channel_list.begin(), _channel_list.end(), cmd_body.parameters[0]);
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

int Bot::handle_join(cmd_obj& cmd_body, struct pollfd& pfd) {
  if (cmd_body.command == "JOIN") {
    std::list<Channel>::iterator it_chan = std::find(
        _channel_list.begin(), _channel_list.end(), cmd_body.parameters[0]);
    if (it_chan == _channel_list.end())
      return (1);
    it_chan->set_status(ACCEPTED);
    return (0);
  }
  else if (cmd_body.parameters[0] == _nick){
    std::list<Channel>::iterator it_chan = std::find(
        _channel_list.begin(), _channel_list.end(), cmd_body.parameters[2]);
    if (it_chan == _channel_list.end())
      return (1);
    it_chan->set_status(ACCEPTED);
    return (0);
  }
  return (1);
}

  int check_for_swears(cmd_obj & cmd_body);
