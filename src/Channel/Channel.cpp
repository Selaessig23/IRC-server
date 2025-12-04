#include "Channel.hpp"
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "../Client/Client.hpp"
#include "../debug.hpp"

Channel::Channel(std::string name) : _name(name), _topic("Default"), _modes(0) {
  DEBUG_PRINT("Channel is created");
#ifdef DEBUG
  print_channel_info();
#endif
}

Channel::Channel(const Channel& other)
    : _name(other._name), _topic(other._topic), _modes(other._modes) {}

Channel& Channel::operator=(const Channel& other) {
  Channel temp(other);
  std::swap(*this, temp);
  return (*this);
}

Channel::~Channel() {
  DEBUG_PRINT("Channel is destructed");
}

// Member management methods
void Channel::new_member(Client* _new) {
  this->_members.push_back(_new);
  DEBUG_PRINT("New member is added to the channel");
#ifdef DEBUG
  print_channel_info();
#endif
}

void Channel::new_operator(Client* _new) {
  _operators.push_back(_new);
  DEBUG_PRINT("New operator is assigned for the channel");
#ifdef DEBUG
  print_channel_info();
#endif
}

void Channel::new_invited(Client* _new) {
  _invited.push_back(_new);
  DEBUG_PRINT("The user is invited to the channel");
#ifdef DEBUG
  print_channel_info();
#endif
}

// SETTERS
void Channel::set_topic(std::string topic) {
  _topic = topic;
}

void Channel::set_user_limit(size_t limit) {
  _user_limit = limit;
  // _limit_mode = true;
}

// void Channel::set_invite_mode() {
//   if (_invite_mode)
//     _invite_mode = false;
//   else
//     _invite_mode = true;
// }

void Channel::set_key(std::string key) {
  _key = key;
  // _key_mode = true;
}

void Channel::set_modes(std::string flag) {
  if (flag == "+i")
    _modes |= MODE_INVITE;
  else if (flag == "+k")
    _modes |= MODE_KEY;
  else if (flag == "+l")
    _modes |= MODE_LIMIT;
  else if (flag == "+t")
    _modes |= MODE_TOPIC;
  else if (flag == "-i")
    _modes &= ~MODE_INVITE;
  else if (flag == "-k")
    _modes &= ~MODE_KEY;
  else if (flag == "-l")
    _modes &= ~MODE_LIMIT;
  else if (flag == "-t")
    _modes &= ~MODE_TOPIC;
}

// GETTERS
std::string Channel::get_name() {
  return (_name);
}

std::string Channel::get_topic() {
  return (_topic);
}

std::string Channel::get_key() {
  return (_key);
}

// bool Channel::get_key_mode() {
//   return (_key_mode);
// }

// bool Channel::get_invite_mode() {
//   return (_invite_mode);
// }

// bool Channel::get_topic_mode() {
//   return (_topic_mode);
// }

// bool Channel::get_limit_mode() {
//   return (_limit_mode);
// }

int Channel::get_user_limit() {
  return (_user_limit);
}

size_t Channel::get_members_size() {
  return (_members.size());
}

size_t Channel::get_operators_size() {
  return (_operators.size());
}

/**
 * @brief function to create a std::vector of all nicknames of the
 * channel members
 */
std::vector<std::string> Channel::get_members_nicks(void) {
  std::vector<std::string> ret;
  ret.reserve(_members.size());
  for (std::list<Client*>::const_iterator it = _members.begin();
       it != _members.end(); it++) {
    ret.push_back((*it)->get_nick());
  }
  return (ret);
}

std::list<Client*> Channel::get_members() {
  return (this->_members);
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

/**
 * @brief channel info to send each client after they join a channel 
 */

void Channel::print_channel_info() {
  // std::cout << get_name() << " [+i]:" << get_invite_mode()
  //           << " [+t]:" << get_topic_mode() << " [+k]:" << get_key_mode()
  //           << " [+l]:" << get_limit_mode() << std::endl;
  std::cout << "Members[" << get_members_size() << "]: ";
  for (std::list<Client*>::iterator it = _members.begin();
       !_members.empty() && it != _members.end(); it++) {
    std::cout << (*it)->get_nick() << ", ";
  };
  std::cout << std::endl;
  std::cout << "Operators[" << get_operators_size() << "]: ";
  for (std::list<Client*>::iterator it = _operators.begin();
       !_operators.empty() && it != _operators.end(); it++) {
    std::cout << (*it)->get_nick() << ", ";
  };
  std::cout << std::endl;
}