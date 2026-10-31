#include "Spooky.h"
#include "shared/Frame.h"

#define S NUM_STRIPS
#define L NUM_LEDS_PER_STRIP

void stepGlitch(float t)
{
  static float blackoutUntil = 0;
  if (blackoutUntil > t + 10) // the animation was restarted
    blackoutUntil = 0;
  if (blackoutUntil > t)
  {
    frameFill(rgb(0, 0, 0));
    return;
  }
  if (randFloat(0, 1) < 0.004f)
    blackoutUntil = t + randFloat(0.1f, 0.4f);

  for (int s = 0; s < S; s++)
    for (int l = 0; l < L; l++)
      framePut(s, l, scaleColor(rgb(40, 0, 70), 0.7f + 0.3f * sinf(l * 0.5f + t * 9 + s)));

  static const RGB glitchColors[3] = {rgb(255, 255, 255), rgb(255, 0, 40), rgb(0, 255, 230)};
  int segments = randFloat(0, 1) < 0.25f ? 1 + randInt(3) : 0;
  for (int i = 0; i < segments; i++)
  {
    int s = randInt(S), start = randInt(L), length = 3 + randInt(14);
    RGB color = glitchColors[randInt(3)];
    for (int l = start; l < start + length; l++)
      framePut(s, l, color);
  }
}

void stepReaper(float t)
{
  frameFade(0.9f);
  float pos = (L - 1) * (0.5f - 0.5f * cosf(t * 1.1f));
  for (int s = 0; s < S; s++)
    for (int k = -2; k <= 2; k++)
      frameAdd(s, (int)roundf(pos) + k, scaleColor(rgb(190, 140, 255), 1 - abs(k) / 3.0f));
}

void stepDigitalRain(float t)
{
  struct Drop
  {
    int s;
    float p, v;
  };
  static Drop drops[S * 2];
  static bool ready = false;
  if (!ready)
  {
    for (int i = 0; i < S * 2; i++)
      drops[i] = Drop{i % S, (float)-randInt(L), randFloat(0.3f, 0.9f)};
    ready = true;
  }

  frameFade(0.88f);
  for (Drop &d : drops)
  {
    d.p += d.v;
    if (d.p > L + 5)
    {
      d.p = -randInt(20);
      d.v = randFloat(0.3f, 0.9f);
    }
    int l = (int)floorf(d.p);
    framePut(d.s, l, rgb(170, 255, 170));
    frameAdd(d.s, l - 1, rgb(0, 140, 20));
  }
}

void stepPumpkinSwirl(float t)
{
  static const RGB colors[2] = {hexColor(0xff5000), hexColor(0x6a00ff)};
  for (int s = 0; s < S; s++)
    for (int l = 0; l < L; l++)
      framePut(s, l, gradientColor(colors, 2, (l + s * 5 - t * 18) / 40));
}

void stepCandyCorn(float t)
{
  static const RGB colors[3] = {hexColor(0xffd000), hexColor(0xff5a00), hexColor(0xfff0d0)};
  for (int s = 0; s < S; s++)
  {
    for (int l = 0; l < L; l++)
    {
      float x = fmodf(fmodf((l - t * 9) / 7, 3) + 3, 3);
      int i = (int)x % 3;
      float f = x - (int)x;
      framePut(s, l, f < 0.8f ? colors[i] : mixColor(colors[i], colors[(i + 1) % 3], (f - 0.8f) / 0.2f));
    }
  }
}
