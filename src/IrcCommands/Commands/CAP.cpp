#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

int IrcCommands::cap(Server& base, const struct cmd_obj& cmd,
                     int fd_curr_client) {
  std::list<Client>::iterator client = base._client_list.begin();
  for (; client != base._client_list.end(); client++) {
    if (client->get_client_fd() == fd_curr_client)
      break;
  }
  std::string response;
  if (cmd.parameters[0] == "LS") {
    response = RES_CAP_LS;
    client->set_auth_state(AUTH_CAP);
  } else if (cmd.parameters[0] == "REQ" &&
             cmd.parameters[1].find("sasl") != std::string::npos) {
    client->set_auth_state(AUTH_SASL);
    std::cout << "SASL" << std::endl;
    std::string response_1 = RES_CAP_SASL;
    send_message(base, RPL_NONE, false, &response_1, *client);
    response = RES_AUTH;
  } else if (cmd.parameters[0] == "END") {
    client->set_auth_state(AUTH_CAP_DONE);
  } else {
    response = RES_CAP_UNKNOWN;
    send_message(base, RPL_NONE, false, &response, *client);
    return SYNTHAX;
  }
  send_message(base, RPL_NONE, false, &response, *client);
  return (0);
}
