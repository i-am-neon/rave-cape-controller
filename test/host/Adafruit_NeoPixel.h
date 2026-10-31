#pragma once
#include <cstdint>
#include <vector>
#define NEO_GRB 0
#define NEO_KHZ800 0
struct Adafruit_NeoPixel {
  std::vector<uint8_t> buf; uint16_t n; int shows=0;
  Adafruit_NeoPixel(uint16_t n, int, int) : buf(n*3), n(n) {}
  void begin() {} void setBrightness(uint8_t) {} void show() { shows++; }
  uint8_t *getPixels() { return buf.data(); } uint16_t numPixels() { return n; }
  void setPixelColor(uint16_t i, uint8_t r, uint8_t g, uint8_t b) { buf[i*3]=r; buf[i*3+1]=g; buf[i*3+2]=b; }
  void setPixelColor(uint16_t i, uint32_t c) { buf[i*3]=c>>16; buf[i*3+1]=c>>8; buf[i*3+2]=c; }
};
