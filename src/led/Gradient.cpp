#include "Gradient.h"
#include "shared/LedStrip.h"

void setGradient(uint32_t topColor, uint32_t bottomColor)
{
  int numLEDs = strips[0].numPixels(); // Assuming all strips have the same number of LEDs

  for (int i = 0; i < NUM_STRIPS; i++)
  {
    for (int j = 0; j < numLEDs; j++)
    {
      float ratio = float(j) / float(numLEDs - 1);
      uint8_t r = (uint8_t)(((topColor >> 16 & 0xFF) * (1 - ratio)) + ((bottomColor >> 16 & 0xFF) * ratio));
      uint8_t g = (uint8_t)(((topColor >> 8 & 0xFF) * (1 - ratio)) + ((bottomColor >> 8 & 0xFF) * ratio));
      uint8_t b = (uint8_t)(((topColor & 0xFF) * (1 - ratio)) + ((bottomColor & 0xFF) * ratio));

      strips[i].setPixelColor(j, strips[i].Color(r, g, b));
    }
    strips[i].show();
  }
}
