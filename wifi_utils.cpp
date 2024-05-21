#include "wifi_utils.h"

void printWiFiStatus(int status) {
  // Esta función imprime un mensaje basado en el estado de la conexión WiFi
  switch (status) {
    case WL_IDLE_STATUS:
      Serial.println("\nWiFi status: WL_IDLE_STATUS\n");
      break;
    case WL_NO_SSID_AVAIL:
      Serial.println("\nWiFi status: WL_NO_SSID_AVAIL\n");
      break;
    case WL_SCAN_COMPLETED:
      Serial.println("\nWiFi status: WL_SCAN_COMPLETED\n");
      break;
    case WL_CONNECTED:
      Serial.println("\nWiFi status: WL_CONNECTED\n");
      break;
    case WL_CONNECT_FAILED:
      Serial.println("\nWiFi status: WL_CONNECT_FAILED\n");
      break;
    case WL_CONNECTION_LOST:
      Serial.println("\nWiFi status: WL_CONNECTION_LOST\n");
      break;
    case WL_DISCONNECTED:
      Serial.println("\nWiFi status: WL_DISCONNECTED\n");
      break;
    default:
      Serial.println("\nWiFi status: Unknown\n");
      break;
  }
}

void init_wifi() {
  int wifi_try_next_ap = 0;
  int print_status_counter = 0;

  if (SERIAL_WIFI_DEBUG) {
    Serial.println("Starting WiFi scan...");
    int numNetworks = WiFi.scanNetworks();

    if (numNetworks == 0) {
      Serial.println("No networks found.");
    } else {
      Serial.printf("Found %d networks:\n", numNetworks);

      for (int i = 0; i < numNetworks; ++i) {
        Serial.printf("SSID: %s, RSSI: %d dBm, Encryption Type: %d\n",
                      WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.encryptionType(i));
        delay(10);
      }
    }
  }

  SERIAL_WIFI_DEBUG&& Serial.printf("Connecting to WiFi: \'%s\' using password: \'%s\'\n", wifiSSID, wifiPass);
  //WiFi.mode(WIFI_STA); //Default
  WiFi.begin(wifiSSID, wifiPass);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  while (WiFi.status() != WL_CONNECTED) {

    if (wifi_try_next_ap == 5 && wifiSSID_ALT != NULL) {
      SERIAL_WIFI_DEBUG&& Serial.println("Changing to alt ssid");
      WiFi.disconnect();
      WiFi.begin(wifiSSID_ALT, wifiPass_ALT);
    } else if (wifi_try_next_ap == 10) {
      SERIAL_WIFI_DEBUG&& Serial.println("Changing to main ssid");
      WiFi.disconnect();
      WiFi.begin(wifiSSID, wifiPass);
      wifi_try_next_ap = 0;
    }

    wifi_try_next_ap++;

    SERIAL_WIFI_DEBUG&& Serial.print(".");
    print_status_counter++;
    if (SERIAL_WIFI_DEBUG && print_status_counter == 10) {
      int wifi_status = WiFi.status();
      printWiFiStatus(wifi_status);
      print_status_counter = 0;
    }
    delay(5000);
  }
}