#include "Client.hpp"
#include <netinet/in.h>   // for sockaddr_in
#include <sys/socket.h>   // for sockaddr_in

Client::Client(long id, struct sockaddr_in addr)
    : _id(id), _client_addr(addr) {}

Client::Client(const Client& other)
    : _id(other._id), _client_addr(other._client_addr) {}

Client::~Client(){
#ifdef DEBUG
  std::cout << "Destructor of Client called." << std::endl;
#endif
}
