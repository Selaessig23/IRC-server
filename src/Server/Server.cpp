#include "Server.hpp"
#include <netinet/in.h>  //for socket, bind, listen, accept
#include <poll.h>
#include <sys/socket.h>  //sockaddr_in
#include <unistd.h>      //for close()
#include <unistd.h>
#include <cstring>  //memset
#include <iostream>
#include <stdexcept>  // to throw exceptions for runtime
#include "../CONSTANTS.hpp"
#include <poll.h>

Server::Server(int port, std::string& pw) {
  // TODO: validate Port num
  // On Unix/Linux, binding to ports <1024 usually requires root privileges.
  // Ports like 53 (DNS) or 123 (NTP) are either UDP or reserved
  // Ephemeral ports (49152–65535): These are meant for client-side connections,
  // not servers. 	Technically, you can bind a server here, but clients
  // might have trouble 	connecting due to firewall/NAT setups. Ports
  // that are already in use by another service will throw an error when trying
  // to bind to them
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
//   if (bind(_fd_server, reinterpret_cast<struct sockaddr*>(&_addr), sizeof(_addr)) < 0) {
  if (bind(_fd_server, (struct sockaddr*)&_addr, sizeof(_addr)) < 0) {
    close(_fd_server);
    throw std::runtime_error("Binding Error.");
  }
  // TODO:verify pw, if there are function specific rules
  _pw = pw;
}

Server::~Server() {};

int	Server::initiatePoll(int client_fd)
{
	if (client_fd != 0)
	{
		  struct pollfd ServerPoll;
		  ServerPoll.fd = client_fd;
		  ServerPoll.events = POLLIN;
		  ServerPoll.revents = 0;
		  _poll_fds.push_back(ServerPoll);
	}
	// int poll(struct pollfd *fds, nfds_t nfds, int timeout);
	// timeout = 0 for non-blocking
	poll(&_poll_fds[0], _poll_fds.size(), 0);
}

/**
 * @brief function to establish server loop
 * @return returns 1 in case of an error, otherwise 0
 */
int Server::init() {
	// set the socket to be non-blocking
	// fcntl(_fd_server, F_SETFL, O_NONBLOCK);
  // This creates a passive socket like used in server applications
  if (listen(_fd_server, MAX_QUEUED) < 0) {
    close(_fd_server);
    std::cout << "Listen Error" << std::endl;
    return 1;
  }
  #ifdef DEBUG
    std::cout << "Server listening on port: " << _port << std::endl;
  #endif
  // attempt to establish a server loop that accepts several clients to connect
  // use the socket_fd to check for new connections with poll-function
  struct pollfd ServerPoll;
  ServerPoll.fd = _fd_server;
  ServerPoll.events = POLLIN;
  ServerPoll.revents = 0;
  _poll_fds.push_back(ServerPoll);
  const char *msg_welcome = "Hello from server!\n";
  const char *msg_waiting= "Please say something, server is waiting on response!\n";
  ssize_t recv_len = 0;
  char buf[1024];
  while (1)
  {
	  int client_fd = 0;
  	  #ifdef DEBUG
	    std:: cout << "Waiting on new connections ..." << std::endl;
  	  #endif
  	client_fd = accept(_fd_server, NULL, NULL); // waits until connection was created
   	if (client_fd < 0)
   	{
   		std::cout << "Error: problems with accept" << std::endl;
   		return (1); // return error or continue loop?
   	}
	else
	{
		send(client_fd, msg_welcome, strlen(msg_welcome), 0);
	}
	initiatePoll(client_fd);
		// move it to poll-function
		while (recv_len <= 0)
		{
			std::cout << "Waiting on response from client" << std::endl;
			send(client_fd, msg_waiting, strlen(msg_waiting), 0);
			recv_len = recv(client_fd, buf, sizeof(buf) - 1, 0); // waits until it receives any responce from client
		}
		buf[recv_len] = '\0';
		std::cout << "Message from client: " << buf << "length: " << recv_len << std::endl;
	// TODO: add client_fd to vector of Client-class to manage connections to different clients
  }
  return 0;
}
