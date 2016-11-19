#ifndef BaseHeader_h
#define BaseHeader_h

#include <Arduino.h>
#include <Logging.h>
#include "Config.h"

#define min(a,b) ((a)<(b)?(a):(b))
/*
 #define min(X, Y)                \
 ({ typeof (X) x_ = (X);          \
    typeof (Y) y_ = (Y);          \
    (x_ < y_) ? x_ : y_; })*/

enum Mode { OFF, HYPERION_UDP, STATIC_COLOR, RAINBOW, FIRE2012 };

#endif

