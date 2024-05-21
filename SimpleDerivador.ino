/*
  SimpleDerivador
  by DieGarGon 
  https://github.com/diegargon/SimpleDerivador
  License: Commons Clause License Condition v1.0
*/

#include "simple_derivador.h"


/******************************************************/
// BEGIN CONFIGURATION

/* Logic Configuration */

//When begin to derivate (Watt)
const int beginDerivation = -100;
// Keep current power if power fluctuate within +- XX Watss
const int keepCurrentMargin = 25;
// Max Power percent 100% allowed
const int maxPower = 100;

// Your wifi config
const char *wifiSSID = "Test";
const char *wifiPass = "";
//Set to NULL if not use
const char *wifiSSID_ALT = "Test2";
const char *wifiPass_ALT = "";
//MQTT Server config
const char *MQTT_SERVER = "192.168.2.74";
const char *MQTT_USER = "mqtt";
const char *MQTT_PASS = "mqttpassword";
const uint16_t MQTT_PORT = 1883;
const char *MQTT_GRID_POWER = "shellies/shellyem-C45BBE6A8DCA/emeter/0/power";
/* NOT RELEVANT, just for display, set to NULL without "" if not use */
const char *MQTT_SOLAR_POWER_1 = "shellies/shellyem-C45BBE780ED8/emeter/0/power";
const char *MQTT_SOLAR_POWER_2 = "shellies/shellyem-C45BBE780ED8/emeter/1/power";

/* Shelly Direct NOT WORK YET*/
//const char *shelly_status = "http://192.168.1.120/status";


/* AC Dimmer configuration */
/* 
  Pinout Dimmer->ESP32
  
  [VNC] - +5v
  [GND] - Ground
  [Z-C] - Zero/Sync Pin
  [PSM] - Thyristor Pin
  
*/

//Zero Pin / Sync Pin
const int syncPin = 13;
// Thyristor Pin
const int thyristorPin = 27;



//END CONFIGURATION
/******************************************************/

uint64_t mac = ESP.getEfuseMac();
int old_grid_power = 0;
int last_rssi = 0;
unsigned long millisInterval = 0;

int mqtt_reconnections = 0;
int wifi_disconnections = 0;
int derivation = 0;
int mqtt_status = 0;
int grid_power = 10;
int solar_power_1 = 0;
int solar_power_2 = 0;
int solar_power_total = 0;
int flashPinBtn = 0;
int passBtn = 0;

WiFiClient espClient;
PubSubClient client(espClient);
DimmableLightLinearized light(thyristorPin);


String derivator_id;
String mqtt_stateTopic;
String mqtt_derivationTopic;
String mqtt_rssiTopic;
String mqtt_reconnectTopic;
String wifi_disconnectTopic;


String mqtt_avtyTopic;
String mqtt_btnPassthroughtTopic;
String mqtt_btnPassthroughtTopic_status;

void setup() {

  Serial.begin(115200);
  delay(100);

  // Set uniq id
  String derivator_name = String((unsigned long)((mac & 0xFFFF0000) >> 16), HEX) + String((unsigned long)((mac & 0x0000FFFF)), HEX);
  derivator_name.toUpperCase();

  derivator_id = "derivator_" + derivator_name;

  mqtt_stateTopic = "derivators/" + String(derivator_id) + "/STATE";
  mqtt_avtyTopic = "derivators/" + String(derivator_id) + "/LWT";
  mqtt_derivationTopic = "derivators/" + String(derivator_id) + "/derivation";
  mqtt_rssiTopic = "derivators/" + String(derivator_id) + "/rssi";
  mqtt_reconnectTopic = "derivators/" + String(derivator_id) + "/mqtt_rec";
  wifi_disconnectTopic = "derivators/" + String(derivator_id) + "/wifi_rec";

  mqtt_btnPassthroughtTopic_status = "derivators/" + String(derivator_id) + "/passthrought_status";
  mqtt_btnPassthroughtTopic = "derivators/" + String(derivator_id) + "/passthrought";
  // Dimmer Module
  init_dimmer();
  // OLED graphics.
  if (OLED) {
    init_oled(wifiSSID);
  }

  // Connect to wifi.
  init_wifi();

  if (OLED) {
    draw_screen(grid_power, solar_power_total, derivation, mqtt_status, mqtt_reconnections);
  }
  // Init MQTT
  init_mqtt();
}


