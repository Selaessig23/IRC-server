#include <map>
#include <sstream>
#include "../includes/types.hpp"

namespace Parsing {
  /**
 * @brief breaks down incomming string into cmd and params
 * (1) tokenizes the string
 * (2) validates and first error handling
 *
 * TODO:
 * (1) check for tags: Optional metadata on a message, starting with ('@', 0x40)
 *      - Basically, a series of <key>[=<value>] segments, separated by (';', 0x3B).
 * (2) check for prefix(source): optional; the presence of a prefix is indicated with a single leading ASCII
   colon character (':', 0x3b) (sic!), without gap between colon and prefix
 * (3) devide tokens into command and parameters
 *      - The command must either be a valid IRC command or a three (3) digit number represented in ASCII text.
 *      - up to 15 parameters
 *      - different types of parameters: 
 *        + trailing, requires ':': Any, possibly *empty*, sequence of octets not including NUL or CR or LF
 *         (in which case that character is stripped and the rest of the message is treated as the final parameter, including any spaces it contains)
 *        + middle: Any *non-empty* sequence of octets not including SPACE or NUL or CR or LF
 * (4) consider syntax rules of irc-protocoll
 *      - The prefix, command, and all parameters are separated by one (or more) ASCII space character(s)
 *      - TAB is considered to be a non-white-space
 *      - NUL is not allowed within messages
 *      - IRC messages are always lines of characters terminated with a CR-LF (Carriage Return - Line Feed) pair, 
 *      - the messages shall not exceed 512 characters in length (including CR-LF)
 * see: https://modern.ircdocs.horse/#message-format | https://www.rfc-editor.org/rfc/rfc1459.html#section-2.3.1
 *
 */
  int ParseCommand(std::string input, cmd_obj& command_body) {
    std::istringstream input_stream(input);
    std::string token;
    std::vector<std::string> parsed_elements;

    command_body.error = NO_ERR;
    while (input_stream >> token) {
      parsed_elements.push_back(token);
    }

    if (parsed_elements.empty()) {
      command_body.error = EMPTY_CMD;
      return command_body.error;
    }
    std::map<std::string, CMD_TYPE> commands;
    commands["PRIVMSG"] = PRIVMSG;
    commands["PASS"] = PASS;
    commands["JOIN"] = JOIN;
    commands["NICK"] = NICK;
    commands["CAP"] = CAP;

    std::map<std::string, CMD_TYPE>::iterator it;
    for (it = commands.begin(); it != commands.end(); ++it) {
      if (parsed_elements[0] == it->first) {
        command_body.command = it->second;
        break;
      }
    }
    if (it == commands.end())
	    command_body.command = UNKNOWN;

    if (command_body.command == UNKNOWN) {
      command_body.error = UNKNOWN_CMD;
      return command_body.error;
    }

    return NO_ERR;
  }
}  // namespace Parsing
