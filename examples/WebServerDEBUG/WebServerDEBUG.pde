#include <Ethernet.h>

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
  Ethernet.begin(mac, ip);
  server.begin();

#ifdef ETHERSHIELD_DEBUG
  Serial.begin(9600);
#endif
}

void loop() {
  Client client = server.available();

#ifdef ETHERSHIELD_DEBUG
  printSocketDebug();
#endif

  if (client) {

#ifdef ETHERSHIELD_DEBUG
    Serial.println("New client!");
#endif

    // an http request ends with a blank line
    boolean current_line_is_blank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

#ifdef ETHERSHIELD_DEBUG
        Serial.print(c);
#endif

        // if we've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so we can send a reply
        if (c == '\n' && current_line_is_blank) {
          // send a standard http response header

#ifdef ETHERSHIELD_DEBUG
          Serial.println("Received headers!");
#endif

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html><meta http-equiv=\"refresh\" content=\"3\"><body>");
          
          // output the value of each analog input pin
          for (int i = 0; i < 6; i++) {
            client.print("A");
            client.print(i);
            client.print(" = ");
            client.print(analogRead(i));
            client.print("<br>");
          }
          client.print("\nmillis() = <b>");
          client.print(millis());
          client.print("</b></html></body>");
          break;
        }
        if (c == '\n') {
          // we're starting a new line
          current_line_is_blank = true;
        } else if (c != '\r') {
          // we've gotten a character on the current line
          current_line_is_blank = false;
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
