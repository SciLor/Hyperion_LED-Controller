#include "Config.h"

Config::Config() {
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
    EEPROM.end();
    saveConfig(_cfgStruct);
    Log.info("Configuration at 0x%x with v%i (v%i expected), new configuration created", CONFIG_START_ADDRESS, version, CONFIG_ACTIVE_VERSION);
  }
}
ConfigStruct Config::saveConfig(ConfigStruct cfg) {
  EEPROM.begin(sizeof(ConfigStruct));
  EEPROM.put(CONFIG_START_ADDRESS, cfg);
  Log.info("Configuration saved at 0x%x with v%i", CONFIG_START_ADDRESS, cfg.version);
  //EEPROM.commit(); (done with end())
  EEPROM.end();
}

ConfigStruct Config::getConfig() {
  return Config()._cfgStruct; //Workaround
}

