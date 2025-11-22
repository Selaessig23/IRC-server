#include <iostream>
#include <sstream>
#include "../Client/Client.hpp"
#include "Commands.hpp"
#include "../includes/types.hpp"

/**
 * @brief function to return the error message of errorcode
 *
 * TODO
 * (1) add all required error message to corresponding error codes
 * (2) ADD Command struct as parameter and fill out where required
 */
static std::string getError(enum PARSE_ERR err) {
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
 * @brief function to sent out the error messages
 * to a specific client
 *
 * TODO
 * (1) set tag (only if clients support them, to check with CAP LS negotiation)
 * (2) set prefix: usually servers name (consider key ":" to set prefix)
 * (3) ADD 3-number digit (error-code)
 */
void Commands::ft_errorprint(enum PARSE_ERR err, Client& curr_client) {
  std::string out;
  int error_code = static_cast<int>(err);
  std::stringstream ss;
  ss << error_code;

  out += ":MUM_s_server";  // servers name
  out += " " + ss.str();   // numeric reply
  if (!curr_client.getNick().empty())
    out = " <" + curr_client.getNick() + ">";
  out += getError(err);
  out += "\r\n";
  curr_client.setClientOut(out);
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
  Commands::ft_errorprint(ERR_NOTREGISTERED, to_check);
  return (0);
}
