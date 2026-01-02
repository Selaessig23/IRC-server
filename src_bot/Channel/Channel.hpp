#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <string>

class Channel {
 private:
  std::string _name;
  int _status;  //to handle registration status (invited, applied, member)
  std::map<std::string, int> warned_members;

 public:
  Channel(std::string name);
  Channel(const Channel& other);
  Channel operator=(const Channel& other);
  ~Channel();

  void set_status(int status);
  //overload for find-functionality
  bool operator==(const std::string& other) const;
};

#endif
