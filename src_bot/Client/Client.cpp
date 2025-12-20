#include "Client.hpp"
#include <arpa/inet.h>   // htonl
#include <netinet/in.h>  // for socket, bind, listen, accept
#include <poll.h>
#include <sys/socket.h>  // sockaddr_in
#include <unistd.h>      // for close()
#include <cstring>       // memset
#include <iostream>
#include <map>

//hi wh

//
/**
 *
 * @brief the constructor is responsible for the check of the following issues:
 * (1) the data_input gets checked
 * (2) if the connection can be established
 * if there is an error, an exception gets thrown
 * setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 *
 * if there is an error, an exception is thrown
 *
 * Restriction: program only works if it runs on the same host like server
 * (localhost)
 *
 * TODO:
 * (1) close socket in case of an error
 */

Client::Client(int port, std::string pw, std::string data_input) {
  (void)pw;
  (void)data_input;
  // Prepare the address and port for the server socket
  std::memset(&_client_addr, 0, sizeof(_client_addr));
  _client_addr.sin_family = AF_INET;  // IPv4
  _client_addr.sin_addr.s_addr =
      htonl(INADDR_LOOPBACK);  // CONSTANT for 127.0.0.1, localhost
  _client_addr.sin_port = htons(port);
  _client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (_client_fd < 0)
    throw std::runtime_error("Socket creation error.");
  if (connect(_client_fd, reinterpret_cast<struct sockaddr*>(&_client_addr),
              sizeof(_client_addr))) {
    close(_client_fd);
    // params: int sockfd, const struct sockaddr *addr, socklen_t addrlen
    throw std::runtime_error("Connection creation error.");
  }
}

  Client::~Client() {}

  /**
 * @brief this function tries to connect to the server
 * if there is an error while trying to connect to the server,
 * it will be tried 3 times otherwise an exception is thrown
 * it uses the password of the server, 
 * sets the nick to SWEARBOT, and
 * sets user to [...]
 *
 */
  int Client::init(int attempts) {
    // [...']
    (void)attempts;
    return (0);
  }

