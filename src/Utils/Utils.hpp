#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

class Server;  // forward declaration
extern Server* g_server;

namespace Utils {

  void signal_handler(int signal);
  bool ft_convert_to_int(int& value, const std::string& to_convert);
  int validate_input(int argc, char** argv, int& port);

};  // namespace Utils

#endif