#include "dimmer.h"

void init_dimmer() {
  if(SERIAL_DEBUG_DIMMER) {
    Serial.print("Initializing dimmer... ");
  }
  DimmableLightLinearized::setSyncPin(syncPin);
  DimmableLightLinearized::begin();
  delay(20);
  SERIAL_DEBUG && Serial.println("Done!");
}

void setDimmerPower(int derivation) {
  SERIAL_DEBUG_DIMMER && Serial.printf("Derivation change to : %i\n", derivation);    
  //Receive 0-100 convert to 0 to 255  
  light.setBrightness(round((derivation * 255) / 100));
  
}