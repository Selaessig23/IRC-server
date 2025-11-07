#include "Server.hpp"
#include "../CONSTANTS.hpp"
#include <cstring> //memset
#include <iostream>
#include <netinet/in.h> //for socket, bind, listen, accept
#include <poll.h>
#include <sys/socket.h> //sockaddr_in
#include <unistd.h> //for close()
#include <stdexcept> // to throw exceptions for runtime
#include <unistd.h>

Server::Server(int port, std::string &pw)
{
  // TODO: validate Port num
  // On Unix/Linux, binding to ports <1024 usually requires root privileges.
  // Ports like 53 (DNS) or 123 (NTP) are either UDP or reserved
  // Ephemeral ports (49152–65535): These are meant for client-side connections, not servers. 
  // 	Technically, you can bind a server here, but clients might have trouble 
  // 	connecting due to firewall/NAT setups.
  // Ports that are already in use by another service will throw an error when trying to bind to them
  _port = port;
  _fd_server = socket(AF_INET, SOCK_STREAM, 0);
  if (_fd_server < 0) 
	  throw std::runtime_error("Socket creation error.");
  // TODO: destroy socket on error and in destructor
  std::memset(&_addr, 0, sizeof(_addr));
  _addr.sin_family = AF_INET;
  _addr.sin_addr.s_addr = INADDR_ANY;
  _addr.sin_port = htons(port);
  // Assign socket to IP & Port
  if (bind(_fd_server, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
  {
	  close(_fd_server);
	  throw std::runtime_error("Binding Error.");
  }
  //TODO:verify pw, if there are function specific rules 
  _pw = pw;
}

Server::~Server() {};

/**
 * @brief function to establish server loop
 * @return returns 1 in case of an error, otherwise 0
 */
int Server::init()
{
  // This creates a passive socket like used in server applications
  if (listen(_fd_server, MAX_QUEUED) < 0)
  {
	  close(_fd_server);
	  std::cout << "Listen Error" << std::endl;
	  return 1;
  }
  std::cout << "Server listening on port: " << _port << std::endl;
  // attepmt to establish a server loop that accepts several clients to connect
  struct pollfd ServerPoll;
  ServerPoll.revents = 0;
  _poll_fds.push_back(ServerPoll);
//   while (1)
//   {
// 	int client_fd = accept(_fd_server, NULL, NULL); 
// 	if (client_fd < 0)
// 	{
// 		std::cout << "error with accept" << std::endl;
// 		return (1); //return error or continue loop?
// 	}
    // try to use the client connection (read/write)
//   } 
  return 0;
}

