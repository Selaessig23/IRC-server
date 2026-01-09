#include "Utils.hpp"
#include <cstdlib>  // for exxit
#include <climits>  //for INT_MIN / INT_MAX
#include <iostream>
#include <sstream>  //for std::stringstream
#include "../Bot/Bot.hpp"
#include "../debug.hpp"
#include "../includes/CONSTANTS.hpp"

extern Bot* g_bot;

void Utils::signal_handler(int signal) {
  (void)signal;
  DEBUG_PRINT("Exit of client caused by signal: " << signal);
  delete g_bot;
  exit(0);
}

/**
 * @brief function to convert a string to int
 * it checks for max-min values before conversion
 * it checks for leftover junk
 * it does not skip leading whitespaces
 * it ignores trailing whitespaces
 * 
 * @param value a reference to the int value that should be used for converted value
 * @param to_convert the string that has to be converted to int
 *
 * @return returns true if succesful, false if converions failed
 */
bool Utils::ft_convert_to_int(int& value, const std::string& to_convert) {
  std::stringstream ss(to_convert);
  long long temp;
  char c;

  if (!(ss >> temp)) {
    return (false);
  }
  if (ss >> c) {
    return (false);
  }
  if (temp > INT_MAX || temp < INT_MIN) {
    return (false);
  }
  value = static_cast<int>(temp);
  return (true);
}

/**
 * @brief function to validate the input parameters
 */
int Utils::validate_input(int argc, char** argv, int& port) {
  if (argc != 4) {
    std::cerr << "Check number of arguments. Required 3: portno | pw | path of "
                 "swear.csv"
              << std::endl;
    return (1);
  }
  ft_convert_to_int(port, argv[1]);
  if (port < PORT_MIN || port > PORT_MAX) {
    std::cerr << "Provided port no " << port << " out of range!" << std::endl;
    return (1);
  }
  return (0);
};

