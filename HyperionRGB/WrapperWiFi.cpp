#include "WrapperWiFi.h"

WrapperWiFi::WrapperWiFi(const char* ssid, const char* password, const char* hostname) {  
  _ssid = ssid;
  _password = password;
  _hostname = hostname;
  byte empty[4] = {0};
  memcpy(_ip, empty, sizeof(_ip));
  memcpy(_subnet, empty, sizeof(_subnet));
  memcpy(_dns, empty, sizeof(_dns));
}

WrapperWiFi::WrapperWiFi(const char* ssid, const char* password, const byte ip[4], const byte subnet[4], const byte dns[4], const char* hostname) {  
  _ssid = ssid;
  _password = password;
  _hostname = hostname;
  if (ip[0] != 0) {
    memcpy(_ip, ip, sizeof(_ip));
    memcpy(_subnet, subnet, sizeof(_subnet));
    memcpy(_dns, dns, sizeof(_dns));
  } else {
    byte empty[4] = {0};
    memcpy(_ip, empty, sizeof(_ip));
    memcpy(_subnet, empty, sizeof(_subnet));
    memcpy(_dns, empty, sizeof(_dns));
  }
}

bool WrapperWiFi::connect(void) {
  Log.debug("WrapperWiFi(ssid=\"%s\", password=\"%s\")", _ssid, _password);

  Log.info("Connecting to WiFi %s", _ssid);
  
  WiFi.mode(WIFI_STA);
  if (_ip[0] != 0) {
    Log.info("Using static ip");
    WiFi.config(_ip, _dns, _subnet);
  } else {
    Log.info("Using dynamic ip");
  }
  
  WiFi.begin(_ssid, _password);

  return (WiFi.waitForConnectResult() == WL_CONNECTED);
}
void WrapperWiFi::begin(void) {
  for (uint8 i=0; i<5; i++) {
    if (connect()) {
      Log.info("Connected successfully, IP address: %s", WiFi.localIP().toString().c_str());
      _apMode = false;
      return;
    }
  }
  
  //WiFi connection failed.
  if (_hostname[0] == '\0') 
    _hostname = "HyperionRGB";
  Log.error("WiFi Connection failed!");
  #ifdef CONFIG_ENABLE_WEBCONFIG || !CONFIG_OVERWRITE_WEBCONFIG
  WiFi.mode(WIFI_AP);
  delay(100);
  
  const char *ssid = _hostname;
  const char *password = CONFIG_OTA_AP_PASSWORD;
  Log.info("AP with SSID=%s and password %s for configuration is started...", ssid, password);
  if (WiFi.softAP(ssid, password)) {
    _apMode = true;
    Log.info("IP=%s", WiFi.softAPIP().toString().c_str());
    return;
  } else {
    Log.error("Creating AP failed, check if hostname is okay and password isn't to short...");
  }
  #endif
  Log.error("Restarting...");
  delay(5000);
  ESP.restart();
}

bool WrapperWiFi::isAP(void) {
  return _apMode;
}
bool WrapperWiFi::isAPConnected(void) {
  if (!isAP())
    return false;
  return (WiFi.softAPgetStationNum() > 0);
}

