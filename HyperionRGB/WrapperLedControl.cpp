#include "WrapperLedControl.h"

void WrapperLedControl::begin() {
  #ifdef CONFIG_LED_CLOCKLESS_CHIPSET
    Log.debug("Chipset=%s, dataPin=%i, clockPin=%s, colorOrder=%i, ledCount=%i", "Clockless", CONFIG_LED_DATAPIN, "NONE", CONFIG_LED_COLOR_ORDER, CONFIG_LED_COUNT);
  #elif defined CONFIG_LED_PWM
    Log.debug("Chipset=%s, redPin=%i, greenPin=%i, bluePin=%i, ledCount=%i", "PWM", CONFIG_LED_PWM_RED, CONFIG_LED_PWM_GREEN, CONFIG_LED_PWM_BLUE, CONFIG_LED_COUNT);
    #if CONFIG_LED_COUNT != 1
      #error "PWM only supports LED count set to one (even if you have multiple LEDs on your strip, they will all show the same color)"
    #endif
  #else
    Log.debug("Chipset=%i, dataPin=%i, clockPin=%i, colorOrder=%i, ledCount=%i", CONFIG_LED_SPI_CHIPSET, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER, CONFIG_LED_COUNT);
  #endif
  
  _ledCount = CONFIG_LED_COUNT;
  leds = new CRGB[_ledCount];
  _fire2012Heat = new byte[_ledCount];
  
  #ifdef CONFIG_LED_CLOCKLESS_CHIPSET
    FastLED.addLeds<CONFIG_LED_CLOCKLESS_CHIPSET, CONFIG_LED_DATAPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
  #elif defined CONFIG_LED_PWM
    //Nothing to to
  #else
    FastLED.addLeds<CONFIG_LED_SPI_CHIPSET, CONFIG_LED_DATAPIN, CONFIG_LED_CLOCKPIN, CONFIG_LED_COLOR_ORDER>(leds, _ledCount);
  #endif
}

void WrapperLedControl::show(void) {
  #if defined CONFIG_LED_PWM
    analogWrite(CONFIG_LED_PWM_RED, map(leds[0].red, 0, 255, 0, PWMRANGE));
    analogWrite(CONFIG_LED_PWM_GREEN, map(leds[0].green, 0, 255, 0, PWMRANGE));
    analogWrite(CONFIG_LED_PWM_BLUE, map(leds[0].blue, 0, 255, 0, PWMRANGE));
  #else
    FastLED.show();
  #endif
}

void WrapperLedControl::clear(void) {
  #if defined CONFIG_LED_PWM
    leds[0] = CRGB::Black;
  #else
    FastLED.clear();
  #endif
}

void WrapperLedControl::fillSolid(CRGB color) {
  fill_solid(leds, _ledCount, color);
  show();
}

void WrapperLedControl::fillSolid(byte r, byte g, byte b) {
  fillSolid(CRGB(r, g, b));
}

void WrapperLedControl::rainbowStep(void) {
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

CRGB WrapperLedControl::wheel(byte wheelPos) {
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

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  50

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

void WrapperLedControl::fire2012Step(void) {
    // Step 1.  Cool down every cell a little
   for( int i = 0; i < _ledCount; i++) {
     _fire2012Heat[i] = qsub8( _fire2012Heat[i],  random8(0, ((COOLING * 10) / _ledCount) + 2));
   }
 
   // Step 2.  _fire2012Heat from each cell drifts 'up' and diffuses a little
   for( int k=_ledCount - 1; k >= 2; k--) {
     _fire2012Heat[k] = (_fire2012Heat[k - 1] + _fire2012Heat[k - 2] + _fire2012Heat[k - 2] ) / 3;
   }
   
   // Step 3.  Randomly ignite new 'sparks' of _fire2012Heat near the bottom
   if( random8() < SPARKING ) {
     int y = random8(min(7, _ledCount - 1));
     _fire2012Heat[y] = qadd8(_fire2012Heat[y], random8(160,255));
   }

   // Step 4.  Map from _fire2012Heat cells to LED colors
   for( int j = 0; j < _ledCount; j++) {
     CRGB color = HeatColor( _fire2012Heat[j]);
     int pixelnumber;
     if(_fire2012Direction) {
       pixelnumber = (_ledCount-1) - j;
     } else {
       pixelnumber = j;
     }
     leds[pixelnumber] = color;
   }
   show();
}
