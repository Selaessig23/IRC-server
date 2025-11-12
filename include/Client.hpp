#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <netinet/in.h>   // for sockaddr_in
#include <sys/socket.h>   // for sockaddr_in
#include <iostream>

class Client{
 private:
  long _id;
  std::string _name;
  struct sockaddr_in _client_addr;

 public:
  Client(long id, struct sockaddr_in addr);
  Client(const Client& other);
  Client operator=(const Client& other);
  ~Client();

} ;

# endif
