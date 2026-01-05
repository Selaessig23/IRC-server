#include <iomanip>  // For std::setw and std::setfill
#include <iostream>
#include <sstream>
#include "../Client/Client.hpp"
#include "../Server/Server.hpp"
#include "../includes/CONSTANTS.hpp"
#include "../includes/types.hpp"
#include "IrcCommands.hpp"

/**
 * @brief function to return the reply message of replymessagecode (rpl)
 *
 * TODO
 * (1) add all required rpl messages according to rpl_code
 */
std::string IrcCommands::get_rpl(Server& base, const cmd_obj& cmd,
                                 enum RPL_MSG rpl) {
  std::string out;
  switch (rpl) {
    case RPL_WELCOME:
      return (" :Welcome to the " + base._network_name + " Network, " +
              cmd.client->get_nick() + "!" + cmd.client->get_user() + "@" +
              cmd.client->get_host());
    case RPL_YOURHOST:
      return (" :Your host is " + base._server_name + ", running version " +
              base._version);
    case RPL_CREATED:
      return (" :This server was created " + base._created_at);
    case RPL_INTERN_SETNICK:
      return (" :Nickname was set to " + cmd.client->get_nick());
    case RPL_INTERN_CHANGENICK:
      return (" :Nickname was changed to " + cmd.client->get_nick());
    case RPL_INTERN_SETUSER:
      return (" :No ident server\nUser gets registered with username\n" +
              cmd.client->get_user() + " and real name " +
              cmd.client->get_realname());
    case RPL_CHANNELMODEIS:
      return ("<channel> <modestring> <mode arguments>...");
    case RPL_CREATIONTIME:
      return ("<channel> <creationtime>");
    case RPL_NOTOPIC:
      return ("<channel> :No topic is set");
    case RPL_TOPIC:
      return ("<channel> topic: ");
    case RPL_TOPICWHOTIME:
      return ("<channel> topic is set by ");
    case RPL_INVITING:
      return ("invites " + cmd.parameters[0] + " to " + cmd.parameters[1]);
    case RPL_NAMREPLY:
      return (
          "<symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}");  //to be added
    case RPL_ENDOFNAMES:
      return ("<channel> :End of /NAMES list");  //to be added
    case RPL_YOUREOPER:
      return (":You are now an IRC operator");
    default:
      return (" UNKNOWN REPLY");
  }
}

/**
 * @brief function to return the error message of errorcode (err)
 *
 * TODO
 * (1) add all required error messages to corresponding error codes
 * (2) maybe remove paramter base if not required
 */
std::string IrcCommands::get_error(Server& base, const cmd_obj& cmd,
                                   enum PARSE_ERR err) {
  (void)base;
  std::string out;
  std::string source;
  if (cmd.parameters.empty())
    source = "* ";
  else
    source = cmd.parameters[0];
  switch (err) {
    case EMPTY_CMD:
      return (" Command empty");
    case ERR_NOSUCHNICK:
      return (source + " :No such nick/channel");
    case ERR_NOSUCHCHANNEL:
      return ("<channel> :No such channel");
    case ERR_CANNOTSENDTOCHAN:
      return ("<channel> :No such channel");
    case ERR_INVALIDCAPCMD:
      return (source + " :Cannot handle CAP command with this target");
    case ERR_NORECIPIENT:
      return (" :No recipient given (" + cmd.command + ")");
    case ERR_NOTEXTTOSEND:
      return (" :No text to send");
    case ERR_INPUTTOOLONG:
      return (" :Input line was too long");
    case ERR_UNKNOWNCOMMAND:
      return (cmd.command + " :Unknown command");
    case ERR_NONICKNAMEGIVEN:
      return (" :No nickname given");
    case ERR_ERRONEUSNICKNAME:
      return (source + " :Erroneous nickname");
    case ERR_NICKNAMEINUSE:
      return (source + " :Nickname is already in use");
    case ERR_NICKCOLLISION:
      return (source + " :Nickname collision KILL from " +
              cmd.client->get_user() + "@" + cmd.client->get_host());
    case ERR_USERNOTINCHANNEL:
      return ("<nick> <channel> :They aren't on that channel");
    case ERR_NOTONCHANNEL:
      return ("<channel> :You're not on that channel");
    case ERR_USERONCHANNEL:
      return ("<nick> <channel> :is already on channel");
    case ERR_NOTREGISTERED:
      return (" :You have not registered");
    case ERR_NEEDMOREPARAMS:
      return (cmd.command + " :Not enough parameters");
    case ERR_ALREADYREGISTERED:
      return (" :You may not reregister");
    case ERR_PASSWDMISMATCH:
      return (" :Password incorrect");
    case ERR_CHANNELISFULL:
      return (" <channel> :Cannot join channel (+l)");
    case ERR_INVITEONLYCHAN:
      return (" <channel> :Cannot join channel (+i)");
    case ERR_BADCHANNELKEY:
      return (" <channel> :Cannot join channel (+k)");
    case ERR_NOPRIVILEGES:
      return (" :Permission Denied- You're not an IRC operator");
    case ERR_CHANOPRIVSNEEDED:
      return ("<channel> :You're not channel operator");
    default:
      return (out);
  }
}

/**
 * @brief function to sent out direct messages as answer on COMMAND-requests from client
 * direct communication from server to client, 
 * whether error-msg or rpl-messages (success)
 *
 * TODO
 * (1) set tag (only if clients support them, to check with CAP LS negotiation)
 * (2) think about changing the parameters
 */
void IrcCommands::send_message(Server& base, const cmd_obj& cmd,
                               int numeric_msg_code, bool error,
                               std::string* msg) {
  std::string out;
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(3) << numeric_msg_code;

  out += ":";
  out += base._server_name + " ";
  if (!msg)
    out += ss.str() + " ";
  if (!cmd.client->get_nick().empty())
    out += cmd.client->get_nick() + " ";
  else
    out += "* ";
  if (msg)
    out += *msg;
  else if (error == true)
    out += get_error(base, cmd, static_cast<PARSE_ERR>(numeric_msg_code));
  else
    out += get_rpl(base, cmd, static_cast<RPL_MSG>(numeric_msg_code));
  out += "\r\n";
  cmd.client->add_client_out(out);
  base.set_pollevent(cmd.client->get_client_fd(), POLLOUT);
}

/**
 * @brief function to check if the client is already registered by 
 * server-access-password, nickname and user
 * it sends an error message to client in case if not
 * 451 ERR_NOTREGISTERED
 *
 * @return 1 in case of registered, 0 if not
 */
bool IrcCommands::client_register_check(Server& base, Client& to_check) {
  (void)base;
  unsigned char reg_status = to_check.get_register_status();
  unsigned char registered = PASS | NICK | USER;
  if (reg_status == registered)
    return (1);
  else
    return (0);
}
