// all constants for configuration could go here
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#define MAX_QUEUED 10

#define PORT_MIN 1024
#define PORT_MAX 49151
#define MAX_CMD_BYTES 512
#define MAX_TAG_BYTES 4096

#define RES_CAP_LS ":server CAP * LS :sasl\r\n"
#define RES_CAP_SASL ":server CAP * ACK :sasl\r\n"
#define RES_CAP_UNKNOWN ":server CAP UNKNOWN PARAMS\r\n"
#endif  // CONSTANTS_HPP
