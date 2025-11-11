#include "Server.hpp"
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
#include "../CONSTANTS.hpp"

// TODO: validate Port num
// On Unix/Linux, binding to ports <1024 usually requires root privileges.
// Ports like 53 (DNS) or 123 (NTP) are either UDP or reserved
// Ephemeral ports (49152–65535): These are meant for client-side connections,
// not servers. 	Technically, you can bind a server here, but clients
// might have trouble 	connecting due to firewall/NAT setups. Ports
// that are already in use by another service will throw an error when trying
// to bind to them

int Server::exit(void) {
  this->~Server();
  return (0);
}

Server::~Server() {

  for (fd_iterator it = _poll_fds.begin(); it != _poll_fds.end(); it++) {
    close(it->fd);
  }
}

Server::Server(int port, std::string& pw) {
  _port = port;
  _fd_server = socket(AF_INET, SOCK_STREAM, 0);
  if (_fd_server < 0)
    throw std::runtime_error("Socket creation error.");
  // TODO: destroy socket on error and in destructor
  std::memset(&_addr, 0, sizeof(_addr));
  _addr.sin_family = AF_INET;
  _addr.sin_addr.s_addr = INADDR_ANY;
  _addr.sin_port = htons(port);
  // Assign socket to IP & Port
  //   if (bind(_fd_server, reinterpret_cast<struct sockaddr*>(&_addr), sizeof(_addr)) < 0) {
  if (bind(_fd_server, (struct sockaddr*)&_addr, sizeof(_addr)) < 0) {
    close(_fd_server);
    throw std::runtime_error("Binding Error.");
  }
  // TODO:verify pw, if there are function specific rules
  _pw = pw;
}

/**
 * @brief function to establish server loop
 * @return returns 1 in case of an error, otherwise 0
 */
int Server::init() {
  // This creates a passive socket like used in server applications
  fcntl(_fd_server, F_SETFL, O_NONBLOCK);
  if (listen(_fd_server, MAX_QUEUED) < 0) {
    close(_fd_server);
    std::cout << "Listen Error" << std::endl;
    return 1;
  }

  std::cout << "Server listening on port: " << _port << std::endl;
  // attepmt to establish a server loop that accepts several clients to connect
  struct pollfd ServerPoll;
  ServerPoll.revents = 0;
  _poll_fds.push_back(ServerPoll);
  std::vector<int> client_fd;
  ssize_t recv_len = 0;
  char buf[1024];
  while (1) {
    //std:: cout << "Waiting on new connections ..." << std::endl;
    int size = client_fd.size();
    client_fd.push_back(
        accept(_fd_server, NULL, NULL));  // waits until connection was created
                                          // if (client_fd < 0)
                                          // {
    // 	std::cout << "Error: problems with accept" << std::endl;
    // 	return (1); // return error or continue loop?
    // }
    // else if (client_fd > 0) // not necessary as accept waits until there is any connection
    // {
    send(client_fd[size - 1], MSG_WELCOME, strlen(MSG_WELCOME), 0);

    //std::cout << "Waiting on response from client" << std::endl;
    send(client_fd[size - 1], MSG_WAITING, strlen(MSG_WAITING), 0);
    for (std::vector<int>::iterator start = client_fd.begin();
         start < client_fd.end(); start++) {
      recv_len = recv(*start, buf, sizeof(buf) - 1,
                      0);  // waits until it receives any responce from client
      buf[recv_len] = '\0';
      if (*start != -1) {
        std::cout << "Message from client fd: " << *start << " - " << buf
                  << "length: " << recv_len << std::endl;
        std::memset(buf, 0, 1024);
        recv_len = 0;
      }
    }
    //}
    // TODO: add client_fd to vector of Client-class to manage connections to different clients
  }
  return 0;
}
