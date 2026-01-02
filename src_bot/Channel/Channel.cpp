#include "Channel.hpp"
#include "../includes/CONSTANTS.hpp"

Channel::Channel(std::string name) : _name(name) {
  _status |= INVITED;
}

void Channel::set_status(int status){
	_status |= status;
}

std::map<std::string, int>& Channel::get_members(){
	return (_warned_members);
}

/**
 * @brief function to get the amount of strikes of a nick
 *
 * @return if nick was not found in list (== no strike yet), it returns -1
 */
int Channel::get_strikes(const std::string &nick){
 std::map<std::string, int>::iterator it_member = _warned_members.find(nick);
 if (it_member == _warned_members.end())
	 return (-1);
 else 
	 return it_member->second;
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

