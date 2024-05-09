#include "EmanateOneColor.h"
#include "shared/LedStrip.h"

extern int currentMaxBrightness;

void setEmanateOneColor(uint32_t color)
{
  static unsigned long lastUpdate = 0;
  const long updateInterval = 100;
  const float maxDistance = NUM_LEDS_PER_STRIP / 2;
  static float currentRadius = 0;

  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate > updateInterval)
  {
    lastUpdate = currentMillis;

    for (int strip = 0; strip < NUM_STRIPS; strip++)
    {
      for (int led = 0; led < NUM_LEDS_PER_STRIP; led++)
      {
        float distanceFromCenter = abs((NUM_LEDS_PER_STRIP / 2) - led);
        float brightness = 0.1;
        if (distanceFromCenter <= currentRadius)
        {
          brightness = ((currentRadius - distanceFromCenter) / maxDistance) * (currentMaxBrightness / 255.0);
          brightness = std::max(0.1f, std::min(brightness, 1.0f)); // Use std:: to specify the namespace
        }

        uint8_t r = (color >> 16 & 0xFF) * brightness;
        uint8_t g = (color >> 8 & 0xFF) * brightness;
        uint8_t b = (color & 0xFF) * brightness;

        strips[strip].setPixelColor(led, strips[strip].Color(r, g, b));
      }
      strips[strip].show();
    }

    currentRadius += 1;
    if (currentRadius > NUM_LEDS_PER_STRIP)
    {
      currentRadius = 0;
    }
  }
}
