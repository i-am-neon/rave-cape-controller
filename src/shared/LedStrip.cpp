#include "LedStrip.h"

int ledPins[NUM_STRIPS] = {22, 21, 4, 15, 32, 26, 12, 13};

Adafruit_NeoPixel strips[NUM_STRIPS] = {
    Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, ledPins[0], NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, ledPins[1], NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, ledPins[2], NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, ledPins[3], NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, ledPins[4], NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, ledPins[5], NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, ledPins[6], NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(NUM_LEDS_PER_STRIP, ledPins[7], NEO_GRB + NEO_KHZ800)};

void setupStrips()
{
  for (int i = 0; i < NUM_STRIPS; i++)
  {
    strips[i].begin();
    strips[i].setBrightness(BRIGHTNESS);
    strips[i].show(); // Initialize all pixels to 'off'
  }
}

void setBrightness(int brightness)
{
  for (int i = 0; i < NUM_STRIPS; i++)
  {
    strips[i].setBrightness(brightness);
    strips[i].show();
  }
}
