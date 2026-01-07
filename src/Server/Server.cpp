#include "Server.hpp"
#include <arpa/inet.h>  // for inet_ntoa()
#include <fcntl.h>
#include <netinet/in.h>  //for socket, bind, listen, accept
#include <poll.h>
#include <stdlib.h>
#include <sys/socket.h>  //sockaddr_in
#include <unistd.h>      //for close()
#include <algorithm>     // for std::swap
#include <cstring>       //memset
#include <iostream>
#include <stdexcept>  // to throw exceptions for runtime
#include <vector>
#include "../Client/Client.hpp"
#include "../IrcCommands/IrcCommands.hpp"
#include "../debug.hpp"
#include "../includes/CONSTANTS.hpp"
#include "../includes/types.hpp"

Server::~Server() {
  DEBUG_PRINT("Destructor of Server called.");
  for (fd_iterator it = _poll_fds.begin() + 1; it != _poll_fds.end(); it++) {
    close(it->fd);
  }
  //Close Server last
  close(_poll_fds.begin()->fd);
  delete _irc_commands;
}

Server::Server(int port, std::string& pw)
    : _network_name("MUMs_network"),
      _server_name("MUMs_server"),
      _version("0.0.0.0.0.9"),
      _port(port),
      _pw(pw),
      _irc_commands(new IrcCommands()) {
  //created at
  _created_at = get_current_date_time();
  _port = port;
  _fd_server = socket(AF_INET, SOCK_STREAM, 0);
  if (_fd_server < 0) {
    delete _irc_commands;
    throw std::runtime_error("Socket creation error.");
  }
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
    delete _irc_commands;
    throw std::runtime_error("Binding Error.");
  }
}

Server::Server(const Server& other)
    : _network_name(other._network_name),
      _server_name(other._server_name),
      _version(other._version),
      _created_at(other._created_at),
      _port(other._port),
      _fd_server(other._fd_server),
      _addr(other._addr),
      _pw(other._pw),
      _poll_fds(other._poll_fds),
      _client_list(other._client_list),
      _irc_commands(other._irc_commands) {}

Server Server::operator=(const Server& other) {
  Server temp(other);
  std::swap(*this, temp);
  return (*this);
}

/**
 * @brief function to add a new connection to poll struct
 */
int Server::add_new_client_to_poll(int client_fd) {
  struct pollfd ServerPoll;
  ServerPoll.fd = client_fd;
  ServerPoll.events = POLLIN;
  ServerPoll.revents = 0;
  _poll_fds.push_back(ServerPoll);
  return (0);
}

/**
 * @brief function to create and add new client to list of client-class
 * (1) it creates new client class
 * (2) client is add to client class
 * (3) welcome-message from server gets written to client buffer
 *     and poll event of client fd gets set to POLLIN AND 
 *     POLLOUT (to write welcome message)
 */
int Server::handle_new_client() {
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd =
      accept(_fd_server, (struct sockaddr*)&client_addr, &client_len);
  DEBUG_PRINT(
      "New client connection from: " << inet_ntoa(client_addr.sin_addr));
  DEBUG_PRINT("FD of new client: " << client_fd);
  add_new_client_to_poll(client_fd);
  std::vector<struct pollfd>::iterator it = _poll_fds.begin();
  // check  for find function
  for (; it != _poll_fds.end() && it->fd != client_fd; it++) {}
  if (it != _poll_fds.end())
    it->events = POLLOUT | POLLIN;
  Client NewClient((_client_list.size() + 1), client_fd, client_addr);
  _client_list.push_back(NewClient);
  return (0);
}

/**
 * @brief function to run the poll loop
 * (main server loop)
 * it checks all fds of clients & server for 
 * (1) new incomming connections (of server/socket-fd)
 * (2) events of the clients
 */
int Server::initiate_poll() {
  while (1) {
    poll(&_poll_fds[0], _poll_fds.size(), 0);
    for (std::vector<struct pollfd>::iterator it = _poll_fds.begin();
         it != _poll_fds.end(); it++) {
      if (it == _poll_fds.begin() && it->revents != 0) {
        DEBUG_PRINT("Revent: " << it->revents);
        handle_new_client();
        break;
      }
      if (it->revents & POLLIN) {
        if (handle_pollin(*it))
          break;
      }
      if (it->revents & POLLOUT) {
        handle_pollout(*it);
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
  _poll_fds.push_back(ServerPoll);
  if (initiate_poll())
    return (1);
  return 0;
}

// Getters
std::list<Channel>& Server::get_channel_list() {
  return (_channel_list);
}
