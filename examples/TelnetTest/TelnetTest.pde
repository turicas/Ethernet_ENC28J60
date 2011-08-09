#include <Server.h>
#include <Ethernet.h>
#include <Client.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 1, 123 };

Server server(80);

#ifdef ETHERSHIELD_DEBUG
#include <inttypes.h>
uint8_t *debugCodes;

void printSocketDebug() {
  debugCodes = Ethernet.returnDebug();
  if (debugCodes[0] != 255) {
    Serial.println("DEBUG:");
    for (int i = 0; debugCodes[i] != 255; i++) {
      Serial.print("  ");
      Serial.println(debug2str(debugCodes[i]));
    }
    Ethernet.clearDebug();
  }
}
#endif

void setup() {
  Serial.begin(9600);
  Serial.println("*** Init");
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop()
{
  Client client = server.available();

#ifdef ETHERSHIELD_DEBUG
  printSocketDebug();
#endif

  if (client) {
#ifdef ETHERSHIELD_DEBUG
  Serial.println("*** New client!");
#endif

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.println(c);
        client.write(c);
      }
    }
#ifdef ETHERSHIELD_DEBUG
  Serial.println("*** Client disconnected.");
#endif

  }
}

