#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>  // for sockaddr_in
#include <poll.h>
#include <iostream>
#include <map>

class Client {
 private:
  long _id;
  int _client_fd;
  bool _registered;
  struct sockaddr_in _client_addr;
  struct pollfd* _poll;
  std::string _received_packs;
  std::string _nick;
  std::string _user;
  std::string _host;
  std::string _output_buffer;
  std::map<std::string, bool> channel_inscriptions;

 public:
  Client();
  Client(long id, int fd, struct sockaddr_in addr, struct pollfd& poll_struct);
  Client(const Client& other);
  Client operator=(const Client& other);
  ~Client();

  //member functions

  //getter and setter function
  std::string& get_client_out();
  void set_client_out(std::string new_output);
  void add_to_received_packs(std::string new_pack);
  void clip_current_command(size_t delimiter);
  std::string get_received_packs();
  void add_client_out(std::string newOutput);
  int get_client_fd();
  std::string& get_nick();
  bool get_register_status();
  void set_server_poll();
};

#endif  //CLIENT_HPP
