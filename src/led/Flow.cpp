#include "Flow.h"
#include "shared/LedStrip.h"
#include "shared/ColorUtils.h"

// Global variables to keep track of the position and last update time
uint16_t flowPosition = 0; // This will track the overall position in the flow cycle
unsigned long previousMillisFlow = 0;
const long flowInterval = 50; // Interval at which to update the LEDs (adjustable)

void setFlow(const std::vector<uint32_t> &colors)
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisFlow >= flowInterval)
  {
    previousMillisFlow = currentMillis; // Update the last time we refreshed the LEDs

    int numLEDs = strips[0].numPixels();

    for (int i = 0; i < NUM_STRIPS; i++)
    {
      for (int j = 0; j < numLEDs; j++)
      {
        // Calculate the index for the gradient in reverse order
        float position = (float)((numLEDs + flowPosition - j) % numLEDs) / numLEDs;
        uint32_t color = interpolateGradient(colors, position);
        strips[i].setPixelColor(j, color);
      }
      strips[i].show(); // Send the updated colors to the strip
    }

    flowPosition = (flowPosition + 1) % numLEDs; // Decrement flowPosition, wrapping around at numLEDs
  }
}

uint32_t interpolateGradient(const std::vector<uint32_t> &colors, float position)
{
  // Ensure smooth transition by extending the gradient to wrap around
  std::vector<uint32_t> extendedColors = colors;
  extendedColors.push_back(colors[0]);

  if (extendedColors.size() < 2)
    return extendedColors[0];

  float segmentLength = 1.0 / (extendedColors.size() - 1);
  int leftIndex = position / segmentLength;
  int rightIndex = leftIndex + 1;

  if (rightIndex >= extendedColors.size())
    rightIndex = 0;

  float localPosition = (position - (leftIndex * segmentLength)) / segmentLength;

  return interpolateColor(extendedColors[leftIndex], extendedColors[rightIndex], localPosition);
}
