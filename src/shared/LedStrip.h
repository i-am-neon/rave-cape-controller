#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <Adafruit_NeoPixel.h>

#define NUM_STRIPS 8
#define NUM_LEDS_PER_STRIP 60

extern int currentMaxBrightness;   // Global variable for the current maximum brightness
#define DEFAULT_MAX_BRIGHTNESS 255 // Default maximum brightness

// Power cap: the rainbow flow at full brightness (one full channel per LED) is the
// most each power bank has proven it can sustain, so no strip may draw more than that.
#define MAX_CHANNEL_SUM_PER_STRIP (NUM_LEDS_PER_STRIP * 255UL)

extern Adafruit_NeoPixel strips[NUM_STRIPS]; // Array of strips

void setupStrips();
void setBrightness(int brightness);
void showStrip(Adafruit_NeoPixel &strip); // Use instead of strip.show() so the power cap applies

#endif
