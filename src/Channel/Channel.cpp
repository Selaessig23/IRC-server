#include "Channel.hpp"
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "../Client/Client.hpp"
#include "../IrcCommands/IrcCommands.hpp"
#include "../Server/Server.hpp"
#include "../debug.hpp"
#include "../includes/CONSTANTS.hpp"

Channel::Channel(std::string name)
    : _name(name),
      _creation_time(get_current_date_time()),
      _topic(),
      _topic_time(),
      _topic_who(),
      _key(),
      _user_limit(),
      _modes(MODE_TOPIC) {
  DEBUG_PRINT("Channel is created");
}

Channel::Channel(const Channel& other)
    : _name(other._name),
      _creation_time(other._creation_time),
      _topic(other._topic),
      _topic_time(other._topic_time),
      _topic_who(other._topic_who),
      _key(other._key),
      _user_limit(other._user_limit),
      _modes(other._modes) {}

Channel& Channel::operator=(const Channel& other) {
  Channel temp(other);
  std::swap(*this, temp);
  return (*this);
}

Channel::~Channel() {
  DEBUG_PRINT("Channel is destructed");
}

// MEMBER MANAGEMENT METHODS
void Channel::new_member(Client* _new, bool is_oper) {
  _members.insert(std::pair<Client*, bool>(_new, is_oper));
  DEBUG_PRINT("New member is added to the channel");
#ifdef DEBUG
  print_channel_info();
#endif
}

void Channel::new_invited(Client* _new) {
  _invited.push_back(_new);
}

void Channel::remove_from_members(Client* removal) {
  _members.erase(removal);
}

void Channel::remove_from_invited(Client* removal) {
  std::list<Client*>::iterator it = _invited.begin();
  for (; it != _invited.end(); it++) {
    if (*it == removal) {
      _invited.erase(it);
      break;
    }
  }
}

bool Channel::update_chanops_stat(std::string nick, bool status) {
  std::map<Client*, bool>::iterator it = _members.begin();
  for (; it != _members.end(); it++) {
    if (it->first->get_nick() == nick) {
      it->second = status;
      return (1);
    }
  }
  return (0);
}

// SETTERS
void Channel::set_topic(std::string topic, std::string nick) {
  _topic = topic;
  _topic_time = get_current_date_time();
  _topic_who = nick;
}

void Channel::set_user_limit(size_t limit) {
  _user_limit = limit;
}

void Channel::set_key(std::string key) {
  _key = key;
}

void Channel::adjust_modes(int mode, bool status) {
  if (status)
    _modes |= mode;
  else
    _modes &= ~mode;
#ifdef DEBUG
  std::cout << _modes << std::endl;
#endif
}

// GETTERS
const std::string& Channel::get_name() {
  return (_name);
}

std::string Channel::get_creation_time() {
  return (_creation_time);
}

std::string Channel::get_topic() {
  return (_topic);
}

std::string Channel::get_topic_time() {
  return (_topic_time);
}

std::string Channel::get_topic_who() {
  return (_topic_who);
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

std::map<Client*, bool>& Channel::get_members() {
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

// Helpers
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

std::string Channel::get_nicks_string() {
  std::string nicks;
  std::map<Client*, bool>::iterator it = _members.begin();
  while (it != _members.end()) {
    if (it->second)
      nicks += "@";
    nicks += it->first->get_nick();
    ++it;
    if (it != _members.end())
      nicks += ", ";
  }
  return (nicks);
}

/**
 * @brief channel info to show each in the events of successful 
 * JOIN or MODE calls
 * TODO: it will be adjusted to the new structure of message sending
 */

void Channel::print_channel_info() {
  std::cout << get_name() << std::endl;
  std::cout << get_modes_string() << std::endl;
  std::cout << "Members[" << _members.size() << "]: ";
  for (std::map<Client*, bool>::iterator it = _members.begin();
       !_members.empty() && it != _members.end(); it++) {
    if (it->second)
      std::cout << "@";
    std::cout << it->first->get_nick() << ", ";
  };
  std::cout << std::endl;
  std::cout << "Invited[" << _invited.size() << "]: ";
  for (std::list<Client*>::iterator it = _invited.begin();
       !_invited.empty() && it != _invited.end(); it++) {
    std::cout << (*it)->get_nick() << ", ";
  };
  std::cout << std::endl;
}
