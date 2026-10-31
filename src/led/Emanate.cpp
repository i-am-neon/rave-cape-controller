#include "Emanate.h"
#include "shared/LedStrip.h"
#include "shared/ColorUtils.h"

void setEmanate(const std::vector<uint32_t> &colors)
{
  static unsigned long lastUpdate = 0;
  const long updateInterval = 20;     // 50 fps
  const float radiusStep = 0.4f;      // LEDs per update: 1 LED per 50 ms, as before
  static float currentRadius = 0;     // Current position of the wave front

  unsigned long currentMillis = millis();
  unsigned long elapsed = currentMillis - lastUpdate;
  if (elapsed >= updateInterval)
  {
    lastUpdate = currentMillis;
    // Move by the time that actually passed, so the speed holds even if a frame runs long
    float step = radiusStep * min(elapsed, 100UL) / updateInterval;

    int numLEDs = strips[0].numPixels();

    for (int strip = 0; strip < NUM_STRIPS; strip++)
    {
      for (int led = 0; led < numLEDs; led++)
      {
        // Calculate position from the center
        int distanceFromCenter = abs((numLEDs / 2) - led);
        float position = fmodf(distanceFromCenter + currentRadius, numLEDs) / numLEDs;

        // Interpolate color for the current LED position
        uint32_t gradientColor = interpolateGradient(colors, position);

        // Apply the color without changing brightness
        strips[strip].setPixelColor(led, gradientColor);
      }
      showStrip(strips[strip]);
    }

    currentRadius = fmodf(currentRadius + step, numLEDs);
  }
}
