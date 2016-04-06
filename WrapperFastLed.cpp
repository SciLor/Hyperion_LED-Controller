#include "WrapperFastLed.h"

#ifdef CONFIG_TYPE_WEBCONFIG
  #define addLedsChipset(chipset, dataPin, clockPin, colorOrder, ledCount) \
      switch(chipset) { \
        case SPI_LPD8806: \
          addLedsDataPin(LPD8806, dataPin, clockPin, colorOrder, ledCount); \
          break; \
        case SPI_WS2801: \
          addLedsDataPin(WS2801, dataPin, clockPin, colorOrder, ledCount); \
          break; \
        case SPI_APA102: \
          addLedsDataPin(APA102, dataPin, clockPin, colorOrder, ledCount); \
          break; \
      } \
  /*
        case SPI_WS2803: \
          addLedsDataPin(WS2803, dataPin, clockPin, colorOrder, ledCount); \
          break; \
        case SPI_SM16716: \
          addLedsDataPin(SM16716, dataPin, clockPin, colorOrder, ledCount); \
          break; \
        case SPI_P9813: \
          addLedsDataPin(P9813, dataPin, clockPin, colorOrder, ledCount); \
          break; \
        case SPI_DOTSTAR: \
          addLedsDataPin(DOTSTAR, dataPin, clockPin, colorOrder, ledCount); \
          break; \
  
  */
  
  #define addLedsDataPinCase(chipset, dataPin, clockPin, colorOrder, ledCount) \
    case dataPin: \
      addLedsClockPin(chipset, dataPin, clockPin, colorOrder, ledCount); \
      break; \
  
  #define addLedsDataPin(chipset, dataPin, clockPin, colorOrder, ledCount) \
    switch(dataPin) { \
      addLedsDataPinCase(chipset, D1,  clockPin, colorOrder, ledCount); \
      addLedsDataPinCase(chipset, D2,  clockPin, colorOrder, ledCount); \
    } \
  /*
      addLedsDataPinCase(chipset, D0,  clockPin, colorOrder, ledCount); \
      addLedsDataPinCase(chipset, D3,  clockPin, colorOrder, ledCount); \
      addLedsDataPinCase(chipset, D4,  clockPin, colorOrder, ledCount); \
      addLedsDataPinCase(chipset, D5,  clockPin, colorOrder, ledCount); \
      addLedsDataPinCase(chipset, D6,  clockPin, colorOrder, ledCount); \
      addLedsDataPinCase(chipset, D7,  clockPin, colorOrder, ledCount); \
      addLedsDataPinCase(chipset, D8,  clockPin, colorOrder, ledCount); \
      addLedsDataPinCase(chipset, D9,  clockPin, colorOrder, ledCount); \
      addLedsDataPinCase(chipset, D10, clockPin, colorOrder, ledCount); \
  */
   
  #define addLedsClockPinCase(chipset, dataPin, clockPin, colorOrder, ledCount) \
    case clockPin: \
      addLedsColorCode(chipset, dataPin, clockPin, colorOrder, ledCount); \
      break; \
  
  #define addLedsClockPin(chipset, dataPin, clockPin, colorOrder, ledCoun) \
    switch(clockPin) { \
      addLedsClockPinCase(chipset, dataPin, D1,  colorOrder, ledCount); \
      addLedsClockPinCase(chipset, dataPin, D2,  colorOrder, ledCount); \
    } \
    /*
      addLedsClockPinCase(chipset, dataPin, D0,  colorOrder, ledCount); \
      addLedsClockPinCase(chipset, dataPin, D3,  colorOrder, ledCount); \
      addLedsClockPinCase(chipset, dataPin, D4,  colorOrder, ledCount); \
      addLedsClockPinCase(chipset, dataPin, D5,  colorOrder, ledCount); \
      addLedsClockPinCase(chipset, dataPin, D6,  colorOrder, ledCount); \
      addLedsClockPinCase(chipset, dataPin, D7,  colorOrder, ledCount); \
      addLedsClockPinCase(chipset, dataPin, D8,  colorOrder, ledCount); \
      addLedsClockPinCase(chipset, dataPin, D9,  colorOrder, ledCount); \
      addLedsClockPinCase(chipset, dataPin, D10, colorOrder, ledCount); \
     */
    
  #define addLedsColorCodeCase(chipset, dataPin, clockPin, colorOrder, ledCount) \
    case colorOrder: \
      addLedsFinal(chipset, dataPin, clockPin, colorOrder, ledCount); \
      break; \
  
  #define addLedsColorCode(chipset, dataPin, clockPin, colorOrder, ledCount) \
    switch(colorOrder) { \
      addLedsColorCodeCase(chipset, dataPin, clockPin, RGB, ledCount); \
      addLedsColorCodeCase(chipset, dataPin, clockPin, RBG, ledCount); \
      addLedsColorCodeCase(chipset, dataPin, clockPin, BGR, ledCount); \
      addLedsColorCodeCase(chipset, dataPin, clockPin, BRG, ledCount); \
      addLedsColorCodeCase(chipset, dataPin, clockPin, GBR, ledCount); \
      addLedsColorCodeCase(chipset, dataPin, clockPin, GRB, ledCount); \
    } \
  
  #define addLedsFinal(chipset, dataPin, clockPin, colorOrder, ledCount) FastLED.addLeds<chipset, dataPin, clockPin, colorOrder>(leds, ledCount); \
  
  void WrapperFastLed::begin(uint8_t chipset, uint8_t dataPin, uint8_t clockPin, uint8_t colorOrder, uint16_t ledCount) {
    Log.debug("Chipset=%i, dataPin=%i, clockPin=%i, colorOrder=%i, ledCount=%i", chipset, dataPin, clockPin, colorOrder, ledCount);
    _ledCount = ledCount;
    leds = new CRGB[_ledCount];
  
    //FastLED.addLeds<Config::chipset, Config::dataPin, Config::clockPin, Config::colorOrder>(leds, _ledCount);
    //FastLED.addLeds<WS2801, D1, D2, RGB>(leds, _ledCount);
  
    addLeds(chipset, dataPin, clockPin, colorOrder, _ledCount);
  }
  
  void WrapperFastLed::addLeds(uint8_t chipset, uint8_t dataPin, uint8_t clockPin, uint8_t colorOrder, uint16_t ledCount) {
    Chipset eChipset = static_cast<Chipset>(chipset);
    EOrder eColorOrder = static_cast<EOrder>(colorOrder);
  
    addLedsChipset(eChipset, dataPin, clockPin, eColorOrder, ledCount);
  }
  
