#include "WrapperJsonServer.h"

WrapperJsonServer::WrapperJsonServer()
  : _tcpServer(0) {}

WrapperJsonServer::WrapperJsonServer(uint16_t ledCount, uint16_t tcpPort)
  : _tcpServer(tcpPort) {
  _ledCount = ledCount;
  _tcpPort = tcpPort;
  _activeLedColor = new byte[3];
}

void WrapperJsonServer::begin(void) {
  Log.info("Open port %i for TCP...", _tcpPort);
  _tcpServer.begin();
}

void WrapperJsonServer::handle(void) {
  if (_tcpClient) {
    handleConnection(false);
  } else {
    _tcpClient = _tcpServer.available();
    if (_tcpClient) {
      handleConnection(true);
    }
  }
}

void WrapperJsonServer::handleConnection(boolean newClient) {
  if (_tcpClient.connected()) {
    if (newClient) {
      Log.info("TCP-Client connected");
    }
    while (_tcpClient.available()) {
      readData();
    }
  } else {
    Log.info("TCP-Client disconnected");
    _tcpClient.stop();
  }
}

void WrapperJsonServer::readData(void) {
  String data = _tcpClient.readStringUntil('\n');
  Log.debug("Received data: %s", data.c_str());
  StaticJsonBuffer<TCP_BUFFER> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(data.c_str());
  if (root.success()) {
    String command = root["command"].asString();
    if (command.equals("serverinfo")) {
      Log.info("serverinfo");     
      _tcpClient.print("{\"info\":{\"effects\":["
          "{\"args\":{\"speed\":1.0},\"name\":\"Hyperion UDP\",\"script\":\"hyperion_udp\"},"
          "{\"args\":{\"speed\":2.0},\"name\":\"Rainbow mood\",\"script\":\"rainbow\"},"
          "{\"args\":{\"speed\":62.5},\"name\":\"Fire2012\",\"script\":\"fire2012\"}"
        "],"
        "\"activeLedColor\":[{\"RGB Value\":[");
        //Dirty hack, workaround for each leds state of newer hyperion versions
        /*  "activeLedColor" : [
             {
                "HEX Value" : [ "0xFF0036" ],
                "HSL Value" : [ 347, 1.0, 0.50 ],
                "RGB Value" : [ 255, 0, 54 ]
             }
          ], */
        _tcpClient.print(_activeLedColor[0]);
        _tcpClient.print(",");
        _tcpClient.print(_activeLedColor[1]);
        _tcpClient.print(",");
        _tcpClient.print(_activeLedColor[2]);
        
        _tcpClient.print("]}],"
          "\"hostname\":\"ESP8266\","
          "\"priorities\":[],\"transform\":[{\"blacklevel\":[0.0,0.0,0.0],\"gamma\":[1.0,1.0,1.0],\"id\":\"default\",\"saturationGain\":1.0,\"threshold\":[0.0,0.0,0.0],\"valueGain\":1.0,\"whitelevel\":[1.0,1.0,1.0]}]},"
          "\"success\":true}");
        _tcpClient.println("");
    } else if (command.equals("color")) {
      int duration = root["duration"];
      ledColorWipe(root["color"][0], root["color"][1], root["color"][2]);
      _tcpClient.println("{\"success\":true}");
    } else if (command.equals("clear") || command.equals("clearall")) {
      clearCmd();
      _tcpClient.println("{\"success\":true}");
    } else if (command.equals("effect")) {
      String effect = root["effect"]["name"].asString();
      double effectSpeed = root["effect"]["args"]["speed"];
      int interval = 0;
      if (effectSpeed > 0) {
        interval = (int)(1000.0 / effectSpeed);
      }
      
      if (effect.equals("Hyperion UDP")) {
        effectChange(HYPERION_UDP);
      } else if (effect.equals("Rainbow mood")) {
        effectChange(RAINBOW, interval);
      } else if (effect.equals("Fire2012")) {
        effectChange(FIRE2012, interval);
      }
      _tcpClient.println("{\"success\":true}");
    } else {
      _tcpClient.println("{\"success\":false}");
    }
  } else {
    Log.error("JSON not parsed");
  }
}

void WrapperJsonServer::onLedColorWipe(void(* function) (byte, byte, byte)) {
  ledColorWipePointer = function;
}
void WrapperJsonServer::ledColorWipe(byte r, byte g, byte b) {
  _activeLedColor[0] = r;
  _activeLedColor[1] = g;
  _activeLedColor[2] = b;
  if (ledColorWipePointer) {
    ledColorWipePointer(r, g, b);
  }
}

void WrapperJsonServer::onClearCmd(void(* function) (void)) {
  clearCmdPointer = function;
}
void WrapperJsonServer::clearCmd(void) {
  _activeLedColor[0] = 0;
  _activeLedColor[1] = 0;
  _activeLedColor[2] = 0;
  if (clearCmdPointer) {
    clearCmdPointer();
  }
}

void WrapperJsonServer::onEffectChange(void(* function) (Mode, int)) {
  effectChangePointer = function;
}
void WrapperJsonServer::effectChange(Mode effect, int interval/* = 0*/) {
  if (effectChangePointer) {
    effectChangePointer(effect, interval);
  }
}

