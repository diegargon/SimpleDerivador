# SimpleDerivador

**Español**

Derivador excedentes de energia solar, para placas chinas.

Necesita recibir la informacion de energia del grid por MQTT. No soporta la lectura de  inversores u otros.
Puede recibir tambien informacion de la produccion solar pero solo se utiliza para mostrar en la pantalla

Nota: Este programa al contrario que 'freeds' no recolecta datos.

**Bad English**

Diverter surplus solar energy, for Chinese Dimmer board

Need receive the grid power info from MQTT. Not support reading from solar inverters or others.
Can receive solar production but only is used to display.

Note: Unlike freeds this program not recolect your data.

## How Works

"SimpleDerivador" directs the surplus solar production to a resistive load, commonly a water heater or an electric stove.

It needs an input via MQTT with the information about electricity consumption (for example, taken with Shelby EM) and 
interprets a negative value as we are injecting electricity into the grid. When that occurs will begin to directs that excess 
to the resistive load.

## Current Status
  * Works

## Breaking changes
  * (240520) None

## Last Relevant Changes
  * (240520) Passthrought mode (HA button to force derivation)
  * (240520) Rewrite many things
  * (240520) Improve HA auto-discovery
  * (230000) Publish derivation to MQTT 
  * (230000) Alt Wifi
  * (230000) Preliminary HA auto-discovery implementation
  * (230000) Many

## USAGE

Edit SimpleDerivador.ino , the configuration options (Between Configuration comment) and burn to ESP32 with Arduino .

Connect the dimmer pins to 5v and GND to the ESP32 5v and GND pins, and Z/C and PSM to the 
corresponding pins (SimpleDerivador)

## NEXT FEATURES
  * Rewrite, clean the code and redesign, need that before for get the power values directly from a Shelly device. 

## MID/LONG TERM FEATURES

  * Energy ingest directly from Shelly.
  * Relay supports
  * MASTER/SLAVES mode
  * web interface (very basic interface)
  * Optional ESP32 Power Meter option (using a clamp)

## FAQ/Q&A

Will you add support for read data directly from X solar inverter? I will not add, and probably if someone want add 
first we need some code cleanup and rewrite. 

Will you add support for read data directly from a shelly device? Yes. Need a rewrite for make MQTT optional but yes i planning add it.


## Testing Software:

Arduino 2.0.3

  * Mosquitto MQTT Server
  * Home Assistant 2023/2024 (Optional)

## Testing Hardware:

  * Shelly EM sending MQTT 
  * Dimmer HL 24A 600V - 14€ (Aliexpress)
  * ESP32 with OLED (htit-wb32) - 14€ - Aliexpress (mst work with any esp32 with or without U8g2 screen)
  * 1500w/100L Electric Water Heater
  * 1000w Electric Stove

## Librarys dependencies:

  * ArduinoSTL - Basic by Mike Matera
  * U8g2 for Screen by oliver
  * PubSubClient for MQTT by Nick O'Leary
  * Dimmable Light for Arduino by Fabiano Riccardi
  * ArduinoJson by Benoit Blanchon

## Electric Water Heater Stats

I don't currently have much excess energy. If the sun is good, peaks of 800W, but mostly between 300W-500W. The water heater has a capacity of 100L, 
and with one shower a day, as long as there are no cloudy days, it maintains (derives about 3kW daily). With two showers, it falls short, so I probably 
need around 5kW daily. For winter, I definitely need more solar panels. 

I also use HA+PVPC automatation to force the heater to turn on for 3 hours during  the lowest electricity tariff when is necessary (cloudy days or 
two showers in a day).
