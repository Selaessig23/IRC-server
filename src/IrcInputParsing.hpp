#ifndef IRCINPUTPARSING_HPP
#define IRCINPUTPARSING_HPP

#include <iostream>
#include <vector>

struct parsed_input {
  std::string prefix;
  std::string command;
  std::vector<std::string> parameters;
};

/**
 * @brief class to check the clients input
 * (1) it checks for correct syntax
 * (2) it writes the checked input into parsed_input struct (if input is valid)
 *
 * @return if input is valid, it returns 0, otherwise it returns the error code of parsing
 */
class IrcInputParsing {
 private:
  // [...] helper function to do parsing
 public:
  static int parse_input(std::string to_check, struct parsed_input& to_write);
};

#endif
