#include <Ethernet.h>
#define LED 8

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 111, 2 };
char link[30], LEDStatus[4];

Server server(80);

void setup() {
  pinMode(LED, OUTPUT);
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop() {
  Client client = server.available();
  if (client) {
    char request[10];
    int i = 0;
    boolean current_line_is_blank = true;
    request[9] = '\0';
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (i < 9) {
          request[i] = c;
          i++;
        }
        if (c == '\n' && current_line_is_blank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          
          client.println("<html><head><title>Arduino etherShield</title></head><body>");
          client.println("<h1>Arduino etherShield Example</h1>");
          client.println("<table>");
          client.print("<tr><td>Analog inputs:</td><td><b>");
          for (int x = 0; x < 6; x++) {
            client.print(x);
            client.print(" = ");
            client.print(5000 * (analogRead(x) / 1024.0));
            client.print(" mV<br>");
          }
          client.println("</b></td></tr><tr><td>uptime:</td> <td><b>");
          client.print(millis() / 1000.0);
          client.println(" seconds</b></td></tr>");
          if (strncmp("GET /off", request, 8) == 0) {
            digitalWrite(LED, LOW);
            sprintf(LEDStatus, "OFF");
            sprintf(link, "<a href=\"/on\">Turn on</a>");
          }
          else {
            sprintf(LEDStatus, "ON");
            digitalWrite(LED, HIGH);
            sprintf(link, "<a href=\"/off\">Turn off</a>");
          }
          client.print("<tr><td>LED status:</td> <td><b>");
          client.print(LEDStatus);
          client.println("</b></td></tr><tr><td colspan=\"2\" align=\"center\">");
          client.println(link);
          client.println("</td></tr></table></body></html>");
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
