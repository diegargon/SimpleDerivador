void init_wifi() {
  int wifi_next_ap = 0;

  Serial.print("Connecting to wifi");

  WiFi.begin(wifiSSID, wifiPass);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);    

  while (WiFi.status() != WL_CONNECTED) {        
    if(wifi_next_ap == 5 && wifiSSID_ALT != NULL) {
      Serial.println("Changing to alt ssid");
      WiFi.disconnect();
      WiFi.begin(wifiSSID_ALT, wifiPass_ALT);      
    } else if (wifi_next_ap == 10) {  
      Serial.println("Changing to main ssid");    
      WiFi.disconnect();
      WiFi.begin(wifiSSID, wifiPass);
      wifi_next_ap = 0;
    }
    
    wifi_next_ap++;
        
    Serial.print(".");
    delay(5000);
  }



}