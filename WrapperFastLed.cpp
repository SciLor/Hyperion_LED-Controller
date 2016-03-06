#include "WrapperFastLed.h"

void WrapperFastLed::begin(void) {
  leds = new CRGB [Config::ledCount];

  #ifdef CHIPSET_CLOCKLESS_NAME
    FastLED.addLeds<CHIPSET_CLOCKLESS_NAME, Config::dataPin, Config::colorOrder>(leds, Config::ledCount);
  #else
    FastLED.addLeds<Config::chipset, Config::dataPin, Config::clockPin, Config::colorOrder>(leds, Config::ledCount);
  #endif
}

void WrapperFastLed::show(void) {
  FastLED.show();
}

void WrapperFastLed::clear(void) {
  FastLED.clear();
}

void WrapperFastLed::fillSolid(const struct CRGB& color) {
  fill_solid(leds, Config::ledCount, color);
}

void WrapperFastLed::fillSolid(byte r, byte g, byte b) {
  CRGB color = CRGB();
  color.setRGB(r, g, b);
  fill_solid(leds, Config::ledCount, color);
}

void WrapperFastLed::rainbowStep(void) {
  for (int i=0; i < Config::ledCount; i++) {
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
