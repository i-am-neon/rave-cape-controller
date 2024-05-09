#include "FlowOneColor.h"
#include "shared/LedStrip.h"

extern int currentMaxBrightness;

void setFlowOneColor(uint32_t color)
{
  static unsigned long lastUpdate = 0;
  const long updateInterval = 30; // Quick updates for smooth flow
  static int currentPosition = 0; // Current position of the wave front

  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate > updateInterval)
  {
    lastUpdate = currentMillis;

    for (int strip = 0; strip < NUM_STRIPS; strip++)
    {
      for (int led = 0; led < NUM_LEDS_PER_STRIP; led++)
      {
        // Calculate the brightness based on the wave's position
        float brightness = 0.2; // Default minimum brightness
        int distanceFromWave = abs(currentPosition - led);

        if (distanceFromWave < 10)
        { // Wave width of 10 LEDs
          brightness = ((10 - distanceFromWave) / 10.0) * (currentMaxBrightness / 255.0);
        }

        // Apply the calculated brightness
        uint8_t r = (color >> 16 & 0xFF) * brightness;
        uint8_t g = (color >> 8 & 0xFF) * brightness;
        uint8_t b = (color & 0xFF) * brightness;

        strips[strip].setPixelColor(led, strips[strip].Color(r, g, b));
      }
      strips[strip].show();
    }

    currentPosition++;
    if (currentPosition >= NUM_LEDS_PER_STRIP)
    {
      currentPosition = 0; // Reset the position to loop the animation
    }
  }
}
