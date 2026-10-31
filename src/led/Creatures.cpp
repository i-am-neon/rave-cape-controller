#include "Spooky.h"
#include "shared/Frame.h"

#define S NUM_STRIPS
#define L NUM_LEDS_PER_STRIP

void stepBloodDrip(float t)
{
  struct Drop
  {
    bool active;
    int s;
    float p, v, hang;
  };
  static Drop drops[16];
  static float pool[S];

  frameFade(0.86f);
  if (randFloat(0, 1) < 0.05f)
  {
    for (Drop &d : drops)
    {
      if (!d.active)
      {
        d = Drop{true, randInt(S), 0, 0, randFloat(0.4f, 1.4f)};
        break;
      }
    }
  }
  for (Drop &d : drops)
  {
    if (!d.active)
      continue;
    if (d.hang > 0)
    {
      // Ooze out of the shoulder seam before falling
      d.hang -= SPOOKY_TICK;
      d.p = 2 * (1 - d.hang / 1.4f);
    }
    else
    {
      d.v += 0.06f;
      d.p += d.v;
    }
    for (int k = 0; k < 3; k++)
      frameAdd(d.s, (int)floorf(d.p) - k, scaleColor(rgb(200, 0, 0), 1 - k * 0.3f));
    if (d.p >= L - 1)
    {
      pool[d.s] = 1;
      d.active = false;
    }
  }
  for (int s = 0; s < S; s++)
  {
    pool[s] = max(0.15f, pool[s] * 0.995f);
    for (int k = 0; k < 4; k++)
      frameAdd(s, L - 1 - k, scaleColor(rgb(120, 0, 0), pool[s] * (1 - k / 4.0f)));
    for (int k = 0; k < 2; k++)
      frameAdd(s, k, rgb(40, 0, 0));
  }
}

void stepCauldron(float t)
{
  struct Bubble
  {
    bool active;
    int s, dead;
    float p, v, pop;
  };
  static Bubble bubbles[40];

  for (int s = 0; s < S; s++)
  {
    for (int l = 0; l < L; l++)
    {
      float y = l / (float)(L - 1);
      framePut(s, l, mixColor(rgb(12, 0, 22), rgb(0, 70, 5), powf(y, 2.2f)));
    }
  }
  if (randFloat(0, 1) < 0.18f)
  {
    for (Bubble &b : bubbles)
    {
      if (!b.active)
      {
        b = Bubble{true, randInt(S), 0, L - 1, randFloat(0.25f, 0.6f), randFloat(5, 40)};
        break;
      }
    }
  }
  for (Bubble &b : bubbles)
  {
    if (!b.active)
      continue;
    b.p -= b.v;
    int l = (int)floorf(b.p);
    if (b.p > b.pop)
    {
      frameAdd(b.s, l, rgb(70, 255, 40));
      frameAdd(b.s, l + 1, rgb(10, 90, 10));
    }
    else
    {
      // Pop into a puff of purple mist
      b.dead++;
      frameAdd(b.s, l, scaleColor(rgb(170, 60, 255), 1 - b.dead / 8.0f));
    }
    if (b.dead >= 8)
      b.active = false;
  }
}
