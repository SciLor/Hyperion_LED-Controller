#ifndef WrapperFastLed_h
#define WrapperFastLed_h

#include "BaseHeader.h"
#ifdef HW_FASTLED
  #include <FastLED.h>
#elif HW_NEOPIXEL
  #include <NeoPixelBus.h>
#endif

#define colorSaturation CONFIG_MAX_COLOR
#define colorFactor 255 / CONFIG_MAX_COLOR

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
      #ifdef HW_FASTLED
        fillSolid(CRGB color),
      #elif HW_NEOPIXEL
        setPixel(byte i, byte r, byte g, byte b),
        fillSolid(RgbColor color),
      #endif
      fillSolid(byte r, byte g, byte b),
      rainbowStep(void);

    #ifdef HW_FASTLED
      CRGB* leds;
    #elif HW_NEOPIXEL
      typedef NeoPixelBus<CONFIG_NEO_FEATURE, CONFIG_NEO_METHOD> MyPixelBus;
      MyPixelBus * strip;
    #endif
      
  private: 
    #ifdef HW_FASTLED
      #ifdef CONFIG_TYPE_WEBCONFIG
       void 
         addLeds(uint8_t chipset, uint8_t dataPin, uint8_t clockPin, uint8_t colorOrder, uint16_t ledCount);
      #endif
      CRGB wheel(byte wheelPos);
    #elif HW_NEOPIXEL
      RgbColor wheel(byte wheelPos);
    #endif
    byte _rainbowStepState;
    int _ledCount;
};

#endif
