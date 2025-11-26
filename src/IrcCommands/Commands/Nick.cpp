#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief functin to create or change the nickname of a certain client
 *
 * TODO:
 * (1) think about client feedback in case of success
 * (2) think about if all other clients should get a feedback in case
 * of the client has changed his nick (maybe only as a channel notice)
 *
 * @return 0, in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS (461)
 * ERR_ALREADYREGISTERED (462)
 * ERR_PASSWDMISMATCH (464)
 *  ->chars that introduce ambiguity in other commands 
 *    (prefix, tags, channels, spaces)
 *
 * not in use: 
 * ERR_NICKCOLLISION (436) ->only relevant for multi-server setup
 */
int IrcCommands::nick(Server& base, const struct cmd_obj& cmd,
                      int fd_curr_client) {
  std::list<Client>::iterator it_client = base._client_list.begin();
  for (; it_client != base._client_list.end(); it_client ++) {
    if (it_client->get_client_fd() == fd_curr_client)
      break;
  }
  //check for parameters
  if (cmd.parameters.empty())
	  send_message(base, ERR_NONICKNAMEGIVEN, true, NULL, *it_client);
  //check if there are chars within the nickname that are not allowed
  for (std::string::iterator it; it != cmd.parameters[0].end(); it++){
	  if (*it == ' ' || *it == ':' || *it == '#' || *it == '&' || *it == '@'){
	   send_message(base, ERR_NONICKNAMEGIVEN, true, NULL, *it_client);
	   return(ERR_NONICKNAMEGIVEN);
	  }
  }
  //check if nickname already exists
  for (std::list<Client>::iterator it = base._client_list.begin(); it != base._client_list.end(); it ++) {
    if (it_client->get_nick() == cmd.parameters[0]){
	   send_message(base, ERR_ALREADYREGISTERED, true, NULL, *it_client);
	   return(ERR_NONICKNAMEGIVEN);
    }
  }
  std::string nick_old = it_client->get_nick();
  it_client->set_nick(*cmd.parameters.begin());
  if (it_client->get_nick().empty())
    send_message(base, RPL_INTERN_SETNICK, false, NULL, *it_client);
  else
    send_message(base, RPL_INTERN_CHANGENICK, false, &nick_old, *it_client);
  return(0);
}
