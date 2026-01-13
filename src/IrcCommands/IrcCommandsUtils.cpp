#include <iomanip>  // For std::setw and std::setfill
#include <iostream>
#include <map>
#include <sstream>
#include "../Channel/Channel.hpp"
#include "../Client/Client.hpp"
#include "../Server/Server.hpp"
#include "../includes/CONSTANTS.hpp"
#include "../includes/types.hpp"
#include "IrcCommands.hpp"

/**
 * @brief function to return the reply message of replymessagecode (rpl)
 *
 */
std::string IrcCommands::get_rpl(Server& base, const cmd_obj& cmd,
                                 enum RPL_MSG rpl, Channel* chan) {
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
      if (chan)
        return (chan->get_name() + " Modes: [" + chan->get_modes_string() +
                "]");
      else
        return (": Error on channel configs");
    case RPL_CREATIONTIME:
      if (chan)
        return (chan->get_name() + " is created on " +
                chan->get_creation_time());
      else
        return (": Error on channel configs");
    case RPL_NOTOPIC:
      if (chan)
        return (chan->get_name() + " :No topic is set");
      else
        return (": Error on channel configs");
    case RPL_TOPIC:
      if (chan)
        return (chan->get_name() + " topic: " + chan->get_topic());
      else
        return (": Error on channel configs");
    case RPL_TOPICWHOTIME:
      if (chan)
        return (chan->get_name() + " topic is set by " + chan->get_topic_who() +
                " on " + chan->get_topic_time());
      else
        return (": Error on channel configs");
    case RPL_INVITING:
      return ("invites " + cmd.parameters[0] + " to " + cmd.parameters[1]);
    case RPL_NAMREPLY:
      if (chan)
        return (chan->get_name() + " " + chan->get_nicks_string());
      else
        return (": Error on channel configs");
    case RPL_ENDOFNAMES:
      if (chan)
        return (chan->get_name() + " :End of /NAMES list");
      else
        return (": Error on channel configs");
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
 * (2) maybe remove paramter base if not required
 */
std::string IrcCommands::get_error(Server& base, const cmd_obj& cmd,
                                   enum PARSE_ERR err, Channel* chan) {
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
      return (cmd.parameters[0] + " :No such channel");
    case ERR_CANNOTSENDTOCHAN:
      return (cmd.parameters[0] + " :No such channel");
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
      if (chan)
        return ("<nick> " + chan->get_name() + " :They aren't on that channel");
      else
        return (": Error on channel configs");
    case ERR_NOTONCHANNEL:
      if (chan)
        return (chan->get_name() + " :You're not on that channel");
      else
        return (" :You're not on that channel");
    case ERR_USERONCHANNEL:
      if (chan)
        return ("<nick> " + chan->get_name() + " :is already on channel");
      else
        return (": Error on channel configs");
    case ERR_NOTREGISTERED:
      return (" :You have not registered");
    case ERR_NEEDMOREPARAMS:
      return (cmd.command + " :Not enough parameters");
    case ERR_ALREADYREGISTERED:
      return (" :You may not reregister");
    case ERR_PASSWDMISMATCH:
      return (" :Password incorrect");
    case ERR_CHANNELISFULL:
      if (chan)
        return (chan->get_name() + " :Cannot join channel (+l)");
      else
        return (" :Cannot join channel (+l)");
    case ERR_INVITEONLYCHAN:
      if (chan)
        return (chan->get_name() + " :Cannot join channel (+i)");
      else
        return (" :Cannot join channel (+i)");
    case ERR_BADCHANNELKEY:
      if (chan)
        return (chan->get_name() + " :Cannot join channel (+k)");
      else
        return (" :Cannot join channel (+k)");
    case ERR_NOPRIVILEGES:
      return (" :Permission Denied- You're not an IRC operator");
    case ERR_CHANOPRIVSNEEDED:
      if (chan)
        return (chan->get_name() + " :You're not channel operator");
      else
        return (" :You're not channel operator");
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
                               int numeric_msg_code, Client* target,
                               Channel* chan) {
  bool error = (numeric_msg_code >= 400) ? true : false;
  std::string out;
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(3) << numeric_msg_code;

  out += ":";
  out += base._server_name + " ";
  out += ss.str() + " ";
  if (!cmd.client->get_nick().empty())
    out += cmd.client->get_nick() + " ";
  else
    out += "* ";
  if (error)
    out += get_error(base, cmd, static_cast<PARSE_ERR>(numeric_msg_code), chan);
  else
    out += get_rpl(base, cmd, static_cast<RPL_MSG>(numeric_msg_code), chan);
  out += "\r\n";
  target->add_client_out(out);
  base.set_pollevent(target->get_client_fd(), POLLOUT);
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

/**
 * @brief function to get all membres the sender is connteced with via sharing the same
 * channel membership
 *
 * @rturn returns 0 in case there is no connected member, otherwise 1
 */
int IrcCommands::get_all_recipients(std::list<Client*>& all_rec, Server& base,
                                    Client* sender) {
  if (sender->get_channels().empty())
    return (0);
  std::map<std::string, bool> _channel_list = sender->get_channels();
  for (std::map<std::string, bool>::iterator it_chan_name =
           _channel_list.begin();
       it_chan_name != _channel_list.end(); it_chan_name++) {
    if (base.get_channel_list().empty())
      return (0);
    std::list<Channel>::iterator it_chan_all = base.get_channel_list().begin();
    for (; it_chan_all != base.get_channel_list().end(); it_chan_all++) {
      if (it_chan_name->first == it_chan_all->get_name()) {
        if (it_chan_all->get_members().empty())
          return (0);
        for (std::map<Client*, bool>::iterator it_chan_members =
                 it_chan_all->get_members().begin();
             it_chan_members != it_chan_all->get_members().end();
             it_chan_members++) {
          all_rec.push_back(it_chan_members->first);
        }
      }
    }
  }
  if (all_rec.empty())
    return (0);
  else
    return (1);
}

