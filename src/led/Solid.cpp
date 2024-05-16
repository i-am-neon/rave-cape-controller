#include "Solid.h"
#include "shared/LedStrip.h"
#include "shared/ColorUtils.h"

void setSolid(const std::vector<uint32_t> &colors)
{
  int numLEDs = strips[0].numPixels();

  for (int i = 0; i < NUM_STRIPS; i++)
  {
    for (int j = 0; j < numLEDs; j++)
    {
      uint32_t color = interpolateColorBasedOnCount(colors, j, numLEDs);
      strips[i].setPixelColor(j, color);
    }
    strips[i].show();
  }
}
