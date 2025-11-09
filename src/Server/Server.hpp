#include <netinet/in.h>  //for socket, bind, listen, accept
#include <sys/socket.h>  //sockaddr_in
#include <iostream>
#include <vector>

#ifndef SERVER_HPP
#define SERVER_HPP

class Server {

 private:
  int				_port;
  int 				_fd_server;
  struct sockaddr_in		_addr;
  std::string			_pw;
  std::vector<struct pollfd>	_poll_fds;

 public:
  // OCF
  Server(int port, std::string& pw);
  Server(const Server& other);
  Server& operator=(const Server& other);
  ~Server();

  // function to activate the IRC-Server (run the server-loop)
  int init();
};

#endif  // SERVER_HPP
