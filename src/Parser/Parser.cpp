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

    while (input_stream >> token) {
      parsed_elements.push_back(token);
    }

    if (parsed_elements.empty()) {
      command_body.error = EMPTY_CMD;
      return command_body.error;
    }

    command_body.error = NO_ERR;
    command_body.command = UNKNOWN;
    return NO_ERR;
  }
}  // namespace Parsing
