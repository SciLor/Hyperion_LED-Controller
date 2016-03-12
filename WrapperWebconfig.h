#ifndef WrapperWebconfig_h
#define WrapperWebconfig_h

#include "BaseHeader.h"
#include <ESP8266WebServer.h>

class WrapperWebconfig {
  public:
    void
      begin(),
      handle(void);
  private:
    void
      handleNotFound(void),
      handleRoot(void);
      
    String
      escape(String text),
      escape(char* text),
      escape(uint8_t text),
      escape(uint16_t text),
      
      htmlTemplate(String title, String content),
      config(void);
    ESP8266WebServer _server;
    int _port = 80;
};

#endif
