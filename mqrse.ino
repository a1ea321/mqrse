/*
A program that beeps in morse code according to texts sent to a MQTT topic.

Assumptions:
- Spaces never come at the beginning.
- Spaces never come after a space.

https://morsecode.world/international/timing.html

Dit: 1 unit
Dah: 3 units
Intra-character space (gap between dits and dahs within a character): 1 unit
Inter-character space (gap between the characters of a word): 3 units
Word space (the gap between two words): 7 units

Every  dot/dash is followed by an intra, so INTER-=1.
Every character is followed by an INTER, so SPACE-=3.

PARIS takes 50 units (including the word space at the end).

UNIT is in milliseconds.
*/

// configure for yourself
#define UNIT  100
#define SSID "ssid"
#define PASSWORD "password"
#define MQTT_SERVER "test.mosquitto.org"
#define CLIENT_ID "mqrse001"
#define TOPIC "morse"
#define BUZZER D2
// end of config

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DOT   digitalWrite(BUZZER,HIGH);delay(1*UNIT);digitalWrite(BUZZER,LOW);delay(UNIT);
#define DASH  digitalWrite(BUZZER,HIGH);delay(3*UNIT);digitalWrite(BUZZER,LOW);delay(UNIT);
#define INTER delay(2*UNIT);
#define SPACE delay(4*UNIT);

void morse(char c) {
    switch (c) {
        case 'A': case 'a': DOT DASH break;
        case 'B': case 'b': DASH DOT DOT DOT break;
        case 'C': case 'c': DASH DOT DASH DOT break;
        case 'D': case 'd': DASH DOT DOT break;
        case 'E': case 'e': DOT break;
        case 'F': case 'f': DOT DOT DASH DOT break;
        case 'G': case 'g': DASH DASH DOT break;
        case 'H': case 'h': DOT DOT DOT DOT break;
        case 'I': case 'i': DOT DOT break;
        case 'J': case 'j': DOT DASH DASH DASH break;
        case 'K': case 'k': DASH DOT DASH break;
        case 'L': case 'l': DOT DASH DOT DOT break;
        case 'M': case 'm': DASH DASH break;
        case 'N': case 'n': DASH DOT break;
        case 'O': case 'o': DASH DASH DASH break;
        case 'P': case 'p': DOT DASH DASH DOT break;
        case 'Q': case 'q': DASH DASH DOT DASH break;
        case 'R': case 'r': DOT DASH DOT break;
        case 'S': case 's': DOT DOT DOT break;
        case 'T': case 't': DASH break;
        case 'U': case 'u': DOT DOT DASH break;
        case 'V': case 'v': DOT DOT DOT DASH break;
        case 'W': case 'w': DOT DASH DASH break;
        case 'X': case 'x': DASH DOT DOT DASH break;
        case 'Y': case 'y': DASH DOT DASH DASH break;
        case 'Z': case 'z': DASH DASH DOT DOT break;
        case '0': DASH DASH DASH DASH DASH break;
        case '1': DOT DASH DASH DASH DASH break;
        case '2': DOT DOT DASH DASH DASH break;
        case '3': DOT DOT DOT DASH DASH break;
        case '4': DOT DOT DOT DOT DASH break;
        case '5': DOT DOT DOT DOT DOT break;
        case '6': DASH DOT DOT DOT DOT break;
        case '7': DASH DASH DOT DOT DOT break;
        case '8': DASH DASH DASH DOT DOT break;
        case '9': DASH DASH DASH DASH DOT break;
        case ',': DASH DASH DOT DOT DASH DASH break;
        case '?': DOT DOT DASH DASH DOT DOT break;
        case ':': DASH DASH DASH DOT DOT DOT break;
        case '&': DOT DASH DOT DOT DOT break;
        case ' ': SPACE; return;
        default: return;
    }
    INTER;
}

WiFiClient _c;
PubSubClient client(_c);

void reconnect_if_needed() {
    while (!client.connected()) {
        if (!client.connect(CLIENT_ID)) {
            delay(5000);
        }
    }
    client.subscribe(TOPIC);
}

void callback(char* topic, byte* payload, unsigned int length) {
    for (unsigned int i = 0; i < length; i++) {
        morse((char)payload[i]);
    }
}

void setup() {
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW);

    delay(10);
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) delay(500);

    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);
}

void loop() {
    reconnect_if_needed();
    client.loop();
} 
