#include <map>
#include <sstream>
#include "../debug.hpp"
#include "../includes/types.hpp"
#include "./Parser_utils.cpp"

namespace Parsing {
  /**
 * @brief breaks down incomming string into cmd and params
 * (1 --> commented out for testing reasons) it checks if message ends with CR-LF, if not it returns an individual message
 *     as there is no official one: input line to long (max size of input according to recv-buf is 8750 bytes)
 * (2) tokenizes the string
 * (3) it checks for max size of message (if correclty terminated with CR-LF)
 * (4) it splits the tags and add them into com_obj.tags, if there are tags
 * (5) it checks if there is a prefix and adds it to com_onj.prefix
 * (6) it validates commands and does first error handling, if command does not exist
 * (7) it add paramters to com_obj_parameters
 * (8) it returns an error if size of parameters > 15: no error code provided by protocol, must clients return
 *    417 ERR_INPUTTOOLONG (so do I)
 *
 * DONE:
 * (1) check for tags: Optional metadata on a message, starting with ('@', 0x40)
 *      - Basically, a series of <key>[=<value>] segments, separated by (';', 0x3B).
 *      - if set, they extend the max length if the message by 4096 additional bytes (only for tags)
 * (2) check for prefix(source): optional; the presence of a prefix is indicated with a single leading ASCII
   colon character (':', 0x3b) (sic!), without gap between colon and prefix
 * (3) devide tokens into command and parameters
 *      - up to 15 parameters
 * (4) consider syntax rules of irc-protocoll
 *      - The prefix, command, and all parameters are separated by one (or more) ASCII space character(s)
 *      - TAB is considered to be a non-white-space
 *      - NUL is not allowed within messages: how to check it?
 *      - IRC messages are always lines of characters terminated with a CR-LF (Carriage Return - Line Feed) pair, 
 *      - the messages shall not exceed 512 characters in length (including CR-LF)
 *
 * TODO:
 * (1) consider different types of parameters:
 *        + trailing, requires ':': Any, possibly *empty*, sequence of octets not including NUL or CR or LF
 *         (in which case that character is stripped and the rest of the message is treated as the final parameter, including any spaces it contains)
 *        + middle: Any *non-empty* sequence of octets not including SPACE or NUL or CR or LF
 *
 * see: https://modern.ircdocs.horse/#message-format | https://www.rfc-editor.org/rfc/rfc1459.html#section-2.3.1
 *
 */
  int ParseCommand(std::string input, cmd_obj& command_body) {
    std::istringstream input_stream(input);
    std::string token;
    std::vector<std::string> parsed_elements;

    // commented out as it makes testing harder
    //     if (input[input.size() - 1] != '\n' || input[input.size() - 2] != '\r') {
    //       command_body.error = ERR_INPUTTOOLONG;
    //       return (417);
    //     }
    command_body.error = NO_ERR;
    while (input_stream >> token) {
      parsed_elements.push_back(token);
    }

    if (parsed_elements.empty()) {
      // should be treated silently
      command_body.error = EMPTY_CMD;
      return command_body.error;
    }

    if (((!(*parsed_elements.begin()).empty()) &&
         ((*parsed_elements.begin())[0] == '@') &&
         ((parsed_elements.begin()->size() > 4096) ||
          ((input.size() - parsed_elements.begin()->size()) > 512))) ||
        (input.size() > 512)) {
      command_body.error = ERR_INPUTTOOLONG;
      return (417);
    }
    std::vector<std::string>::iterator it = parsed_elements.begin();

    if ((*it)[0] == '@') {
      std::string tags = *parsed_elements.begin();
      unsigned long cut = 0;
      unsigned long pos = tags.find(';', cut);
      while (pos != std::string::npos) {
        command_body.tags.push_back(tags.substr(cut, pos - cut));
        cut = pos + 1;
        pos = tags.find(';', cut);
      }
      if (cut < tags.size()) {
        command_body.tags.push_back(tags.substr(cut));
      }
      it++;
    }

    if (it != parsed_elements.end() && (*it)[0] == ':') {
      command_body.prefix = *it;
      it++;
    }

    if (it == parsed_elements.end()) {
      command_body.error = EMPTY_CMD;
      return command_body.error;
    }
    std::map<std::string, CMD_TYPE> commands = build_command_map();
    std::map<std::string, CMD_TYPE>::iterator it_comm = commands.find(*it);

    if (it_comm != commands.end()) {
      command_body.command = it_comm->second;
    } else if (isValidNumericCommand(*it)) {
      command_body.command = NUMERIC;
      command_body.numeric_code = std::atoi((*it).c_str());
    } else {
      command_body.command = UNKNOWN;
      command_body.error = UNKNOWN_CMD;
      return command_body.error;
    }

    it++;

    for (; it != parsed_elements.end(); it++) {
      command_body.parameters.push_back(*it);
    }
    if (!command_body.parameters.empty() &&
        command_body.parameters.size() > 15) {
      command_body.error = ERR_INPUTTOOLONG;
      return (417);
    }

    return NO_ERR;
  }
}  // namespace Parsing
