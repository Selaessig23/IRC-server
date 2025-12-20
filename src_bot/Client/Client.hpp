#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>  //for socket, bind, listen, accept
#include <sys/socket.h>  //sockaddr_in
#include <poll.h>
#include <string>
#include <map>
#include <vector>

class Client {
 private:
  int _client_fd;
  struct sockaddr_in _client_addr;
  unsigned char _registered;
  std::string _received_packs;
  std::string _nick;
  std::string _user;
  std::string _host;
  std::string _servername;
  std::string _realname;
  std::string _output_buffer;
  std::map<std::string, bool> channel_inscriptions;
  std::vector<std::string> _swear_words;
 
  //member function (helper)
  int poll_loop();

 public:
  Client(int port, std::string pw, std::string data_input);
  Client(const Client& other);
  Client operator=(const Client& other);
  ~Client();

  int init(int attempts);
};

#endif // CLIENT_HPP

