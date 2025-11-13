#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <netinet/in.h>   // for sockaddr_in
#include <sys/socket.h>   // for sockaddr_in
#include <iostream>

class Client{
 private:
  long _id;
  int _client_fd;
  struct sockaddr_in _client_addr;
  std::string _name;
  std::string _output_buffer;

 public:
  Client(long id, int fd, struct sockaddr_in addr);
  Client(const Client& other);
  Client operator=(const Client& other);
  ~Client();

} ;

# endif
