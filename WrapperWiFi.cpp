#include "WrapperWiFi.h"

WrapperWiFi::WrapperWiFi(const char* ssid, const char* password) {  
  _ssid = ssid;
  _password = password;
}

void WrapperWiFi::begin(void) {
  Log.debug("WrapperWiFi(ssid=\"%s\", password=\"%s\")", _ssid, _password);

  Log.info("Connecting to WiFi %s", _ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid, _password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Log.error("WiFi Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Log.info("Connected successfully, IP address: %s", WiFi.localIP().toString().c_str());
}

