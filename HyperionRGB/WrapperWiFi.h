#ifndef WrapperWiFi_h
#define WrapperWiFi_h

#include "BaseHeader.h"
#include <WiFi.h>
#include <ESPmDNS.h>

class WrapperWiFi {
  public:
    WrapperWiFi() {};
    WrapperWiFi(const char* ssid, const char* password);
    WrapperWiFi(const char* ssid, const char* password, const byte ip[4], const byte subnet[4], const byte dns[4]);
    void begin(void);
  private:
    const char* _ssid;
    const char* _password;
    byte _ip[4];
    byte _subnet[4];
    byte _dns[4];
};

#endif
