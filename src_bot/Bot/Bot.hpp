#ifndef BOT_HPP
#define BOT_HPP

#include <netinet/in.h>  //for socket, bind, listen, accept
#include <sys/socket.h>  //sockaddr_in
#include <poll.h>
#include <string>
#include <map>
#include <set>

class Bot{
 private:
  int _client_fd;
  std::string _pw;
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
  std::set<std::string> _swear_words;
 
  //member function (helper)
  int poll_loop();

 public:
  Bot(int port, std::string pw, std::string data_input);
  Bot(const Bot& other);
  Bot operator=(const Bot& other);
  ~Bot();

  int init(int attempts);
};

#endif // CLIENT_HPP

