#ifndef Config_h
#define Config_h

#include "BaseHeader.h"

#include <EEPROM.h>

#include "ConfigStatic.h"

#ifdef CONFIG_TYPE_WEBCONFIG
  #include "ConfigStructures.h"
#endif

enum Mode { RAINBOW, STATIC_COLOR, AMBILIGHT, OFF };

class Config {
  #ifdef CONFIG_TYPE_WEBCONFIG
    public:
      static ConfigStruct getConfig(void);
      static void saveConfig(ConfigStruct cfg);
    private:
      static ConfigStruct _cfgStruct;
      static boolean _cfgLoaded;
  #endif
};

#endif
