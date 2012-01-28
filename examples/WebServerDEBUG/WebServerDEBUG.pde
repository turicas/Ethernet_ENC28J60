#include <Ethernet.h>
#define SIZE 50

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 1, 123 };

Server server(80);

char response[SIZE];
int size = 0;

void setup() {
#ifdef ETHERSHIELD_DEBUG
	Serial.begin(9600);
#endif
	Ethernet.begin(mac, ip);
	server.begin();
	while (size < SIZE) {
		response[size] = 'A';
		size++;
	}
	response[size] = '\0';
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

				if (c == '\n' && current_line_is_blank) {
#ifdef ETHERSHIELD_DEBUG
					Serial.println("Received headers!");
#endif

					int i = 0;
					sprintf(response, "millis() = %lu", millis());
					while (response[i] != '\0') { i++; }
					client.println("HTTP/1.0 200 OK");
					client.println("Content-Type: text/html");
					client.print("Content-Length: ");
					client.println(i);
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

		client.stop();
	}
}
