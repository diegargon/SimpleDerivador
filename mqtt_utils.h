#ifndef MQTT_UTILS_H
#define MQTT_UTILS_H

#include <Arduino.h>
#include <cmath>
#include <PubSubClient.h>
#include "simple_derivador.h"

// max packet is 128 overwrite with mqtt.setBuffer if not not work
#define MAX_MQTT_PACKET 1024*8

void init_mqtt();
void mqtt_connect();
void mqtt_callback(char *topic, byte *payload, unsigned int length);
void sendMQTTStatusDiscoveryMsg();

#endif