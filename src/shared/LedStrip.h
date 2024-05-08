#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <Adafruit_NeoPixel.h>

#define NUM_STRIPS 8
#define NUM_LEDS_PER_STRIP 60
#define BRIGHTNESS 255

extern Adafruit_NeoPixel strips[NUM_STRIPS]; // Array of strips

void setupStrips();

#endif
