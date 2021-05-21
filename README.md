# Hyperion LED Controller for ESP8266/ESP32

This code allows you to use a ESP8266/ESP32 with a fitting led strip as extension for [hyperion](https://github.com/hyperion-project) (ambilight clone).
You need to configure hyperion to stream the leds as UDP to the esp. 

English Tutorial: https://hyperion-project.org/threads/tutorial-wireless-led-extension-with-esp8266-esp32-for-hyperion.3004/

French Tutorial: https://ambimod.jimdo.com/2017/01/12/tuto-faire-de-l-ambilight-sans-fil-avec-un-esp8266-nodemcu-et-la-biblioth%C3%A8que-fastled/

German Tutorial: https://forum-raspberrypi.de/forum/thread/25242-tutorial-esp8266-nodemcu-addon-wifi-led-controller-udp/


It will also work with enigmalight.

Tested with following following libraries (other versions may work):
## IDE
a) Arduino IDE 1.8.13

## Board Library
a) esp8266 2.7.4 http://arduino.esp8266.com/stable/package_esp8266com_index.json

b) for esp32 https://github.com/espressif/arduino-esp32

# Libraries
a) ArduinoThread 2.1.1

b) ArduinoJSON 6.18.0

c) LinkedList 1.3.2

d) FastLED 3.4.0

e) Logging https://github.com/SciLor/Arduino-logging-library - install manually: Download zip from github and install via Arduino IDE, Sketch -> Include Library -> Add .ZIP Library

f) ESP32 Webserver https://github.com/nhatuan84/esp32-webserver - install manually (for esp32 only)

## Installation

### Configuration of the board
Attention, enabling Logging can slowdown the leds responsiveness!

#### Basic
1. Go to the `HyperionRGB` folder and create a copy of `ConfigStatic.h.example`. Remove the `.example` suffix
2. Configure the `ConfigStatic.h` for your needs:
   - Select your LED chip type. All LEDs of the [FastLed](https://github.com/FastLED/FastLED) libraries are supported
   - Configure the used LED pins. You can also change the Pin Order. The NodeMCU order doesn't work sometimes to please also try the `RAW_PIN_ORDER``
   - Define the number of used LEDs
   - Define one of the standard modes which are active when your light is idle. Choose one from: OFF, HYPERION_UDP, STATIC_COLOR, RAINBOW, FIRE2012
   - You maydefine Wifi configuration but you can also change it from the Webinterface
3. Open the `HyperionRGB.ino` the Arduino IDE
4. Compile and upload to your board

#### AP fallback
HyperionRGB will try to connected to the saved WiFi for 3 times and then falls back into AP Mode for 60 seconds. It will open a new WiFi network with the standard name "HyperionRGB" and password "HyperionRGB"- The SSID/Password may be overriden via the static config. (hostname/password)

#### UDP Protocol
This section describes the different UDP Protocol formats.
##### RAW - Protocol 0 - Raw Format
Standard format, will expect the leds data via udp as a block
3 bytes per LED, Full LED set
```
[LED1: {0: R, 1: G, 2: B}, LED2: ...]
```
##### FRAGMENT - Protocol 2 - Fragmented Format 
One byte per block:

0. Update ID & 0xF
1. Fragment ?!
2. First LED ID, high byte
3. First LED ID, low byte
4. [LED1: {0: R, 1: G, 2: B}, LED2: ...]

##### TPM2 - Protocol 3 - TPM2.net
0. 0x9C //magic byte
1. 0xDA //data frame
2. Data length (LED count * 3), high byte
3. Data length (LED count * 3), low byte
4. Fragment ID
5. Fragment ID maximum
6. {0: R, 1: G, 2: B}
7. 0x36

### Configuration of Hyperion
#### Standalone
If you just want a single wireless LED-Stripe you can just configure hyperion as always, except you need to set the fitting device as following:
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
```

#### As second instance
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
# For Hyperion NG throug web interface

Go to Confiruration -> General, select Controller type: **udpraw** set Target IP (your ESP IP addr) and Port (**19446**).


### Configuration for Engimalight
Please use UDP Protocol "UDP_RAW" in ConfigStatic.h for the ESP part.
You may need to change the output to the IP of your ESP! The prefix doesn't matter as the ESP should ignore it (Untested).

enigmalight.conf
```
[device]
name		ambilight
output		192.168.1.123
type		udp
port		19446
interval  	20000
prefix		41 64 61 00 71 24
channels	100
rate		115200
debug		off
```
Source: https://www.vuplus-support.org/wbb4/index.php?thread/107895-enigmalight-ambilight-f%C3%BCr-4k-boxen-arm/&postID=1661992#post1661992

There's a detailed instruction page for [controlling multiple devices](https://hyperion-project.org/wiki/Controlling-Multiple-Devices).

If you like my work spread the word!
Donation: http://www.scilor.com/donate.html
