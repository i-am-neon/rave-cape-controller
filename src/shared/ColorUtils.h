#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <Arduino.h>
#include <vector>

uint32_t interpolateColor(uint32_t color1, uint32_t color2, float ratio);
uint32_t interpolateColorBasedOnCount(const std::vector<uint32_t> &colors, int index, int total);

#endif
