#ifndef WrapperWS2801_h
#define WrapperWS2801_h

#include "BaseHeader.h"
#include <Adafruit_WS2801.h>

class WrapperWS2801 {
  public:
    WrapperWS2801(uint16_t n, uint8_t dpin, uint8_t cpin);

    void
      begin(void),
      rainbowStep(void),
      rainbowCycle(uint8_t wait),
      colorWipe(byte r, byte g, byte b);
    
    uint32_t
      Color(byte r, byte g, byte b),
      Wheel(byte WheelPos);
    
    Adafruit_WS2801 strip;
  private:   
    byte _rainbowStepState;
};

#endif
