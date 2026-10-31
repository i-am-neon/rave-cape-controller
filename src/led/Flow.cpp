#include "Flow.h"
#include "shared/LedStrip.h"
#include "shared/ColorUtils.h"

// Global variables to keep track of the position and last update time
float flowPosition = 0; // This will track the overall position in the flow cycle
unsigned long previousMillisFlow = 0;
const long flowInterval = 20;      // 50 fps
const float flowStep = 0.4f;       // LEDs per update: 1 LED per 50 ms, as before

void setFlow(const std::vector<uint32_t> &colors)
{
  unsigned long currentMillis = millis();
  unsigned long elapsed = currentMillis - previousMillisFlow;
  if (elapsed >= flowInterval)
  {
    previousMillisFlow = currentMillis; // Update the last time we refreshed the LEDs
    // Move by the time that actually passed, so the speed holds even if a frame runs long
    float step = flowStep * min(elapsed, 100UL) / flowInterval;

    int numLEDs = strips[0].numPixels();

    for (int i = 0; i < NUM_STRIPS; i++)
    {
      for (int j = 0; j < numLEDs; j++)
      {
        // Calculate the index for the gradient in reverse order
        float position = fmodf(numLEDs + flowPosition - j, numLEDs) / numLEDs;
        uint32_t color = interpolateGradient(colors, position);
        strips[i].setPixelColor(j, color);
      }
      showStrip(strips[i]); // Send the updated colors to the strip
    }

    flowPosition = fmodf(flowPosition + step, numLEDs);
  }
}
