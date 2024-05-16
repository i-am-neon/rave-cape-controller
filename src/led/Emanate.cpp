#include "Emanate.h"
#include "shared/LedStrip.h"
#include "shared/ColorUtils.h"

void setEmanate(const std::vector<uint32_t> &colors)
{
  static unsigned long lastUpdate = 0;
  const long updateInterval = 50; // Interval for smoother animation
  static int currentRadius = 0;   // Current position of the wave front

  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate > updateInterval)
  {
    lastUpdate = currentMillis;

    int numLEDs = strips[0].numPixels();

    for (int strip = 0; strip < NUM_STRIPS; strip++)
    {
      for (int led = 0; led < numLEDs; led++)
      {
        // Calculate position from the center
        int distanceFromCenter = abs((numLEDs / 2) - led);
        float position = (float)((distanceFromCenter + currentRadius) % numLEDs) / numLEDs;

        // Interpolate color for the current LED position
        uint32_t gradientColor = interpolateGradient(colors, position);

        // Apply the color without changing brightness
        strips[strip].setPixelColor(led, gradientColor);
      }
      strips[strip].show();
    }

    currentRadius = (currentRadius + 1) % numLEDs; // Increment and wrap currentRadius
  }
}
