#include "shared/LedStrip.h"

// Global variables to keep track of the position and last update time
uint16_t j = 0; // This will track the overall position in the rainbow cycle
unsigned long previousMillis = 0;
const long interval = 10; // Interval at which to update the LEDs (adjustable)

// Generate rainbow colors across 0-255 positions.
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strips[0].Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strips[0].Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strips[0].Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Function to update the LEDs in a non-blocking manner
void setRainbowFlow()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis; // Update the last time we refreshed the LEDs

    for (int i = 0; i < NUM_STRIPS; i++)
    {
      for (int k = 0; k < strips[i].numPixels(); k++)
      {
        // Update each pixel in the strip with the next color in the wheel
        strips[i].setPixelColor(strips[i].numPixels() - 1 - k, Wheel(((k * 256 / strips[i].numPixels()) + j) & 255));
      }
      strips[i].show(); // Send the updated colors to the strip
    }

    j = (j + 1) % 256; // Increment j, wrapping around at 256
  }
}