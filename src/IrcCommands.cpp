#include "IrcCommands.hpp"

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
int	IrcCommands::PASS(struct parsed_input &input, std::string &to_compare) const
{
	if (input.command == "PASS"
			&& input.parameters.size() == 1
			&& *input.parameters.begin() == to_compare)
		return (1);
	else
		return (0);
}

