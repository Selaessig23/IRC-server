#include <netinet/in.h>  //for socket, bind, listen, accept
#include <sys/socket.h>  //sockaddr_in
#include <unistd.h>
#include <iostream>
#include <list>
#include <vector>
#include "Client.hpp"

#ifndef SERVER_HPP
#define SERVER_HPP

class Server {
  typedef std::vector<struct pollfd>::iterator fd_iterator;

 private:
  int _port;
  int _fd_server;
  struct sockaddr_in _addr;
  std::string _pw;
  std::vector<struct pollfd> _poll_fds;
  std::list<Client> _client_list;

  //member function (helper)
  int InitiatePoll();
  int AddNewClient(int client_fd);

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
