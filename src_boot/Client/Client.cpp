/**
 * @brief the constructor is responsible for the check of the following issues:
 * the data_input gets checked
 *
 * if there is an error, an exception is thrown
 */
 Client(std::string irc_address, int port, std::string pw, std::string data_input){
	     // Prepare the address and port for the server socket
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET; // IPv4
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1, localhost
    sa.sin_port = htons(PORT);

  _client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (_client_fd < 0)
    throw std::runtime_error("Socket creation error.");
 }


/**
 * @brief this function tries to connect to the server
 * if there is an error while trying to connect to the server,
 * it will be tried 3 times otherwise an exception is thrown
 */
int Client::init(){
	// [...']
}
