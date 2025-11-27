#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

int IrcCommands::authenticate(Server& base, const struct cmd_obj& cmd,
                              int fd_curr_client) {
  std::list<Client>::iterator client = base._client_list.begin();
  for (; client != base._client_list.end(); client++) {
    if (client->get_client_fd() == fd_curr_client)
      break;
  }
  std::string pw = cmd.parameters[0];
  std::string response = "START AUTH: " + pw;
  send_message(base, RPL_NONE, false, &response, *client);
  return (0);
}
