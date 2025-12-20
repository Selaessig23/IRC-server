#include "Bot.hpp"
#include <arpa/inet.h>   // htonl
#include <netinet/in.h>  // for socket, bind, listen, accept
#include <poll.h>
#include <sys/socket.h>  // sockaddr_in
#include <unistd.h>      // for close()
#include <cstring>       // memset
#include <fstream>       // std::ifstream,
#include <iostream>
#include <map>
#include <sstream>  // std::stringstream, std::stringbuf
#include <stdexcept>

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
 * setsockopt(_fd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 *
 * if there is an error, an exception is thrown
 *
 * Restriction: program only works if it runs on the same host like server
 * (localhost)
 *
 * TODO:
 * (1) close socket in case of an error
 */

Bot::Bot(int port, std::string pw, std::string data_input) : _pw(pw) {
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
}

Bot::~Bot() {}

/**
 * @brief this function tries to connect to the server
 * if there is an error while trying to connect to the server,
 * it will be tried 3 times otherwise an exception is thrown
 * it uses the password of the server, 
 * sets the nick to SWEARBOT, and
 * sets user to [...]
 *
 */
int Bot::init(int attempts) {
  // [...']
  (void)attempts;
  return (0);
}

