#include "Frame.h"

RGB frame[NUM_STRIPS][NUM_LEDS_PER_STRIP];

static float clampChannel(float v)
{
  return v < 0 ? 0 : v > 255 ? 255 : v;
}

RGB rgb(float r, float g, float b)
{
  return RGB{r, g, b};
}

RGB hexColor(uint32_t hex)
{
  return RGB{(float)((hex >> 16) & 0xFF), (float)((hex >> 8) & 0xFF), (float)(hex & 0xFF)};
}

RGB mixColor(RGB a, RGB b, float t)
{
  return RGB{a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t};
}

RGB scaleColor(RGB c, float k)
{
  return RGB{c.r * k, c.g * k, c.b * k};
}

RGB hsvColor(float h, float s, float v)
{
  h = (h - 360 * floorf(h / 360)) / 60; // wrap into 0-360 without fmodf (hot path)
  int i = (int)h;
  float f = h - i;
  float p = v * (1 - s), q = v * (1 - s * f), t = v * (1 - s * (1 - f));
  float r, g, b;
  switch (i)
  {
  case 0: r = v, g = t, b = p; break;
  case 1: r = q, g = v, b = p; break;
  case 2: r = p, g = v, b = t; break;
  case 3: r = p, g = q, b = v; break;
  case 4: r = t, g = p, b = v; break;
  default: r = v, g = p, b = q; break;
  }
  return RGB{r * 255, g * 255, b * 255};
}

RGB paletteColor(const float *positions, const RGB *colors, int count, float x)
{
  x = x < 0 ? 0 : x > 1 ? 1 : x;
  for (int i = 1; i < count; i++)
  {
    if (x <= positions[i])
    {
      return mixColor(colors[i - 1], colors[i], (x - positions[i - 1]) / (positions[i] - positions[i - 1]));
    }
  }
  return colors[count - 1];
}

RGB gradientColor(const RGB *colors, int count, float pos)
{
  pos = fmodf(fmodf(pos, 1) + 1, 1);
  float segment = 1.0f / count;
  int i = (int)(pos / segment);
  if (i >= count)
    i = count - 1;
  RGB next = colors[(i + 1) % count];
  return mixColor(colors[i], next, (pos - i * segment) / segment);
}

void framePut(int s, int l, RGB c)
{
  if (s < 0 || s >= NUM_STRIPS || l < 0 || l >= NUM_LEDS_PER_STRIP)
    return;
  frame[s][l] = RGB{clampChannel(c.r), clampChannel(c.g), clampChannel(c.b)};
}

void frameAdd(int s, int l, RGB c)
{
  if (s < 0 || s >= NUM_STRIPS || l < 0 || l >= NUM_LEDS_PER_STRIP)
    return;
  RGB p = frame[s][l];
  framePut(s, l, RGB{p.r + c.r, p.g + c.g, p.b + c.b});
}

void frameFill(RGB c)
{
  for (int s = 0; s < NUM_STRIPS; s++)
    for (int l = 0; l < NUM_LEDS_PER_STRIP; l++)
      framePut(s, l, c);
}

void frameFade(float k)
{
  for (int s = 0; s < NUM_STRIPS; s++)
    for (int l = 0; l < NUM_LEDS_PER_STRIP; l++)
      frame[s][l] = scaleColor(frame[s][l], k);
}

void showFrame()
{
  for (int s = 0; s < NUM_STRIPS; s++)
  {
    for (int l = 0; l < NUM_LEDS_PER_STRIP; l++)
    {
      // setPixelColor applies the strip brightness; showStrip applies the power cap
      strips[s].setPixelColor(l, (uint8_t)frame[s][l].r, (uint8_t)frame[s][l].g, (uint8_t)frame[s][l].b);
    }
    showStrip(strips[s]);
  }
}

#define SIN_TABLE_SIZE 1024
static float sinTable[SIN_TABLE_SIZE + 1];
static bool sinTableReady = false;

float fastSin(float x)
{
  if (!sinTableReady)
  {
    for (int i = 0; i <= SIN_TABLE_SIZE; i++)
      sinTable[i] = sinf(i * 2 * PI / SIN_TABLE_SIZE);
    sinTableReady = true;
  }
  float index = x * (SIN_TABLE_SIZE / (2 * PI));
  float whole = floorf(index);
  int i = ((int)whole) & (SIN_TABLE_SIZE - 1); // wraps negative angles too
  return sinTable[i] + (sinTable[i + 1] - sinTable[i]) * (index - whole);
}

int randInt(int n)
{
  return n <= 0 ? 0 : (int)random(n);
}

float randFloat(float a, float b)
{
  return a + (b - a) * (random(1000000) / 1000000.0f);
}
