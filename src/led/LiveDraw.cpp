#include "LiveDraw.h"
#include "Spooky.h"
#include "shared/Frame.h"

// Same brush and trail as the app (RaveCapeMobile/cape/protocol.ts)
#define DRAW_FADE_PER_FRAME 0.942f
#define BRUSH_STRIP_SCALE 4.0f
#define BRUSH_RADIUS_SQUARED 25.0f
#define BRUSH_FALLOFF 4.0f
#define BRUSH_STAMP_SPACING 0.5f
#define MAX_FINGERS 4

struct Finger
{
  bool down;
  float strip, led;
};
static Finger fingers[MAX_FINGERS];

static void stampBrush(float strip, float led, RGB color)
{
  for (int s = 0; s < NUM_STRIPS; s++)
  {
    float dx = (s - strip) * BRUSH_STRIP_SCALE;
    if (dx * dx >= BRUSH_RADIUS_SQUARED)
      continue;
    for (int l = (int)floorf(led - 5); l <= (int)ceilf(led + 5); l++)
    {
      float d2 = dx * dx + (l - led) * (l - led);
      if (d2 < BRUSH_RADIUS_SQUARED)
        frameAdd(s, l, scaleColor(color, expf(-d2 / BRUSH_FALLOFF)));
    }
  }
}

static void drawPoint(int finger, float x, float y, RGB color)
{
  float strip = x * (NUM_STRIPS - 1), led = y * (NUM_LEDS_PER_STRIP - 1);
  Finger &f = fingers[finger];
  if (!f.down)
  {
    stampBrush(strip, led, color);
  }
  else
  {
    // Join to the previous point so fast strokes stay continuous
    float distance = hypotf((strip - f.strip) * BRUSH_STRIP_SCALE, led - f.led);
    int stamps = max(1, (int)ceilf(distance / BRUSH_STAMP_SPACING));
    for (int i = 1; i <= stamps; i++)
    {
      float k = (float)i / stamps;
      stampBrush(f.strip + (strip - f.strip) * k, f.led + (led - f.led) * k, color);
    }
  }
  f = Finger{true, strip, led};
}

void handleDrawPoints(const std::string &points)
{
  size_t pos = 0;
  while (pos < points.length())
  {
    size_t end = points.find(';', pos);
    if (end == std::string::npos)
      end = points.length();
    int finger, x, y;
    unsigned int hex;
    if (sscanf(points.substr(pos, end - pos).c_str(), "%d,%d,%d,%x", &finger, &x, &y, &hex) == 4 &&
        finger >= 0 && finger < MAX_FINGERS)
    {
      drawPoint(finger, constrain(x, 0, 1000) / 1000.0f, constrain(y, 0, 1000) / 1000.0f, hexColor(hex));
    }
    pos = end + 1;
  }
}

void handleDrawFingerUp(const std::string &finger)
{
  int f = atoi(finger.c_str());
  if (f >= 0 && f < MAX_FINGERS)
    fingers[f].down = false;
}

void stepLiveDraw(float t)
{
  frameFade(DRAW_FADE_PER_FRAME);
}
