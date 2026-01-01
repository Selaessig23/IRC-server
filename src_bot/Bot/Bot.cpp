#include "Bot.hpp"
#include <arpa/inet.h>   // htonl
#include <netinet/in.h>  // for socket, bind, listen, accept
#include <poll.h>
#include <sys/socket.h>  // sockaddr_in
#include <unistd.h>      // for close()
#include <cstring>       // memset
#include <fstream>       // std::ifstream,
#include <iostream>
#include <sstream>  // std::stringstream, std::stringbuf
#include <stdexcept>
#include "../Parser/Parser.hpp"
#include "../debug.hpp"
#include "../includes/types.hpp"

/**
 * TODO
 * (1) add all parameters to copy constructor
 */

/**
 * @brief function to open an inputfile, check for access 
 * and sent it to a std::stringstream buffer 
 * 
 * consider: In C++98, open() can accept a const char* more reliably than std::string directly, 
 * so infile.c_str() is safer and more portable in older standards or use const char directly 
 * (instead of using const std::string& infile)
*/
bool ft_open_inputfile(const char* path_infile, std::stringstream& buffer) {
  //validate path of database
  std::ifstream inputfile;
  //attempt to open infile-parameter
  inputfile.open(path_infile, std::fstream::in);
  //check if it was opened successful (fail also considers permission rights)
  if (!inputfile.is_open() || inputfile.fail()) {
    return (false);
  }
  //read from inputfile to buffer
  buffer << inputfile.rdbuf();
  return (true);
}

//
/**
 *
 * @brief the constructor is responsible for the check of the following issues:
 * (1) the data_input gets checked
 * (2) if the connection can be established
 * if there is an error, an exception gets thrown
 *
 * TODO
 * WHY? DO I NEED IT?
 * setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 *
 * Restriction: program only works if it runs on the same host like server
 * (localhost)
 */

Bot::Bot(int port, std::string pw, std::string data_input)
    : _pw(pw),
      _registered(false),
      _nick("42BOT"),
      _user("Max"),
      _host("42host"),
      _realname("Max Bot") {
  // check if input file is valid
  std::stringstream buf;
  if (!ft_open_inputfile(data_input.c_str(), buf))
    throw std::invalid_argument("Input file not readable.");
  std::string word;
  while (std::getline(buf, word)) {
    if (!word.empty())
      //maybe add another test if there is only one word per line
      _swear_words.insert(word);
  }
  // Prepare the address and port for the server socket
  std::memset(&_client_addr, 0, sizeof(_client_addr));
  _client_addr.sin_family = AF_INET;  // IPv4
  _client_addr.sin_addr.s_addr =
      htonl(INADDR_LOOPBACK);  // CONSTANT for 127.0.0.1, localhost
  _client_addr.sin_port = htons(port);
  _client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (_client_fd < 0)
    throw std::runtime_error("Socket creation error.");
  if (connect(_client_fd, reinterpret_cast<struct sockaddr*>(&_client_addr),
              sizeof(_client_addr))) {
    close(_client_fd);
    // params: int sockfd, const struct sockaddr *addr, socklen_t addrlen
    throw std::runtime_error("Connection creation error.");
  }
  DEBUG_PRINT("Successfully connected to IRC server (not registered yet)");
}

// all member variables to be added
Bot::Bot(const Bot& other) {}

Bot Bot::operator=(const Bot& other) {
  Bot temp(other);
  std::swap(*this, temp);
  return (*this);
}

Bot::~Bot() {
  close(_client_fd);
  DEBUG_PRINT("Destructor was called succesfully");
}

/**
 * @brief this function tries to register to the server by
 * using a server password (if there is one) PASS
 * NICK
 * USER
 *
 * TODO
 * (it throws an exception if registration process fails)
 * and tries to become operator
 */
int Bot::register_at_irc(struct pollfd& pfd) {
  std::string out;
  out += "PASS " + _pw + "\r\n";
  out += "NICK " + _nick + "\r\n";
  out += "USER " + _user + " " + _host + " * " + _realname + "\r\n";
  _output_buffer += out;
  pfd.events |= POLLOUT;
}

/**
 * @brief function to handle a pollout event of the bot
 * to send from bots buffer to sever until buffer is empty
 * if buffer is empty, poll-event POLLOUT of the bot gets removed
 * everything that has been sent to server becomes removed from bots out-buffer
 */
void Bot::handle_pollout(struct pollfd& pfd) {

  int size_sent =
      send(pfd.fd, _output_buffer.c_str(), strlen(_output_buffer.c_str()), 0);
  std::string new_out = _output_buffer;
  new_out.erase(0, size_sent);
  _output_buffer = new_out;
  if (_output_buffer.empty())
    pfd.events &= ~POLLOUT;
}

/**
 * @brief function to handle a pollin event from one of the clients fds
 * in case there is no input to read, it is interpreted as server was lost
 *
 * the bot is able to handle the following RPL_MSGs from an irc-server
 * RPL_WELCOME = 001,
 * CONFIRMATION of operator-status
 * RPL_INVITING = 341,
 * PRIVMSG of channels the bot is member of
 * 
 */
int Bot::handle_pollin(struct pollfd& pfd) {

  char buf[8750];
  int recv_len = recv(pfd.fd, buf, sizeof(buf) - 1, 0);

  if (recv_len <= 0) {
    close(pfd.fd);  //add handling if server-connection ends
    return (1);
  }

  buf[recv_len] = '\0';

  _received_packs += buf;

  while (_received_packs.find("\r\n") != std::string::npos) {

    cmd_obj cmd_body;
    PARSE_ERR err = Parsing::parse_command(cmd_body);

#ifdef DEBUG
    if (err)
      std::cout << "\nERR: " << err << std::endl;
    else
      debug_parsed_cmds(cmd_body);
#else
    (void)err;
#endif
    if (cmd_body.command ==
        "001")  // case RPL_WELCOME:" :Welcome to the " + base._network_name + " Network, " +
      _registered = true;
    else if (_registered == false) // mabe remove it, too complex
      register_at_irc(
          pfd);  //check if I need a static variable here to not run into an endless loop
    else if (cmd_body.command == "341")
	    handle_invitation(cmd_body);
      // case RPL_INVITING: "<client> <nick> <channel> :INVITES YOU"
    else if (cmd_body.command == "PRIVMSG")
	    check_for_swears(cmd_body); // sanctioning(pfd);
  }
  return (0);
}

/**
 * @brief this functions sets up the poll-loop
 * to enable the bot to communicate with the irc-server
 *
 * sets the nick to SWEARBOT, and
 * sets user to [...]
 *
 */
int Bot::init_poll() {
  struct pollfd client_poll;
  client_poll.fd = _client_fd;
  client_poll.events = POLLIN;
  client_poll.revents = 0;
  register_at_irc(client_poll);
  while (1) {
    poll(&client_poll, sizeof(client_poll), 0);
    if (client_poll.revents & POLLIN) {
      if (handle_pollin())
        break;
    }
    if (client_poll.revents & POLLOUT) {
      handle_pollout(client_poll);
    }
  }

  // [...']
  return (0);
}

