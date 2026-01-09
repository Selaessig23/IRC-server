#include "Server.hpp"
#include <arpa/inet.h>  // for inet_ntoa()
// #include <fcntl.h>
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

Server::Server(int port, std::string& pw)
    : _network_name("MUMs_network"),
      _server_name("MUMs_server"),
      _version("0.0.0.0.0.9"),
      _port(port),
      _pw(pw),
      _irc_commands(new IrcCommands()) {
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

Server::~Server() {
  DEBUG_PRINT("Destructor of Server called.");
  for (fd_iterator it = _poll_fds.begin() + 1; it != _poll_fds.end(); it++) {
    close(it->fd);
  }
  close(_poll_fds.begin()->fd);
  delete _irc_commands;
}

// Getters
std::list<Channel>& Server::get_channel_list() {
  return (_channel_list);
}

/**
 * @brief function to find a client of _client_list
 * @return it returns a pointer to the client or NULL if it was not found
 */
Client* Server::find_client_by_fd(int fd) {
  std::list<Client>::iterator it = _client_list.begin();
  for (; it != _client_list.end(); ++it) {
    if (it->get_client_fd() == fd)
      return &(*it);
  }
  return NULL;
}

/**
 * @brief function to remove (kill) a client from server,
 * it considers:
 *  pollfd struct
 *  _client_list
 *  _channel_list
 */
void Server::remove_client(int fd) {

  for (std::list<Client>::iterator it_client = _client_list.begin();
       it_client != _client_list.end(); it_client++) {
    if (it_client->get_client_fd() == fd) {
      for (std::list<Channel>::iterator it_chan = _channel_list.begin();
           it_chan != _channel_list.end();) {
        it_chan->remove_from_members(&(*it_client));
        it_chan->remove_from_invited(&(*it_client));
        if (it_chan->get_members().empty())
          it_chan = _channel_list.erase(it_chan);
        else
          it_chan++;
      }
      _client_list.erase(it_client);
      break;
    }
  }
  for (std::vector<struct pollfd>::iterator it = _poll_fds.begin();
       it != _poll_fds.end(); ++it) {
    if (it->fd == fd) {
      _poll_fds.erase(it);
      break;
    }
  }
  close(fd);
  DEBUG_PRINT("Case delete client: " << fd);
}

/**
 * @brief function to create and add new client to list of client-class
 * (1) it creates new client class
 * (2) client is add to client class
 * (3) welcome-message from server gets written to client buffer
 *     and poll event of client fd gets set to POLLIN 
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
  Client NewClient((_client_list.size() + 1), client_fd, client_addr);
  _client_list.push_back(NewClient);
  return (0);
}

/**
 * @brief function to activate the server
 * (1) activation (listen)
 * (2) init poll struct (1st element == socket-fd)
 * (3) init poll loop
 *
 * fcntl commented out as there was no case where the server got blocked
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
