
#include <Thread.h>
#include <ThreadController.h>
#include "BaseHeader.h"

#include "EnhancedThread.h"

#include "LoggerInit.h"

#include "WrapperWiFi.h"
#include "WrapperOTA.h"
#include "WrapperLedControl.h"
#include "WrapperUdpLed.h"
#include "WrapperJsonServer.h"

#include "WrapperWebconfig.h"

#define LED LED_BUILTIN // LED in NodeMCU at pin GPIO16 (D0) or LED_BUILTIN @Lolin32.
int ledState = LOW;

LoggerInit loggerInit;

WrapperWiFi wifi;
WrapperOTA ota;

WrapperLedControl ledStrip;

WrapperUdpLed udpLed;
WrapperJsonServer jsonServer;

#ifdef CONFIG_ENABLE_WEBCONFIG
  WrapperWebconfig webServer;
#endif

Mode activeMode;
boolean autoswitch;

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

void changeMode(Mode newMode, int interval = 0) {
  if (newMode != activeMode) {
    Log.info("Mode changed to %i", newMode);
    activeMode = newMode;
    if (!autoswitch)
      udpLed.stop();
    
    switch (activeMode) {
      case OFF:
        ledStrip.clear();
        ledStrip.show();
        break;
      case STATIC_COLOR:
        break;
      case RAINBOW:
        if (interval == 0)
          interval = 500;
        animationThread.setInterval(interval);
        break;
      case FIRE2012:
        if (interval == 0)
          interval = 16;
        animationThread.setInterval(interval);
        break;
      case HYPERION_UDP:
        if (!autoswitch)
          udpLed.begin();
    }
    if (interval > 0)
      Log.debug("Interval set to %ims", interval);
  }
}

void updateLed(int id, byte r, byte g, byte b) {
  if (activeMode == HYPERION_UDP) {
    Log.verbose("LED %i, r=%i, g=%i, b=%i", id + 1, r, g, b);
    ledStrip.leds[id].setRGB(r, g, b);
  }
}
void refreshLeds(void) {
  if (activeMode == HYPERION_UDP) {
    Log.debug("refresh LEDs");
    ledStrip.show();
    if (autoswitch)
      resetThread.reset();
  } else if (autoswitch) {
    changeMode(HYPERION_UDP);
    Log.info("Autoswitch to HYPERION_UDP");
  }
}

void ledColorWipe(byte r, byte g, byte b) {
  Log.debug("LED color wipe: r=%i, g=%i, b=%i", r, g, b);
  changeMode(STATIC_COLOR);
  ledStrip.fillSolid(r, g, b);
}
void resetMode(void) {
  Log.info("Reset Mode");
  #ifdef CONFIG_ENABLE_WEBCONFIG
    changeMode(static_cast<Mode>(Config::getConfig()->led.idleMode));
  #else
    changeMode(CONFIG_LED_STANDARD_MODE);
  #endif
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
    autoswitch = cfg->led.autoswitch;
    
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
    autoswitch = CONFIG_LED_HYPERION_AUTOSWITCH;
    
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
  ledStrip = WrapperLedControl();

  statusThread.onRun(statusInfo);
  statusThread.setInterval(5000);
  threadController.add(&statusThread);

  animationThread.onRun(animationStep);
  animationThread.setInterval(1000);
  
  resetThread.onRun(resetMode);
  #ifdef CONFIG_ENABLE_WEBCONFIG
    resetThread.setInterval(Config::getConfig()->led.timeoutMs);
  #else
    resetThread.setInterval(CONFIG_LED_STANDARD_MODE_TIMEOUT_MS);
  #endif
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

  if (autoswitch || activeMode == HYPERION_UDP)
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
    case HYPERION_UDP:
      break;
  }
}
