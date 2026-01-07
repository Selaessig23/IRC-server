/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstracke <mstracke@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 11:03:31 by mstracke          #+#    #+#             */
/*   Updated: 2026/01/07 18:30:20 by mstracke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <climits>  //for INT_MIN / INT_MAX
#include <csignal>  //signal
#include <cstdlib>
#include <iostream>
#include <sstream>  //for std::stringstream
#include "Server/Server.hpp"
#include "debug.hpp"
#include "includes/CONSTANTS.hpp"

// TODO:
// AF_INET == IPv4 | SOCK_STREAM = two-way connection-based byte streams |
// protocl number (if several)

Server* g_server;

void signal_handler(int signal) {
  (void)signal;
  DEBUG_PRINT("Shutdown of server caused by signal: " << signal);
  delete g_server;
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
bool ft_convert_to_int(int& value, const std::string& to_convert) {
  std::stringstream ss(to_convert);
  long long temp;
  char c;

  if (!(ss >> temp)) {
    std::cerr << "Invalid int: $" << to_convert << "$" << std::endl;
    return (false);
  }
  if (ss >> c) {
    std::cerr << "Invalid int: $" << to_convert << "$" << std::endl;
    return (false);
  }
  if (temp > INT_MAX || temp < INT_MIN) {
    std::cout << "Invalid int, out of int range: " << temp << std::endl;
    return (false);
  }
  value = static_cast<int>(temp);
  return (true);
}

int validate_input(int argc, char** argv, int& port) {
  if (argc != 3) {
    std::cerr << "Check number of arguments. Required 2: portno | pw"
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

int main(int argc, char* argv[]) {

  int port;
// test debug-mode
#ifdef DEBUG
  std::cout << "[DEBUG] Debug mode is ON" << std::endl;
#endif
  if (validate_input(argc, argv, port))
    return (1);
  std::string pw(argv[2]);
  try {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGPIPE, SIG_IGN);
    g_server = new Server(port, pw);
    if (g_server->init()) {
      delete g_server;
      return 1;
    }
  } catch (const std::runtime_error& e) {
    std::cout << "Caught a runtime_error: " << e.what() << '\n';
  }
  return (0);
}
