#include "ColorUtils.h"

uint32_t interpolateColor(uint32_t color1, uint32_t color2, float ratio)
{
  uint8_t r = (uint8_t)(((color1 >> 16 & 0xFF) * (1 - ratio)) + ((color2 >> 16 & 0xFF) * ratio));
  uint8_t g = (uint8_t)(((color1 >> 8 & 0xFF) * (1 - ratio)) + ((color2 >> 8 & 0xFF) * ratio));
  uint8_t b = (uint8_t)(((color1 & 0xFF) * (1 - ratio)) + ((color2 & 0xFF) * ratio));
  return (r << 16) | (g << 8) | b;
}

uint32_t interpolateColorBasedOnCount(const std::vector<uint32_t> &colors, int index, int total)
{
  float ratio = float(index) / float(total - 1);
  if (colors.size() == 1)
  {
    return colors[0];
  }
  else if (colors.size() == 2)
  {
    return interpolateColor(colors[0], colors[1], ratio);
  }
  else if (colors.size() == 3)
  {
    if (index < total / 2)
    {
      return interpolateColor(colors[0], colors[1], 2 * ratio);
    }
    else
    {
      return interpolateColor(colors[1], colors[2], 2 * (ratio - 0.5));
    }
  }
  return 0; // Default color if no valid color setup
}
