#include "PulseOneColor.h"
#include "shared/LedStrip.h"

extern int currentMaxBrightness; // Ensure global brightness is accessible

void setPulseOneColor(uint32_t color)
{
  static float brightness = 0.2; // Should be static to maintain state between calls
  static bool increasing = true; // Should be static to maintain state between calls
  const float brightnessStep = 0.05;
  const long interval = 50;                // Adjusted for faster animation
  static unsigned long previousMillis = 0; // Initialization moved out of loop

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    uint8_t r = ((color >> 16 & 0xFF) * brightness * currentMaxBrightness / 255);
    uint8_t g = ((color >> 8 & 0xFF) * brightness * currentMaxBrightness / 255);
    uint8_t b = ((color & 0xFF) * brightness * currentMaxBrightness / 255);

    for (int i = 0; i < NUM_STRIPS; i++)
    {
      for (int k = 0; k < strips[i].numPixels(); k++)
      {
        strips[i].setPixelColor(k, strips[i].Color(r, g, b));
      }
      strips[i].show();
    }

    if (increasing)
    {
      brightness += brightnessStep;
      if (brightness >= 1.0)
      {
        brightness = 1.0; // Ensure it does not exceed 1.0
        increasing = false;
      }
    }
    else
    {
      brightness -= brightnessStep;
      if (brightness <= 0.2)
      {
        brightness = 0.2; // Ensure it does not go below 0.2
        increasing = true;
      }
    }
  }
}
