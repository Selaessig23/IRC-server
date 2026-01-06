/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstracke <mstracke@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 11:03:31 by mstracke          #+#    #+#             */
/*   Updated: 2026/01/06 11:18:46 by mstracke         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>  //signal
#include <cstdlib>  //atoi
#include <iostream>
#include "Bot/Bot.hpp"
#include "debug.hpp"
#include "includes/CONSTANTS.hpp"
#include "Utils/Utils.hpp"

Bot* g_bot;

void signal_handler(int signal) {
  (void)signal;
  DEBUG_PRINT("Exit of client caused by signal: " << signal);
  delete g_bot;
  exit(0);
}

int main(int argc, char* argv[]) {
  int port;
// test debug-mode
#ifdef DEBUG
  std::cout << "[DEBUG] Debug mode of bot is ON" << std::endl;
#endif
  if (Utils::validate_input(argc, argv, port))
    return (1);
  std::string pw(argv[2]);
  std::string data_input(
      argv[3]);  //to verify, maybe when constructing the object
  try {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);
    g_bot = new Bot(port, pw, data_input);
    if (g_bot->init_poll()) {
      delete g_bot;
      return 1;
    }
  } catch (const std::runtime_error& e) {
    std::cout << "Caught a runtime_error: " << e.what() << '\n';
  } catch (const std::invalid_argument& e) {
    std::cout << "Caught an error caused by an invalid argument: " << e.what()
              << '\n';
  }
  return (0);
}
