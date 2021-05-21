#ifndef WrapperUdpLed_h
#define WrapperUdpLed_h

#include "BaseHeader.h"
#include <WiFiUdp.h>

class WrapperUdpLed {
  public:
    WrapperUdpLed() {};
    WrapperUdpLed(uint16_t ledCount, uint16_t udpPort, UdpProtocol udpProtocol);
    
    void
      begin(void),
      stop(void),
      handle(void);

    void
      onUpdateLed(void(* function) (int, byte, byte, byte)),
      onRefreshLeds(void(* function) (void));
      
  private:
    WiFiUDP _udp;
    uint16_t _ledCount;
    uint16_t _udpPort;
    UdpProtocol _udpProtocol;
    byte* _udpBuffer;
    uint16_t _bufferSize;
    boolean _opened;
    uint16_t _tpm2LedId;

    void 
      updateLed(int id, byte r, byte g, byte b),
      (* updateLedPointer) (int, byte, byte, byte);

    void 
      refreshLeds(void),
      (* refreshLedsPointer) (void);

   void 
      handleProtocolRaw(int bytes),
      handleProtocolFragment(int bytes),
      handleProtocolTPM2(int bytes);
};

#endif

