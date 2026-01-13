#ifndef IRCCOMMANDS_HPP
#define IRCCOMMANDS_HPP

#include <map>
#include <string>
#include "../Client/Client.hpp"
#include "../includes/types.hpp"

class Server;   //forward declaration
class Channel;  //forward declaration

class IrcCommands {
 private:
  typedef int (IrcCommands::*function)(Server& base, const struct cmd_obj& cmd);

  std::map<std::string, function> _irc_commands;

  // Commands
  int pass(Server& base, const struct cmd_obj& cmd);
  int pong(Server& base, const struct cmd_obj& cmd);
  int nick(Server& base, const struct cmd_obj& cmd);
  int cap(Server& base, const struct cmd_obj& cmd);
  int user(Server& base, const struct cmd_obj& cmd);
  int join(Server& base, const struct cmd_obj& cmd);
  int part(Server& base, const struct cmd_obj& cmd);
  int privmsg(Server& base, const struct cmd_obj& cmd);
  int kill(Server& base, const struct cmd_obj& cmd);
  int mode(Server& base, const struct cmd_obj& cmd);
  int invite(Server& base, const struct cmd_obj& cmd);
  int kick(Server& base, const struct cmd_obj& cmd);
  int oper(Server& base, const struct cmd_obj& cmd);
  int topic(Server& base, const struct cmd_obj& cmd);

  // helper functions
  bool client_register_check(Server& base, Client& to_check);
  std::string get_error(Server& base, const cmd_obj& cmd, enum PARSE_ERR err,
                        Channel* chan);
  std::string get_rpl(Server& base, const cmd_obj& cmd, enum RPL_MSG rpl,
                      Channel* chan);
  int send_privmsg(Server& base, Client& sender, Client& receiver,
                   const std::string& msg, const std::string& channel);
  int new_channel(Server& base, const struct cmd_obj& cmd,
                  const std::string& chan_name);
  int join_existing_channel(Server& base, const struct cmd_obj& cmd,
                            Channel* chan, const std::string& cmd_key);
  int join_0(Server& base, const struct cmd_obj& cmd);
  int update_modes(Server& base, const struct cmd_obj& cmd, Channel* chan);
  void send_mode_message(Server& base, const struct cmd_obj& cmd,
                         Client* target, Channel* chan,
                         const std::string& update);
  void send_join_message(Server& base, const struct cmd_obj& cmd,
                         Client* target, Channel* chan);
  void send_part_message(Server& base, const struct cmd_obj& cmd,
                         Client* target, Channel* chan);
  void send_invite_message(Server& base, const struct cmd_obj& cmd,
                           Client* target, Channel* chan);
  void send_kick_message(Server& base, const struct cmd_obj& cmd,
                         Client* target, Channel* chan);

 public:
  IrcCommands();
  IrcCommands(const IrcCommands& other);
  IrcCommands operator=(const IrcCommands& other);
  ~IrcCommands();
  int exec_command(Server& base, struct cmd_obj& cmd);
  void send_message(Server& base, const cmd_obj& cmd, int numeric_msg_code,
                    Client* target, Channel* chan);
  int get_all_recipients(std::list<Client*>& all_rec, Server& base,
                         Client* sender);
};

#endif  //IRCCOMMANDS_HPP
