/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mstracke <mstracke@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 11:03:31 by mstracke          #+#    #+#             */
/*   Updated: 2025/11/05 12:51:04 by uschmidt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "src/Server/Server.hpp"
#include <cstdlib> //atoi
#include <iostream>

/**
 * @param1: port to use
 * @param2: pw required by any IRC client that tries to connect to this
 * webserver
 */
int main(int argc, char *argv[]) {

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
  Server server = Server();
  if (server.init(port)) {
    return 1;
  }
  if (server.set_pw(argv[2])) {
    return 1;
  }
  // TO-DO's:
  // play around with required (& allowed) c-functions
  // ERROR handling using exceptions
  // AF_INET == IPv4 | SOCK_STREAM = two-way connection-based byte streams |
  // protocl number (if several)
  // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
  return (0);
}
