#ifndef WrapperFastLed_h
#define WrapperFastLed_h

#include "BaseHeader.h"

#define FASTLED_ESP8266_RAW_PIN_ORDER
//#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
//#define FASTLED_ESP8266_D1_PIN_ORDER

#include <FastLED.h>

class WrapperFastLed {
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
