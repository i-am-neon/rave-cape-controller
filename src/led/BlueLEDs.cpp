#include "shared/LedStrip.h"

void setAllLEDsToBlue()
{
  for (int i = 0; i < NUM_STRIPS; i++)
  {
    for (int j = 0; j < strips[i].numPixels(); j++)
    {
      strips[i].setPixelColor(j, strips[i].Color(0, 0, 255)); // Set color to blue
    }
    strips[i].show();
  }
}
