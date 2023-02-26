# SimpleDerivador

**Español**

Derivador excedentes de energia solar, más simple que freeds, para placas chinas

Necesita recibir la energia del grid por MQTT. No soporta la lectura de  inversores u otros.

**English**

Diverter surplus solar energy, simpler than freeds, for Chinese Dimmer board

Need receive the grid power by MQTT. Not support reading from  solar inverters or others.


## Current Status
  * Initial/Beta .
  * Works for very basic usage, first write.

## Last Changes
  * Preliminary HA auto-discovery implementation
  * Publish derivation to MQTT 
  * ALT Wifi
  * Many

## USAGE

Edit SimpleDerivador.ino the configuration section and burn with Arduino.

## NEXT FEATURES
  * Rewrite

## LONG TERM FEATURES
  * MASTER/SLAVES mode
  * Relay supports
  * web interface (very basic interface)
  * Power Meter (using a clamp)

## Q&A

Will you add support for read data directly from inverter? probably not (i will not add), 

Will you add support for read data directly from a shelly device? perhaps, not a priority


## Testing Software:

Arduino 2.0.3
Mosquitto MQTT Server

## Testing Hardware (Aliexpress):

Shelby EM sending MQTT 

Dimmer HL 24A-600V - 14€

ESP32 with OLED (htit-wb32) - 14€

## Librarys dependencies:

ArduinoSTL - Basic by Mike Matera

U8g2 for Screen by oliver

PubSubClient for MQTT by Nick O'Leary

Dimmable Light for Arduino by Fabiano Riccardi

ArduinoJson by Benoit Blanchon