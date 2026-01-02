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

int check_for_invitation(cmd_obj& cmd_body);
int check_for_swears(cmd_obj& cmd_body);
