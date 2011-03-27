#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 111, 2 };
byte server[] = { 192, 168, 111, 1 };

Client client(server, 80);

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
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  
  delay(1000);
  Serial.println("connecting...");
  int isConnected = client.connect();

#ifdef ETHERSHIELD_DEBUG
  printSocketDebug();
  Serial.println(client.debug());
#endif
  
  if (isConnected) {
    Serial.println("connected");
    client.println("GET /search?q=arduino HTTP/1.0");
    client.println();
  }
  else {
    Serial.println("connection failed");
  }
}

void loop() {
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
}
