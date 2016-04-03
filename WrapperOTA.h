#ifndef WrapperOTA_h
#define WrapperOTA_h

#include "BaseHeader.h"
#include <ArduinoOTA.h>

class WrapperOTA {
  public:
    void
      begin(const char* hostname),
      handle(void);
};

#endif
