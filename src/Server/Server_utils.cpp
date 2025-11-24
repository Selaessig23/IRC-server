#include <ctime>    // for time-related functions
#include <iomanip>  // For std::setw and std::setfill
#include <sstream>  // for std::ostringstream
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
