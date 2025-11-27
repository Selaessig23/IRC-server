// all constants for configuration could go here
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#define MAX_QUEUED 10

#define PORT_MIN 1024
#define PORT_MAX 49151
#define MAX_CMD_BYTES 512
#define MAX_TAG_BYTES 4096

#define RES_CAP_LS "CAP * LS :sasl"
#define RES_CAP_SASL "CAP * ACK :sasl"
#define RES_CAP_UNKNOWN "CAP UNKNOWN PARAMS"
#define RES_AUTH "AUTHENTICATE PLAIN"
#endif  // CONSTANTS_HPP
