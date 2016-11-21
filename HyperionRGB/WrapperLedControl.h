#ifndef WrapperLedControl_h
#define WrapperLedControl_h

#include "BaseHeader.h"

#include <FastLED.h>

class WrapperLedControl {
  public:
    void
      begin(),
      show(void),
      clear(void),
      fillSolid(CRGB color),
      fillSolid(byte r, byte g, byte b),
      rainbowStep(void),
      fire2012Step(void);

    CRGB* leds;
      
  private:       
    CRGB wheel(byte wheelPos);
    byte _rainbowStepState;
    boolean _fire2012Direction;
    byte* _fire2012Heat;
    int _ledCount;
};

#endif
