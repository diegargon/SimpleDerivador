#ifndef SCREEN_H
#define SCREEN_H

#include <U8g2lib.h>
#include <WiFi.h>

void init_oled(char *wifiSSID);
void draw_screen(int power, int solar, int current_power, int mqtt_status, int mqtt_reconnections);

#endif