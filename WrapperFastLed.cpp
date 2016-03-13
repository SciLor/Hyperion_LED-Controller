#include "WrapperFastLed.h"

void WrapperFastLed::begin(const ESPIChipsets chipset, const uint8_t dataPin, const uint8_t clockPin, const uint8_t ledCount, const EOrder colorOrder) {
  _ledCount = ledCount;
  
  leds = new CRGB[_ledCount];

  #ifdef CHIPSET_CLOCKLESS_NAME
    //FastLED.addLeds<CHIPSET_CLOCKLESS_NAME, dataPin, colorOrder>(leds, ledCount);
  #else
    //FastLED.addLeds<chipset, dataPin, clockPin, colorOrder>(leds, ledCount);
    FastLED.addLeds<Config::chipset, Config::dataPin, Config::clockPin, Config::colorOrder>(leds, _ledCount);
  #endif
}

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
