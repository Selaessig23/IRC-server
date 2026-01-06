#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace Utils {

  bool ft_convert_to_int(int& value, const std::string& to_convert);
  int validate_input(int argc, char** argv, int& port);
};  // namespace Utils

#endif
