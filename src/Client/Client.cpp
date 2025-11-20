#include "Client.hpp"
#include "../debug.hpp"
#include <iostream>
#include <netinet/in.h>   // for sockaddr_in
#include <sys/socket.h>   // for sockaddr_in
#include <algorithm> // for std::swap
#include <poll.h>

/** TODO:
 * (1) check copy constructor for new variables 
 * (2)  we could make the copy assignment operator more efficient by using our own class-specfic
 * swap function (e. g. as friend)
 */

Client::Client(long id, int fd, struct sockaddr_in addr, struct pollfd &pollstruct)
    : _id(id), _client_fd(fd), _registered(0), _client_addr(addr), _poll(&pollstruct){}

Client::Client(const Client& other)
    : _id(other._id), _client_fd(other._client_fd), _registered(other._registered),_client_addr(other._client_addr), _poll(other._poll), _output_buffer(other._output_buffer) {}

Client	Client::operator=(const Client &other)
{
	Client temp(other);
	std::swap(*this, temp);
	return (*this);
}

Client::~Client(){
  DEBUG_PRINT("Destructor of Client called.");
}

std::string &Client::getClientOut(){
	return(this->_output_buffer);
}

/**
 * @brief this function overwrites Clients ouput
 * with newOutput (copy)
 */
void Client::setClientOut(std::string newOutput){
 this->_output_buffer = newOutput;
}

/**
 * @brief this function adds new ouput (reference) 
 * to the Clients output buffer
 */
void Client::addClientOut(std::string &newOutput){
 this->_output_buffer += newOutput;
}

/**
 * @brief this function adds new ouput (copy) 
 * to the Clients output buffer
 */
void Client::addClientOut(std::string newOutput){
 this->_output_buffer += newOutput;
}

int Client::getClientFd(){
	return (this->_client_fd);
}

std::string &Client::getNick(){
	return (this->_nick);
}

bool Client::getRegisterStatus(){
	return(this->_registered);
}

void Client::setServerPoll(){
 if (_poll->events == POLLIN)
	 _poll->events = POLLOUT;
 else if (_poll->events == POLLOUT)
	 _poll->events = POLLIN;
}
