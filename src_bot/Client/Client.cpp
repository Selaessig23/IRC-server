#include "Client.hpp"
#include <netinet/in.h>  // for socket, bind, listen, accept
#include <poll.h>
#include <sys/socket.h>  // sockaddr_in
#include <cstring>       // memset
#include <iostream>
#include <map>

//hi wh

//
/**
 *
 * @brief the constructor is responsible for the check of the following issues:
 * the data_input gets checked
 * _fd_server = socket(AF_INET, SOCK_STREAM, 0);
 * setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 * if (bind(_fd_server, reinterpret_cast<struct sockaddr*>(&_addr),
 * instead of bind on the client side connect is used
 *
 * if there is an error, an exception is thrown
 *
 * TODO:
 * (1) close socket in case of an error
 */

Client::Client(std::string irc_address, int port, std::string pw,
               std::string data_input) {
  (void)irc_address;
  (void)pw;
  (void)data_input;
  // Prepare the address and port for the server socket
  std::memset(&_client_addr, 0, sizeof(_client_addr));
  _client_addr.sin_family = AF_INET;  // IPv4
  _client_addr.sin_addr.s_addr =
      htonl(INADDR_LOOPBACK);  // 127.0.0.1, localhost
  _client_addr.sin_port = htons(port);
  _client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (_client_fd < 0)
    throw std::runtime_error("Socket creation error.");
}

Client::~Client() {}

/**
 * @brief this function tries to connect to the server
 * if there is an error while trying to connect to the server,
 * it will be tried 3 times otherwise an exception is thrown
 */
int Client::init(int attempts) {
  // [...']
  (void)attempts;
  return (0);
}

