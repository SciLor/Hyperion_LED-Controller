# ESP8266 Hyperion LED Controller

This code allows you to use a ESP8266 with a fitting led strip as extension for [hyperion](https://github.com/hyperion-project) (ambilight clone).
You need to configure hyperion to stream the leds as UDP to the esp.

German Tutorial:
https://forum-raspberrypi.de/forum/thread/25242-tutorial-esp8266-nodemcu-addon-wifi-led-controller-udp/

Tested with following following libraries (other versions may work):
# IDE
a) Arduino IDE 1.6.12, 1.8.5

# Board Library
a) esp8266 2.3.0 http://arduino.esp8266.com/stable/package_esp8266com_index.json - add to board urls

# Libraries
a) ArduinoThread 2.1.0, 2.1.1
b) ArduinoJSON 5.7.0, 5.12.0
c) LinkedList 1.2.3
d) FastLED 3.1.3
e) Logging https://github.com/SciLor/Arduino-logging-library - install manually: Download zip from github and install via Arduino IDE, Sketch -> Include Library -> Add .ZIP Library

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
