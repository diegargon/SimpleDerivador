

void init_mqtt() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqtt_callback);
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //String clientId = "ESP32-";
    //clientId +=  chipid;
    Serial.print(esp_id);
    Serial.print(": Attempting MQTT connection...");    
    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
    if (client.connect(esp_id.c_str(), MQTT_USER, MQTT_PASS)) {
      mqtt_status = 1;
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      client.endPublish();
      // ... and resubscribe
      client.subscribe(MQTT_GRID_POWER);
      client.subscribe(MQTT_SOLAR_POWER);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 10 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqtt_callback(char *topic, byte *payload, unsigned int length) {

  char power[32] = { '\0' };
  char solar[32] = { '\0' };

  payload[length] = '\0';

  if (strcmp(topic, MQTT_GRID_POWER) == 0) {
    strncpy(power, (char *)payload, length);
    grid_power = atoi(power);
    if(SERIAL_DEBUG){
      Serial.print("MQTT Recive grid power: ");
      Serial.println(grid_power);
    }
  }
  if (strcmp(topic, MQTT_SOLAR_POWER) == 0) {
    strncpy(solar, (char *)payload, length);
    solar_power = atoi(solar);
    if(SERIAL_DEBUG) {
      Serial.print("MQTT Recive solar power: ");
      Serial.println(solar_power);
    }
  }
  
  if (OLED) {
    draw_screen(grid_power, solar_power, derivation, mqtt_status, mqtt_reconnections);
  }
}
