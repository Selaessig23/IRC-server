/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstracke <mstracke@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 11:03:31 by mstracke          #+#    #+#             */
/*   Updated: 2025/11/06 20:29:57 by mstracke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>  //signal
#include <cstdlib>  //atoi
#include <iostream>
#include "Server/Server.hpp"

/**
 * @param1: port to use
 * @param2: pw required by any IRC client that tries to connect to this
 * webserver
 */

// TODO:
// play around with required (& allowed) c-functions
// ERROR handling using exceptions
// AF_INET == IPv4 | SOCK_STREAM = two-way connection-based byte streams |
// protocl number (if several)
// int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// verify port is int

Server* g_server;

void signal_handler(int signal) {
  std::cout << "SIGNAL: " << signal << std::endl;
  g_server->exit();
  delete g_server;
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
  std::string pw(argv[2]);
  try {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);
    g_server = new Server(port, pw);
    if (g_server->init()) {
      return 1;
    }
  } catch (const std::runtime_error& e) {
    std::cout << "Caught a runtime_error: " << e.what() << '\n';
  }
  return (0);
}
