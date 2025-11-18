#include "../includes/types.hpp"

namespace Parsing {
  int parseCommand(std::string input, command_obj& command_body) {
    int space = input.find(' ');
    std::vector<std::string> parsed_elements;
    while (space >= 0) {
      parsed_elements.push_back(input.substr(0, space));
      input.replace(0, space, "");
    }
    if (parsed_elements.size() == 0) {
      command_body.error = PARSE_ERR::EMPTY_CMD;
      return command_body.error;
    }
    return 0;
  };
}  // namespace Parsing
