#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <ctime>  // for time-related functions
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include "../Client/Client.hpp"

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
 * (1) a function that returns channel_info
 */

class Channel {

 private:
  std::string _name;
  std::string _creation_time;
  std::string _topic;
  std::string _topic_time;
  std::string _topic_who;
  std::string _key;
  size_t _user_limit;
  /**
   * @brief Map of channel members to their operator status.
   * The key is a pointer to a Client participating in the channel.
   * The bool value indicates whether the client is a channel operator:
   */
  std::map<Client*, bool> _members;
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
  void new_member(Client* _new, bool is_oper);
  void new_invited(Client* _new);
  void remove_from_members(Client* removal);
  void remove_from_invited(Client* removal);
  bool update_chanops_stat(std::string nick, bool status);

  // Getters
  std::string get_name();
  std::string get_creation_time();
  std::string get_topic();
  std::string get_topic_time();
  std::string get_topic_who();
  std::string get_key();
  size_t get_user_limit();
  int get_modes();
  std::string get_modes_string();
  size_t get_members_size();
  size_t get_operators_size();
  std::vector<std::string> get_members_nicks();
  std::string get_nicks_string();
  std::map<Client*, bool>& get_members();
  std::list<Client*>& get_invited();

  // Setters
  void adjust_modes(int mode, bool status);
  void set_topic(std::string topic, std::string nick);
  void set_key(std::string key);
  void set_user_limit(size_t limit);

  // Helpers
  void print_channel_info();
};

#endif
