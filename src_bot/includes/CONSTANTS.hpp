// all constants for configuration could go here
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#define MAX_QUEUED 10

#define PORT_MIN 1024
#define PORT_MAX 49151
#define MAX_CMD_BYTES 512
#define MAX_TAG_BYTES 4096

/**
 * used for authentication process
 * all need to be set to connect to server
 */
#define PASS 0x001 // password was set
#define NICK 0x002 // nickname was set
#define USER 0x004 // users data was set

/**
 * used for channel subscription process
 */
#define INVITED 0x001
#define APPLIED 0x002
#define ACCEPTED 0x004

#endif  // CONSTANTS_HPP
