#include "socket.h"
#include "ip_arp_udp_tcp.h"

uint8_t myMacAddress[6], myIpAddress[4], myGatewayIpAddress[4],
        mySubnetAddress[4];

typedef struct socketData {
    uint8_t protocol;
    uint16_t sourcePort;
    uint8_t flag;
    uint8_t state;
} SocketData;
SocketData _SOCKETS[MAX_SOCK_NUM];

uint8_t socket(SOCKET s, uint8_t protocol, uint16_t sourcePort, uint8_t flag) {
    _SOCKETS[s].protocol = protocol;
    _SOCKETS[s].sourcePort = sourcePort;
    _SOCKETS[s].flag = flag;
    _SOCKETS[s].state = SOCK_CLOSED; //TODO: really need this?
}

uint8_t listen(SOCKET s) {
}

uint8_t connect(SOCKET s, uint8_t *address, uint16_t port) {
    //send ACK etc. and wait until SOCK_ESTABLISHED
}

uint16_t send(SOCKET s, const uint8_t *buffer, uint16_t length) {
}

uint16_t recv(SOCKET s, const uint8_t *buffer, uint16_t length) {
    //if there are bytes to read for socket s, write it on buffer

    //probably do not call the function that does verifications
}

uint8_t disconnect(SOCKET s) {
    //do not call the function that does verifications
}

uint8_t close(SOCKET s) {
    //do not call the function that does verifications
}

uint8_t getSn_SR(SOCKET s) {
    //get socket status
    //TODO: change on standard Ethernet library to do not use this

    //should call the function that does all the verifications on packets comming
    return _SOCKETS[s].state;
}

uint16_t getSn_RX_RSR(SOCKET s) {
    //return the size of the receive buffer for that socket
    //TODO: change on standard Ethernet library to do not use this

    //should call the function that does all the verifications on packets comming
}

/* *** Functions below are OK *** */

void iinchip_init() {
    //TODO: change on standard Ethernet library to do not use this
    
    //do nothing
}

void sysinit(uint8_t txSize, uint8_t rxSize) {
    //TODO: change on standard Ethernet library to do not use this
    int i;
    for (i = 0; i < MAX_SOCK_NUM; i++) {
        _SOCKETS[i].state = SOCK_CLOSED;
    }
}

void setSHAR(uint8_t *macAddress) {
    //TODO: change on standard Ethernet library to do not use this
    int i;
    for (i = 0; i < 6; i++) {
        myMacAddress[i] = macAddress[i];
    }
}

void setSIPR(uint8_t *ipAddress) {
    //TODO: change on standard Ethernet library to do not use this
    int i;
    for (i = 0; i < 4; i++) {
        myIpAddress[i] = ipAddress[i];
    }
}

void setGAR(uint8_t *gatewayIpAddress) {
    //TODO: change on standard Ethernet library to do not use this
    int i;
    for (i = 0; i < 4; i++) {
        myGatewayIpAddress[i] = gatewayIpAddress[i];
    }
}

void setSUBR(uint8_t *subnetAddress) {
    //TODO: change on standard Ethernet library to do not use this
    int i;
    for (i = 0; i < 4; i++) {
        mySubnetAddress[i] = subnetAddress[i];
    }

    init_ip_arp_udp_tcp(myMacAddress, myIpAddress, 99);
}
