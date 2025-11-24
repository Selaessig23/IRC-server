#ifndef PARSING_HPP
#define PARSING_HPP

#include "../includes/types.hpp"

/**
 * @brief namespace including all the pre executon parsing of incoming commands
 * (1) it checks for correct syntax
 * (2) it writes the checked input into parsed_input struct (if input is valid)
 *
 * @return if input is valid, it returns 0, otherwise it returns the error code
 * of parsing
 */
namespace Parsing {
  PARSE_ERR parse_command(std::string input, cmd_obj& command_obj);
};

#endif  //PARSING_HPP
