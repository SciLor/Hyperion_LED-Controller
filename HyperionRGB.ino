
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

#include "WrapperWebconfig.h"

#define LED D0 // LED in NodeMCU at pin GPIO16 (D0).
int ledState = LOW;

LoggerInit loggerInit;

WrapperWiFi wifi;
WrapperOTA ota;

WrapperFastLed ledStrip;

WrapperUdpLed udpLed;
WrapperJsonServer jsonServer;

#ifdef CONFIG_ENABLE_WEBCONFIG
  WrapperWebconfig webServer;
#endif

Mode activeMode;

ThreadController threadController = ThreadController();
Thread statusThread = Thread();
EnhancedThread animationThread = EnhancedThread();
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

void animationStep() {
  switch (activeMode) {
    case RAINBOW:
      ledStrip.rainbowStep();
      break;
    case FIRE2012:
      ledStrip.fire2012Step();
      break;
  }
}

void changeMode(Mode newMode) {
  if (newMode != activeMode) {
    Log.info("Mode changed to %i", newMode);
    activeMode = newMode;

    switch (activeMode) {
      case RAINBOW:
        animationThread.setInterval(500);
        break;
      case FIRE2012:
        animationThread.setInterval(16);
        break;
    }
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
  Log.info("Reset Mode");
  changeMode(CONFIG_LED_STANDARD_MODE);
  resetThread.enabled = false;
}

void initConfig(void) {
  #if defined(CONFIG_OVERWRITE_WEBCONFIG) && defined(CONFIG_ENABLE_WEBCONFIG)
    Config::loadStaticConfig();
  #endif

  const char* ssid;
  const char* password;
  const byte* ip;
  const byte* subnet;
  const byte* dns;
  uint16_t jsonServerPort;
  uint16_t udpLedPort;

  #ifdef CONFIG_ENABLE_WEBCONFIG
    //TODO Fallback
    ConfigStruct* cfg = Config::getConfig();
    
    ssid = cfg->wifi.ssid;
    password = cfg->wifi.password;
    ip = Config::cfg2ip(cfg->wifi.ip);
    subnet = Config::cfg2ip(cfg->wifi.subnet);
    dns = Config::cfg2ip(cfg->wifi.dns);
    jsonServerPort = cfg->ports.jsonServer;
    udpLedPort = cfg->ports.udpLed;
    
    Log.info("CFG=%s", "EEPROM config loaded");
    Config::logConfig();
  #else
    ssid = CONFIG_WIFI_SSID;
    password = CONFIG_WIFI_PASSWORD;
    #ifdef CONFIG_WIFI_STATIC_IP
      ip = CONFIG_WIFI_IP;
      subnet = CONFIG_WIFI_SUBNET;
      dns = CONFIG_WIFI_DNS;
    #else
      const byte empty[4] = {0};
      ip = empty;
    #endif
    jsonServerPort = CONFIG_PORT_JSON_SERVER;
    udpLedPort = CONFIG_PORT_UDP_LED;
    
    Log.info("CFG=%s", "Static config loaded");
  #endif
  
  wifi = WrapperWiFi(ssid, password, ip, subnet, dns);
  udpLed = WrapperUdpLed(CONFIG_LED_COUNT, udpLedPort);
  jsonServer = WrapperJsonServer(CONFIG_LED_COUNT, jsonServerPort);
}

void handleEvents(void) {
  ota.handle();
  udpLed.handle();
  jsonServer.handle();
  #ifdef CONFIG_ENABLE_WEBCONFIG
    webServer.handle();
  #endif

  threadController.run();
}

void setup(void) {
  LoggerInit loggerInit = LoggerInit(115200);
  
  initConfig();
  ota = WrapperOTA();
  ledStrip = WrapperFastLed();

  statusThread.onRun(statusInfo);
  statusThread.setInterval(5000);
  threadController.add(&statusThread);

  animationThread.onRun(animationStep);
  animationThread.setInterval(500);
  
  resetThread.onRun(resetMode);
  resetThread.setInterval(5000);
  resetThread.enabled = false;
  threadController.add(&resetThread);
  
  ledStrip.begin();
  resetMode();
  animationStep();

  wifi.begin();

  #ifdef CONFIG_ENABLE_WEBCONFIG
    webServer = WrapperWebconfig();
    webServer.begin();
    ota.begin(Config::getConfig()->wifi.hostname);
  #else
    ota.begin(CONFIG_WIFI_HOSTNAME); 
  #endif
  
  udpLed.begin();
  udpLed.onUpdateLed(updateLed);
  udpLed.onRefreshLeds(refreshLeds);

  jsonServer.begin();
  jsonServer.onLedColorWipe(ledColorWipe);
  jsonServer.onClearCmd(resetMode);
  jsonServer.onEffectChange(changeMode);

  pinMode(LED, OUTPUT);   // LED pin as output.
  Log.info("HEAP=%i", ESP.getFreeHeap());
}

void loop(void) {
  handleEvents();
  switch (activeMode) {
    case RAINBOW:
    case FIRE2012:
      animationThread.runIfNeeded();
      break;
    case STATIC_COLOR:
      break;
    case AMBILIGHT:
      break;
  }
}
