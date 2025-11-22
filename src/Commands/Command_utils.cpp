#include <iostream>
#include <sstream>
#include "../Client/Client.hpp"
#include "Commands.hpp"
#include "../includes/types.hpp"

/**
 * @brief function to create reply-messages from server to client
 *
 * TODO
 * (1) add all required rpl messags according to rpl_code
 * (2) as soon as it becomes part of friend: add required paramters
 */
static std::string get_rpl(enum RPL_MSG rpl) {
  std::string out;
  switch (rpl) {
    case RPL_WELCOME:
      return (" :Welcome to the <networkname> Network, <nick>[!<user>@<host>]");
    case RPL_YOURHOST:
      return (" :Your host is <servername>, running version <version>");
    case RPL_CREATED:
      return (" This server was created <datetime>");
  }
}

/**
 * @brief function to return the error message of errorcode
 *
 * TODO
 * (1) add all required error messages to corresponding error codes
 * (2) as soon as it becomes part of friend: add required paramters
 */
static std::string get_error(enum PARSE_ERR err) {
  std::string out;
  switch (err) {
    case EMPTY_CMD:
      return (" Command empty");
    case ERR_INPUTTOOLONG:
      return " :Input line was too long";
    case ERR_UNKNOWNCOMMAND:
      return (" <command> :Unknown command");
    case ERR_NOTREGISTERED:
      return (" :You have not registered");
    case ERR_NEEDMOREPARAMS:
      return " :<command> :Not enough parameters";
    case ERR_ALREADYREGISTERED:
      return " :You may not reregister";
    case ERR_PASSWDMISMATCH:
      return " :Password incorrect";
    default:
      return (out);
  }
}

/**
 * @brief function to sent out messages from server to client
 *
 * TODO
 * (1) set tag (only if clients support them, to check with CAP LS negotiation)
 * (2) as soon as it becomes part of friend: use private attributes of Server directly
 */
void Commands::send_message(int numeric_msg_code, bool error, Client& curr_client) {
  std::string out;
  std::stringstream ss;
  ss << numeric_msg_code;

  out += ":MUMs_server";  // servers name
  out += " " + ss.str(); 
  if (!curr_client.getNick().empty())
    out = " <" + curr_client.getNick() + ">";
  if (error == true)
	  out += get_error(static_cast<PARSE_ERR>(numeric_msg_code));
  else
	  out+= get_rpl(static_cast<RPL_MSG>(numeric_msg_code));
  out += "\r\n";
  curr_client.addClientOut(out);
  curr_client.setServerPoll();
}

/**
 * @brief function to check if the client is already registered by 
 * server-access-password
 * it sends an error message to client in case if not
 * 451 ERR_NOTREGISTERED
 *
 * @return 1 in case of registered, 0 if not
 */
bool Commands::client_register_check(Client& to_check) {
  if (to_check.getRegisterStatus() == 1)
    return (1);
  // send  error to Client
  Commands::send_message(ERR_NOTREGISTERED, true, to_check);
  return (0);
}
