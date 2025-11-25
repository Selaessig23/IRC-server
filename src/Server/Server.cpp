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
#include "../Parser/Parser.hpp"
#include "../debug.hpp"
#include "../includes/CONSTANTS.hpp"
#include "../includes/types.hpp"

/** TODO:
 * (1) validate Port num
 * On Unix/Linux, binding to ports <1024 usually requires root privileges.
 * Ports like 53 (DNS) or 123 (NTP) are either UDP or reserved
 * Ephemeral ports (49152–65535): These are meant for client-side connections,
 * not servers. 	Technically, you can bind a server here, but clients
 * might have trouble 	connecting due to firewall/NAT setups. Ports
 * that are already in use by another service will throw an error when trying
 * to bind to them
 * (2) destroy socket on error and in destructor
 * (3) check copy_constructor if there are new variables
 * (4) we could make the copy assignment operator more efficient by using our own class-specfic
 * swap function (e. g. as friend)
 * (5) check if we need the fcntl function in Server::initiate()
 */

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
 *     and poll event of client fd gets changed to POLLOUT
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
    it->events = POLLOUT;
  Client NewClient((_client_list.size() + 1), client_fd, client_addr, *it);
  _client_list.push_back(NewClient);
  return (0);
}

#ifndef debug
/**
 * @brief function to set all fds of clients to POLLOUT
 */
void ft_pollfds_to_out(std::vector<struct pollfd>& pollfd) {
  for (std::vector<struct pollfd>::iterator it = (pollfd.begin() + 2);
       it != pollfd.end(); it++) {
    it->events = POLLOUT;
  }
}
#endif

/**
 * @brief function to run the poll loop
 * (main server loop)
 * it checks all fds of clients & server for 
 * (1) new incomming connections (of server/socket-fd)
 * (2) event of stdin of server -- if DEBUG-mode
 * (3) events of the clients
 *
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
#ifdef debug
      else if (_client_list.empty() == 0 && it->fd == 0 &&
               it->revents & POLLIN) {
	// stdin does not work well with revc as it is a stream: where is eof?
//         char stdinbuf[1024];
//         int recv_len = 0;
//         recv_len = recv(it->fd, stdinbuf, sizeof(stdinbuf) - 1, 0);
//         stdinbuf[recv_len] = '\0';
        std::string input;
	// unsafe to not check for eol when using getline
        std::getline(std::cin, input);
        std::list<Client>::iterator it_client = _client_list.begin();
        for (; it_client != _client_list.end(); it_client++) {
//           it_client->setClientOut(stdinbuf);
          it_client->set_client_out(input + '\n');
                 }
        ft_pollfds_to_out(_poll_fds);
        break;
      }
#endif
     else if (it != _poll_fds.begin()){

        if (it->fd != 0 && it->revents & POLLIN) {
          char buf[8750];
        int recv_len = recv(it->fd, buf, sizeof(buf) - 1, 0);
        if (!recv_len) {
          close(it->fd);
          _poll_fds.erase(it);
          break;
        } else {
          buf[recv_len] = '\0';
          std::list<Client>::iterator it_clients = _client_list.begin();
          for (; it_clients != _client_list.end(); it_clients++) {
            if (it->fd == it_clients->get_client_fd())
              break;
          }
          cmd_obj cmd_body;
          PARSE_ERR err = Parsing::parse_command(buf, cmd_body);
          std::cout << "ERR: " << err << std::endl;
#ifdef DEBUG
          if (err) {
            _irc_commands->send_message(*this, cmd_body.error, true, NULL,
                                        *it_clients);
            std::cout << "ERR: " << err << std::endl;
          } else {
            std::cout << "\nCMD_BDY: " << std::endl;
            if (cmd_body.error)
              std::cout << "ERR: " << cmd_body.error << std::endl;
            if (!cmd_body.tags.empty())
              std::cout << "TAGS: " << *cmd_body.tags.begin() << std::endl;
            if (!cmd_body.prefix.empty())
              std::cout << "PREFIX: " << cmd_body.prefix << std::endl;
            if (!cmd_body.command.empty())
              std::cout << "CMD: " << cmd_body.command << std::endl;
            if (!cmd_body.parameters.empty()) {
              std::cout << "PARAS:";
              std::vector<std::string>::iterator it =
                  cmd_body.parameters.begin();
              for (; it != cmd_body.parameters.end(); it++) {
                std::cout << "\n  " << *it << std::endl;
              }
            }
          }
          DEBUG_PRINT("Message from client fd: " << it->fd);
          DEBUG_PRINT(" revent: " << it->revents);
          DEBUG_PRINT(" - " << buf);
          DEBUG_PRINT("length: " << recv_len);
#endif
          _irc_commands->exec_command(*this, cmd_body, it->fd);
          std::memset(buf, 0, 1024);  // not necessary
          recv_len = 0;               // not necessary
        }
      }
      if (it->revents & POLLOUT) {
        std::list<Client>::iterator it_client = _client_list.begin();
        for (; it_client != _client_list.end() &&
               it->fd != it_client->get_client_fd();
             it_client++) {}
        if (it_client != _client_list.end()) {
          int size_sent = send(it->fd, it_client->get_client_out().c_str(),
                               strlen(it_client->get_client_out().c_str()), 0);
          std::string new_out = it_client->get_client_out();
          new_out.erase(0, size_sent);
          it_client->set_client_out(new_out);
          if (it_client->get_client_out().empty())
            it->events = POLLIN;
 
 

              it->events = POLLIN;
          }
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
  _poll_fds.push_back(ServerPoll);
#ifndef debug
  ServerPoll.fd = STDIN_FILENO;
  ServerPoll.events = POLLIN;
  ServerPoll.revents = 0;
  _poll_fds.push_back(ServerPoll);
#endif
  if (initiate_poll())
    return (1);
  return 0;
}
