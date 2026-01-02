#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <string>

class Channel {
 private:
  std::string name;
  int status;  //to handle registration status (invited, member)
  std::map<std::string, int> warned_members;

 public:
  Channel();
  Channel(const Channel& other);
  Channel operator=(const Channel& other);
  ~Channel();
};

#endif
