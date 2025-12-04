#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <ctime>  // for time-related functions
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "../Client/Client.hpp"

#define MODE_INVITE (1 << 0)
#define MODE_KEY (1 << 1)
#define MODE_LIMIT (1 << 2)
#define MODE_TOPIC (1 << 3)

/**
 * @brief
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
 * TODO:
 * (1) a bitmask to check the modes 
 * (2) if channel is mutable we'll need a setter function
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
  int _modes;

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
  int get_user_limit();
  size_t get_members_size();
  size_t get_operators_size();
  std::vector<std::string> get_members_nicks();
  std::list<Client*> get_members();

  // Setters
  void set_modes(std::string flag);
  void set_topic(std::string topic);
  void set_key(std::string key);
  void set_user_limit(size_t limit);

  void print_channel_info();
};
#endif
