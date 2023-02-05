
void init_dimmer() {
  Serial.println();
  Serial.print("Initializing the dimmable light class... ");
  DimmableLightLinearized::setSyncPin(syncPin);
  DimmableLightLinearized::begin();
  Serial.println("Done!");
}