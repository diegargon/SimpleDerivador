# SimpleDerivador

**Español**

Derivador excedentes de energia solar, más simple que freeds, para placas chinas.

Necesita recibir la informacion de energia del grid por MQTT. No soporta la lectura de  inversores u otros.
Puede recibir tambien informacion de la produccion solar pero solo se utiliza para mostrar en la pantalla

Nota: Este programa al contrario que freeds no recolecta datos sin tu consentimiento

**Bad English**

Diverter surplus solar energy, simple than freeds, for Chinese Dimmer board

Need receive the grid power info from MQTT. Not support reading from solar inverters or others.
Can receive solar production but only is used to display.

Note: Unlike freeds this program not recolect your data without consent.

## How Works

"SimpleDerivador" directs the surplus solar production to a resistive load, commonly a heater or an electric stove.

It needs an input via MQTT with the information about electricity consumption (for example, taken with Shelby EM) and 
interprets a negative value as injecting electricity into the grid. Then, it starts and directs that excess to the resistive load.

## Current Status
  * Works

## Last Changes
  * Passthrought mode (HA button to force derivation)
  * Rewrite many things
  * Improve HA auto-discovery
  * Preliminary HA auto-discovery implementation
  * Publish derivation to MQTT 
  * ALT Wifi
  * Many

## USAGE

Edit simple_derivation.h the configuration options and burn to ESP32 with Arduino .

Connect the dimmer pins to 5v and GND to the ESP32 5v and GND pins, and Z/C and PSM to the 
corresponding pins (SimpleDerivador)

## NEXT FEATURES
  * Rewrite & Clean and redesign

## LONG TERM FEATURES
  * MASTER/SLAVES mode
  * Relay supports
  * web interface (very basic interface)
  * Power Meter (using a clamp)

## Q&A

Will you add support for read data directly from X inverter? probably not (i will not add), 

Will you add support for read data directly from a shelly device? perhaps, not a priority


## Testing Software:

Arduino 2.0.3

  * Mosquitto MQTT Server
  * Home Assistant 2023 (Optional)

## Testing Hardware (Aliexpress):

  * Shelby EM sending MQTT 
  * Dimmer HL 24A 600V - 14€
  * ESP32 with OLED (htit-wb32) - 14€ (Will work with any esp32 with or without U8g2 screen)

## Librarys dependencies:

  * ArduinoSTL - Basic by Mike Matera
  * U8g2 for Screen by oliver
  * PubSubClient for MQTT by Nick O'Leary
  * Dimmable Light for Arduino by Fabiano Riccardi
  * ArduinoJson by Benoit Blanchon
