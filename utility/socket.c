#include "socket.h"
#include "net.h"
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#define BUFFER_SIZE         550
#define MAX_ITERATIONS      1000

#define NO_STATE            0
#define GOT_MAC             1
#define ARP_REQUEST_SENT    2
#define TCP_SYN_SENT        3

uint8_t myMacAddress[6], myIpAddress[4], myGatewayIpAddress[4],
        mySubnetAddress[4];
static uint8_t buffer[BUFFER_SIZE + 1];
uint16_t packetLength, sendPacketLength = 0;
#ifdef ETHERSHIELD_DEBUG
#define ETHERSHIELD_DEBUG_SIZE 70
char SOCKET_DEBUG[ETHERSHIELD_DEBUG_SIZE];

char *socketDebug() {
    return SOCKET_DEBUG;
}
void socketClearDebug() {
    int i;
    for (i = 0; i < ETHERSHIELD_DEBUG_SIZE; i++) {
        SOCKET_DEBUG[i] = 0;
    }
}
#endif

typedef struct socketData {
    uint8_t protocol;
    uint16_t sourcePort;
    uint8_t flag;
    uint8_t state;
    uint8_t *buffer;
    uint16_t bytesToRead;
    uint16_t firstByte;
    uint8_t clientState;
    uint8_t destinationMac[6];
} SocketData;
SocketData _SOCKETS[MAX_SOCK_NUM];

#ifdef ETHERSHIELD_DEBUG
void turnLEDsOn() {
    enc28j60PhyWrite(PHLCON, 0x880); //turn on
}

void turnLEDsOff() {
    enc28j60PhyWrite(PHLCON, 0x990); //turn off
}
#endif

uint8_t socket(SOCKET s, uint8_t protocol, uint16_t sourcePort, uint8_t flag) {
    _SOCKETS[s].protocol = protocol;
    _SOCKETS[s].sourcePort = sourcePort;
    _SOCKETS[s].flag = flag;
    _SOCKETS[s].state = SOCK_INIT;
    _SOCKETS[s].bytesToRead = 0;
    _SOCKETS[s].firstByte = 0;
    _SOCKETS[s].clientState = NO_STATE;
}

