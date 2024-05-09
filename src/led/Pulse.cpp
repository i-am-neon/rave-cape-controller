#include "Pulse.h"
#include "shared/LedStrip.h"

extern int currentMaxBrightness; // Ensure global brightness is accessible

// Add missing function declaration for 'interpolateColor'
uint32_t interpolateColor(uint32_t color1, uint32_t color2, float ratio);

void setPulse(const std::vector<uint32_t> &colors)
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

    int numLEDs = strips[0].numPixels(); // Assuming all strips have the same number of LEDs

    for (int i = 0; i < NUM_STRIPS; i++)
    {
      for (int j = 0; j < numLEDs; j++)
      {
        float ratio = float(j) / float(numLEDs - 1);
        uint32_t gradientColor = colors[0]; // Default to the first color if no gradient
        if (colors.size() == 2)
        {
          gradientColor = interpolateColor(colors[0], colors[1], ratio);
        }
        else if (colors.size() == 3)
        {
          gradientColor = (j < numLEDs / 2) ? interpolateColor(colors[0], colors[1], float(j) / (numLEDs / 2 - 1)) : interpolateColor(colors[1], colors[2], float(j - numLEDs / 2) / (numLEDs / 2));
        }

        uint8_t r = ((gradientColor >> 16 & 0xFF) * brightness * currentMaxBrightness / 255);
        uint8_t g = ((gradientColor >> 8 & 0xFF) * brightness * currentMaxBrightness / 255);
        uint8_t b = ((gradientColor & 0xFF) * brightness * currentMaxBrightness / 255);

        strips[i].setPixelColor(j, strips[i].Color(r, g, b));
      }
      strips[i].show();
    }

    // Update brightness
    if (increasing)
    {
      brightness += brightnessStep;
      if (brightness >= 1.0)
      {
        brightness = 1.0;
        increasing = false;
      }
    }
    else
    {
      brightness -= brightnessStep;
      if (brightness <= 0.2)
      {
        brightness = 0.2;
        increasing = true;
      }
    }
  }
}

uint32_t interpolateColor(uint32_t color1, uint32_t color2, float ratio)
{
  uint8_t r = (uint8_t)(((color1 >> 16 & 0xFF) * (1 - ratio)) + ((color2 >> 16 & 0xFF) * ratio));
  uint8_t g = (uint8_t)(((color1 >> 8 & 0xFF) * (1 - ratio)) + ((color2 >> 8 & 0xFF) * ratio));
  uint8_t b = (uint8_t)(((color1 & 0xFF) * (1 - ratio)) + ((color2 & 0xFF) * ratio));
  return (r << 16) | (g << 8) | b;
}
