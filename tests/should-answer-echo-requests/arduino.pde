#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 1, 123 };

Server server(80);

void setup() {
    Ethernet.begin(mac, ip);
    server.begin();
}

void loop() {
    server.available();
}
