#include "Pulse.h"
#include "shared/LedStrip.h"
#include "shared/ColorUtils.h"

extern int currentMaxBrightness;

void setPulse(const std::vector<uint32_t> &colors)
{
  static float brightness = 0.2;
  static bool increasing = true;
  const float brightnessStep = 0.02; // 0.05 per 50 ms, now in 50 fps steps
  const long interval = 20;
  static unsigned long previousMillis = 0;

  unsigned long currentMillis = millis();
  unsigned long elapsed = currentMillis - previousMillis;
  if (elapsed >= interval)
  {
    previousMillis = currentMillis;
    // Step by the time that actually passed, so the pulse speed holds even if a frame runs long
    float step = brightnessStep * min(elapsed, 100UL) / interval;
    int numLEDs = strips[0].numPixels();

    for (int i = 0; i < NUM_STRIPS; i++)
    {
      for (int j = 0; j < numLEDs; j++)
      {
        uint32_t gradientColor = interpolateColorBasedOnCount(colors, j, numLEDs);

        uint8_t r = ((gradientColor >> 16 & 0xFF) * brightness * currentMaxBrightness / 255);
        uint8_t g = ((gradientColor >> 8 & 0xFF) * brightness * currentMaxBrightness / 255);
        uint8_t b = ((gradientColor & 0xFF) * brightness * currentMaxBrightness / 255);

        strips[i].setPixelColor(j, strips[i].Color(r, g, b));
      }
      showStrip(strips[i]);
    }

    if (increasing)
    {
      brightness += step;
      if (brightness >= 1.0)
      {
        brightness = 1.0;
        increasing = false;
      }
    }
    else
    {
      brightness -= step;
      if (brightness <= 0.2)
      {
        brightness = 0.2;
        increasing = true;
      }
    }
  }
}
