# ifndef COMMANDS_HPP
# define COMMANDS_HPP

#include <iostream>
#include "includes/types.hpp"
#include "Client/Client.cpp"

namespace Commands{
	int pass(const struct cmd_obj &cmd, const std::list<Client> client_list, Client curr_client, const std::string &to_check);

# endif // Commands.hpp
