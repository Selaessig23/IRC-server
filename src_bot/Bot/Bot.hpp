#ifndef BOT_HPP
#define BOT_HPP

#include <netinet/in.h>  //for socket, bind, listen, accept
#include <poll.h>
#include <sys/socket.h>  //sockaddr_in
#include <list>
#include <set>
#include <string>
#include "../includes/types.hpp"
#include "../Channel/Channel.hpp"

class Bot {
 private:
  int _client_fd;
  std::string _pw;
  struct sockaddr_in _client_addr;  //necessary?
  bool _registered;
  bool _operator;
  std::string _received_packs;
  std::string _nick;
  std::string _user;
  std::string _host;
  std::string _realname;
  std::string _output_buffer;
  std::list<Channel> _channel_list;
  std::set<std::string> _swear_words;

  //member function (helper)
  void handle_pollout(struct pollfd& pfd);
  int handle_pollin(struct pollfd& pfd);
  void register_at_irc(struct pollfd& pfd);
  void become_operator(struct pollfd& pfd);
  int handle_invitation(cmd_obj& cmd_body, struct pollfd& pfd);
  int handle_join(cmd_obj& cmd_body, struct pollfd& pfd);
  int check_for_swears(cmd_obj& cmd_body, struct pollfd& pfd);
  void sanctioning(const std::string &nick, std::string &channel, std::string& out);
  int kill_clients(const std::string &nick);

 public:
  Bot(int port, std::string pw, std::string data_input);
  Bot(const Bot& other);
  Bot operator=(const Bot& other);
  ~Bot();

  int init_poll();
};

#endif  // CLIENT_HPP

