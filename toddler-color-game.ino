#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* ========= CONFIG - Copy config.h.example to config.h ========= */
#include "config.h"

/* ========= GLOBALS ========= */

WiFiClient espClient;
PubSubClient client(espClient);

const int NUM_BUTTONS = 4;
int ActiveButton = 0;

/* ========= BUTTON STRUCT ========= */

struct Button {
  int buttonPin;
  int ledPin;
  const char* name;
  bool last;
};

Button buttons[] = {
  { D1, D5, "red",    HIGH },
  { D2, D6, "blue",   HIGH },
  { D3, D7, "green",  HIGH },
  { D4, D8, "yellow", HIGH }
};

/* ========= HELPERS ========= */

Button* getNextActiveButton() {
  int next;
  do {
    next = random(NUM_BUTTONS);
  } while (next == ActiveButton);

  ActiveButton = next;
  return &buttons[ActiveButton];
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(MQTT_CLIENT_ID)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying...");
      delay(1000);
    }
  }
}

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

/* ========= SETUP ========= */

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\nToddler Color Game - Booting...");
  randomSeed(ESP.getChipId());

  connectWiFi();

  client.setServer(MQTT_BROKER, MQTT_PORT);
  reconnectMQTT();

  for (auto &b : buttons) {
    pinMode(b.buttonPin, INPUT_PULLUP);
    pinMode(b.ledPin, OUTPUT);
    digitalWrite(b.ledPin, LOW);
    b.last = digitalRead(b.buttonPin);
  }

  // Turn on first active button LED
  Button* activeButton = &buttons[ActiveButton];
  digitalWrite(activeButton->ledPin, HIGH);

  Serial.printf("Starting with button: %s\n", activeButton->name);
  Serial.printf("Publishing to: %s\n", MQTT_TOPIC);
}

/* ========= LOOP ========= */

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  // Scan all buttons
  for (auto &b : buttons) {
    bool cur = digitalRead(b.buttonPin);

    // Detect button press (HIGH -> LOW)
    if (b.last == HIGH && cur == LOW) {
      if (&b == &buttons[ActiveButton]) {
        // Correct button pressed
        Serial.printf("Correct! Button: %s\n", b.name);
        client.publish(MQTT_TOPIC, b.name);

        // Turn off current LED
        digitalWrite(b.ledPin, LOW);

        // Pick next active button
        Button* next = getNextActiveButton();
        digitalWrite(next->ledPin, HIGH);
        Serial.printf("Next button: %s\n", next->name);
      } else {
        // Incorrect button pressed
        Serial.printf("Wrong! Pressed: %s (expected: %s)\n", b.name, buttons[ActiveButton].name);
      }

      delay(200); // simple debounce
    }

    // Update last state
    b.last = cur;
  }

  delay(20); // main loop pacing
}
