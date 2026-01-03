#include <string>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to respond to PING command from client
 * it also answers with PONG if client was not yet registered
 */
int IrcCommands::pong(Server& base, const struct cmd_obj& cmd) {
  std::string out;
  out += ":";
  out += base._server_name + " ";
  out += "PONG ";
  out += base._server_name;
  if (!cmd.parameters.empty()) {
    out += " ";
    out += cmd.parameters[0];
  }
  out += "\r\n";
  cmd.client->add_client_out(out);
  base.set_pollevent(cmd.client->get_client_fd(), POLLOUT);
  return (0);
}
