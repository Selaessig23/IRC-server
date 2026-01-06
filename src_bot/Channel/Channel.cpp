#include "Channel.hpp"
#include "../debug.hpp"
#include "../includes/CONSTANTS.hpp"

Channel::Channel(std::string name) : _name(name), _status(0) {
  _status |= INVITED;
}

Channel::Channel(const Channel& other)
    : _name(other._name), _status(other._status) {}

Channel Channel::operator=(const Channel& other) {
  Channel temp(other);
  std::swap(*this, temp);
  return (*this);
}

Channel::~Channel() {
  DEBUG_PRINT("Channel destructor called");
}

void Channel::set_status(int status) {
  _status |= status;
}

/**
 * @brief overload for find-functionality
 * it checks for _name of channel
 *
 * @returns true if _name == other
 */
bool Channel::operator==(const std::string& other) const {
  return this->_name == other;
}

