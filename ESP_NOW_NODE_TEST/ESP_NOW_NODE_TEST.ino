/*
    Program for communicating two ESP8266(tips) to a ESP32(node)
    Tips send values to a pre-specified node
    Update something to the ESP to discover its MAC address
*/

// Espnow and wifi libraries for ESP32
#include <esp_now.h>
#include <WiFi.h>

// Serial Baud rate
#define BAUDRATE 9600

// ESP Now channel
#define CHANNEL 1

// Cada mac address é composto por 6 bytes
// Se for enviar para ESPs específicos, coloque cada endereço separado por vírgula
// {0x24, 0x0A, 0xC4, 0x0E, 0x3F, 0xD1}, {0x24, 0x0A, 0xC4, 0x0E, 0x4E, 0xC3}
// Se for enviar para todos, apenas deixe o endereço de broadcast {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

// Mac address of the devices you want to communicate to
// Working with 2 tips
int numTips = 2;
uint8_t tips_MAC[][6] = {{0x84, 0x0D, 0x8E, 0xA8, 0x15, 0x91}, {0x84, 0x0D, 0x8E, 0xA8, 0x15, 0x92}};

typedef struct data_Struct {
  char macStr[18];
  int data;
} data_Struct;

// Communication variable
data_Struct data_Received;

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
  //If there was a error
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
  // Loop to configure all the tips
  for (int i = 0; i < numTips; i++) {
    // Variable to save tip info
    esp_now_peer_info_t tip;
    // Communcation channel
    tip.channel = CHANNEL;
    // 0 - no encryption 1 - encrypted
    tip.encrypt = 0;
    // Copy the tip address to the structure
    memcpy(tip.peer_addr, tips_MAC[i], sizeof(tips_MAC[i]));
    // Register the callback that will inform us when the node received something
    // The function that will be called is OnDataRecv and is declared bellow
    esp_now_register_recv_cb(OnDataRecv);
    // Add tip
    esp_now_add_peer(&tip);
  }
}

// Callback to when some data has been received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  //Copy the Mac Address to the data received structure
  snprintf(data_Received.macStr, sizeof(data_Received.macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Copy the received data to the same structure
  data_Received.data = *data;
}

void printData() {
  //Clear screen when using putty
  Serial.write(27);
  Serial.write("[H");

  Serial.print("MAC: ");
  Serial.println(data_Received.macStr);
  Serial.print("Data: ");
  Serial.println(data_Received.data);
}

void loop() {
  printData();
  delay(1000);
}
