#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>

enum CMD_TYPE {
  UNKNOWN = -1,
  PRIVMSG,
  PASS,
  JOIN,
};

enum PARSE_ERR {
  NO_ERR = 0,
  SYNTHAX = 100,
  UNKNOWN_CMD = 200,
  EMPTY_CMD = 300,
};

struct command_obj {
  PARSE_ERR error;
  std::string prefix;
  CMD_TYPE command;
  std::vector<std::string> parameters;
};

#endif  //TYPES_HPP
