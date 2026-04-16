#include <ESP8266WiFi.h>
#include <espnow.h>

// Structure of incoming data
typedef struct struct_message {
  char command[32];  // for example: "lights on"
} struct_message;

struct_message incomingData;

// GPIO to control relay (Change D1 to the pin connected to your relay)
#define RELAY_PIN 0  // GPIO2 on ESP-01

void onDataReceive(uint8_t *mac, uint8_t *incomingDataBuffer, uint8_t len) {
  memcpy(&incomingData, incomingDataBuffer, sizeof(incomingData));
  Serial.print("Received command: ");
  Serial.println(incomingData.command);

  // Simple control logic
  if (strcmp(incomingData.command, "lights on") == 0) {
    digitalWrite(RELAY_PIN, HIGH);  // turn ON relay
  } else if (strcmp(incomingData.command, "lights off") == 0) {
    digitalWrite(RELAY_PIN, LOW);   // turn OFF relay
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Initially off

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Disable WiFi connection

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onDataReceive);
  
  Serial.println("ESP-01 Receiver Ready");
}

void loop() {
  // Nothing to do in loop, all done in callback
}
