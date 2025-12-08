#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to respond to PING command from client
 *
 * TODO
 * (1) Think about behaviour if client is not registered yet
 *     (normally clients should not send PING as long as they are not registered, but what if yes)
 */
int IrcCommands::pong(Server& base, const struct cmd_obj& cmd) {
  std::string out;
  out += ":";
  out += base._server_name + " ";
  out += "PONG";
  out += " ";
  out += base._server_name;
  if (!cmd.parameters.empty()) {
    out += " ";
    out += cmd.parameters[0];
  }
  cmd.client->add_client_out(out);
  base.set_pollevent(cmd.client->get_client_fd(), POLLOUT);
  return (0);
}
