#include <iomanip>  // For std::setw and std::setfill
#include <iostream>
#include <sstream>
#include "../Client/Client.hpp"
#include "../Server/Server.hpp"
#include "../includes/CONSTANTS.hpp"
#include "../includes/types.hpp"
#include "IrcCommands.hpp"

/**
 * @brief function to return the reply message of replymessagecode (rpl)
 *
 * TODO
 * (1) add all required rpl messags according to rpl_code
 */
std::string IrcCommands::get_rpl(Server& base, enum RPL_MSG rpl) {
  std::string out;
  switch (rpl) {
    case RPL_WELCOME:
      return (" :Welcome to the " + base._network_name + " Network, " +
              "<nick>[!<user>@<host>]");
    case RPL_YOURHOST:
      return (" :Your host is " + base._server_name + " , running version " +
              base._version);
    case RPL_CREATED:
      return (" :This server was created " + base._created_at);
    case RPL_INTERN_SETNICK:
      return (" :Nickname was set.");
    case RPL_INTERN_CHANGENICK:
      return (" :Nickname was changed to ");
    case RPL_INTERN_SETUSER:
      return (
          " :No ident server\nUser gets registered with username\n~ _username "
          "and real name _realname");
    default:
      return (" UNKNOWN REPLY");
  }
}

/**
 * @brief function to return the error message of errorcode (err)
 *
 * TODO
 * (1) add all required error messages to corresponding error codes
 */
std::string IrcCommands::get_error(Server& base, enum PARSE_ERR err) {
  (void)base;
  std::string out;
  switch (err) {
    case EMPTY_CMD:
      return (" Command empty");
    case ERR_NOSUCHNICK:
      return ("<client> <nickname> :No such nick/channel");
    case ERR_CANNOTSENDTOCHAN:
      return ("<client> <channel> :No such channel");
    case ERR_NORECIPIENT:
      return ("<client> :No recipient given (<command>)");
    case ERR_NOTEXTTOSEND:
      return ("<client> :No text to send");
    case ERR_INPUTTOOLONG:
      return (" :Input line was too long");
    case ERR_UNKNOWNCOMMAND:
      return (" <command> :Unknown command");
    case ERR_NONICKNAMEGIVEN:
      return (" <client> :No nickname given");
    case ERR_ERRONEUSNICKNAME:
      return (" <client> <nick> :Erroneous nickname");
    case ERR_NICKNAMEINUSE:
      return (" <client> <nick> :Nickname is already in use");
    case ERR_NICKCOLLISION:
      return (" <client> <nick> :Nickname collision KILL from <user>@<host>");
    case ERR_NOTREGISTERED:
      return (" :You have not registered");
    case ERR_NEEDMOREPARAMS:
      return (" :<command> :Not enough parameters");
    case ERR_ALREADYREGISTERED:
      return (" :You may not reregister");
    case ERR_PASSWDMISMATCH:
      return (" :Password incorrect");
    default:
      return (out);
  }
}

/**
 * @brief function to sent out direct messages as answer on COMMAND-requests from client
 * direct communication from server to client, 
 * whether error-msg or rpl-messages (success)
 *
 * TODO
 * (1) set tag (only if clients support them, to check with CAP LS negotiation)
 * (2) think about changing the parameters
 */
void IrcCommands::send_message(Server& base, int numeric_msg_code, bool error,
                               std::string* msg, Client& curr_client) {
  std::string out;
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << numeric_msg_code;

  out += ":";
  out += base._server_name;
  if (!msg)
    out += " " + ss.str();
  else
    out += " ";
  if (!curr_client.get_nick().empty())
    out = " <" + curr_client.get_nick() + ">";
  if (msg)
    out += *msg;
  else if (error == true)
    out += get_error(base, static_cast<PARSE_ERR>(numeric_msg_code));
  else
    out += get_rpl(base, static_cast<RPL_MSG>(numeric_msg_code));
  out += "\r\n";
  curr_client.add_client_out(out);
  base.set_pollevent(curr_client.get_client_fd(), POLLOUT);
}

/**
 * @brief function to check if the client is already registered by 
 * server-access-password, nickname and user
 * it sends an error message to client in case if not
 * 451 ERR_NOTREGISTERED
 *
 * @return 1 in case of registered, 0 if not
 */
bool IrcCommands::client_register_check(Server& base, Client& to_check) {
  (void)base;
  unsigned char reg_status = to_check.get_register_status();
  unsigned char registered = PASS | NICK | USER;
  if (reg_status == registered)
    return (1);
  else
    return (0);
}
