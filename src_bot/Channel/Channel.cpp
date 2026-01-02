#include "Channel.hpp"
#include "../includes/CONSTANTS.hpp"

Channel::Channel(std::string name) : _name(name) {
  _status |= INVITED;
}

void Channel::set_status(int status){
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

