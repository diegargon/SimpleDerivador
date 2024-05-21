
#include "mqtt_utils.h"

void init_mqtt() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqtt_callback);
  client.setBufferSize(MAX_MQTT_PACKET);
  delay(100);
}

void mqtt_connect() {
  // Loop until we're connected
  while (!client.connected()) {

    SERIAL_DEBUG_MQTT &&Serial.println("Attempting MQTT connection...");

    if ((WiFi.status() != WL_CONNECTED)) {
      SERIAL_DEBUG && Serial.println("WiFi not connected, aborting MQTT connection");
      return;
    }

    // Attempt to connect

    if (client.connect(derivator_id.c_str(), MQTT_USER, MQTT_PASS, mqtt_avtyTopic.c_str(), 1, true, "Offline")) {
      mqtt_status = 1;
      client.publish(mqtt_avtyTopic.c_str(), "Online", true);

      SERIAL_DEBUG_MQTT &&Serial.println("MQTT connected");

      sendMQTTStatusDiscoveryMsg();

      // ... and resubscribe
      client.subscribe(MQTT_GRID_POWER);
      if (MQTT_SOLAR_POWER_1 != NULL) {
        client.subscribe(MQTT_SOLAR_POWER_1);
      }
      if (MQTT_SOLAR_POWER_2 != NULL) {
        client.subscribe(MQTT_SOLAR_POWER_2);
      }
      //Listen button
      client.subscribe(mqtt_btnPassthroughtTopic.c_str());
    } else {
      SERIAL_DEBUG_MQTT &&Serial.printf("MQTT connection failed, rc=%d\n", client.state());
      delay(5000);
    }
  }
  Serial.println("MQTT connect finish ok, sending defaults");
  client.publish(mqtt_stateTopic.c_str(), "Online", true);
  client.publish(mqtt_derivationTopic.c_str(), "0", false);
  client.publish(mqtt_rssiTopic.c_str(), "0", false);

  char mqttrec_cstr[4];
  itoa(mqtt_reconnections, mqttrec_cstr, 10);    
  client.publish(mqtt_reconnectTopic.c_str(), mqttrec_cstr, false);
  
  char wifirec_cstr[4];
  itoa(wifi_disconnections, wifirec_cstr, 10);  
  client.publish(wifi_disconnectTopic.c_str(), wifirec_cstr, false);
  //client.publish(mqtt_btnPassthroughtTopic.c_str(), "OFF", false);
  //client.publish(mqtt_btnPassthroughtTopic_status.c_str(), "OFF", true);
}

void mqtt_callback(char *topic, byte *payload, unsigned int length) {
  if (topic) {
    SERIAL_DEBUG_MQTT &&Serial.printf("Callback called for topic: %s\n", topic);
  } else {
    SERIAL_DEBUG_MQTT &&Serial.println("Empty MQTT topic");
    return;
  }

  char power[12] = { '\0' };
  char solar[12] = { '\0' };
  char passbtn[12] = { '\0' };

  int solar_update = 0;

  payload[length] = '\0';


  if (strcmp(topic, MQTT_GRID_POWER) == 0 && payload) {

    strncpy(power, (char *)payload, length);
    SERIAL_DEBUG_MQTT &&SERIAL_DEBUG_MQTT_VALUES &&Serial.printf("Power: %s, Payload: %s, Length %i\n", power, payload, length);
    if (power) {
      float fpower = atof(power);
      grid_power = floor(fpower);
    }

    SERIAL_DEBUG_MQTT &&SERIAL_DEBUG_MQTT_VALUES &&Serial.printf("MQTT Received grid power: %i\n", grid_power);
  }  

  if (MQTT_SOLAR_POWER_1 != NULL && (strcmp(topic, MQTT_SOLAR_POWER_1) == 0)) {
    if (payload) {
      strncpy(solar, (char *)payload, length);
      float fsolar = atof(solar);
      solar_power_1 = ceil(fsolar);
      solar_update = 1;
    }
  }

  if (MQTT_SOLAR_POWER_2 != NULL && (strcmp(topic, MQTT_SOLAR_POWER_2) == 0)) {
    strncpy(solar, (char *)payload, length);
    float fsolar = atof(solar);
    solar_power_2 = static_cast<int>(fsolar);
    solar_update = 1;
  }

  if (mqtt_btnPassthroughtTopic != NULL && (strcmp(topic, mqtt_btnPassthroughtTopic.c_str()) == 0)) {
    strncpy(passbtn, (char *)payload, length);
    if (strcmp(passbtn, "ON") == 0) {
      passBtn = 1;
      client.publish(mqtt_btnPassthroughtTopic_status.c_str(), "ON", true);
    }
    if (strcmp(passbtn, "OFF") == 0) {
      passBtn = 0;
      client.publish(mqtt_btnPassthroughtTopic_status.c_str(), "OFF", true);
    }
  }
  if (mqtt_btnPassthroughtTopic_status != NULL && (strcmp(topic, mqtt_btnPassthroughtTopic_status.c_str()) == 0)) {
    strncpy(passbtn, (char *)payload, length);
  }

  if (solar_update) {
    solar_power_total = solar_power_1 + solar_power_2;
    if (SERIAL_DEBUG_MQTT && SERIAL_DEBUG_MQTT_VALUES) {
      Serial.printf("MQTT Received solar power: %d\n", solar_power_total);
    }
  }
  SERIAL_DEBUG_MQTT && SERIAL_DEBUG_MQTT_VALUES &&Serial.printf("Grid Power: %i, SolarPowerTotal: %i\n", grid_power, solar_power_total);

  if (OLED) {
    draw_screen(grid_power, solar_power_total, derivation, mqtt_status, mqtt_reconnections);
  }
}


