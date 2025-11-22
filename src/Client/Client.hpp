#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <netinet/in.h>   // for sockaddr_in
#include <iostream>
#include <map>
#include <poll.h>

class Client{
 private:
  long _id;
  int _client_fd;
  bool	_registered;
  struct sockaddr_in _client_addr;
  struct pollfd *_poll;
  std::string _nick;
  std::string _user;
  std::string _host;
  std::string _output_buffer;
  std::map<std::string, bool> channel_inscriptions;

 public:
  Client(long id, int fd, struct sockaddr_in addr, struct pollfd &pollstruct);
  Client(const Client& other);
  Client operator=(const Client& other);
  ~Client();

  //member functions

  //getter and setter function
  std::string &getClientOut();
  void setClientOut(std::string newOutput);
  void addClientOut(std::string &newOutput);
  void addClientOut(std::string newOutput);
  int getClientFd();
  std::string &getNick();
  bool getRegisterStatus();
  void setServerPoll();

} ;

# endif
