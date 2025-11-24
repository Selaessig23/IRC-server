#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>

// IMPORTANT. Unknown serves as the enums size as well, therefore it has to stay the last element
enum CMD_TYPE {
  PRIVMSG,
  PASS,
  JOIN,
  NICK,
  CAP,
  PING,
  UNKNOWN,
};

enum PARSE_ERR {
  NO_ERR = 0,
  SYNTHAX = 100,
  EMPTY_CMD = 300,
  ERR_INPUTTOOLONG = 417,
  ERR_UNKNOWNCOMMAND = 421,
  ERR_NOTREGISTERED = 451,
  ERR_NEEDMOREPARAMS = 461,
  ERR_ALREADYREGISTERED = 462, 
  ERR_PASSWDMISMATCH = 464,
};

enum RPL_MSG {
 RPL_WELCOME = 001,
 RPL_YOURHOST = 002,
 RPL_CREATED = 003,
};

struct cmd_obj {
  PARSE_ERR error;
  std::vector<std::string> tags;
  std::string prefix;
  CMD_TYPE command;
  std::vector<std::string> parameters;
};

#endif  //TYPES_HPP
