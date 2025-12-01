#include <cstring>  //memset
#include <ctime>    // for time-related functions
#include <iomanip>  // For std::setw and std::setfill
#include <iostream>
#include <sstream>  // for std::ostringstream
#include "../IrcCommands/IrcCommands.hpp"
#include "../Parser/Parser.hpp"
#include "../includes/types.hpp"
#include "../debug.hpp"
#include "Server.hpp"

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
    std::vector<std::string>::iterator it = cmd_body.parameters.begin();
    for (; it != cmd_body.parameters.end(); it++) {
      std::cout << "\n  " << *it << std::endl;
    }
  }
}

/**
 * @brief function to set the event of the pollfd struct of client
 * with corresponding fd
 * 
 * TODO: 
 * (1) set to defined value (it overwrites previous event assignments)
 */
void Server::set_server_poll(int fd) {
  std::vector<struct pollfd>::iterator it = _poll_fds.begin();
  for (; it != _poll_fds.end() && it->fd != fd; it++) {}
    if (it->events & POLLOUT)
      it->events &= ~POLLOUT;
    else
      it->events |= POLLOUT;
}

int Server::handle_pollin(struct pollfd& pfd) {

  char buf[8750];
  int recv_len = recv(pfd.fd, buf, sizeof(buf) - 1, 0);

  if (recv_len <= 0) {
    close(pfd.fd);
    for (std::vector<struct pollfd>::iterator it = _poll_fds.begin();
         it != _poll_fds.end(); ++it) {
      if (it->fd == pfd.fd) {
            DEBUG_PRINT("Case delete client: " << pfd.fd);
        _poll_fds.erase(it);
        break;
      }
    }
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

    if (err)
      std::cout << "\nERR: " << err << std::endl;

#ifdef DEBUG
    debug_parsed_cmds(cmd_body);
#endif

    _irc_commands->exec_command(*this, cmd_body, pfd.fd);
  }
  return (0);
}

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
      pfd.events &= ~POLLOUT;
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
