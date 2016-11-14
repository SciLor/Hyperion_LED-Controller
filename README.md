# ESP8266 Hyperion LED Controller

This code allows you to use a ESP8266 with a fitting led strip as extension for hyperion (ambilight clone).
You need to configure hyperion to stream the leds as UDP to the esp.

German Tutorial:
http://www.forum-raspberrypi.de/Thread-hyperion-tutorial-esp8266-nodemcu-addon-wifi-led-controller-udp

Tested with following following libraries (other versions may work):
#IDE
a) Arduino IDE 1.6.12

#Board Library
a) esp8266 2.3.0 http://arduino.esp8266.com/stable/package_esp8266com_index.json - add to board urls


#Libraries
a) ArduinoThread 2.1.0

b) ArduinoJSON 5.7.0

c) LinkedList 1.2.3

d) FastLED 3.1.3

e) Logging https://github.com/SciLor/Arduino-logging-library - install manually