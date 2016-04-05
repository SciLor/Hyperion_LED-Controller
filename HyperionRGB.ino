
#include <Thread.h>
#include <ThreadController.h>
#include "BaseHeader.h"

#include "EnhancedThread.h"

#include "LoggerInit.h"

#include "WrapperWiFi.h"
#include "WrapperOTA.h"
#include "WrapperFastLed.h"
#include "WrapperUdpLed.h"
#include "WrapperJsonServer.h"

#ifdef CONFIG_TYPE_WEBCONFIG
  #include "WrapperWebconfig.h"
#endif

#define LED D0 // LED in NodeMCU at pin GPIO16 (D0).
int ledState = LOW;

LoggerInit loggerInit;

WrapperWiFi wifi;
WrapperOTA ota;

WrapperFastLed ledStrip;

WrapperUdpLed udpLed;
WrapperJsonServer jsonServer;

#ifdef CONFIG_TYPE_WEBCONFIG
  WrapperWebconfig webServer;
#endif

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
  #ifdef CONFIG_TYPE_WEBCONFIG
    webServer.handle();
  #endif

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

  #ifdef CONFIG_TYPE_WEBCONFIG
    wifi = WrapperWiFi(Config::getConfig().wifi.ssid, Config::getConfig().wifi.password); //TODO Fallback
    udpLed = WrapperUdpLed(Config::getConfig().led.count, Config::getConfig().ports.udpLed);
    jsonServer = WrapperJsonServer(Config::getConfig().led.count, Config::getConfig().ports.jsonServer);
    webServer = WrapperWebconfig();
  #elif CONFIG_TYPE_STATIC_CONFIG
    wifi = WrapperWiFi(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    udpLed = WrapperUdpLed(CONFIG_LED_COUNT, CONFIG_PORT_UDP_LED);
    jsonServer = WrapperJsonServer(CONFIG_LED_COUNT, CONFIG_PORT_JSON_SERVER);
  #endif
  
  ota = WrapperOTA();
  ledStrip = WrapperFastLed();
  
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
  
  #ifdef CONFIG_TYPE_WEBCONFIG
    webServer.begin();
    ota.begin(Config::getConfig().wifi.hostname);  
    uint8_t chipset = Config::getConfig().led.chipset;
    uint8_t colorOrder = Config::getConfig().led.colorOrder;
    uint16_t ledCount = Config::getConfig().led.count;
    ledStrip.begin(chipset, Config::getConfig().led.dataPin, Config::getConfig().led.clockPin, colorOrder, ledCount);
  #elif CONFIG_TYPE_STATIC_CONFIG
    ota.begin(CONFIG_WIFI_HOSTNAME); 
    ledStrip.begin();
  #endif
    
  udpLed.begin();
  udpLed.onUpdateLed(updateLed);
  udpLed.onRefreshLeds(refreshLeds);

  jsonServer.begin();
  jsonServer.onLedColorWipe(ledColorWipe);
  jsonServer.onClearCmd(resetMode);

  pinMode(LED, OUTPUT);   // LED pin as output.

  Log.info("HEAP=%i", ESP.getFreeHeap());
}
