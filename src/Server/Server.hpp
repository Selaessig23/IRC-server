#include <netinet/in.h>  //for socket, bind, listen, accept
#include <sys/socket.h>  //sockaddr_in
#include <unistd.h>
#include <iostream>
#include <list>
#include <vector>
#include "../Channel/Channel.hpp"
#include "../Client/Client.hpp"
#include "../includes/types.hpp"
// #include "../IrcCommands/IrcCommands.hpp"

#ifndef SERVER_HPP
#define SERVER_HPP

class IrcCommands;  // forward declaration

class Server {
  typedef std::vector<struct pollfd>::iterator fd_iterator;

 private:
  std::string _network_name;
  std::string _server_name;
  std::string _version;
  std::string _created_at;
  int _port;
  int _fd_server;
  struct sockaddr_in _addr;
  std::string _pw;
  std::vector<struct pollfd> _poll_fds;
  std::list<Client> _client_list;
  std::list<Channel> _channel_list;
  IrcCommands* _irc_commands;

  //member functions (helper)
  int initiate_poll();
  int add_new_client_to_poll(int client_fd);
  int handle_new_client();
  int handle_pollin(struct pollfd& poll_fd);
  int handle_pollout(struct pollfd& poll_fd);
  void set_pollevent(int fd, int event);
  void remove_pollevent(int fd, int event);
  void remove_client(int fd);

 public:
  // OCF
  Server(int port, std::string& pw);
  Server(const Server& other);
  Server operator=(const Server& other);
  ~Server();

  // Getters
  std::list<Channel>& get_channel_list();
  Client* find_client_by_fd(int fd);

  // function to activate the IRC-Server (run the server-loop)
  int init();

  friend class IrcCommands;
};

//helper functions (in ServerUtils.cpp)
void debug_parsed_cmds(cmd_obj& cmd_body);
std::string get_current_date_time();

#endif  // SERVER_HPP
