#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>

struct command_obj {
  int error;
  std::string prefix;
  std::string command;
  std::vector<std::string> parameters;
};

enum PARSE_ERR { SYNTHAX = 100, UNKNOWN_CMD = 200 };

#endif  //TYPES_HPP
