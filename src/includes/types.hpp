#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>

class Client;

enum PARSE_ERR {
  NO_ERR = 0,
  SYNTHAX = 100,
  EMPTY_CMD = 300,
  ERR_INPUTTOOLONG = 417,
  ERR_UNKNOWNCOMMAND = 421,
  ERR_NONICKNAMEGIVEN = 431,
  ERR_ERRONEUSNICKNAME = 432,
  ERR_NICKNAMEINUSE = 433,
  ERR_NICKCOLLISION = 436,
  ERR_NOTREGISTERED = 451,
  ERR_NEEDMOREPARAMS = 461,
  ERR_ALREADYREGISTERED = 462,
  ERR_PASSWDMISMATCH = 464,
};

enum RPL_MSG {
  RPL_WELCOME = 001,
  RPL_YOURHOST = 002,
  RPL_CREATED = 003,
  RPL_INTERN_SETNICK = 101,
  RPL_INTERN_CHANGENICK = 102,
};

enum CLIENT_AUTH_STATE { AUTH_NEW = 0, AUTH_SASL = 1, AUTHENTIFICATED = 2 };

struct cmd_obj {
  PARSE_ERR error;
  std::vector<std::string> tags;
  std::string prefix;
  std::string command;
  Client* client;
  std::vector<std::string> parameters;
};

#endif  //TYPES_HPP
