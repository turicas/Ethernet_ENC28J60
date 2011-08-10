#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 1, 123 };

Server server(80);

void setup() {
#ifdef ETHERSHIELD_DEBUG
  Serial.begin(9600);
#endif

  Ethernet.begin(mac, ip);
  server.begin();
}

void loop() {
  Client client = server.available();

  if (client) {

#ifdef ETHERSHIELD_DEBUG
    Serial.println("New client!");
#endif

    // an http request ends with a blank line
    int current_line_is_blank = 0;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
        if (c == '\n' && current_line_is_blank) {
#ifdef ETHERSHIELD_DEBUG
          Serial.println("Received headers!");
#endif
          char response[30];
          int size;
          sprintf(response, "millis() = <b>%lu</b>", millis());
          for (size = 0; response[size] != '\0'; size++) {}

          client.println("HTTP/1.0 200 OK");
          client.println("Content-Type: text/html");
          client.print("Content-Length: ");
          client.println(size);
          client.println();
          
          client.print(response);
          break;
        }
        else if (c == '\n') {
          current_line_is_blank = 1;
        }
        else if (c != '\r') {
          current_line_is_blank = 0;
        }
      }
    }

#ifdef ETHERSHIELD_DEBUG
    Serial.println("Disconnected");
#endif

    // give the web browser time to receive the data
    delay(1);
    client.stop();
  }
}
