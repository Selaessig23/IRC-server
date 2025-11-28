#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to to send private messages between users, 
 * as well as to send messages to channels
 * IRC syyntax:
 * Command: PRIVMSG
 * Parameters: <target>{,<target>} <text to be sent>
 *
 * TODO
 * (1) check if NOTEXTTOSEND also gets sent if std is empty (current implementation)
 *
 * numeric replies:
 *    ERR_NOSUCHNICK (401)
 *    ERR_NOSUCHSERVER (402)
 *    ERR_CANNOTSENDTOCHAN (404)
 *    ERR_TOOMANYTARGETS (407)
 *    ERR_NORECIPIENT (411)
 *    ERR_NOTEXTTOSEND (412)
 *    ERR_NOTOPLEVEL (413)
 *    ERR_WILDTOPLEVEL (414)
 *    RPL_AWAY (301)
 *
 * @return 0, in case of an error it returns error codes:
 * [...]
 */
int IrcCommands::privmsg(Server& base, const struct cmd_obj& cmd,
                      int fd_curr_client) {
  std::list<Client>::iterator it_client = base._client_list.begin();
  for (; it_client != base._client_list.end(); it_client++) {
    if (it_client->get_client_fd() == fd_curr_client)
      break;
  }

  if (cmd.parameters.empty()) {
    send_message(base, ERR_NORECIPIENT, true, NULL, *it_client);
    return (ERR_NORECIPIENT);
  }

  std::vector<std::string> recipients;
  std::vector<std::string>::const_iterator it_para = cmd.parameters.begin();
  for (; it_para != cmd.parameters.end() && *it_para->begin() == ","; it_para++){
	  recipients.push_back(*it_para);
  }

  if(it_para == cmd.parameters.end() && it_para->empty()){
    send_message(base, ERR_NOTEXTTOSEND, true, NULL, *it_client);
    return (ERR_NOTEXTTOSEND);
  }

  const std::string &msg = *it_para;


  for (std::list<Client>::iterator it = base._client_list.begin();
       it != base._client_list.end(); it++) {
    if (it != it_client && !it->get_nick().empty() &&
        it->get_nick() == cmd.parameters[0]) {
      send_message(base, ERR_NICKNAMEINUSE, true, NULL, *it_client);
      return (ERR_NICKNAMEINUSE);
    }
  }

  std::string nick_old = it_client->get_nick();
  it_client->set_nick(*cmd.parameters.begin());
  if (nick_old.empty())
    send_message(base, RPL_INTERN_SETNICK, false, NULL, *it_client);
  else
    send_message(base, RPL_INTERN_CHANGENICK, false, &nick_old, *it_client);
  return (0);
}
