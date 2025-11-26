/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstracke <mstracke@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 11:03:31 by mstracke          #+#    #+#             */
/*   Updated: 2025/11/20 16:12:57 by mstracke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>  //signal
#include <cstdlib>  //atoi
#include <iostream>
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

int main(int argc, char* argv[]) {

// test debug-mode
#ifdef DEBUG
  std::cout << "[DEBUG] Debug mode is ON" << std::endl;
#endif
  if (argc != 3) {
    std::cerr << "Check number of arguments. Required 2: portno | pw"
              << std::endl;
    return (1);
  }
  int port = std::atoi(argv[1]);
  if (port < PORT_MIN || port > PORT_MAX) {
    std::cerr << "Provided port no " << port << " out of range!" << std::endl;
    return (1);
  }
  std::string pw(argv[2]);
  try {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);
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
