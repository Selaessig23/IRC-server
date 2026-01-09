/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstracke <mstracke@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 11:03:31 by mstracke          #+#    #+#             */
/*   Updated: 2026/01/09 15:26:20 by mstracke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <climits>  //for INT_MIN / INT_MAX
#include <csignal>  //signal
#include <cstdlib>
#include <iostream>
#include <sstream>  //for std::stringstream
#include "Server/Server.hpp"
#include "Utils/Utils.hpp"
#include "debug.hpp"
#include "includes/CONSTANTS.hpp"

Server* g_server;

int main(int argc, char* argv[]) {

  int port;
// test debug-mode
#ifdef DEBUG
  std::cout << "[DEBUG] Debug mode is ON" << std::endl;
#endif
  if (Utils::validate_input(argc, argv, port))
    return (1);
  std::string pw(argv[2]);
  try {
    signal(SIGINT, Utils::signal_handler);
    signal(SIGTERM, Utils::signal_handler);
    signal(SIGQUIT, Utils::signal_handler);
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
