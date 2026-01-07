
#include <cstring>  // for strlen
#include <ctime>    // for time-related functions
#include <iomanip>  // For std::setw and std::setfill
#include <iostream>
#include <sstream>  // for std::ostringstream
#include "../IrcCommands/IrcCommands.hpp"
#include "../Parser/Parser.hpp"
#include "../debug.hpp"
#include "../includes/types.hpp"
#include "Server.hpp"

/**
 * @brief function to get the current time and return it as a std::string
 */
std::string get_current_date_time() {
  std::time_t now = std::time(NULL);
  struct tm* local_time = std::localtime(&now);

  std::ostringstream oss;
  oss << (1900 + local_time->tm_year) << "-" << std::setfill('0')
      << std::setw(2) << (1 + local_time->tm_mon) << "-" << std::setfill('0')
      << std::setw(2) << local_time->tm_mday << " " << std::setfill('0')
      << std::setw(2) << local_time->tm_hour << ":" << std::setfill('0')
      << std::setw(2) << local_time->tm_min << ":" << std::setfill('0')
      << std::setw(2) << local_time->tm_sec;
  return oss.str();
}

void debug_parsed_cmds(cmd_obj& cmd_body) {

  std::cout << "CMD_BDY: " << std::endl;
  if (cmd_body.error)
    std::cout << "ERR: " << cmd_body.error << std::endl;
  if (!cmd_body.tags.empty())
    std::cout << "TAGS: " << *cmd_body.tags.begin() << std::endl;
  if (!cmd_body.prefix.empty())
    std::cout << "PREFIX: " << cmd_body.prefix << std::endl;
  if (!cmd_body.command.empty())
    std::cout << "CMD: " << cmd_body.command << std::endl;
  if (!cmd_body.parameters.empty()) {
    std::cout << "PARAS:" << std::endl;
    std::vector<std::string>::iterator it = cmd_body.parameters.begin();
    for (; it != cmd_body.parameters.end(); it++) {
      std::cout << *it << std::endl;
    }
  }
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
 * in case there is no input to read, it is interpreted as client was lost
 * therefore client gets deleted on server
 */
int Server::handle_pollin(struct pollfd& pfd) {

  char buf[8750];
  int recv_len = recv(pfd.fd, buf, sizeof(buf) - 1, 0);

  if (recv_len <= 0) {
    this->remove_client(pfd.fd);
    return (1);
  }

  buf[recv_len] = '\0';

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
 * to send from clients buffer to client until it buffer is empty
 * if buffer is empty, poll-event POLLOUT of clients fd gets removed
 * everything that has been sent to client becomes removed from clients out-buffer
 */
void Server::handle_pollout(struct pollfd& pfd) {

  std::list<Client>::iterator it_client = _client_list.begin();
  for (;
       it_client != _client_list.end() && pfd.fd != it_client->get_client_fd();
       it_client++) {}
  if (it_client != _client_list.end()) {
    int size_sent = send(pfd.fd, it_client->get_client_out().c_str(),
                         strlen(it_client->get_client_out().c_str()), 0);
    std::string new_out = it_client->get_client_out();
    new_out.erase(0, size_sent);
    it_client->set_client_out(new_out);
    if (it_client->get_client_out().empty())
      remove_pollevent(it_client->get_client_fd(), POLLOUT);
  }
}

Client* Server::find_client_by_fd(int fd) {
  std::list<Client>::iterator it = _client_list.begin();
  for (; it != _client_list.end(); ++it) {
    if (it->get_client_fd() == fd)
      return &(*it);
  }
  return NULL;
}
