#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "../Client/Client.hpp"

/**
 * @brief
 * Server will have a list of Channel objects
 ∗ KICK - Eject a client from the channel
 ∗ INVITE - Invite a client to a channel
 ∗ TOPIC - Change or view the channel topic
 ∗ MODE - Change the channel’s mode:
 * i: Set/remove Invite-only channel
 * t: Set/remove the restrictions of the TOPIC command to channel
 * k: Set/remove the channel key (password)
 * o: Give/take channel operator privilege
 * l: Set/remove the user limit to channel
 *
 * TODO 
 * (1) a bitmask to check the modes 
 * (2) if channel ıs mutable we'll need a setter function
 */

class Channel {

 private:
  std::string _name;
  std::string _topic;
  std::string _key;
  size_t _user_limit;
  std::list<Client*> _members;
  std::list<Client*> _operators;
  std::list<Client*> _invited;

  // booleans for Channel MODES:
  bool _limit_mode;
  bool _key_mode;
  bool _invite_mode;
  bool _topic_mode;

 public:
  Channel(std::string name);
  Channel(const Channel& other);
  Channel& operator=(const Channel& other);
  ~Channel();

  //overload for find-functionality
  bool operator==(const std::string& other) const;

  // Member management
  void new_member(Client* _new);
  void new_operator(Client* _new);
  void new_invited(Client* _new);

  // Getters
  std::string get_name();
  std::string get_topic();
  std::string get_key();
  bool get_key_mode();
  bool get_invite_mode();
  bool get_topic_mode();
  bool get_limit_mode();
  int get_user_limit();
  size_t get_members_size();
  void get_members_fds();
  std::vector<std::string> get_members_nicks();
  std::list<Client*> get_members();
  int get_operators_fds();
  int get_invited_fds();

  // Setters
  void set_topic(std::string topic);
  void set_limit_mode(size_t limit);
  void set_key(std::string key);
  void set_invite_mode();
  void set_user_limit(size_t limit);

  void print_channel_info();
};
#endif
