#include "Client.hpp"
#include "debug.hpp"
#include <netinet/in.h>   // for sockaddr_in
#include <sys/socket.h>   // for sockaddr_in

Client::Client(long id, int fd, struct sockaddr_in addr)
    : _id(id), _client_fd(fd), _client_addr(addr) {}

Client::Client(const Client& other)
    : _id(other._id), _client_fd(other._client_fd), _client_addr(other._client_addr) {}

Client::~Client(){
  DEBUG_PRINT("Destructor of Client called.");
}
