#include "Server.hpp"
#include "../CONSTANTS.hpp"
#include <cstring> //memset
#include <iostream>
#include <netinet/in.h> //for socket, bind, listen, accept

Server::Server() : _fd_server(-1) {};

Server::~Server() {};

int Server::init(int port) {
  // TODO: validate Port num
  _fd_server = socket(AF_INET, SOCK_STREAM, 0);
  if (_fd_server < 0) {
    std::cerr << "Socket creation error" << std::endl;
    return 1;
  }
  // TODO: destroy socket on error and in destructor
  std::memset(&_addr, 0, sizeof(_addr));
  _addr.sin_family = AF_INET;
  _addr.sin_addr.s_addr = INADDR_ANY;
  _addr.sin_port = htons(port);

  // Assign socket to IP & Port
  if (bind(_fd_server, (struct sockaddr *)&_addr, sizeof(_addr)) < 0) {
    std::cerr << "Binding Error" << std::endl;
    return (1);
  };
  if (listen(_fd_server, MAX_QUEUED) < 0) {
    std::cout << "Listen Error" << std::endl;
    return 1;
  }
  std::cout << "Server listening on port: " << port << std::endl;
  return 0;
}

int Server::set_pw(std::string pw) {
  // TODO: validate pw
  _pw = pw;
  return 0;
}
