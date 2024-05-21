#ifndef SIMPLE_DERIVADOR_H
#define SIMPLE_DERIVADOR_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define VERSION 0.82

//#define MAX_LEN 75


//ESP32 with OLED (NOT TESTED YET WITHOUT IT)
#define OLED 0
#define SERIAL_DEBUG 0
#define SERIAL_DEBUG_MQTT 0
#define SERIAL_DEBUG_MQTT_VALUES 0
#define SERIAL_WIFI_DEBUG 0
#define SERIAL_DEBUG_DIMMER 0


/* 

// Check that flag(1) for start 1 for Master
char *mqtt_pass_the_baton = "derivador/ESP-1111/pass_the_baton";
*/
//General
extern const int beginDerivation;
extern const int keepCurrentMargin;
extern const int maxPower;
//BOARD
extern int flashPinBtn; //We use to debug esp serial report
//WIFI
extern const char *wifiSSID;
extern const char *wifiPass;
extern const char *wifiSSID_ALT;
extern const char *wifiPass_ALT;
//MQTT

extern const char *MQTT_SERVER;
extern const char *MQTT_USER;
extern const char *MQTT_PASS;
extern const uint16_t MQTT_PORT;
extern const char *MQTT_GRID_POWER;
extern const char *MQTT_SOLAR_POWER_1;
extern const char *MQTT_SOLAR_POWER_2;

/* Shelly Direct NOT WORK YET*/
extern const char *shelly_status;


/* AC Dimmer configuration */
/* 
  Pinout Dimmer->ESP32
  
  [VNC] - +5v
  [GND] - Ground
  [Z-C] - Zero/Sync Pin
  [PSM] - Thyristor Pin
  
*/

//Zero Pin / Sync Pin
extern const int syncPin;
// Thyristor Pin
extern const int thyristorPin;


//END CONFIGURATION
/******************************************************/

extern int mqtt_status;
extern int grid_power;
extern int solar_power_1;
extern int solar_power_2;
extern int solar_power_total;
extern int derivation;
extern int mqtt_reconnections;
extern int wifi_disconnections;
extern uint64_t mac;
extern int passBtn;

//extern const char *mqtt_btnPassthroughtTopic;
extern String mqtt_stateTopic;
extern String mqtt_avtyTopic;
extern String mqtt_derivationTopic;
extern String mqtt_rssiTopic;
extern String mqtt_reconnectTopic;
extern String wifi_disconnectTopic;
extern String derivator_id;
extern String mqtt_btnPassthroughtTopic;
extern String mqtt_btnPassthroughtTopic_status;

#include "wifi_utils.h"
#include "mqtt_utils.h"
#include "dimmer.h"
#include "screen.h"

extern WiFiClient espClient;
extern PubSubClient client;
extern DimmableLightLinearized light;

void print_esp_full_report ();

#endif