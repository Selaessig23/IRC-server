#include "Channel.hpp"
#include <map>
#include <list>
#include <string>
#include <iostream>
#include <vector>
#include "../debug.hpp"

// Channel::Channel() : _name("default") {}
Channel::Channel(std::string name)  
    :   _name(name),
        _topic("Default"),
        _limit_mode(false),
        _key_mode(false),
        _invite_mode(false),
        _topic_mode(false) {
    DEBUG_PRINT("Channel is created"); 
}

Channel::Channel(const Channel& other) 
    :   _name(other._name),
        _topic(other._topic),
        _limit_mode(other._limit_mode),
        _key_mode(other._key_mode),
        _invite_mode(other._invite_mode),
        _topic_mode(other._topic_mode) {}

Channel& Channel::operator=(const Channel& other) {
    Channel temp(other);
    std::swap(*this, temp);
    return (*this);
}

Channel::~Channel() {
    DEBUG_PRINT("Channel is destructed"); 
}

// Member management methods
void    Channel::new_member(Client* _new) {
    _members.push_back(_new);
    DEBUG_PRINT("New member is added to the channel");
}

void    Channel::new_operator(Client* _new) {
    _operators.push_back(_new);
    DEBUG_PRINT("New operator is assigned for the channel");
}

void    Channel::new_invited(Client* _new) {
    _invited.push_back(_new);
    DEBUG_PRINT("The user is invited to the channel");
}

// SETTERS
void    Channel::set_topic(std::string topic) {
    _topic = topic;    
}

void    Channel::set_user_limit(size_t limit) {
    _user_limit = limit;
    _limit_mode = true;
}

void    Channel::set_invite_mode() {
    if (_invite_mode)
        _invite_mode = false;
    else
        _invite_mode = true;
}

void    Channel::set_key(std::string key) {
    _key = key;
    _key_mode = true;
}

// GETTERS
std::string Channel::get_name() {
    return(_name);
}

std::string Channel::get_topic() {
    return(_topic);
}

std::string Channel::get_key() {
    return(_key);
}

bool    Channel::get_key_mode() {
    return (_key_mode);
}

bool    Channel::get_invite_mode() {
    return (_invite_mode);
}

bool    Channel::get_topic_mode() {
    return (_topic_mode);
}

bool    Channel::get_limit_mode() {
    return (_limit_mode);
}

int     Channel::get_user_limit() {
    return (_user_limit);
}

// int main (void) {
//     std::string key;
//     std::string val;
//     std::string name;
//     std::vector<Channel>  my_channels;
//     while (1) {
//         if (std::cin.eof()) {
//             break;
//         }
//         std::cin >> name;
//         std::cout << "Channel_name: " << name << std::endl;
//         std::cin >> key;
//         std::cout << "Key: " << key << std::endl;
//         std::cin >> val;
//         std::cout << "Val: " << val << std::endl;
//         Channel channel(name);
//         my_channels.push_back(channel);
//         // std::cin >> buf;
//     }
// }