#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 1, 123 };
byte server[] = { 192, 168, 1, 1 };

Client client(server, 80);

void setup() {
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  
  delay(1000);
  Serial.println("connecting...");
  int isConnected = client.connect();
  
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
