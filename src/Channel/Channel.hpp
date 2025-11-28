#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <iostream>
# include <list>
# include "../Client/Client.hpp"

// Server will have a list of Channel objects

// ∗ KICK - Eject a client from the channel
// ∗ INVITE - Invite a client to a channel
// ∗ TOPIC - Change or view the channel topic
// ∗ MODE - Change the channel’s mode:
// · i: Set/remove Invite-only channel
// · t: Set/remove the restrictions of the TOPIC command to channel
// · k: Set/remove the channel key (password)
// · o: Give/take channel operator privilege
// · l: Set/remove the user limit to channel
// a single integer to check the modes by bitwise operations maybe?

class   Channel {

    private:
        std::string         _name;
        std::string         _topic;
        std::string         _key;
        size_t              _user_limit;
        std::list<Client*>  _members;
        std::list<Client*>  _operators;
        std::list<Client*>  _invited;

        // booleans for Channel MODES:
        bool                _limit_mode;     // +l mode: if channel has a limit
        bool                _key_mode;       // +k mode: if channel has a password
        bool                _invite_mode;   //  +i mode: only operators can add new members
        bool                _topic_mode;    //  +t mode: only o

    public:
        // Channel();
        Channel(std::string name);
        Channel(const Channel& other);
        Channel& operator=(const Channel& other);
        ~Channel();

        // Member management methods
        void        new_member(Client* _new);
        void        new_operator(Client* _new);
        void        new_invited(Client* _new);

        // getters
        std::string get_name();
        std::string get_topic();
        std::string get_key();
        bool        get_key_mode();
        bool        get_invite_mode();
        bool        get_topic_mode();
        bool        get_limit_mode();
        int         get_user_limit();
        size_t      get_members_size();
        void         get_members_fds();
        int         get_operators_fds();
        int         get_invited_fds();

        // setters
        // void        set_name();
        void        set_topic(std::string topic);
        // void        set_limit_mode(size_t limit);
        void        set_key(std::string key);
        void        set_invite_mode();
        void        set_user_limit(size_t limit);

        void        print_channel_info();

};
#endif