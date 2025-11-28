#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"
#include "../../Channel/Channel.hpp"
#include "../../debug.hpp"

/**
 * @return 0, in case of an error it returns error codes:
 * ERR_NEEDMOREPARAMS (461)
 *
 * @return it returns 1 if command and password is correct, otherwise it returns 0
 */
int IrcCommands::join(Server& base, const struct cmd_obj& cmd,
                      int fd_curr_client) {
  std::list<Client>::iterator it = base._client_list.begin();
  for (; it != base._client_list.end(); it++) {
    if (it->get_client_fd() == fd_curr_client)
        DEBUG_PRINT("JOIN1");
        break;
  }
  if (cmd.parameters.empty()) {
    send_message(base, ERR_NEEDMOREPARAMS, true, NULL, *it);
    return (ERR_NEEDMOREPARAMS);
  }
  if (base._channel_list.empty()) {
            Channel NewChannel(cmd.parameters[0]);
            base._channel_list.push_back(NewChannel);
            DEBUG_PRINT("JOIN3");
            return (0);
  }

  std::list<Channel>::iterator iter = base._channel_list.begin();
  if (cmd.parameters.size() >= 1) {
    for (; iter != base._channel_list.end(); iter++) {
        if (iter->get_name() == cmd.parameters[0]) {
            iter->new_member(cmd.client);
            DEBUG_PRINT("JOIN2");
            return (0);
        }
        else {
            Channel NewChannel(cmd.parameters[0]);
            NewChannel.new_member(cmd.client);
            base._channel_list.push_back(NewChannel);
            DEBUG_PRINT("JOIN3");
            return (0);
        }
    }
//   if (cmd.parameters.size() == 1) {  // check needed for if channel already exists

//   } else {
//     std::cout << "Channel already exists" << std::endl; 
//     //send_message(base, ERR_PASSWDMISMATCH, true, NULL, *it);
//     return (464);
  }
  return (0);
}