#include <iostream>
#include <netinet/in.h> //for socket, bind, listen, accept
#include <sys/socket.h> //sockaddr_in

#ifndef SERVER_HPP
#define SERVER_HPP

class Server {
private:
  int _fd_server;
  struct sockaddr_in _addr;
  std::string _pw;

public:
  Server();
  ~Server();
  int init(int port);
  int set_pw(std::string pw);
};

#endif // SERVER_HPP
