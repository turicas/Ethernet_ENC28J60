#include "socket.h"
#include "net.h"
#include "ip_arp_udp_tcp.h"
#define BUFFER_SIZE 500

uint8_t myMacAddress[6], myIpAddress[4], myGatewayIpAddress[4],
        mySubnetAddress[4];

typedef struct socketData {
    uint8_t protocol;
    uint16_t sourcePort;
    uint8_t flag;
    uint8_t state;
    int bytesToRead;
} SocketData;
SocketData _SOCKETS[MAX_SOCK_NUM];

uint8_t socket(SOCKET s, uint8_t protocol, uint16_t sourcePort, uint8_t flag) {
    _SOCKETS[s].protocol = protocol;
    _SOCKETS[s].sourcePort = sourcePort;
    _SOCKETS[s].flag = flag;
    _SOCKETS[s].state = SOCK_INIT;
    _SOCKETS[s].bytesToRead = 0;
}

void flushSockets() {
    uint16_t packetLen, data;
    uint8_t buffer[BUFFER_SIZE + 1];

    packetLen = enc28j60PacketReceive(BUFFER_SIZE, buffer);
    if (packetLen == 0) {
        //DEBUG: no data available for reading!
        return;
    }
    else if (eth_type_is_arp_and_my_ip(buffer, packetLen)) {
        //DEBUG: received ARP, answering...
        make_arp_answer_from_request(buffer);
    }
    else if (!eth_type_is_ip_and_my_ip(buffer, packetLen)) {
        //DEBUG: this packet is not for me! ignoring.
        return;
    }
    else if (buffer[IP_PROTO_P] == IP_PROTO_ICMP_V &&
             buffer[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V) {
        //DEBUG: received echo request, sending reply...
        make_echo_reply_from_request(buffer, packetLen);
    }
}

uint8_t listen(SOCKET s) {
    uint8_t port = _SOCKETS[s].sourcePort, i;
    for (i = 0; i < MAX_SOCK_NUM; i++) {
        if (i != s && _SOCKETS[i].sourcePort == port &&
            _SOCKETS[i].state != SOCK_INIT &&
            _SOCKETS[i].state != SOCK_CLOSED) { //TODO: add more states here
            return 0;
        }
    }
    _SOCKETS[s].state = SOCK_LISTEN;
    return 1;
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

    //send FYN packet
    //wait to receive ACK?
}

uint8_t close(SOCKET s) {
    //do not call the function that does verifications
    _SOCKETS[s].state = SOCK_CLOSED;
}

uint8_t getSn_SR(SOCKET s) {
    //get socket status
    //TODO: change on standard Ethernet library to do not use this

    flushSockets();
    return _SOCKETS[s].state;
}

uint16_t getSn_RX_RSR(SOCKET s) {
    //return the size of the receive buffer for that socket
    //TODO: change on standard Ethernet library to do not use this

    flushSockets();
    return _SOCKETS[s].bytesToRead;
}

void iinchip_init() {
    //TODO: change on standard Ethernet library to do not use this
    
    //do nothing
}

void sysinit(uint8_t txSize, uint8_t rxSize) {
    //TODO: change on standard Ethernet library to do not use this
    uint8_t i;
    for (i = 0; i < MAX_SOCK_NUM; i++) {
        _SOCKETS[i].state = SOCK_INIT;
    }
}

void setSHAR(uint8_t *macAddress) {
    //TODO: change on standard Ethernet library to do not use this
    uint8_t i;
    for (i = 0; i < 6; i++) {
        myMacAddress[i] = macAddress[i];
    }
}

void setSIPR(uint8_t *ipAddress) {
    //TODO: change on standard Ethernet library to do not use this
    uint8_t i;
    for (i = 0; i < 4; i++) {
        myIpAddress[i] = ipAddress[i];
    }
}

void setGAR(uint8_t *gatewayIpAddress) {
    //TODO: change on standard Ethernet library to do not use this
    uint8_t i;
    for (i = 0; i < 4; i++) {
        myGatewayIpAddress[i] = gatewayIpAddress[i];
    }
}

void setSUBR(uint8_t *subnetAddress) {
    //TODO: change on standard Ethernet library to do not use this
    uint8_t i;
    for (i = 0; i < 4; i++) {
        mySubnetAddress[i] = subnetAddress[i];
    }

    init_ip_arp_udp_tcp(myMacAddress, myIpAddress, 0); //TODO: change this port
}
