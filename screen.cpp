#include "screen.h"

#define BUFF_LEN 128
#define FONT_ONE_HEIGHT 8
#define FONT_TWO_HEIGHT 20

char chBuffer[BUFF_LEN];

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, 16, 15, 4);

void init_oled(char *wifiSSID) {

  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  u8g2.clearBuffer();

  sprintf(chBuffer, "%s", "Connecting to:");
  u8g2.drawStr(64 - (u8g2.getStrWidth(chBuffer) / 2), 0, chBuffer);
  sprintf(chBuffer, "%s", wifiSSID);
  u8g2.drawStr(64 - (u8g2.getStrWidth(chBuffer) / 2), 31 - (FONT_ONE_HEIGHT / 2), chBuffer);
  u8g2.sendBuffer();
}

void draw_screen(int grid_power, int solar_power, int derivation, int mqtt_status, int mqtt_reconnections) {
  // Display connection stats.

  // Clean the display buffer.  
  u8g2.clearBuffer();
  u8g2.clearDisplay();
  
  // Display the title.

  sprintf(chBuffer, "%s", "[WiFi Stats]");
  u8g2.drawStr(64 - (u8g2.getStrWidth(chBuffer) / 2), 0, chBuffer);

  // Display the ip address assigned by the wifi router.

  char chIp[81];
  WiFi.localIP().toString().toCharArray(chIp, sizeof(chIp) - 1);
  sprintf(chBuffer, "IP:%s", chIp);
  u8g2.drawStr(0, FONT_ONE_HEIGHT * 1, chBuffer);

  // Display the ssid of the wifi router.

  sprintf(chBuffer, "SSID:%s RSSI:%d", WiFi.SSID(), WiFi.RSSI());  
  u8g2.drawStr(0, FONT_ONE_HEIGHT * 2, chBuffer);

  //MQTT Status / Reconnections
  sprintf(chBuffer, "MQTT: %d/%d", mqtt_status, mqtt_reconnections);
  u8g2.drawStr(0, FONT_ONE_HEIGHT * 3, chBuffer);


  //sprintf(chBuffer, "Power: %.0f Solar: %.0f", atof(power),atof(solar));  
  sprintf(chBuffer, "Grid:%d Solar:%d", grid_power, solar_power);
  u8g2.drawStr(0, FONT_ONE_HEIGHT * 4, chBuffer);

  //sprintf(chBuffer, "Solar: %.0f", atof(solar));
  //u8g2.drawStr(0, FONT_ONE_HEIGHT * 6, chBuffer);

  sprintf(chBuffer, "Derivation: %d%%", derivation);  
  u8g2.drawStr(0, FONT_ONE_HEIGHT * 5, chBuffer);
  
  u8g2.sendBuffer();
}

