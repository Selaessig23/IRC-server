#include <iostream>
#include "Commands/Commands.hpp"
#include "includes/types.hpp"
#include "Client/Client.hpp"

/**
 * @brief function to check if client enters the correct password
 * while trying to connect to server 
 *
 * TODO: return correct error code & message in case of an error
 * ERR_NEEDMOREPARAMS (461)
 * ERR_ALREADYREGISTERED (462)
 * ERR_PASSWDMISMATCH (464)
 *
 * @return it returns 1 if command and password is correct, otherwise it returns 0
 */
int Commands::pass(const struct cmd_obj &cmd, const std::list<Client> &clients, const Client &curr_client, const std::string &to_check)
{
	if (curr_client.register == 1)
		return (462);
	if (cmd.parameters.empty())
		return (461);
	if (input.command == "PASS"
			&& input.parameters.size() == 1
			&& *input.parameters.begin() == to_compare)
		return (0);
	else
		return (464);
}

