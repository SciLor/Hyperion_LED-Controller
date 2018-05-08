# Hyperion LED Controller for ESP8266/ESP32

This code allows you to use a ESP8266/ESP32 with a fitting led strip as extension for [hyperion](https://github.com/hyperion-project) (ambilight clone).
You need to configure hyperion to stream the leds as UDP to the esp.

English Tutorial: https://hyperion-project.org/threads/tutorial-wireless-led-extension-with-esp8266-esp32-for-hyperion.3004/

French Tutorial: https://ambimod.jimdo.com/2017/01/12/tuto-faire-de-l-ambilight-sans-fil-avec-un-esp8266-nodemcu-et-la-biblioth%C3%A8que-fastled/

German Tutorial: https://forum-raspberrypi.de/forum/thread/25242-tutorial-esp8266-nodemcu-addon-wifi-led-controller-udp/

Tested with following following libraries (other versions may work):
# IDE
a) Arduino IDE 1.8.5

# Board Library
a) esp8266 2.4.1 http://arduino.esp8266.com/stable/package_esp8266com_index.json

b) for esp32 https://github.com/espressif/arduino-esp32

# Libraries
a) ArduinoThread 2.1.1

b) ArduinoJSON 5.12.0

c) LinkedList 1.2.3

d) FastLED 3.1.6

e) Logging https://github.com/SciLor/Arduino-logging-library - install manually: Download zip from github and install via Arduino IDE, Sketch -> Include Library -> Add .ZIP Library

f) ESP32 Webserver https://github.com/nhatuan84/esp32-webserver - install manually (for esp32 only)

# Installation

# Configuration of the board
1. Go to the `HyperionRGB` folder and create a copy of `ConfigStatic.h.example`. Remove the `.example` suffix
2. Configure the `ConfigStatic.h` for your needs:
   - Select your LED chip type. All LEDs of the [FastLed](https://github.com/FastLED/FastLED) libraries are supported
   - Configure the used LED pins. You can also change the Pin Order. The NodeMCU order doesn't work sometimes to please also try the `RAW_PIN_ORDER``
   - Define the number of used LEDs
   - Define one of the standard modes which are active when your light is idle. Choose one from: OFF, HYPERION_UDP, STATIC_COLOR, RAINBOW, FIRE2012
   - You maydefine Wifi configuration but you can also change it from the Webinterface
3. Open the `HyperionRGB.ino` the Arduino IDE
4. Compile and upload to your board

# Configuration of Hyperion
You need two running hyperion instances. The first grabs the data on e.g. a rasbperry pi and controls any local attached LED strips. This first instance is configured to forward its data to a second hyperion instance on the same machine. Be sure to only forward the UDP data:

```
"forwarder" :
{
"proto" : [ "localhost:19447" ]
},
```

The second hyperion instance is configured to use UDP as device so that it can talk to the ESP directly. This second hyperion instance can run on the same machine as the first instance. Just make sure that you set the UDP ports, hostnames/IPs and LED number accordingly to the values you've configured for the ESP.

```
{
      "colorOrder" : "rgb",
      "maxpacket" : 1450,
      "name" : "AmbiSZ-ESP8266",
      "output" : "ESP8266:19446", ///
      "protocol" : 0,
      "rate" : 250000,
      "type" : "udp"
},


"protoServer" : 
{
"port" : 19447
},
"jsonServer" : 
{
"port" : 19446
},
```

There's a detailed instruction page for [controlling multiple devices](https://hyperion-project.org/wiki/Controlling-Multiple-Devices).

If you like my work spread the word!
Donation: http://www.scilor.com/donate.html