void flushSockets() {
    uint16_t data;

    packetLength = enc28j60PacketReceive(BUFFER_SIZE, buffer);
    if (!packetLength) {
        //DEBUG: no data available for reading!
        return;
    }
    else if (eth_type_is_arp_and_my_ip(buffer, packetLength)) {
        if (arp_packet_is_myreply_arp(buffer)) {
            uint8_t i;
#ifdef ETHERSHIELD_DEBUG
            sprintf(SOCKET_DEBUG, "Received ARP reply!");
#endif
            for (i = 0; i < 6; i++) {
                _SOCKETS[0].destinationMac[i] = buffer[ETH_SRC_MAC + i];
            }
            _SOCKETS[0].clientState = GOT_MAC;
            //TODO: write code to get socket id instead of 0
        }
        else {
#ifdef ETHERSHIELD_DEBUG
            sprintf(SOCKET_DEBUG, "Received ARP request. Answering.");
#endif
            make_arp_answer_from_request(buffer);
        }
    }
    else if (!eth_type_is_ip_and_my_ip(buffer, packetLength)) {
#ifdef ETHERSHIELD_DEBUG
        sprintf(SOCKET_DEBUG, "This packet is not for me! Ignoring...");
#endif
        return;
    }
    else if (buffer[IP_PROTO_P] == IP_PROTO_ICMP_V &&
             buffer[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V) {
#ifdef ETHERSHIELD_DEBUG
            sprintf(SOCKET_DEBUG, "ECHO REQUEST from %d.%d.%d.%d. Sending reply.", buffer[IP_SRC_IP_P], buffer[IP_SRC_IP_P + 1], buffer[IP_SRC_IP_P + 2], buffer[IP_SRC_IP_P + 3]);
#endif
        make_echo_reply_from_request(buffer, packetLength);
    }
    else if (buffer[IP_PROTO_P] == IP_PROTO_TCP_V) {
        //DEBUG: it's TCP and for me! Do I want it?
        uint16_t destinationPort = (buffer[TCP_DST_PORT_H_P] << 8) | buffer[TCP_DST_PORT_L_P];
        uint8_t i, socketSelected = 255;
        for (i = 0; i < MAX_SOCK_NUM; i++) {
            if (_SOCKETS[i].sourcePort == destinationPort &&
                _SOCKETS[i].state == SOCK_LISTEN) {
                socketSelected = i;
                break;
            }
        }
        if (socketSelected == 255) {
#ifdef ETHERSHIELD_DEBUG
            sprintf(SOCKET_DEBUG, "Packet from: %d.%d.%d.%d:%d ignored.",
                    buffer[IP_SRC_IP_P], buffer[IP_SRC_IP_P + 1],
                    buffer[IP_SRC_IP_P + 2], buffer[IP_SRC_IP_P + 3],
                    destinationPort);
#endif
            //return; //TODO: return!
            socketSelected = 0; //TODO: remove this hack
        }
        //TODO: change next 'if' to 'else if'
        //DEBUG: ok, the TCP packet is for me and I want it.
        if (buffer[TCP_FLAGS_P] == (TCP_FLAG_SYN_V | TCP_FLAG_ACK_V)) {
#ifdef ETHERSHIELD_DEBUG
            sprintf(SOCKET_DEBUG, "Received TCP SYN+ACK. Sending ACK.", socketSelected);
#endif
            //TODO: verify if I'm waiting for this SYN+ACK
            make_tcp_ack_from_any(buffer);
            _SOCKETS[0].clientState = SOCK_ESTABLISHED;
            //TODO: write code to get socket id instead of 0
        }
        else if (buffer[TCP_FLAGS_P] & TCP_FLAGS_SYN_V) {
#ifdef ETHERSHIELD_DEBUG
            sprintf(SOCKET_DEBUG, "Received TCP SYN. Sending SYN+ACK.", socketSelected);
#endif
            _SOCKETS[socketSelected].state = SOCK_ESTABLISHED;
            make_tcp_synack_from_syn(buffer);
        }
        else if (buffer[TCP_FLAGS_P] & TCP_FLAGS_ACK_V) {
            init_len_info(buffer);
            data = get_tcp_data_pointer();
            if (!data) {
                if (buffer[TCP_FLAGS_P] & TCP_FLAGS_FIN_V) {
#ifdef ETHERSHIELD_DEBUG
                    sprintf(SOCKET_DEBUG, "Received ACK+FIN. Closing socket.", socketSelected);
#endif
                    make_tcp_ack_from_any(buffer);
                    _SOCKETS[socketSelected].state = SOCK_CLOSED;
                }
                return;
            }
            else {
#ifdef ETHERSHIELD_DEBUG
            sprintf(SOCKET_DEBUG, "Received ACK. Packet have data.", socketSelected);
#endif
                int i, dataSize = packetLength - (&buffer[data] - buffer);
                _SOCKETS[socketSelected].state = SOCK_ESTABLISHED;
                _SOCKETS[socketSelected].buffer = malloc((BUFFER_SIZE) * sizeof(uint8_t)); //TODO: and about the TCP/IP/Ethernet overhead?
                for (i = 0; i < dataSize; i++) {
                    _SOCKETS[socketSelected].buffer[i] = buffer[data + i];
                }
                _SOCKETS[socketSelected].bytesToRead = i;
                //make_tcp_ack_from_any(buffer);
                return;
            }
        }
        else {
#ifdef ETHERSHIELD_DEBUG
            sprintf(SOCKET_DEBUG, "don't know what to do");
#endif
        }
    }
}

uint8_t listen(SOCKET s) {
    _SOCKETS[s].state = SOCK_LISTEN;
    return 1;
}

uint8_t connect(SOCKET s, uint8_t *destinationIp, uint16_t destinationPort) {
    uint16_t packetChecksum, i;
    char buffer[59];

    //TODO: create an ARP table?
    make_arp_request(buffer, destinationIp);
    _SOCKETS[s].state = ARP_REQUEST_SENT;

    for (i = 0; _SOCKETS[s].clientState != GOT_MAC && i < MAX_ITERATIONS; i++) {
        flushSockets(); //it'll fill destinationMac on socket struct
    }
    if (_SOCKETS[s].clientState != GOT_MAC) {
        return 0;
    }

    make_eth_ip_new(buffer, _SOCKETS[s].destinationMac);
    // total length field in the IP header must be set:
    // 20 bytes IP + 24 bytes (20tcp+4tcp options)
    buffer[IP_TOTLEN_H_P] = 0;
    buffer[IP_TOTLEN_L_P] = IP_HEADER_LEN + TCP_HEADER_LEN_PLAIN + 4;
    make_ip(buffer);
    buffer[TCP_FLAG_P] = TCP_FLAG_SYN_V;
    make_tcphead(buffer, 1, 1, 0);
    // calculate the checksum, len=8 (start from ip.src) +
    // TCP_HEADER_LEN_PLAIN + 4 (one option: mss)
    packetChecksum = checksum(&buffer[IP_SRC_P], TCP_HEADER_LEN_PLAIN + 12, 2);
    buffer[TCP_CHECKSUM_H_P] = packetChecksum >> 8;
    buffer[TCP_CHECKSUM_L_P] = packetChecksum & 0xff;
    // add 4 for option mss:
    enc28j60PacketSend(IP_HEADER_LEN + TCP_HEADER_LEN_PLAIN + 4 + ETH_HEADER_LEN,
                       buffer);
    _SOCKETS[s].clientState = TCP_SYN_SENT;

    for (i = 0; _SOCKETS[s].state != SOCK_ESTABLISHED && i < MAX_ITERATIONS; i++) {
        flushSockets();
    }
    return _SOCKETS[s].state == SOCK_ESTABLISHED;
    //TODO: Maybe use a default timeout to receive SYN+ACK
}

uint16_t send(SOCKET s, const uint8_t *bufferToSend, uint16_t length) {
    sendPacketLength = fill_tcp_data2(buffer, sendPacketLength, bufferToSend, length);
} //TODO: do it per socket

uint16_t recv(SOCKET s, uint8_t *buffer, uint16_t length) {
    int i, j;
    if (!_SOCKETS[s].bytesToRead) {
        return;
    }
    else if (length == 1) {
        buffer[0] = _SOCKETS[s].buffer[_SOCKETS[s].firstByte];
        _SOCKETS[s].firstByte++;
        if (_SOCKETS[s].firstByte == _SOCKETS[s].bytesToRead) {
            _SOCKETS[s].bytesToRead = 0;
            _SOCKETS[s].firstByte = 0;
            free(_SOCKETS[s].buffer);
        }
    }
    /*
    for (i = _SOCKETS[s].firstByte, j = 0; i < _SOCKETS[s].bytesToRead && i < length; i++, j++) {
        buffer[j] = _SOCKETS[s].buffer[i];
    }
    if (i + 1 == _SOCKETS[s].bytesToRead) {
        free(_SOCKETS[s].buffer);
        _SOCKETS[s].firstByte = 0;
        _SOCKETS[s].bytesToRead = 0;
    }
    else {
        _SOCKETS[s].bytesToRead -= i;
        _SOCKETS[s].firstByte = i;
    }
    */
}

uint8_t disconnect(SOCKET s) {
    if (sendPacketLength) {
        make_tcp_ack_from_any(buffer);
        make_tcp_ack_with_data(buffer, sendPacketLength);
        sendPacketLength = 0;
    }
    //TODO:send FYN packet
    //TODO:wait to receive ACK?
    _SOCKETS[s].state = SOCK_CLOSED; //TODO: remove this hack
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
        _SOCKETS[i].state = SOCK_CLOSED;
    }
#ifdef ETHERSHIELD_DEBUG
    sprintf(SOCKET_DEBUG, "init"); //TODO: remove
#endif
}

void setSHAR(uint8_t *macAddress) {
    //TODO: change on standard Ethernet library to do not use this
    uint8_t i;
    for (i = 0; i < 6; i++) {
        myMacAddress[i] = macAddress[i];
    }
    enc28j60Init(myMacAddress);
    enc28j60clkout(2);
    enc28j60PhyWrite(PHLCON, 0x476); //LEDA = link status, LEDB = RX/TX
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

    init_ip_arp_udp_tcp(myMacAddress, myIpAddress, 1); //TODO: change this port
}
