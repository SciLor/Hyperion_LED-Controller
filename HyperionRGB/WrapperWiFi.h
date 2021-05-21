#ifndef WrapperWiFi_h
#define WrapperWiFi_h

#include "BaseHeader.h"

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <ESPmDNS.h>
#endif

class WrapperWiFi {
  public:
    WrapperWiFi() {};
    WrapperWiFi(const char* ssid, const char* password, const char* hostname);
    WrapperWiFi(const char* ssid, const char* password, const byte ip[4], const byte subnet[4], const byte dns[4], const char* hostname);
    void begin(void);
    bool isAP(void);
    bool isAPConnected(void);
  private:
    bool connect(void);
    const char* _ssid;
    const char* _password;
    const char* _hostname;
    byte _ip[4];
    byte _subnet[4];
    byte _dns[4];
    bool _apMode;
};

#endif
