#include "shared/LedStrip.h"

void setAllLEDsToRed()
{
  for (int i = 0; i < NUM_STRIPS; i++)
  {
    for (int j = 0; j < strips[i].numPixels(); j++)
    {
      strips[i].setPixelColor(j, strips[i].Color(255, 0, 0)); // Set color to red
    }
    strips[i].show();
  }
}
