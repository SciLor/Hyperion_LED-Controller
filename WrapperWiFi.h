#ifndef WrapperWiFi_h
#define WrapperWiFi_h

#include "BaseHeader.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

class WrapperWiFi {
  public:
    WrapperWiFi(const char* ssid, const char* password);
    void begin(void);
  private:
    const char* _ssid;
    const char* _password;
};

#endif
