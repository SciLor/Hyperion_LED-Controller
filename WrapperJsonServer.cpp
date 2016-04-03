#include "WrapperJsonServer.h"

WrapperJsonServer::WrapperJsonServer()
  : _tcpServer(0) {}

WrapperJsonServer::WrapperJsonServer(uint16_t ledCount, uint16_t tcpPort)
  : _tcpServer(tcpPort) {
  _ledCount = ledCount;
  _tcpPort = tcpPort;
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
    if (_tcpClient)
      handleConnection(true);
  }
}

void WrapperJsonServer::handleConnection(boolean newClient) {
  if (_tcpClient.connected()) {
    if (newClient) {
      Log.info("TCP-Client connected");
      _tcpClient.println();
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
  JsonObject& root = jsonBuffer.parseObject(data);
  if (root.success()) {
    String command = root["command"].asString();
    if (command.equals("serverinfo")) {
      Log.info("serverinfo");
      _tcpClient.println("{\"info\":{\"effects\":[{\"args\":{\"speed\":1.0},\"name\":\"Rainbow mood\",\"script\":\"rainbow\"}],\"hostname\":\"ESP8266\",\"priorities\":[],\"transform\":[{\"blacklevel\":[0.0,0.0,0.0],\"gamma\":[1.0,1.0,1.0],\"id\":\"default\",\"saturationGain\":1.0,\"threshold\":[0.0,0.0,0.0],\"valueGain\":1.0,\"whitelevel\":[1.0,1.0,1.0]}]},\"success\":true}");
    } else if (command.equals("color")) {
      ledColorWipe(root["color"][0], root["color"][1], root["color"][2]);
      _tcpClient.println("{\"success\":true}");
    } else if (command.equals("clear")) {
      clearCmd();
      _tcpClient.println("{\"success\":true}");
    } else {
      _tcpClient.println("{\"success\":false}");
    }
    _tcpClient.flush();
  } else {
    Log.error("JSON not parsed");
  }
}

void WrapperJsonServer::onLedColorWipe(void(* function) (byte, byte, byte)) {
  ledColorWipePointer = function;
}
void WrapperJsonServer::ledColorWipe(byte r, byte g, byte b) {
  if (ledColorWipePointer) {
    ledColorWipePointer(r, g, b);
  }
}

void WrapperJsonServer::onClearCmd(void(* function) (void)) {
  clearCmdPointer = function;
}
void WrapperJsonServer::clearCmd(void) {
  if (clearCmdPointer) {
    clearCmdPointer();
  }
}

