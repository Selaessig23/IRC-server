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
  unsigned char _registered;
  struct sockaddr_in _client_addr;
  bool _opertr;
  std::string _received_packs;
  std::string _nick;
  std::string _user;
  std::string _host;
  std::string _servername;
  std::string _realname;
  std::string _output_buffer;
  std::map<std::string, bool> channel_inscriptions;

 public:
  Client();
  Client(long id, int fd, struct sockaddr_in addr);
  Client(const Client& other);
  Client operator=(const Client& other);
  ~Client();

  //overload for find-functionality
  bool operator==(const std::string& other) const;

  //member functions
  void clip_current_command(size_t delimiter);

  //getter and setter and adder functions
  std::string& get_client_out();
  void set_client_out(std::string new_output);
  void add_to_received_packs(std::string new_pack);
  std::string get_received_packs();
  void add_client_out(std::string newOutput);
  int get_client_fd();
  std::string& get_nick();
  std::string& get_user();
  std::string& get_host();
  std::string& get_realname();
  bool get_opertr();
  unsigned char get_register_status();
  void set_register_status(unsigned char to_add);
  void set_nick(std::string nick);
  void set_user(std::string user);
  void set_host(std::string host);
  void set_opertr(bool value);
  void set_realname(std::string realname);
  void set_servername(std::string realname);
};

#endif  //CLIENT_HPP
