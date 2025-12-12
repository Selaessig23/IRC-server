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

Client* g_client;

void signal_handler(int signal) {
  (void)signal;
  DEBUG_PRINT("Exit of client caused by signal: " << signal);
  delete g_cient;
  exit(0);
}

int validate_input(int argc, char** argv, int& port) {
  if (argc != 5) {
    std::cerr << "Check number of arguments. Required 3: address of server | portno | pw | path of swear.csv"
              << std::endl;
    return (1);
  }
  port = std::atoi(argv[2]);
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
  std::cout << "[DEBUG] Debug mode of bot is ON" << std::endl;
#endif
  if (validate_input(argc, argv, port))
    return (1);
  std::string irc_adress(argv[1]); //to verify, maybe when constructing the object
  std::string pw(argv[3]);
  std::string data_input(argv[4]); //to verify, maybe when constructing the object
  try {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);
    g_client = new Client(irc_address, port, pw, data_input);
    if (g_client->init()) {
      delete g_client;
      return 1;
    }
  } catch (const std::runtime_error& e) {
    std::cout << "Caught a runtime_error: " << e.what() << '\n';
  }
  return (0);
}
