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

#include "config.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define · digitalWrite(BUZZER,HIGH);delay(1*UNIT);digitalWrite(BUZZER,LOW);INTRA;
#define _ digitalWrite(BUZZER,HIGH);delay(3*UNIT);digitalWrite(BUZZER,LOW);INTRA;
#define INTRA delay(1*UNIT);
#define INTER delay(2*UNIT);
#define SPACE delay(4*UNIT);

void morse(char c) {
    switch (c) {
        case 'A': case 'a': · _ break;
        case 'B': case 'b': _ · · · break;
        case 'C': case 'c': _ · _ · break;
        case 'D': case 'd': _ · · break;
        case 'E': case 'e': · break;
        case 'F': case 'f': · · _ · break;
        case 'G': case 'g': _ _ · break;
        case 'H': case 'h': · · · · break;
        case 'I': case 'i': · · break;
        case 'J': case 'j': · _ _ _ break;
        case 'K': case 'k': _ · _ break;
        case 'L': case 'l': · _ · · break;
        case 'M': case 'm': _ _ break;
        case 'N': case 'n': _ · break;
        case 'O': case 'o': _ _ _ break;
        case 'P': case 'p': · _ _ · break;
        case 'Q': case 'q': _ _ · _ break;
        case 'R': case 'r': · _ · break;
        case 'S': case 's': · · · break;
        case 'T': case 't': _ break;
        case 'U': case 'u': · · _ break;
        case 'V': case 'v': · · · _ break;
        case 'W': case 'w': · _ _ break;
        case 'X': case 'x': _ · · _ break;
        case 'Y': case 'y': _ · _ _ break;
        case 'Z': case 'z': _ _ · · break;
        case '0': _ _ _ _ _ break;
        case '1': · _ _ _ _ break;
        case '2': · · _ _ _ break;
        case '3': · · · _ _ break;
        case '4': · · · · _ break;
        case '5': · · · · · break;
        case '6': _ · · · · break;
        case '7': _ _ · · · break;
        case '8': _ _ _ · · break;
        case '9': _ _ _ _ · break;
        case ',': _ _ · · _ _ break;
        case '?': · · _ _ · · break;
        case ':': _ _ _ · · · break;
        case '&': · _ · · · break;
        case ' ': SPACE; return;
        default: return;
    }
    INTER;
}

WiFiClient _c;
PubSubClient client(_c);

void reconnect_if_needed() {
    while (!client.connected()) {
        if (client.connect(CLIENT_ID)) {
            client.subscribe(TOPIC);
        }
        else {
            delay(5000);
        }
    }
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
    WiFi.begin(MY_SSID, MY_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) delay(500);

    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);
}

void loop() {
    reconnect_if_needed();
    client.loop();
} 
