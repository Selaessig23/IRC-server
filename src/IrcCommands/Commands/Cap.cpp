#include <iostream>
#include <list>
#include "../../Client/Client.hpp"
#include "../../Server/Server.hpp"
#include "../../includes/CONSTANTS.hpp"
#include "../../includes/types.hpp"
#include "../IrcCommands.hpp"

/**
 * @brief function to handle CAP LS negotiation, as some versions of clients like
 * irssi v1.2.3 are waiting for this specific response
 */
int IrcCommands::cap(Server& base, const struct cmd_obj& cmd) {
  //   std::string response;
  //   if (cmd.parameters[0] == "END") {
  //     if (cmd.client->get_nick().empty())
  //       response = "CAP *";
  //     else
  //       response = "CAP " + cmd.client->get_nick();
  //     response += " END";
  //     send_message(base, cmd, RPL_NONE, false, &response);
  //   } else {
  //     send_message(base, cmd, ERR_INVALIDCAPCMD, true, NULL);
  //     return (ERR_INVALIDCAPCMD);
  //   }
  if (cmd.parameters[0] != "END") {
    send_message(base, cmd, ERR_INVALIDCAPCMD, true, NULL);
    return (ERR_INVALIDCAPCMD);
  }
  return (0);
}
