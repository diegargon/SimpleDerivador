/*
  SimpleDerivador
  by DieGarGon
  https://github.com/diegargon/SimpleDerivador
  License: Commons Clause License Condition v1.0
*/
#include <WiFi.h>
#include <WiFiMulti.h>
#include <PubSubClient.h>
#include <dimmable_light_linearized.h>
#include <ArduinoJson.h>
#include "screen.h"

#define MAX_LEN 75
// max packet is 128 overwrite with mqtt.setBuffer if not not work
#define MAX_MQTT_PACKET 1024
/* 

// Check that flag(1) for start 1 for Master
char *mqtt_pass_the_baton = "derivador/ESP-1111/pass_the_baton";
*/

/******************************************************/
// BEGIN CONFIGURATION
//ESP32 with OLED (NOT TESTED YET WITHOUT IT)
#define OLED 1
#define SERIAL_DEBUG 1


//Zero Pin / Sync Pin
const int syncPin = 13;
// Thyristor Pin
const int thyristorPin = 27;
//When begin derivation
const int beginDerivation = -100;
// keep current power if power value are within +- XX
const int keepCurrentMargin = 25;
// Power percent 100%
const int maxPower = 100;


// Your wifi config
char *wifiSSID = "Casa";
char *wifiPass = "";
//Set to NULL if not use
char *wifiSSID_ALT = "Taller";
char *wifiPass_ALT = "";
//MQTT Server confiog
const char *MQTT_SERVER = "192.168.2.74";
const char *MQTT_USER = "mqtt";
const char *MQTT_PASS = "mqttpassword";
const uint16_t MQTT_PORT = 1883;
const char *MQTT_GRID_POWER = "shellies/shellyem-C45BBE6A8DCA/emeter/0/power";
/* NOT RELEVANT, just for display, set to NULL if not use */
const char *MQTT_SOLAR_POWER_1 = "shellies/shellyem-C45BBE780ED8/emeter/0/power";
const char *MQTT_SOLAR_POWER_2 = "shellies/shellyem-C45BBE780ED8/emeter/1/power";
//END CONFIGURATION
/******************************************************/

String derivator_id;
uint64_t mac = ESP.getEfuseMac();
int old_grid_power = 0;
int grid_power = 0;
int solar_power_total = 0;
int solar_power_1 = 0;
int solar_power_2 = 0;
int derivation = 0;
int mqtt_reconnections = 0;
int mqtt_status = 0;
int last_rssi = 0;

String mqtt_derivationTopic;
String mqtt_avtyTopic;
String mqtt_rssiTopic;

unsigned long millisInterval = 0;

WiFiClient espClient;
PubSubClient client(espClient);
DimmableLightLinearized light(thyristorPin);


void setup() {

  Serial.begin(115200);
  while (!Serial)
    ;

  // Set uniq id
  derivator_id = String((unsigned long)((mac & 0xFFFF0000) >> 16), HEX) + String((unsigned long)((mac & 0x0000FFFF)), HEX);
  derivator_id.toUpperCase();
  derivator_id = "derivator_" + derivator_id;

  mqtt_avtyTopic = "derivators/" + String(derivator_id) + "/LWT";
  mqtt_derivationTopic = "derivators/" + String(derivator_id) + "/STATE";
  mqtt_rssiTopic = "derivators/" + String(derivator_id) + "/RSSI";
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
  
  unsigned long currentMillisInterval = millis();

  /*
    Check Wifi
  */
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi Disconnect");
  }

  /*
    if grid power change or pass 600ms and ...
  */

  if (((old_grid_power != grid_power) || (currentMillisInterval - millisInterval > 600)) && ((grid_power <= (beginDerivation - keepCurrentMargin)) || (grid_power >= (beginDerivation + keepCurrentMargin)))) {
    millisInterval = currentMillisInterval;
    //-100-50 = -150 && -100+50 = -50
    Serial.print("Current grid power change from ");
    Serial.print(old_grid_power);
    Serial.print(" to ");
    Serial.println(grid_power);

    if ((grid_power < beginDerivation) && (derivation < maxPower)) {
      derivation++;
      light.setBrightness(round((derivation * 255) / 100));
      Serial.print("Derivation +change to :");
      Serial.println(derivation);
      derivation_change = 1;
    } else if (grid_power > beginDerivation && (derivation > 0)) {
      derivation--;
      light.setBrightness(round((derivation * 255) / 100));
      Serial.print("Derivation -change to :");
      Serial.println(derivation);
      derivation_change = 1;
    }

    old_grid_power = grid_power;
  }

  if (!client.connected()) {
    mqtt_status = 0;
    mqtt_reconnect();
    mqtt_reconnections++;
    if (SERIAL_DEBUG) {
      Serial.println("Reconnectiong MQTT");
    }
  } else {
    //Publish derivation change
    if (derivation_change) {
      char deriv_cstr[4];
      itoa(derivation, deriv_cstr, 10);      
      client.publish(mqtt_derivationTopic.c_str(), deriv_cstr);
      client.endPublish();
    }

    //Publish RSSI change
    if (last_rssi != WiFi.RSSI() && (  WiFi.RSSI() < (last_rssi - 3) ) || ( WiFi.RSSI() > (last_rssi + 3) )) {
      char rssi_char[10];
      last_rssi = WiFi.RSSI();
      itoa(last_rssi, rssi_char, 10);
      
      client.publish(mqtt_rssiTopic.c_str(), rssi_char);
      client.endPublish();      
    }
  }

  client.loop();

  delay(200);
}
