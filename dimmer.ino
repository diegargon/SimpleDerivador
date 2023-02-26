
void init_dimmer() {
  Serial.println();
  Serial.print("Initializing dimmer... ");
  DimmableLightLinearized::setSyncPin(syncPin);
  DimmableLightLinearized::begin();
  Serial.println("Done!");
}