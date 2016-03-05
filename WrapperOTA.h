#ifndef WrapperOTA_h
#define WrapperOTA_h

#include "BaseHeader.h"
#include <ArduinoOTA.h>

class WrapperOTA {
  public:
    WrapperOTA(const char* hostname);
    void
      begin(void),
      handle(void);
  private:
    const char* _hostname;
};

#endif
