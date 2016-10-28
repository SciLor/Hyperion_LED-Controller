#ifndef Config_h
#define Config_h

#include "BaseHeader.h"

#include <EEPROM.h>

#include "ConfigStatic.h"
#include "ConfigStructures.h"

class Config {
  public:
    static ConfigStruct getConfig(void);
    static void saveConfig(ConfigStruct cfg);
    static void loadStaticConfig(void);
    static byte* cfg2ip(ConfigIP ip);
    static ConfigIP ip2cfg(const byte ip[4]);
  private:
    static void initConfig(void);
    static ConfigStruct _cfgStruct;
    static boolean _cfgLoaded;
};

#endif
