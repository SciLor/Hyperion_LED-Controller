#include "WrapperUdpLed.h"

WrapperUdpLed::WrapperUdpLed(uint16_t ledCount, uint16_t udpPort, uint8_t udpProtocol) {
  _udp = WiFiUDP();
  _ledCount = ledCount;
  _udpPort = udpPort;
  _udpProtocol = udpProtocol;

  _bufferSize = _ledCount * 3; //3 bytes per LED

  _udpBuffer = new byte [_bufferSize+1];
  _udpBuffer[_bufferSize] = 0;

  _opened = false;
}

void WrapperUdpLed::begin(void) {
  if (!_opened) {
    Log.info("Open port %i for UDP with protocol %i...", _udpPort, _udpProtocol);
    if (_udp.begin(_udpPort)) {
      Log.info("success");
      _opened = true;
    } else {
      Log.error("no success");
    } 
  }
}
void WrapperUdpLed::stop(void) {
  if (_opened) {
    Log.info("Close port %i for UDP...", _udpPort);
    _udp.stop();
    _opened = false;
  }
}
void WrapperUdpLed::handle(void) {
  int bytes = _udp.parsePacket();
  if (bytes > 0) {
    Log.debug("UDP-Packet received, length: %i", bytes);
    //See @https://hyperion-project.org/wiki/UDP-Device
    if (_udpProtocol == 0) {
      //Protocol 0: Raw Format, 3 bytes per LED, Full LED set.
      /// {0: R, 1: G, 2: B}
      if (bytes == _bufferSize) {
        _udp.readBytes(_udpBuffer, _bufferSize);
        Log.verbose("Contents: %s", _udpBuffer);
        for (int i=0; i<_ledCount; i++) {
          updateLed(i, _udpBuffer[i*3+0], _udpBuffer[i*3+1], _udpBuffer[i*3+2]);
        }
        refreshLeds();
      } else {
        Log.debug("UDP-Packet size expected=%i, actual=%i", _bufferSize, bytes);
      }
    } else (_udpProtocol == 2) {
      //Protocol 2: 
      /// 0: Update ID & 0xF
      /// 1: Fragment ?!
      /// 2: First LED ID, high byte / 3: First LED ID, low byte --> int16_t?
      /// 4: {0: R, 1: G, 2: B}
    } else (_udpProtocol == 3) {
      //Protocol 3: TPM2.net
      /// 0: 0x9C
      /// 1: 0xDA
      /// 2: Data length (LED count * 3), high byte, 3: Data length (LED count * 3), low byte --> int16_t?
      /// 4: Fragment ID
      /// 5: Fragment ID maximum
      /// 6: {0: R, 1: G, 2: B}
      /// 7: 0x36
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


