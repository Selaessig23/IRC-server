#include <ctime>    // for time-related functions
#include <iomanip>  // For std::setw and std::setfill
#include <sstream>  // for std::ostringstream
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
