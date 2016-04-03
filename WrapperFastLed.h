#ifndef WrapperFastLed_h
#define WrapperFastLed_h

#include "BaseHeader.h"
#include <FastLED.h>

class WrapperFastLed {
  public:
    void
      begin(const uint8_t chipset, const uint8_t dataPin, const uint8_t clockPin, const uint8_t colorOrder, const uint16_t ledCount),
      show(void),
      clear(void),
      fillSolid(CRGB color),
      fillSolid(byte r, byte g, byte b),
      rainbowStep(void);

    CRGB* leds;
      
  private: 
    void 
      addLeds(const uint8_t chipset, const uint8_t dataPin, const uint8_t clockPin, const uint8_t colorOrder, const uint16_t ledCount);
      
    CRGB wheel(byte wheelPos);
    byte _rainbowStepState;
    int _ledCount;
};

#endif
