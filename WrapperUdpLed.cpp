#include "WrapperUdpLed.h"

WrapperUdpLed::WrapperUdpLed(uint16_t ledCount, uint16_t udpPort) {
  _udp = WiFiUDP();
  _ledCount = ledCount;
  _udpPort = udpPort;

  _bufferSize = _ledCount * 3; //3 bytes per LED

  _udpBuffer = new byte [_bufferSize+1];
  _udpBuffer[_bufferSize] = 0;
}

void WrapperUdpLed::begin(void) {
  Log.info("Open port %i for UDP...", _udpPort);
  if (_udp.begin(19446)) {
    Log.info("success");
  } else {
    Log.error("no success");
  }  
}

void WrapperUdpLed::handle(void) {
  int bytes = _udp.parsePacket();
  if (bytes > 0) {
    Log.debug("UDP-Packet received, length: %i", bytes);
    if (bytes == _bufferSize) {
      _udp.readBytes(_udpBuffer, _bufferSize);
      Log.debug("Contents: %s", _udpBuffer);
      for (int i=0; i<_ledCount; i++) {
        updateLed(i, _udpBuffer[i*3+0], _udpBuffer[i*3+1], _udpBuffer[i*3+2]);
      }
      refreshLeds();
    }
  }
}

void WrapperUdpLed::onUpdateLed(void(* function) (int, byte, byte, byte)) {
  updateLedPointer = function;
}
void WrapperUdpLed::updateLed(int id, byte r, byte g, byte b) {
  if (updateLedPointer) {
    updateLedPointer(id, r, g, b);
  }
}

void WrapperUdpLed::onRefreshLeds(void(* function) (void)) {
  refreshLedsPointer = function;
}
void WrapperUdpLed::refreshLeds(void) {
  if (refreshLedsPointer) {
    refreshLedsPointer();
  }
}


