#include <map>
#include <sstream>
#include "../includes/types.hpp"

namespace Parsing {
  /**
 * @brief breaks down incomming string into cmd and params
 * (1) tokenizes the string
 * (2) validates and first error handling
 */
  int ParseCommand(std::string input, cmd_obj& command_body) {
    std::istringstream input_stream(input);
    std::string token;
    std::vector<std::string> parsed_elements;

    command_body.error = NO_ERR;
    while (input_stream >> token) {
      parsed_elements.push_back(token);
    }

    if (parsed_elements.empty()) {
      command_body.error = EMPTY_CMD;
      return command_body.error;
    }
    std::map<std::string, CMD_TYPE> commands;
    commands["PRIVMSG"] = PRIVMSG;
    commands["PASS"] = PASS;
    commands["JOIN"] = JOIN;
    commands["NICK"] = NICK;
    commands["CAP"] = CAP;

    std::map<std::string, CMD_TYPE>::iterator it;
    for (it = commands.begin(); it != commands.end(); ++it) {
      if (parsed_elements[0] == it->first) {
        command_body.command = it->second;
        break;
      }
    }

    if (command_body.command == UNKNOWN) {
      command_body.error = UNKNOWN_CMD;
      return command_body.error;
    }

    return NO_ERR;
  }
}  // namespace Parsing
