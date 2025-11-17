#include "../include/Client.hpp"
#include "../include/debug.hpp"
#include <iostream>
#include <netinet/in.h>   // for sockaddr_in
#include <sys/socket.h>   // for sockaddr_in
#include <algorithm> // for std::swap

/** TODO:
 * (1) check copy constructor for new variables 
 * (2)  we could make the copy assignment operator more efficient by using our own class-specfic
 * swap function (e. g. as friend)
 */

Client::Client(long id, int fd, struct sockaddr_in addr)
    : _id(id), _client_fd(fd), _client_addr(addr) {}

Client::Client(const Client& other)
    : _id(other._id), _client_fd(other._client_fd), _client_addr(other._client_addr), _output_buffer(other._output_buffer) {}

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
 * @brief this function overwrites Clients ouput
 * with newOutput (reference)
 */
void Client::addClientOut(std::string &newOutput){
 this->_output_buffer += newOutput;
}

void Client::addClientOut(std::string newOutput){
 this->_output_buffer += newOutput;
}

int Client::getClientFd(){
	return (this->_client_fd);
}
