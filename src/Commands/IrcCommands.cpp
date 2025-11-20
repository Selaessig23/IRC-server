#include "IrcCommands.hpp"
#include <map>
#include "../includes/types.hpp"

IrcCommands::IrcCommands() {
  this->_irc_commands.insert(std::make_pair(PASS, &Commands::pass));
}

/**
 * @brief class to check the parsed input of client for command
 * (1) it checks which command is enquired by client
 * (2) it runs the commands for parsing
 *
 * @return it returns 1 if command was not found
 */
int IrcCommands::exec_command(struct cmd_obj &cmd, std::list<Client> &client_list,
                              int fd_curr_client, const std::string& to_check) {
  function to_execute;
  std::map<enum CMD_TYPE, function>::iterator it = _irc_commands.find(cmd.command); 
  if (it == _irc_commands.end())
    //error: command not found
    return (1);
  else
    to_execute = _irc_commands.find(cmd.command)->second;
  to_execute(cmd, client_list, fd_curr_client, to_check);
}
