#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 1, 123 };

Server server(80);

void setup() {
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop() {
  Client client = server.available();
  if (client) {
    boolean current_line_is_blank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && current_line_is_blank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.print("Arduino Web Server");
          break;
        }
        if (c == '\n') {
          current_line_is_blank = true;
        } else if (c != '\r') {
          current_line_is_blank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
}
