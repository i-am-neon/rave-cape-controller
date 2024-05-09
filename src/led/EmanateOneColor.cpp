#include "EmanateOneColor.h"
#include "shared/LedStrip.h"

extern int currentMaxBrightness;

void setEmanateOneColor(uint32_t color)
{
  static unsigned long lastUpdate = 0;
  const long updateInterval = 50; // Reduced interval for smoother animation
  const int maxDistance = NUM_LEDS_PER_STRIP / 2;
  static int currentRadius = 0; // Current position of the wave front
  const int waveWidth = 10;     // Width of the wave of brightness

  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate > updateInterval)
  {
    lastUpdate = currentMillis;

    for (int strip = 0; strip < NUM_STRIPS; strip++)
    {
      for (int led = 0; led < NUM_LEDS_PER_STRIP; led++)
      {
        // Calculate distance from the center
        int distanceFromCenter = abs((NUM_LEDS_PER_STRIP / 2) - led);
        float brightness = 0.2; // Start all LEDs at 10% brightness

        // Calculate brightness based on position in the wave
        if (distanceFromCenter <= currentRadius && distanceFromCenter >= currentRadius - waveWidth)
        {
          brightness = ((waveWidth - (currentRadius - distanceFromCenter)) / (float)waveWidth) * (currentMaxBrightness / 255.0);
        }
        else if (distanceFromCenter < currentRadius - waveWidth)
        {
          brightness = 0.2; // Dim the LEDs that are not part of the current wave
        }

        // Apply the calculated brightness
        uint8_t r = (color >> 16 & 0xFF) * brightness;
        uint8_t g = (color >> 8 & 0xFF) * brightness;
        uint8_t b = (color & 0xFF) * brightness;

        strips[strip].setPixelColor(led, strips[strip].Color(r, g, b));
      }
      strips[strip].show();
    }

    currentRadius++;
    if (currentRadius - waveWidth > maxDistance)
    {
      currentRadius = 0; // Reset the wave after it has fully propagated
    }
  }
}
