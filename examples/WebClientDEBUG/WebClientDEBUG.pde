#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 1, 177 };
byte server[] = { 192, 168, 1, 3 };

Client client(server, 80);
#ifdef ETHERSHIELD_DEBUG
char dbg[70];
#endif

void setup() {
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  
  delay(1000);
  Serial.println("connecting...");
  int isConnected = client.connect();
#ifdef ETHERSHIELD_DEBUG
  sprintf(dbg, "%s", Ethernet.returnDebug());
  if (dbg[0] != 0) {
    Serial.print("DEBUG: ");
    Serial.println(dbg);
    Ethernet.clearDebug();
  }
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