#elif CONFIG_TYPE_STATIC_CONFIG
  void WrapperFastLed::begin() {
    #ifdef CONFIG_LED_CLOCKLESS_CHIPSET
      Log.debug("Chipset=%s, dataPin=%i, clockPin=%s, colorOrder=%i, ledCount=%i", "Clockless", CONFIG_LED_DATAPIN, "NONE", CONFIG_LED_COLOR_ORDER, CONFIG_LED_COUNT);
    #else
      Log.debug("Chipset=%i, dataPin=%i, clockPin=%i, colorOrder=%i, ledCount=%i", CONFIG_LED_SPI_CHIPSET, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER, CONFIG_LED_COUNT);
    #endif
    
    _ledCount = CONFIG_LED_COUNT;
    leds = new CRGB[_ledCount];
    
    #ifdef CONFIG_LED_CLOCKLESS_CHIPSET
      FastLED.addLeds<CONFIG_LED_CLOCKLESS_CHIPSET, CONFIG_LED_DATAPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
    #else
      FastLED.addLeds<CONFIG_LED_SPI_CHIPSET, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
    #endif
  }
#endif

void WrapperFastLed::show(void) {
  FastLED.show();
}

void WrapperFastLed::clear(void) {
  FastLED.clear();
}

void WrapperFastLed::fillSolid(CRGB color) {
  fill_solid(leds, _ledCount, color);
  show();
}

void WrapperFastLed::fillSolid(byte r, byte g, byte b) {
  fillSolid(CRGB(r, g, b));
}

void WrapperFastLed::rainbowStep(void) {
  for (int i=0; i < _ledCount; i++) {
    leds[i] = wheel((i + _rainbowStepState) % 255);
  }  
  show();
  
  if (_rainbowStepState < 255) {
    _rainbowStepState++;
  } else {
    _rainbowStepState = 0;
  }
}

CRGB WrapperFastLed::wheel(byte wheelPos) {
  CRGB color = CRGB();
  if (wheelPos < 85) {
   return color.setRGB(wheelPos * 3, 255 - wheelPos * 3, 0);
  } else if (wheelPos < 170) {
   wheelPos -= 85;
   return color.setRGB(255 - wheelPos * 3, 0, wheelPos * 3);
  } else {
   wheelPos -= 170; 
   return color.setRGB(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  return color;
}
