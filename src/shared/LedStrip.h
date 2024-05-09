#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <Adafruit_NeoPixel.h>

#define NUM_STRIPS 8
#define NUM_LEDS_PER_STRIP 60

extern int currentMaxBrightness;   // Global variable for the current maximum brightness
#define DEFAULT_MAX_BRIGHTNESS 255 // Default maximum brightness

extern Adafruit_NeoPixel strips[NUM_STRIPS]; // Array of strips

void setupStrips();
void setBrightness(int brightness);

#endif
