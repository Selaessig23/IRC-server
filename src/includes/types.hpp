#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>

struct parsed_input {
  int error;
  std::string prefix;
  std::string command;
  std::vector<std::string> parameters;
};

#endif  //TYPES_HPP
