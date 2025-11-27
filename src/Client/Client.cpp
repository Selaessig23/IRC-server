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

Client::Client(long id, int fd, struct sockaddr_in addr,
               struct pollfd& poll_struct)
    : _id(id),
      _client_fd(fd),
      _registered(0),
      _client_addr(addr),
      _poll(&poll_struct) {}

Client::Client(const Client& other)
    : _id(other._id),
      _client_fd(other._client_fd),
      _registered(other._registered),
      _client_addr(other._client_addr),
      _poll(other._poll),
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

void Client::set_auth_state(CLIENT_AUTH_STATE auth_state) {
  _auth_state = auth_state;
}

CLIENT_AUTH_STATE Client::get_auth_state() {
  return _auth_state;
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

bool Client::get_register_status() {
  return (this->_registered);
}

void Client::set_server_poll(int event) {
  if (event == POLLIN)
    _poll->events = POLLIN;
  else if (event == POLLOUT)
    _poll->events = POLLOUT;
}

void Client::set_nick(std::string nick) {
  this->_nick = nick;
}
