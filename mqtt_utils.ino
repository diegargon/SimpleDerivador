
void init_mqtt() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqtt_callback);
  client.setBufferSize(MAX_MQTT_PACKET);
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(derivator_id);
    Serial.print(": Attempting MQTT connection...");

    // Attempt to connect
    
    if (client.connect(derivator_id.c_str(), MQTT_USER, MQTT_PASS, mqtt_avtyTopic.c_str(), 1, true, "Offline")) {
      mqtt_status = 1;      
      client.publish(mqtt_avtyTopic.c_str(), "Online", true);

      Serial.println("MQTT connected");

      sendMQTTStatusDiscoveryMsg();
      // ... and resubscribe
      client.subscribe(MQTT_GRID_POWER);
      if (MQTT_SOLAR_POWER_1 != NULL) {
        client.subscribe(MQTT_SOLAR_POWER_1);
      }
      if (MQTT_SOLAR_POWER_2 != NULL) {
        client.subscribe(MQTT_SOLAR_POWER_2);
      }
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

  char power[12] = { '\0' };
  char solar[12] = { '\0' };
  int solar_update = 0;

  payload[length] = '\0';

  if (strcmp(topic, MQTT_GRID_POWER) == 0) {
    strncpy(power, (char *)payload, length);
    grid_power = atoi(power);
    if (SERIAL_DEBUG) {
      Serial.print("MQTT Received grid power: ");
      Serial.println(grid_power);
    }
  }
  if (MQTT_SOLAR_POWER_1 != NULL && (strcmp(topic, MQTT_SOLAR_POWER_1) == 0)) {
    strncpy(solar, (char *)payload, length);
    solar_power_1 = atoi(solar);
    solar_update = 1;
  }

  if (MQTT_SOLAR_POWER_2 != NULL && (strcmp(topic, MQTT_SOLAR_POWER_2) == 0)) {
    strncpy(solar, (char *)payload, length);
    solar_power_2 = atoi(solar);
    solar_update = 1;
  }

  if (solar_update) {
    solar_power_total = solar_power_1 + solar_power_2;

    if (SERIAL_DEBUG) {
      Serial.print("MQTT Received solar power: ");
      Serial.println(solar_power_total);
    }
  }

  if (OLED) {
    draw_screen(grid_power, solar_power_total, derivation, mqtt_status, mqtt_reconnections);
  }
}


void sendMQTTStatusDiscoveryMsg() {

  String discoveryTopic = "homeassistant/sensor/" + String(derivator_id) + "/config";

  DynamicJsonDocument doc(MAX_MQTT_PACKET);
  char buffer[512];
  size_t n;

  doc["name"] = String(derivator_id) + " status";
  doc["stat_t"] = mqtt_derivationTopic;
  doc["avty_t"] = mqtt_avtyTopic;
  doc["ic"] = "mdi:information-outline",
  doc["unit_of_meas"] = "%";
  doc["dev_cla"] = "power_factor";
  doc["uniq_id"] = String(derivator_id);
  doc["frc_upd"] = true;
  doc["pl_avail"] = "Online";
  doc["pl_not_avail"] = "Offline";

  JsonObject dev = doc.createNestedObject("dev");
  JsonArray ids = dev.createNestedArray("ids");
  ids.add(String(derivator_id));
  dev["name"] = "Derivator";
  dev["mdl"] = "Derivator 0.1";
  dev["sw"] = "0.1 (Derivator)";
  dev["mf"] = "DieGarGon";

  n = serializeJson(doc, buffer);

  Serial.println("Sending discovery topic");
  client.publish(discoveryTopic.c_str(), (uint8_t *)buffer, n, true);
}

