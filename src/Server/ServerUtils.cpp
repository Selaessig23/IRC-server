#include <ctime>    // for time-related functions
#include <iomanip>  // For std::setw and std::setfill
#include <sstream>  // for std::ostringstream
#include "../IrcCommands/IrcCommands.hpp"
#include "../Parser/Parser.hpp"
#include "../includes/types.hpp"
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

void Server::handle_pollin(struct pollfd& pfd) {

  char buf[8750];
  int recv_len = recv(pfd.fd, buf, sizeof(buf) - 1, 0);

  if (recv_len <= 0) {
    close(pfd.fd);

    for (std::vector<struct pollfd>::iterator it = _poll_fds.begin();
         it != _poll_fds.end(); ++it) {
      if (it->fd == pfd.fd) {
        _poll_fds.erase(it);
        break;
      }
    }
    return;
  }

  buf[recv_len] = '\0';

  Client* client = find_client_by_fd(pfd.fd);
  if (!client)
    return;

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
}

Client* Server::find_client_by_fd(int fd) {
  std::list<Client>::iterator it = _client_list.begin();
  for (; it != _client_list.end(); ++it) {
    if (it->get_client_fd() == fd)
      return &(*it);
  }
  return NULL;
}
