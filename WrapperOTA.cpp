#include "WrapperOTA.h"

void WrapperOTA::begin(const char* hostname) {
  Log.info("Prepare OTA");
  Log.debug("WrapperOTA(hostname=\"%s\")", hostname);

  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(hostname);

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"schnulli");

  ArduinoOTA.onStart([]() {
    Log.info("Start OTA Update");
  });
  ArduinoOTA.onEnd([]() {
    Log.info("OTA Update Finished");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Log.info("OTA Progress: %i%%", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    if (error == OTA_AUTH_ERROR) Log.error("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Log.error("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Log.error("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Log.error("Receive Failed");
    else if (error == OTA_END_ERROR) Log.error("End Failed");
  });
  ArduinoOTA.begin();
  Log.info("OTA Ready");
}

void WrapperOTA::handle(void) {
  ArduinoOTA.handle();
}

