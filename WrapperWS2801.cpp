#include "WrapperWS2801.h"

WrapperWS2801::WrapperWS2801(uint16_t n, uint8_t dpin, uint8_t cpin) {
  strip = Adafruit_WS2801(n, dpin, cpin);
}

void WrapperWS2801::begin(void) {
  strip.begin();
  strip.show();
}

void WrapperWS2801::rainbowStep(void) {
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel( (i + _rainbowStepState) % 255));
  }  
  strip.show();
  
  if (_rainbowStepState < 255) {
    _rainbowStepState++;
  } else {
    _rainbowStepState = 0;
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void WrapperWS2801::rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

void WrapperWS2801::colorWipe(byte r, byte g, byte b) {
  uint32_t c = Color(r, g, b);
  
  for (int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t WrapperWS2801::Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t WrapperWS2801::Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
/*
void WrapperWS2801::onRoundSet(CALLBACK(fn)) {
  _onRoundSet = fn;
}
void WrapperWS2801::roundSet() {
  if (_onRoundSet) {
    _onRoundSet();
  }
} */