void sendMQTTStatusDiscoveryMsg() {

  SERIAL_DEBUG_MQTT &&Serial.println("Sending MQTT Discovery message");

  String discoveryTopic = "homeassistant/sensor/" + String(derivator_id) + "/config";

  DynamicJsonDocument doc(MAX_MQTT_PACKET);
  char buffer[4096];
  size_t n;

  doc["name"] = "Status";  
  doc["state_topic"] = mqtt_stateTopic;
  doc["availability_topic"] = mqtt_avtyTopic;
  doc["uniq_id"] = String(derivator_id) + "_status";
  doc["frc_upd"] = true;
  doc["pl_avail"] = "Online";
  doc["pl_not_avail"] = "Offline";
  //doc["mac"] = mac; Where?
  doc["ret"] = true;

  JsonObject dev = doc.createNestedObject("dev");
  JsonArray ids = dev.createNestedArray("ids");
  ids.add(String(derivator_id));
  dev["name"] = "SimpleDerivator";
  dev["mdl"] = "SimpleDerivator";
  dev["sw"] = VERSION;
  dev["mf"] = "DieGarGon";

  //Esto falla y deja de funcionar el autodiscovery
  //JsonObject cns = dev.createNestedObject("cns");
  //cns["mac"] = mac;

  n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), (uint8_t *)buffer, n);

  discoveryTopic = "homeassistant/sensor/" + String(derivator_id) + "/derivation/config";
  doc["name"] = "Derivation";
  doc["state_topic"] = mqtt_derivationTopic;
  doc["uniq_id"] = String(derivator_id) + "_derivion";
  doc["ic"] = "mdi:information-outline",
  doc["unit_of_meas"] = "%";
  doc["dev_cla"] = "power_factor";
  doc["frc_upd"] = true;

  //doc["val_tpl"] = "{{ value_json.derivation|default(0) }}";

  n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), (uint8_t *)buffer, n), true;


  discoveryTopic = "homeassistant/sensor/" + String(derivator_id) + "/rssi/config";
  doc["name"] = "RSSI";
  doc["state_topic"] = mqtt_rssiTopic;
  doc["ic"] = "mdi:information-outline",
  doc["unit_of_meas"] = "dBm";
  doc["uniq_id"] = String(derivator_id) + "_rssi";
  doc["dev_cla"] = "signal_strength";
  doc["frc_upd"] = true;

  n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), (uint8_t *)buffer, n, true);

  discoveryTopic = "homeassistant/switch/" + String(derivator_id) + "/switch_1/config";
  doc["name"] = "Passthrought";
  doc["state_topic"] = mqtt_btnPassthroughtTopic_status;  
  doc["cmd_t"] = mqtt_btnPassthroughtTopic;
  doc["uniq_id"] = String(derivator_id) + "_passthr";
  doc["pl_on"] = "ON";
  doc["pl_off"] = "OFF";
  doc["dev_cla"] = "switch";
  doc["frc_upd"] = true;


  n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), (uint8_t *)buffer, n, true);

  discoveryTopic = "homeassistant/sensor/" + String(derivator_id) + "/mqtt_rec/config";
  doc["name"] = "MQTT Reconnect";
  doc["state_topic"] = mqtt_reconnectTopic;
  doc["ic"] = "mdi:information-outline",
  doc["unit_of_meas"] = "";
  doc["uniq_id"] = String(derivator_id) + "_mqtt_rec";
  doc["dev_cla"] = "signal_strength";
  doc["frc_upd"] = true;

  n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), (uint8_t *)buffer, n, true);  


  discoveryTopic = "homeassistant/sensor/" + String(derivator_id) + "/wifi_disc/config";
  doc["name"] = "Wifi Disconnections";
  doc["state_topic"] = wifi_disconnectTopic;
  doc["ic"] = "mdi:information-outline",  
  doc["unit_of_meas"] = "";
  doc["uniq_id"] = String(derivator_id) + "_wifi_dis";
  doc["dev_cla"] = "signal_strength";
  doc["frc_upd"] = true;

  n = serializeJson(doc, buffer);
  client.publish(discoveryTopic.c_str(), (uint8_t *)buffer, n, true);    

}
