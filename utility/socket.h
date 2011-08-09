#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <inttypes.h>

//#define ETHERSHIELD_DEBUG

#define SOCK_CLOSED         0x00
#define SOCK_INIT           0x13
#define SOCK_LISTEN         0x14
#define SOCK_ESTABLISHED    0x17
#define SOCK_CLOSE_WAIT     0x1C
#define SOCK_FIN_WAIT       0x18

#define Sn_MR_TCP           0x01
//TODO: change to IP_PROTO_TCP

#define MAX_SOCK_NUM        4
//TODO: it should be at socket.h in the standard Ethernet library, not in types.h

typedef uint8_t SOCKET;

#ifdef ETHERSHIELD_DEBUG
#define DEBUG_RECEIVED_ARP_REPLY                0
#define DEBUG_ANSWERING_RECEIVED_ARP_REQUEST    1
#define DEBUG_IGNORING_PACKET_NOT_FOR_ME        2
#define DEBUG_REPLYING_ECHO_REQUEST             3
#define DEBUG_IGNORED_PACKET                    4
#define DEBUG_RECEIVED_TCP_SYNACK_SENDING_ACK   5
#define DEBUG_RECEIVED_TCP_SYN_SENDING_SYNACK   6
#define DEBUG_RECEIVED_ACKFIN_CLOSING_SOCKET    7
#define DEBUG_RECEIVED_ACK_PACKET_HAVE_NO_DATA  8
#define DEBUG_DONT_KNOW_WHAT_TO_DO              9
#define DEBUG_SENT_ARP_REQUEST                  10
#define DEBUG_MAC_RECEIVED_SENDING_TCP_SYN      11
#define DEBUG_TCP_SYN_SENT                      12
#define DEBUG_INIT                              13
char *debugCode2String(uint8_t debugCode);
uint8_t *socketDebug();
void socketClearDebug();
#endif

uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag);

uint8_t listen(SOCKET s);

uint8_t connect(SOCKET s, uint8_t *address, uint16_t port);

uint16_t send(SOCKET s, const uint8_t *buffer, uint16_t length);

uint16_t recv(SOCKET s, uint8_t *buffer, uint16_t length);

uint8_t disconnect(SOCKET s);

uint8_t close(SOCKET s);

uint8_t getSn_SR(SOCKET s); //get socket status
//TODO: change on standard Ethernet library to do not use this

uint16_t getSn_RX_RSR(SOCKET s); //return the size of the receive buffer for that socket
//TODO: change on standard Ethernet library to do not use this

void iinchip_init();
//TODO: change on standard Ethernet library to do not use this

void sysinit(uint8_t txSize, uint8_t rxSize);
//TODO: change on standard Ethernet library to do not use this

void setSHAR(uint8_t *macAddress);
//TODO: change on standard Ethernet library to do not use this

void setSIPR(uint8_t *ipAddress);
//TODO: change on standard Ethernet library to do not use this

void setGAR(uint8_t *gatewayAddress);
//TODO: change on standard Ethernet library to do not use this

void setSUBR(uint8_t *subnetAddress);
//TODO: change on standard Ethernet library to do not use this

#endif
