#include "Server.hpp"
#include <arpa/inet.h>  // for inet_ntoa()
#include <fcntl.h>
#include <netinet/in.h>  //for socket, bind, listen, accept
#include <poll.h>
#include <stdlib.h>
#include <sys/socket.h>  //sockaddr_in
#include <unistd.h>      //for close()
#include <cstring>       //memset
#include <iostream>
#include <stdexcept>  // to throw exceptions for runtime
#include <vector>
#include "CONSTANTS.hpp"
#include "Client.hpp"
#include "debug.hpp"

// TODO: validate Port num
// On Unix/Linux, binding to ports <1024 usually requires root privileges.
// Ports like 53 (DNS) or 123 (NTP) are either UDP or reserved
// Ephemeral ports (49152–65535): These are meant for client-side connections,
// not servers. 	Technically, you can bind a server here, but clients
// might have trouble 	connecting due to firewall/NAT setups. Ports
// that are already in use by another service will throw an error when trying
// to bind to them
// destroy socket on error and in destructor

Server::~Server() {
  DEBUG_PRINT("Destructor of Server called.");
  for (fd_iterator it = _poll_fds.begin() + 1; it != _poll_fds.end(); it++) {
    close(it->fd);
  }
  //Close Server last
  close(_poll_fds.begin()->fd);
}

Server::Server(int port, std::string& pw) {
  _port = port;
  _fd_server = socket(AF_INET, SOCK_STREAM, 0);
  if (_fd_server < 0)
    throw std::runtime_error("Socket creation error.");
  int opt = 1;
  setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  std::memset(&_addr, 0, sizeof(_addr));
  _addr.sin_family = AF_INET;
  _addr.sin_addr.s_addr = INADDR_ANY;
  _addr.sin_port = htons(port);
  // Assign socket to IP & Port
  if (bind(_fd_server, reinterpret_cast<struct sockaddr*>(&_addr),
           sizeof(_addr)) < 0) {
    close(_fd_server);
    throw std::runtime_error("Binding Error.");
  }
  // TODO:verify pw, if there are function specific rules
  _pw = pw;
}

/**
 * @brief function to add a new connection to poll struct
 */
int Server::AddNewClient(int client_fd) {
  struct pollfd ServerPoll;
  ServerPoll.fd = client_fd;
  ServerPoll.events = POLLIN;
  ServerPoll.revents = 0;
  _poll_fds.push_back(ServerPoll);
  return (0);
}

/**
 * @brief function to initiate the poll loop
 * (main server loop)
 * (1) it checks for new incomming connections (of server/socket-fd)
 * (2) it checks for events of the clients
 */
int Server::InitiatePoll() {
  while (1) {
    poll(&_poll_fds[0], _poll_fds.size(), 0);
    for (std::vector<struct pollfd>::iterator it = _poll_fds.begin();
         it != _poll_fds.end(); it++) {
      if (it == _poll_fds.begin() && it->revents != 0) {
        struct sockaddr_in
            client_addr;  // would make sense to move this into a client class
        socklen_t client_len = sizeof(
            client_addr);  // would make sense to move this into a client class
        int client_fd =
            accept(_fd_server, (struct sockaddr*)&client_addr, &client_len);
        Client newClient((_client_list.size() + 1), client_fd, client_addr);
        _client_list.push_back(newClient);
        DEBUG_PRINT("New client connection from: " << inet_ntoa(client_addr.sin_addr));
        DEBUG_PRINT("FD of new client: " << client_fd);
        send(client_fd, MSG_WELCOME, strlen(MSG_WELCOME), 0);
        send(client_fd, MSG_WAITING, strlen(MSG_WAITING), 0);
        AddNewClient(client_fd);
        DEBUG_PRINT("Revent: " << it->revents);
      } else if (it->revents != 0) {
        char buf[1024];
        int recv_len = recv(it->fd, buf, sizeof(buf) - 1, 0);  // waits until it receives any responce from client
        if (!recv_len) {
          _poll_fds.erase(it);
          break;
        }
        else {
          buf[recv_len] = '\0';
          std::cout << "Message from client fd: " << it->fd << " revent: "<< it->revents <<" - " << buf
                    << "length: " << recv_len << std::endl;
          std::memset(buf, 0, 1024);  // not necessary
          recv_len = 0;               // not necessary
        }
      }
    }
  }
  return (0);
}

/**
 * @brief function to activate the server
 * (1) activation (listen)
 * (2) init poll struct (1st element == socket-fd)
 * (3) init poll loop
 *
 * fcntl commented out as we are still unsure if required
 * respective if we are allowed to use it
 */
int Server::init() {
  // fcntl(_fd_server, F_SETFL, O_NONBLOCK); // 
  if (listen(_fd_server, MAX_QUEUED) < 0) {
    close(_fd_server);
    std::cout << "Listen Error" << std::endl;
    return 1;
  }
  DEBUG_PRINT("Server listening on port: " << _port);
  struct pollfd ServerPoll;
  ServerPoll.fd = _fd_server;
  ServerPoll.events = POLLIN;
  ServerPoll.revents = 0;
  _poll_fds.reserve(1024);
  _poll_fds.push_back(ServerPoll);
  InitiatePoll();
  return 0;
}
