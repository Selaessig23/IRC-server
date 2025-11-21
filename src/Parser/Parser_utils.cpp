#include <map>
#include <sstream>
#include "../includes/types.hpp"

bool isValidNumericCommand(const std::string& str) {
  if (str.length() != 3)
    return false;

  for (size_t i = 0; i < str.length(); i++) {
    if (!std::isdigit(str[i]))
      return false;
  }
  return true;
}

std::map<std::string, CMD_TYPE> build_command_map(void) {
  std::map<std::string, CMD_TYPE> commands;
  commands["PRIVMSG"] = PRIVMSG;
  commands["PASS"] = PASS;
  commands["JOIN"] = JOIN;
  commands["NICK"] = NICK;
  commands["CAP"] = CAP;
  return commands;
}
