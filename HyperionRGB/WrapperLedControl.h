#ifndef WrapperLedControl_h
#define WrapperLedControl_h

#include "BaseHeader.h"

#include <FastLED.h>

class WrapperLedControl {
  public:
    void
      begin(uint16_t ledCount),
      show(void),
      clear(void),
      fillSolid(CRGB color),
      fillSolid(byte r, byte g, byte b),
      rainbowStep(void),
      rainbowV2Step(void),
      rainbowFullStep(void),
      fire2012Step(void);

    CRGB* leds;
      
  private:       
    CRGB wheel(byte wheelPos);
    byte _rainbowStepState;
    uint16_t _rainbowV2StepState;
    boolean _fire2012Direction;
    byte* _fire2012Heat;
    uint16_t _ledCount;
};

#endif
