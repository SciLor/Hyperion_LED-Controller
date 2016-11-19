#include "Config.h"

ConfigStruct Config::_cfgStruct;
boolean Config::_cfgLoaded = false;

void Config::saveConfig() {
  EEPROM.begin(sizeof(ConfigStruct));
  EEPROM.put(CONFIG_START_ADDRESS, _cfgStruct);
  _cfgStruct.version = CONFIG_ACTIVE_VERSION;
  Log.info("Configuration saved at 0x%x with v%i", CONFIG_START_ADDRESS, _cfgStruct.version);
  //EEPROM.commit(); (done with end())
  EEPROM.end();
  _cfgLoaded = false;
}

void Config::initConfig(void) {
  if (!_cfgLoaded) {
    EEPROM.begin(sizeof(ConfigStruct));
    uint8_t version = EEPROM.read(CONFIG_START_ADDRESS);
    if (version == CONFIG_ACTIVE_VERSION) {
      EEPROM.get(CONFIG_START_ADDRESS, _cfgStruct);
      EEPROM.end();
      Log.info("Configuration read at 0x%x with v%i", CONFIG_START_ADDRESS, version);
    } else {
      //init config
      _cfgStruct.version = CONFIG_ACTIVE_VERSION;
      strncpy(_cfgStruct.wifi.hostname, "ESP8266", 32);
      _cfgStruct.ports.jsonServer = 19444;
      _cfgStruct.ports.udpLed = 19446;
      _cfgStruct.led.timeoutMs = 5000;
      _cfgStruct.led.autoswitch = true;
      EEPROM.end();
      saveConfig();
      Log.info("Configuration at 0x%x with v%i (v%i expected), new configuration created", CONFIG_START_ADDRESS, version, CONFIG_ACTIVE_VERSION);
    }
    _cfgLoaded = true;
  }
}

ConfigStruct *Config::getConfig(void) {
  initConfig();
  return &_cfgStruct;
}

void Config::loadStaticConfig(void) {
  Log.info("CFG=%s", "loadStaticConfig initConfig");
  initConfig();
  strlcpy(_cfgStruct.wifi.ssid, CONFIG_WIFI_SSID, sizeof(_cfgStruct.wifi.ssid));
  strlcpy(_cfgStruct.wifi.password, CONFIG_WIFI_PASSWORD, sizeof(_cfgStruct.wifi.password));
  strlcpy(_cfgStruct.wifi.hostname, CONFIG_WIFI_HOSTNAME, sizeof(_cfgStruct.wifi.hostname));

  #ifdef CONFIG_WIFI_STATIC_IP
    _cfgStruct.wifi.ip = ip2cfg(CONFIG_WIFI_IP);
    _cfgStruct.wifi.subnet = ip2cfg(CONFIG_WIFI_SUBNET);
    _cfgStruct.wifi.dns = ip2cfg(CONFIG_WIFI_DNS);
  #else
    _cfgStruct.wifi.ip.a = 0;
    _cfgStruct.wifi.ip.b = 0;
    _cfgStruct.wifi.ip.c = 0;
    _cfgStruct.wifi.ip.d = 0;
    
    _cfgStruct.wifi.subnet.a = 0;
    _cfgStruct.wifi.subnet.b = 0;
    _cfgStruct.wifi.subnet.c = 0;
    _cfgStruct.wifi.subnet.d = 0;
    
    _cfgStruct.wifi.dns.a = 0;
    _cfgStruct.wifi.dns.b = 0;
    _cfgStruct.wifi.dns.c = 0;
    _cfgStruct.wifi.dns.d = 0;
  #endif
  _cfgStruct.led.idleMode = CONFIG_LED_STANDARD_MODE;
  _cfgStruct.led.timeoutMs = CONFIG_LED_STANDARD_MODE_TIMEOUT_MS; 
  _cfgStruct.led.autoswitch = CONFIG_LED_HYPERION_AUTOSWITCH;

  _cfgStruct.ports.jsonServer = CONFIG_PORT_JSON_SERVER;
  _cfgStruct.ports.udpLed = CONFIG_PORT_UDP_LED;

  saveConfig();
  Log.info("CFG=%s", "loadStaticConfig END");
}

void Config::logConfig(void) {
  initConfig();
  Log.debug("CFG Show Config");
  
  Log.debug("+WIFI+");
  Log.debug("  ssid=%s", _cfgStruct.wifi.ssid);
  Log.debug("  password=%s", _cfgStruct.wifi.password);
  Log.debug("  ip=%i.%i.%i.%i", _cfgStruct.wifi.ip.a, _cfgStruct.wifi.ip.b, _cfgStruct.wifi.ip.c, _cfgStruct.wifi.ip.d);
  Log.debug("  subnet=%i.%i.%i.%i", _cfgStruct.wifi.subnet.a, _cfgStruct.wifi.subnet.b, _cfgStruct.wifi.subnet.c, _cfgStruct.wifi.subnet.d);
  Log.debug("  dns=%i.%i.%i.%i", _cfgStruct.wifi.dns.a, _cfgStruct.wifi.dns.b, _cfgStruct.wifi.dns.c, _cfgStruct.wifi.dns.d);
  Log.debug("  hostname=%s", _cfgStruct.wifi.hostname);

  Log.debug("+LED+");
  Log.debug("  idleMode=%i", _cfgStruct.led.idleMode);
  Log.debug("  timeoutMs=%i", _cfgStruct.led.timeoutMs);
  Log.debug("  autoswitch=%i", _cfgStruct.led.autoswitch);

  Log.debug("+PORTS+");
  Log.debug("  jsonServer=%i", _cfgStruct.ports.jsonServer);
  Log.debug("  udpLed=%i", _cfgStruct.ports.udpLed);
  
}

byte *Config::cfg2ip(ConfigIP ipStruct) {
  Log.verbose("CFG=cfg2ip: %i.%i.%i.%i", ipStruct.a, ipStruct.b, ipStruct.c, ipStruct.d);
  byte *ipByte = new byte[4];
  ipByte[0] = ipStruct.a;
  ipByte[1] = ipStruct.b;
  ipByte[2] = ipStruct.c;
  ipByte[3] = ipStruct.d;
  //byte ipByte[] = { ipStruct.a, ipStruct.b, ipStruct.c, ipStruct.d };
  return ipByte;
}

ConfigIP Config::ip2cfg(const byte ip[4]) {
  Log.verbose("CFG=ip2cfg: %i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
  ConfigIP cfgIp;
  cfgIp.a = ip[0];
  cfgIp.b = ip[1];
  cfgIp.c = ip[2];
  cfgIp.d = ip[3];
  return cfgIp;
}

