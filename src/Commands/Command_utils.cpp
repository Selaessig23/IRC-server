#include <iostream>
#include "../Client/Client.hpp"
#include "Commands.hpp"

/**
 * @brief function to return the error message of errorcode
 *
 * TODO
 * (1) add all required error message to corresponding error codes
 * (2) ADD Command struct as parameter and fill out where required
 */
static std::string getError(enum PARSE_ERR err) {
  std::string out;
  if (err == ERR_NOTREGISTERED)
    out = " :You have not registered";
  else if (err == EMPTY_CMD)
    out = "Command empty";
  else if (err == ERR_UNKNOWNCOMMAND)
    out = "<command> :Unknown command";
  else if (err == ERR_ALREADYREGISTERED)
    out = " :You may not reregister";
  else if (err == ERR_INPUTTOOLONG)
    out = " :Input line was too long";
  else if (err == ERR_PASSWDMISMATCH)
    out = " :Password incorrect";
  else if (err == ERR_NEEDMOREPARAMS)
    out = " :<command> :Not enough parameters";
  return (out);
}

/**
 * @brief function to sent out the error messages
 * to a specific client
 *
 * TODO
 * (1) set tag
 * (2) set prefix
 * (3) ADD 3-number digit (error-code)
 */
void Commands::ft_errorprint(enum PARSE_ERR err, Client& curr_client) {
  std::string out;
  if (!curr_client.getNick().empty())
    out = "<" + curr_client.getNick() + ">";
  out += getError(err);
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
