/*
    Program for communicating two ESP8266(tips) to a ESP32(node)
    Tips send values to a pre-specified node
    Update something to the ESP to discover its MAC address
*/

// Espnow and wifi libraries for ESP8266
#include <espnow.h>
#include <ESP8266WiFi.h>

#ifndef ESP_OK
#define ESP_OK 0
#endif

// Serial Baud rate
#define BAUDRATE 9600

// ESP Now channel
#define CHANNEL 1

// Mac address of the device you want to communicate to
uint8_t node_MAC[] = {0xC8, 0x2B, 0x96, 0x87, 0x99, 0xC0};

// Communication variable
int values = 0;

// timing varaibles
unsigned long lastTime = 0;
unsigned long timerDelay = 2000;
//--------------------------------------------------------------------

void setup() {
  Serial.begin(BAUDRATE);

  // Wifi mode - station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  //Initialize ESP Now
  InitESPNow();
}

void InitESPNow() {
  //If connection was succesfull
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  //If there was an error
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
  // Device mode
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

// Register the callback that will inform us when the tip sent something
  // The function that will be called is OnDataSent and is declared bellow
  esp_now_register_send_cb(OnDataSent);

  //pair with another ESP-NOW device to send data
  //It accepts: mac address, role, wi-fi channel, key, and key length.
  esp_now_add_peer(node_MAC, ESP_NOW_ROLE_CONTROLLER, CHANNEL, NULL, 0);
}

// Callback to when some data has been sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Sent Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  }
  else {
    Serial.println("Delivery fail");
  }
}

void send() {
  esp_now_send(node_MAC, (uint8_t*) &values, sizeof(values));
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    values = random(0, 10);
    send();
    lastTime = millis();
  }
}
