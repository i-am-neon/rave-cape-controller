#ifndef FRAME_H
#define FRAME_H

#include <Arduino.h>
#include "shared/LedStrip.h"

// Full-brightness frame buffer for the spooky animations. showFrame() applies the strip
// brightness and the power cap, matching the output stage of tools/emulator/index.html.
// Pixel 0 of each strip is at the shoulders, pixel 59 at the hem.

struct RGB
{
  float r, g, b;
};

extern RGB frame[NUM_STRIPS][NUM_LEDS_PER_STRIP];

RGB rgb(float r, float g, float b);
RGB hexColor(uint32_t hex);
RGB mixColor(RGB a, RGB b, float t);
RGB scaleColor(RGB c, float k);
RGB hsvColor(float h, float s, float v); // h 0-360, s/v 0-1
RGB paletteColor(const float *positions, const RGB *colors, int count, float x);
RGB gradientColor(const RGB *colors, int count, float pos); // wraps back to the first color

void framePut(int s, int l, RGB c);
void frameAdd(int s, int l, RGB c);
void frameFill(RGB c);
void frameFade(float k);
void showFrame();

int randInt(int n);                // 0 .. n-1
float randFloat(float a, float b); // a .. b

#endif
