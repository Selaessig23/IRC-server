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
#include "../Utils/Utils.hpp"
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
static bool ft_open_inputfile(const char* path_infile,
                              std::stringstream& buffer) {
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
 * setsockopt(_fd_client, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 *
 * Restriction: program only works if it runs on the same host like server
 * (localhost)
 */

Bot::Bot(int port, std::string pw, std::string data_input)
    : _pw(pw),
      _registered(false),
      _operator(false),
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
  _client_fd = 0;
  int opt = 1;
  setsockopt(_client_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
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

Bot::Bot(const Bot& other)
    : _client_fd(other._client_fd),
      _pw(other._pw),
      _client_addr(other._client_addr),
      _registered(other._registered),
      _operator(other._operator),
      _received_packs(other._received_packs),
      _nick(other._nick),
      _user(other._user),
      _host(other._host),
      _realname(other._realname),
      _output_buffer(other._output_buffer),
      _channel_list(other._channel_list),
      _swear_words(other._swear_words),
      _warned_members(other._warned_members) {}

Bot Bot::operator=(const Bot& other) {
  Bot temp(other);
  std::swap(*this, temp);
  return (*this);
}

Bot::~Bot() {
  close(_client_fd);
  DEBUG_PRINT("Destructor was called successfully");
}

void Bot::add_to_received_packs(std::string new_pack) {
  _received_packs += new_pack;
}

std::string Bot::get_received_packs() {
  return _received_packs;
}

/**
 * @brief Remove command AND the \r\n delimiter
 */
void Bot::clip_current_command(size_t delimiter) {
  if (delimiter + 2 <= _received_packs.size()) {
    _received_packs = _received_packs.substr(delimiter + 2);
  } else {
    _received_packs.clear();
  }
}

/**
 * @brief function to get the amount of strikes of a nick
 *
 * @return if nick was not found in list (== no strike yet), it returns -1
 */
int Bot::get_strikes(const std::string& nick) {
  std::map<std::string, int>::iterator it_member = _warned_members.find(nick);
  if (it_member == _warned_members.end())
    return (-1);
  else
    return it_member->second;
}

/**
 * @brief function to handle a pollout event of the bot
 * to send from bots buffer to sever until buffer is empty
 * if buffer is empty, poll-event POLLOUT of the bot gets removed
 * everything that has been sent to server becomes removed from bots out-buffer
 */
void Bot::handle_pollout(struct pollfd& pfd) {

  //   int size_sent =
  //       send(pfd.fd, _output_buffer.c_str(), strlen(_output_buffer.c_str()), 0);
  int size_sent = send(pfd.fd, _output_buffer.data(), _output_buffer.size(), 0);
  std::string new_out = _output_buffer;
  new_out.erase(0, size_sent);
  _output_buffer = new_out;
  DEBUG_PRINT("outbutbuffer empty? " << _output_buffer);
  if (_output_buffer.empty()) {
    DEBUG_PRINT("outbutbuffer bot empty");
    pfd.events &= ~POLLOUT;
  }
}

#ifdef DEBUG
static void debug_parsed_cmds(cmd_obj& cmd_body) {

  std::cout << "\nCMD_BDY: " << std::endl;
  if (cmd_body.error)
    std::cout << "ERR: " << cmd_body.error << std::endl;
  if (!cmd_body.tags.empty())
    std::cout << "TAGS: " << *cmd_body.tags.begin() << std::endl;
  if (!cmd_body.prefix.empty())
    std::cout << "PREFIX: " << cmd_body.prefix << std::endl;
  if (!cmd_body.command.empty())
    std::cout << "CMD: " << cmd_body.command << std::endl;
  if (!cmd_body.parameters.empty()) {
    std::cout << "PARAS:" << std::endl;
    std::vector<std::string>::iterator it = cmd_body.parameters.begin();
    for (; it != cmd_body.parameters.end(); it++) {
      std::cout << *it << std::endl;
    }
  }
}
#endif

/**
 * @brief function to handle a pollin event from one of the clients fds
 * in case there is no input to read, it is interpreted as server was lost
 *
 * the bot is able to handle the following RPL_MSGs from an irc-server
 * RPL_WELCOME = 001,
 * CONFIRMATION of operator-status
 * RPL_INVITING = 341 | INVITE msg,
 * PRIVMSG of clients and channels the bot is member of
 * 
 */
int Bot::handle_pollin(struct pollfd& pfd) {

  static int count_register = 1;
  static int count_oper = 0;
  char buf[8750];
  int recv_len = recv(pfd.fd, buf, sizeof(buf) - 1, 0);

  if (recv_len <= 0) {
    //add handling if server-connection ends
    return (1);
  }

  buf[recv_len] = '\0';

  _received_packs += buf;

  while (_received_packs.find("\r\n") != std::string::npos) {

    cmd_obj cmd_body;
    cmd_body.bot_obj = this;
    PARSE_ERR err = Parsing::parse_command(cmd_body);

#ifdef DEBUG
    if (err)
      std::cout << "\nERR: " << err << std::endl;
    else
      debug_parsed_cmds(cmd_body);
#else
    (void)err;
#endif
    int err_code = 0;
    Utils::ft_convert_to_int(err_code, cmd_body.command);
    if (_registered == false &&
        cmd_body.command ==
            "001") {  // case RPL_WELCOME:" :Welcome to the " + base._network_name + " Network, " +
      _registered = true;
      count_oper += 1;
      become_operator(pfd);
    } else if (_registered == true && cmd_body.command == "381")
      // case RPL_YOUREOPER: ":You are now an IRC operator"
      _operator = true;
    else if (_registered == true &&
             (cmd_body.command == "JOIN" || cmd_body.command == "353"))
      // handle join
      handle_join(cmd_body);
    else if (
        _registered == true &&
        (cmd_body.command == "341" ||
         cmd_body.command ==
             "INVITE"))  // case RPL_INVITING: "<client> <nick> <channel> :INVITES YOU" || Invite message
      handle_invitation(cmd_body, pfd);
    else if (_registered == true && cmd_body.command == "PRIVMSG")
      check_for_swears(cmd_body, pfd);  // sanctioning
#ifdef DEBUG
    else if (err_code >= 400)
      std::cout << "Bot is unable to handle error messages" << std::endl;
#endif
    // add an error-message reader with dummy response
    else {
      if (_registered == false && count_register >= 4) {
        DEBUG_PRINT("Error in registration process at IRC-server");
        return (1);
      } else if (_registered == false) {
        count_register += 1;
        register_at_irc(pfd);
      } else if (_registered == true && _operator == false && count_oper == 4) {
        count_oper += 1;
        DEBUG_PRINT(
            "No operator-status possible at IRC-server, Bot can only sanction "
            "clients, no KILL");
      } else if (_registered == true && _operator == false && count_oper < 4) {
        count_oper += 1;
        become_operator(pfd);
      }
    }
  }
  return (0);
}

/**
 * @brief this functions handles 
 * (1) communication with irc-server by setting up a poll-loop
 * (2) attempts to register at local irc-server (max. 4 times)
 * (3) attempts to become an operator on the irc-server (max. 4 times)
 *
 * TODO:
 * (1) handle disconnection accordingly
 *
 * @return returns 1 in case of an error (e. g. registration impossible, 
 * disconnection from server-side)
 */
int Bot::init_poll() {
  //   int count_register = 1;
  //   int count_oper = 0;
  struct pollfd client_poll;
  client_poll.fd = _client_fd;
  client_poll.events = POLLIN;
  client_poll.revents = 0;
  register_at_irc(client_poll);
  while (1) {
    poll(&client_poll, 1, -1);
    if (client_poll.revents & POLLIN) {
      if (handle_pollin(client_poll))
        return (1);
    } else if (client_poll.revents & POLLOUT) {
      DEBUG_PRINT("case pollout:" << _output_buffer);
      handle_pollout(client_poll);
    }
  }
  return (0);
}

