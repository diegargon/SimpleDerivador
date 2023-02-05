#include <WiFi.h>
#include <PubSubClient.h>
#include <dimmable_light_linearized.h>
#include "screen.h"


/* 

// Check that flag(1) for start 1 for Master
char *mqtt_pass_the_baton = "derivador/ESP-1111/pass_the_baton";
*/

/******************************************************/
// BEGIN CONFIGURATION 
//ESP32 with OLED (NOT TESTED YET WITHOUT IT)
#define OLED  1 
#define SERIAL_DEBUG 1

//Zero Pin / Sync Pin
const int syncPin = 13;
// Thyristor Pin
const int thyristorPin = 25;
//When begin derivation
const int beginDerivation = -50;
// keep current power if values are within +- XX
const int keepCurrentMargin = 50;
// Power percent 100%
const int maxPower = 100;

// Your wifi config
char wifiSSID[] = "Casa";
char wifiPass[] = "";
//MQTT Server confiog
const char *MQTT_SERVER = "192.168.2.74";
const char *MQTT_USER = "mqtt";
const char *MQTT_PASS = "mqttpassword";
const uint16_t MQTT_PORT = 1883;
const char *MQTT_GRID_POWER = "shellies/shellyem-C45BBE6A8DCA/emeter/0/power";
/* NOT RELEVANT, just for display, at this moment will give error if not provided just add the same to power if you have that value in your mqtt server */
const char *MQTT_SOLAR_POWER = "shellies/shellyem-C45BBE780ED8/emeter/0/power";
//END CONFIGURATION
/******************************************************/

String esp_id;
uint64_t mac = ESP.getEfuseMac();
int old_grid_power = 0;
int grid_power = 0;
int solar_power = 0;
int derivation = 0;
int mqtt_reconnections = 0;
int mqtt_status = 0;

WiFiClient espClient;
PubSubClient client(espClient);
DimmableLightLinearized light(thyristorPin);


void setup() {

  Serial.begin(115200);
  while (!Serial)
    ;
  // Set uniq id
  String mac_hex = String((unsigned long)((mac & 0xFFFF0000) >> 16 ), HEX) + String( (unsigned long)((mac & 0x0000FFFF)), HEX);
  esp_id = "ESP32-" + mac_hex;
  //esp_id.toUpperCase();

  //esp_id.concat(ESP.getEfuseMac());
  // Dimmer Module
  init_dimmer();
  // OLED graphics.
  if (OLED) {
    init_oled(wifiSSID);
  }

  // Connect to wifi.

  Serial.print("Connecting to wifi");
  WiFi.begin(wifiSSID, wifiPass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  //sprintf(chBuffer, "WiFi connected to %s.", wifiSSID);
  Serial.print("WiFi connected to ");
  Serial.println(wifiSSID);

  if (OLED) {
    draw_screen(grid_power, solar_power, derivation, mqtt_status, mqtt_reconnections);
  }
  // Init MQTT
  init_mqtt();
}

void loop() {
  
  if ((old_grid_power != grid_power) && 
  ((grid_power <= (beginDerivation - keepCurrentMargin)) || (grid_power >= (beginDerivation + keepCurrentMargin)))
  ) {
    //-100-50 = -150 && -100+50 = -50    
    Serial.print("Current grid power change from ");
    Serial.print(old_grid_power);
    Serial.print(" to ");
    if ((grid_power < beginDerivation) && (derivation <= maxPower)) {
      derivation++;
      light.setBrightness(round((derivation * 255) / 100));
    } else if (grid_power > beginDerivation && (derivation > 0)) {
      derivation--;
      light.setBrightness(round((derivation * 255) / 100));
    }    
    Serial.println(grid_power);
    old_grid_power = grid_power;
  }

  if (!client.connected()) {    
    mqtt_status = 0;
    mqtt_reconnect();
    mqtt_reconnections++;
    if(SERIAL_DEBUG) {
      Serial.println("Reconnectiong MQTT");
    }
  }

  client.loop();

  delay(200);
}