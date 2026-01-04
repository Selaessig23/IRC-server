#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to handle CAP LS negotiation
 *
 * some versions of clients like irssi v1.2.3 are waiting for this specific response:
 * if (cmd.parameters[0] != "END") {
 *     send_message(base, cmd, ERR_INVALIDCAPCMD, cmd.client, NULL);
 *     return (ERR_INVALIDCAPCMD);
 * }
 *
 * TODO:
 * (1) Think about which version to use
 */
int IrcCommands::cap(Server& base, const struct cmd_obj& cmd) {
  if (cmd.parameters.empty()) {
    send_message(base, cmd, ERR_INVALIDCAPCMD, cmd.client, NULL);
    return (ERR_INVALIDCAPCMD);
  }
  if (cmd.parameters[0] != "END") {
    send_message(base, cmd, ERR_INVALIDCAPCMD, cmd.client, NULL);
    return (ERR_INVALIDCAPCMD);
  }
  std::string response;
  if (cmd.parameters[0] != "END") {
    if (cmd.client->get_nick().empty())
      response = "CAP *";
    else
      response = "CAP " + cmd.client->get_nick();
    response += " LS";
    response += "\r\n";
    cmd.client->add_client_out(response);
    base.set_pollevent(cmd.client->get_client_fd(), POLLOUT);
    return (0);
  }
  return (0);
}
