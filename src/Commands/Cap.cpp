#include "Commands.hpp"
#include "../includes/types.hpp"

/**
 * @brief function to handle the CAP command
 *
 */
int Cap(const struct cmd_obj &cmd, int fd_curr_client)
{
  std::list<Client>::iterator it = clients.begin();
  for (; it != clients.end(); it++) {
    if (it->getClientFd() == fd_curr_client)
      break;
  }
  if (!Commands::client_register_check(*it))
	  return (0);
  if (cmd.command == "CAP")
	  

}
