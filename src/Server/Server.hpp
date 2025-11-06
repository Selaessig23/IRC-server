#include <iostream>
#include <netinet/in.h> //for socket, bind, listen, accept
#include <sys/socket.h> //sockaddr_in
#include <vector>

#ifndef SERVER_HPP
#define SERVER_HPP

class Server {
private:
  int _fd_server;
  struct sockaddr_in _addr;
  std::string _pw;
  std::vector<struct pollfd> _poll_fds;

public:
  Server();
  ~Server();
  int init(int port);
  int set_pw(std::string pw);
};

#endif // SERVER_HPP