void loop() {

  int derivation_change = 0;
  int publish = 0;

  unsigned long currentMillisInterval = millis();

  // Check Wifi

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi Disconnect");
    wifi_disconnections++;
    char wifirec_cstr[4];
    itoa(wifi_disconnections, wifirec_cstr, 10);
    client.publish(wifi_disconnectTopic.c_str(), wifirec_cstr, false);
  }
  //CLEAN
  if (((old_grid_power != grid_power) || (currentMillisInterval - millisInterval > 600)) && ((grid_power <= (beginDerivation - keepCurrentMargin)) || (grid_power >= (beginDerivation + keepCurrentMargin)))) {

    millisInterval = currentMillisInterval;
    //-100-50 = -150 && -100+50 = -50

    if ((grid_power < beginDerivation) && (derivation < maxPower)) {
      derivation++;
      derivation_change = 1;
    } else if (grid_power > beginDerivation && (derivation > 0)) {
      derivation--;
      derivation_change = 1;
    }
    if (passBtn == 1 && derivation < 100) {
      derivation += 5;
      if (derivation > 100) {
        derivation = 100;
      }
      derivation_change = 1;
    }
    if (derivation_change) {
      setDimmerPower(derivation);
    }

    old_grid_power = grid_power;
  }


  if (!client.connected() && WiFi.status() == WL_CONNECTED) {
    mqtt_status = 0;

    mqtt_connect();
    mqtt_reconnections++;
    
    char mqttrec_cstr[4];
    itoa(mqtt_reconnections, mqttrec_cstr, 10);    
    client.publish(mqtt_reconnectTopic.c_str(), mqttrec_cstr, false);
    SERIAL_DEBUG &&Serial.printf("MQTT Reconnections increase %i\n", mqtt_reconnections);

  } else if (!client.connected() && WiFi.status() != WL_CONNECTED) {
    SERIAL_DEBUG &&Serial.println("Wifi seems disconnected");
    init_wifi();
  } else {
    //Publish derivation change
    if (derivation_change) {
      char deriv_cstr[4];
      itoa(derivation, deriv_cstr, 10);
      client.publish(mqtt_derivationTopic.c_str(), deriv_cstr, false);      
      client.endPublish();
    }

    //Publish RSSI change
    if (last_rssi != WiFi.RSSI() && (WiFi.RSSI() < (last_rssi - 3)) || (WiFi.RSSI() > (last_rssi + 3))) {
      //if (last_rssi != WiFi.RSSI()) {
      char rssi_char[10];
      last_rssi = WiFi.RSSI();
      itoa(last_rssi, rssi_char, 10);
      client.publish(mqtt_rssiTopic.c_str(), rssi_char, false);
      client.endPublish();
      SERIAL_DEBUG_MQTT &&Serial.printf("Rssi changed %s\n", rssi_char);
    }
  }

  if (digitalRead(flashPinBtn) == LOW) {
    print_esp_full_report();
  }
  client.loop();

  delay(300);
}


void print_esp_full_report() {
  Serial.println("\n===== ESP32 Full System Report =====");

  // Información del Chip
  Serial.printf("Chip Model: %s\n", ESP.getChipModel());
  Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
  Serial.printf("Number of CPU Cores: %d\n", ESP.getChipCores());
  Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Flash Chip Size: %d bytes\n", ESP.getFlashChipSize());
  Serial.printf("Flash Chip Speed: %d Hz\n", ESP.getFlashChipSpeed());
  Serial.printf("Free Heap Size: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("Sketch Size: %d bytes\n", ESP.getSketchSize());
  Serial.printf("Free Sketch Space: %d bytes\n", ESP.getFreeSketchSpace());
  Serial.printf("SDK Version: %s\n", ESP.getSdkVersion());

  // Información de WiFi
  if (WiFi.isConnected()) {
    Serial.println("WiFi Status: Connected");
    Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Subnet Mask: %s\n", WiFi.subnetMask().toString().c_str());
    Serial.printf("Gateway IP: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
    Serial.printf("MAC Address: %s\n", WiFi.macAddress().c_str());
  } else {
    Serial.println("WiFi Status: Not connected");
  }


  Serial.println("====================================");
}