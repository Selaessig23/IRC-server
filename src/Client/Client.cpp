#include "Client.hpp"
#include <netinet/in.h>  // for sockaddr_in
#include <poll.h>
#include <algorithm>  // for std::swap
#include <iostream>
#include "../debug.hpp"

/** TODO:
 * (1) check copy constructor for new variables 
 * (2)  we could make the copy assignment operator more efficient by using our own class-specfic
 * swap function (e. g. as friend)
 */

Client::Client(long id, int fd, struct sockaddr_in addr)
    : _id(id),
      _client_fd(fd),
      _registered(0),
      _client_addr(addr) {}

Client::Client(const Client& other)
    : _id(other._id),
      _client_fd(other._client_fd),
      _registered(other._registered),
      _client_addr(other._client_addr),
      _received_packs(other._received_packs),
      _nick(other._nick),
      _user(other._user),
      _host(other._host),
      _servername(other._servername),
      _realname(other._realname),
      _output_buffer(other._output_buffer) {}

Client Client::operator=(const Client& other) {
  Client temp(other);
  std::swap(*this, temp);
  return (*this);
}

Client::~Client() {
  DEBUG_PRINT("Destructor of Client called.");
}

std::string& Client::get_client_out() {
  return (this->_output_buffer);
}

/**
 * @brief this function overwrites Clients ouput
 * with newOutput (copy)
 */
void Client::set_client_out(std::string new_output) {
  this->_output_buffer = new_output;
}

void Client::add_to_received_packs(std::string new_pack) {
  _received_packs += new_pack;
};

std::string Client::get_received_packs() {
  return _received_packs;
};

/**
 * @brief Remove command AND the \r\n delimiter
 */
void Client::clip_current_command(size_t delimiter) {
  if (delimiter + 2 <= _received_packs.size()) {
    _received_packs = _received_packs.substr(delimiter + 2);
  } else {
    _received_packs.clear();
  }
}

/**
 * @brief this function adds new ouput (copy) 
 * to the Clients output buffer
 */
void Client::add_client_out(std::string new_output) {
  this->_output_buffer += new_output;
}

int Client::get_client_fd() {
  return (this->_client_fd);
}

std::string& Client::get_nick() {
  return (this->_nick);
}

std::string& Client::get_user() {
  return (this->_user);
}

std::string& Client::get_host() {
  return (this->_host);
}

unsigned char Client::get_register_status() {
  return (this->_registered);
}

void Client::set_register_status(char to_add) {
  _registered |= to_add;
}

void Client::set_nick(std::string nick) {
  this->_nick = nick;
}

void Client::set_user(std::string user) {
  this->_user = user;
}

void Client::set_host(std::string host) {
  this->_host = host;
}

void Client::set_realname(std::string realname) {
  this->_realname = realname;
}

void Client::set_servername(std::string realname) {
  this->_realname = realname;
}

/**
 * @brief overload for find-functionality
 * it checks for _nick
 *
 * @return it returns true if nick == other
 */
bool Client::operator==(const std::string& other) const {
  return this->_nick == other;
}
