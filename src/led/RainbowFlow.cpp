#include "shared/LedStrip.h"

// Generate rainbow colors across 0-255 positions.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strips[0].Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strips[0].Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strips[0].Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Function to update all LEDs to a shifting rainbow pattern
void setRainbowFlow() {
  uint16_t i, j;

  for (j=0; j<256*5; j++) { // 5 cycles of all colors on the wheel
    for (i=0; i < NUM_STRIPS; i++) {
      for (int k=0; k < strips[i].numPixels(); k++) {
        // Reverse the flow by inverting the order of LEDs within each strip
        strips[i].setPixelColor(strips[i].numPixels() - 1 - k, Wheel(((k * 256 / strips[i].numPixels()) + j) & 255));
      }
      strips[i].show();
    }
    delay(1);
  }
}
