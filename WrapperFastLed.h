#ifndef WrapperFastLed_h
#define WrapperFastLed_h

#include "BaseHeader.h"
#include <FastLED.h>

class WrapperFastLed {
  public:
    void
      #ifdef CONFIG_TYPE_WEBCONFIG
        begin(uint8_t chipset, uint8_t dataPin, uint8_t clockPin, uint8_t colorOrder, uint16_t ledCount),
      #elif CONFIG_TYPE_STATIC_CONFIG
        begin(),
      #endif
      show(void),
      clear(void),
      fillSolid(CRGB color),
      fillSolid(byte r, byte g, byte b),
      rainbowStep(void);

    CRGB* leds;
      
  private: 
   #ifdef CONFIG_TYPE_WEBCONFIG
    void 
        addLeds(uint8_t chipset, uint8_t dataPin, uint8_t clockPin, uint8_t colorOrder, uint16_t ledCount);
   #endif
      
    CRGB wheel(byte wheelPos);
    byte _rainbowStepState;
    int _ledCount;
};

#endif
