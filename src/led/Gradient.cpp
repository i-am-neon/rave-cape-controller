#include "Gradient.h"
#include "shared/LedStrip.h"

void setGradient(const std::vector<uint32_t> &colors)
{
  int numLEDs = strips[0].numPixels(); // Assuming all strips have the same number of LEDs

  for (int i = 0; i < NUM_STRIPS; i++)
  {
    for (int j = 0; j < numLEDs; j++)
    {
      float ratio = float(j) / float(numLEDs - 1);
      uint32_t color = 0;
      if (colors.size() == 1)
      {
        color = colors[0];
      }
      else if (colors.size() == 2)
      {
        // Linear interpolation between two colors
        uint8_t r = (uint8_t)(((colors[0] >> 16 & 0xFF) * (1 - ratio)) + ((colors[1] >> 16 & 0xFF) * ratio));
        uint8_t g = (uint8_t)(((colors[0] >> 8 & 0xFF) * (1 - ratio)) + ((colors[1] >> 8 & 0xFF) * ratio));
        uint8_t b = (uint8_t)(((colors[0] & 0xFF) * (1 - ratio)) + ((colors[1] & 0xFF) * ratio));
        color = strips[i].Color(r, g, b);
      }
      else if (colors.size() == 3)
      {
        // Tri-point gradient interpolation
        if (j < numLEDs / 2)
        {
          ratio = float(j) / float(numLEDs / 2 - 1);
          uint8_t r = (uint8_t)(((colors[0] >> 16 & 0xFF) * (1 - ratio)) + ((colors[1] >> 16 & 0xFF) * ratio));
          uint8_t g = (uint8_t)(((colors[0] >> 8 & 0xFF) * (1 - ratio)) + ((colors[1] >> 8 & 0xFF) * ratio));
          uint8_t b = (uint8_t)(((colors[0] & 0xFF) * (1 - ratio)) + ((colors[1] & 0xFF) * ratio));
          color = strips[i].Color(r, g, b);
        }
        else
        {
          ratio = float(j - numLEDs / 2) / float(numLEDs / 2);
          uint8_t r = (uint8_t)(((colors[1] >> 16 & 0xFF) * (1 - ratio)) + ((colors[2] >> 16 & 0xFF) * ratio));
          uint8_t g = (uint8_t)(((colors[1] >> 8 & 0xFF) * (1 - ratio)) + ((colors[2] >> 8 & 0xFF) * ratio));
          uint8_t b = (uint8_t)(((colors[1] & 0xFF) * (1 - ratio)) + ((colors[2] & 0xFF) * ratio));
          color = strips[i].Color(r, g, b);
        }
      }
      strips[i].setPixelColor(j, color);
    }
    strips[i].show();
  }
}
