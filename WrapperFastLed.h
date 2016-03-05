#ifndef WrapperFastLed_h
#define WrapperFastLed_h

#include "BaseHeader.h"
#include <FastLED.h>

class WrapperFastLed {
  public:
    void
      begin(void),
      show(void),
      clear(void),
      fillSolid(const struct CRGB& color),
      fillSolid(byte r, byte g, byte b),
      rainbowStep(void);

    CRGB* leds;
      
  private:   
    CRGB wheel(byte wheelPos);
    byte _rainbowStepState;
};

#endif
