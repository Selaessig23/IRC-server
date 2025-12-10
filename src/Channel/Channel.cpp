#include "Channel.hpp"
#include <iostream>
#include <list>
#include <map>
#include <sstream>
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
void Channel::new_member(Client* _new, bool is_oper) {
  _members.insert(std::pair<Client*, bool>(_new, is_oper));
  DEBUG_PRINT("New member is added to the channel");
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
}

void Channel::set_key(std::string key) {
  _key = key;
}

void Channel::set_mode(int mode, bool status) {
  if (status)
    _modes |= mode;
  else
    _modes &= ~mode;
#ifdef DEBUG
  std::cout << _modes << std::endl;
#endif
}

// GETTERS
std::string& Channel::get_name() {
  return (_name);
}

std::string Channel::get_topic() {
  return (_topic);
}

std::string Channel::get_key() {
  return (_key);
}

size_t Channel::get_user_limit() {
  return (_user_limit);
}

int Channel::get_modes() {
  return (_modes);
}

size_t Channel::get_members_size() {
  return (_members.size());
}

/**
 * @brief function to create a std::vector of all nicknames of the
 * channel members
 */
std::vector<std::string> Channel::get_members_nicks(void) {
  std::vector<std::string> ret;
  ret.reserve(_members.size());
  for (std::map<Client*, bool>::const_iterator it = _members.begin();
       it != _members.end(); it++) {
    ret.push_back(it->first->get_nick());
  }
  return (ret);
}

std::map<Client*, bool> Channel::get_members() {
  return (this->_members);
}

std::list<Client*>& Channel::get_invited() {
  return (this->_invited);
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

// HELPER FUNCTIONS

std::string Channel::get_modes_string() {
  std::string str;
  str += '+';
  if (_modes & MODE_INVITE)
    str += 'i';
  if (_modes & MODE_KEY)
    str += 'k';
  if (_modes & MODE_LIMIT)
    str += 'l';
  if (_modes & MODE_TOPIC)
    str += 't';
  if (_modes & MODE_KEY) {
    str += " ";
    str += get_key();
  }
  if (_modes & MODE_LIMIT) {
    str += " ";
    std::ostringstream oss;
    oss << get_user_limit();
    str += oss.str();
  }
  return (str);
}

/**
 * @brief channel info to send each
 * client after they join a channel 
 */

void Channel::print_channel_info() {
  std::cout << get_name() << " " << get_modes() << std::endl;
  std::cout << "Members[" << get_members_size() << "]: ";
  for (std::map<Client*, bool>::iterator it = _members.begin();
       !_members.empty() && it != _members.end(); it++) {
    std::cout << it->first->get_nick() << ", ";
  };
  std::cout << std::endl;
}