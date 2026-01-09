#include <errno.h>
#include <iostream>
#include "../IrcCommands/IrcCommands.hpp"
#include "../Parser/Parser.hpp"
#include "../debug.hpp"
#include "Server.hpp"

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
 * @brief function to set the event of the pollfd struct of client
 * with corresponding fd
 */
void Server::set_pollevent(int fd, int event) {
  std::vector<struct pollfd>::iterator it = _poll_fds.begin();
  for (; it != _poll_fds.end() && it->fd != fd; it++) {}
  if (it != _poll_fds.end())
    it->events |= event;
}

/**
 * @brief function to remove the event sent as parameter from
 * the corresponding fd in the array (vector) of pollfd structs
 */
void Server::remove_pollevent(int fd, int event) {
  std::vector<struct pollfd>::iterator it = _poll_fds.begin();
  for (; it != _poll_fds.end() && it->fd != fd; it++) {}
  if (it != _poll_fds.end())
    it->events &= ~event;
}

/**
 * @brief function to handle a pollin event from one of the clients fds
 * buffer from recv is added to internal buffer of client-obj and only gets
 * parsed and executed if the internal buffer of the client includes a "\r\n"
 *
 * error-checks:
 * in case there is no input to read, it is interpreted as client was lost
 * therefore client gets deleted on server
 * it checks for recv-errors
 */
int Server::handle_pollin(struct pollfd& pfd) {

  char buf[8750];
  int recv_len = recv(pfd.fd, buf, sizeof(buf) - 1, 0);
  if (recv_len < 0) {
    if (errno == ECONNRESET || errno == ENOTCONN)
      remove_client(pfd.fd);
    DEBUG_PRINT("Error detected in recv(): " << errno);
    return (1);
  }
  if (recv_len == 0) {
    this->remove_client(pfd.fd);
    return (1);
  }
  buf[recv_len] = '\0';
  DEBUG_PRINT("POLLIN buff: " << buf);
  Client* client = find_client_by_fd(pfd.fd);
  if (!client)
    return (1);
  client->add_to_received_packs(buf);

  while (client->get_received_packs().find("\r\n") != std::string::npos) {

    cmd_obj cmd_body;
    cmd_body.client = client;
    PARSE_ERR err = Parsing::parse_command(cmd_body);
#ifdef DEBUG
    if (err)
      std::cout << "\nERR: " << err << std::endl;
    else
      debug_parsed_cmds(cmd_body);
#else
    (void)err;
#endif
    _irc_commands->exec_command(*this, cmd_body);
  }
  return (0);
}

/**
 * @brief function to handle a pollout event of a clients id
 * to send from clients buffer to client until buffer is empty
 * if buffer is empty, poll-event POLLOUT of clients fd gets removed
 * everything that was sent to client becomes removed from clients out-buffer
 *
 * function checks for send-errors to avoid blocking-behaviour and
 * race-conditions
 */
int Server::handle_pollout(struct pollfd& pfd) {

  std::list<Client>::iterator it_client = _client_list.begin();
  for (;
       it_client != _client_list.end() && pfd.fd != it_client->get_client_fd();
       it_client++) {}
  if (it_client != _client_list.end()) {
    int size_sent = send(pfd.fd, it_client->get_client_out().c_str(),
                         it_client->get_client_out().size(), 0);
    if (size_sent <= 0) {
      if (size_sent == 0) {
        DEBUG_PRINT("send returned 0 bytes, closing client fd: " << pfd.fd);
        remove_client(pfd.fd);
      } else if (errno == EPIPE || errno == ECONNRESET ||
                 errno == ETIMEDOUT | errno == ENOTCONN) {
        remove_client(pfd.fd);
      }
      DEBUG_PRINT("Error on send-function: " << errno);
      return (1);
    }
    std::string new_out = it_client->get_client_out();
    new_out.erase(0, static_cast<std::string::size_type>(size_sent));
    it_client->set_client_out(new_out);
    if (it_client->get_client_out().empty())
      remove_pollevent(it_client->get_client_fd(), POLLOUT);
  }
  return (0);
}

/**
 * @brief function to run the poll loop
 * (main server loop)
 * it checks all fds of clients & server for
 * (1) new incoming connections (of server/socket-fd)
 * (2) events of the clients
 */
int Server::initiate_poll() {
  while (1) {
    int ret_poll = 0;
    ret_poll = poll(&_poll_fds[0], _poll_fds.size(), -1);
    if (ret_poll < 0) {
      if (errno == EINTR)
        initiate_poll();
      DEBUG_PRINT("Error in poll-function, errno: " << errno);
      return (1);
    }
    for (std::vector<struct pollfd>::iterator it = _poll_fds.begin();
         it != _poll_fds.end(); it++) {
      if (it->revents & (POLLERR | POLLHUP | POLLNVAL)) {
        if (it == _poll_fds.begin()) {
          DEBUG_PRINT(
              "Error on server socket pollfd, revents: " << it->revents);
          return (1);
        }
        remove_client(it->fd);
        break;
      }
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
        if (handle_pollout(*it))
          break;
      }
    }
  }
  return (0);
}

