
#include <Thread.h>
#include <ThreadController.h>

#include "EnhancedThread.h"

#include "LoggerInit.h"

#include "WrapperWiFi.h"
#include "WrapperOTA.h"
#include "WrapperFastLed.h"
#include "WrapperUdpLed.h"
#include "WrapperJsonServer.h"
#include "WrapperWebconfig.h"

#include "BaseHeader.h"

#define LED D0 // LED in NodeMCU at pin GPIO16 (D0).
int ledState = LOW;

LoggerInit loggerInit;

WrapperWiFi wifi;
WrapperOTA ota;

WrapperFastLed ledStrip;

WrapperUdpLed udpLed;
WrapperJsonServer jsonServer;

WrapperWebconfig webServer;

Mode activeMode;

ThreadController threadController = ThreadController();
Thread statusThread = Thread();
EnhancedThread rainbowThread = EnhancedThread();
EnhancedThread resetThread = EnhancedThread();

void statusInfo(void) {
  if (ledState == LOW) {
    ledState = HIGH;
  } else {
    ledState = LOW;
    Log.debug("HEAP=%i", ESP.getFreeHeap());
  }
  digitalWrite(LED, ledState);
}

void rainbowStep() {
  ledStrip.rainbowStep();
}

void changeMode(Mode newMode) {
  activeMode = newMode;
}
void modeFallback(void) {
  changeMode(RAINBOW);
}

void handleEvents(void) {
  ota.handle();
  udpLed.handle();
  jsonServer.handle();
  webServer.handle();

  threadController.run();
}

void loop(void) {
  handleEvents();
  switch (activeMode) {
    case RAINBOW:
      rainbowThread.runIfNeeded();
      break;
    case STATIC_COLOR:
      break;
    case AMBILIGHT:
      break;
  }
}

void updateLed(int id, byte r, byte g, byte b) {
  Log.verbose("LED %i, r=%i, g=%i, b=%i", id + 1, r, g, b);
  ledStrip.leds[id].setRGB(r, g, b);
}
void refreshLeds(void) {
  Log.debug("refresh LEDs");
  ledStrip.show();
  changeMode(AMBILIGHT);
  resetThread.reset();
}

void ledColorWipe(byte r, byte g, byte b) {
  Log.debug("LED color wipe: r=%i, g=%i, b=%i", r, g, b);
  ledStrip.fillSolid(r, g, b);
  changeMode(STATIC_COLOR);
}
void resetMode(void) {
  Log.info("reset Mode");
  changeMode(RAINBOW);
  resetThread.enabled = false;
}

void setup(void) {
  LoggerInit loggerInit = LoggerInit(115200);
  
  wifi = WrapperWiFi(Config::ssid, Config::password);

  ota = WrapperOTA();
  ledStrip = WrapperFastLed();
  
  //udpLed = WrapperUdpLed(Config::getConfig().led.count, Config::getConfig().ports.udpLed);
  //jsonServer = WrapperJsonServer(Config::getConfig().led.count, Config::getConfig().ports.jsonServer);
  udpLed = WrapperUdpLed(Config::ledCount, Config::udpLedPort);
  jsonServer = WrapperJsonServer(Config::ledCount, Config::jsonServerPort);
  webServer = WrapperWebconfig();
   
  resetMode();

  statusThread.onRun(statusInfo);
  statusThread.setInterval(5000);
  threadController.add(&statusThread);

  rainbowThread.onRun(rainbowStep);
  rainbowThread.setInterval(500);

  resetThread.onRun(resetMode);
  resetThread.setInterval(5000);
  resetThread.enabled = false;
  threadController.add(&resetThread);

  wifi.begin();
  webServer.begin();
  ota.begin(Config::getConfig().wifi.hostname);
  //ledStrip.begin(Config::getConfig().led.chipset, Config::getConfig().led.dataPin, Config::getConfig().led.clockPin, Config::getConfig().led.colorOrder, Config::getConfig().led.count);
  ledStrip.begin(Config::chipset, Config::dataPin, Config::clockPin, Config::colorOrder, Config::ledCount);
  //ledStrip.begin(Config::chipset, Config::getConfig().led.dataPin, Config::getConfig().led.clockPin, Config::colorOrder, Config::getConfig().led.count);

  udpLed.begin();
  udpLed.onUpdateLed(updateLed);
  udpLed.onRefreshLeds(refreshLeds);

  jsonServer.begin();
  jsonServer.onLedColorWipe(ledColorWipe);
  jsonServer.onClearCmd(resetMode);

  pinMode(LED, OUTPUT);   // LED pin as output.

  Log.info("HEAP=%i", ESP.getFreeHeap());
}
