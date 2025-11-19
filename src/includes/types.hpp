#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>

// IMPORTANT. Unknown serves as the enums size as well, therefore it has to stay the last element
enum CMD_TYPE {
  PRIVMSG,
  PASS,
  JOIN,
  NICK,
  CAP,
  UNKNOWN,
};

enum PARSE_ERR {
  NO_ERR = 0,
  SYNTHAX = 100,
  UNKNOWN_CMD = 200,
  EMPTY_CMD = 300,
  ERR_INPUTTOOLONG = 417,
};

struct cmd_obj {
  PARSE_ERR error;
  std::vector<std::string> tags;
  std::string prefix;
  CMD_TYPE command;
  std::vector<std::string> parameters;
};

#endif  //TYPES_HPP
