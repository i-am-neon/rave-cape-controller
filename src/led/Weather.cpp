#include "Spooky.h"
#include "shared/Frame.h"

#define S NUM_STRIPS
#define L NUM_LEDS_PER_STRIP

void stepHeartbeat(float t)
{
  for (int s = 0; s < S; s++)
  {
    for (int l = 0; l < L; l++)
    {
      float d = hypotf((s - 3.5f) * 3, l - 12) / 60; // distance from the heart, in seconds of delay
      float ph = fmodf(fmodf(t * 1.05f - d, 1) + 1, 1);
      auto thump = [](float x) { return expf(-(x / 0.06f) * (x / 0.06f)); };
      float e = thump(ph) + thump(ph - 1) + 0.65f * thump(ph - 0.28f);
      framePut(s, l, mixColor(rgb(14, 0, 0), rgb(255, 0, 10), min(1.0f, e) * max(0.25f, 1 - d * 1.2f)));
    }
  }
}

void stepGhostBreath(float t)
{
  float breath = 0.5f - 0.5f * cosf(t * 2 * PI / 6);
  for (int s = 0; s < S; s++)
  {
    for (int l = 0; l < L; l++)
    {
      float wisp = 0.5f + 0.5f * sinf(l * 0.22f - t * 1.4f + s * 1.7f) * sinf(l * 0.09f + t * 0.6f + s * 0.9f);
      framePut(s, l, scaleColor(rgb(150, 200, 255), 0.05f + 0.55f * breath * wisp));
    }
  }
}

void stepSpectralVeil(float t)
{
  for (int s = 0; s < S; s++)
  {
    for (int l = 0; l < L; l++)
    {
      float n = sinf(s * 0.7f + t * 0.5f) + sinf(l * 0.09f - t * 0.8f + s * 0.3f) + sinf((s + l * 0.2f) * 0.5f + t * 0.3f);
      float v = max(0.0f, sinf(l * 0.15f + t * 0.9f + s * 0.5f + n));
      framePut(s, l, hsvColor(270 - 90 * (0.5f + n / 6), 0.9f, 0.12f + 0.8f * v * v));
    }
  }
}

void stepInferno(float t)
{
  static const float positions[4] = {0, 0.35f, 0.7f, 1};
  static const RGB colors[4] = {rgb(45, 0, 0), rgb(170, 0, 0), rgb(255, 90, 0), rgb(255, 210, 80)};
  for (int s = 0; s < S; s++)
  {
    for (int l = 0; l < L; l++)
    {
      // Two interlocking swirls spiralling up the cape; troughs stay deep crimson
      float angle = s / (float)S * 2 * PI;
      float swirl = sinf(angle + l * 0.16f + t * 3.2f);
      float swirl2 = sinf(angle * 2 - l * 0.07f + t * 1.7f + 1.3f);
      float flicker = 0.85f + 0.15f * sinf(t * 23 + s * 7.1f + l * 1.3f);
      float heat = 0.5f + 0.5f * (0.55f * swirl + 0.45f * swirl2);
      framePut(s, l, paletteColor(positions, colors, 4, powf(heat, 1.5f) * flicker));
    }
  }
}

void stepEctoplasm(float t)
{
  static const float positions[4] = {0, 0.4f, 0.75f, 1};
  static const RGB colors[4] = {rgb(30, 0, 50), rgb(120, 0, 200), rgb(40, 255, 80), rgb(200, 255, 200)};
  for (int s = 0; s < S; s++)
  {
    for (int l = 0; l < L; l++)
    {
      // Classic plasma: four interfering waves, one radiating from the middle of the back
      float x = s * 0.9f, y = l * 0.12f;
      float v = sinf(x + t * 1.3f) + sinf(y * 1.5f - t * 2.1f) + sinf((x + y) * 0.7f + t * 1.7f) +
                sinf(hypotf(x - 3, y - 3.5f) * 1.5f - t * 2);
      framePut(s, l, paletteColor(positions, colors, 4, powf((v + 4) / 8, 1.3f)));
    }
  }
}

void stepBanshee(float t)
{
  RGB ring = hsvColor(260 + 60 * sinf(t * 0.5f), 0.45f, 1);
  for (int s = 0; s < S; s++)
  {
    for (int l = 0; l < L; l++)
    {
      // Shockwave rings racing outward from the heart over a swirling violet storm
      float d = hypotf((s - 3.5f) * 3, l - 12);
      float crest = powf(max(0.0f, sinf(d * 0.35f - t * 6)), 3) * max(0.3f, 1 - d / 80);
      float swirl = 0.5f + 0.5f * sinf(s * 0.8f - l * 0.1f + t * 1.5f);
      framePut(s, l, mixColor(mixColor(rgb(25, 0, 45), rgb(70, 0, 130), swirl), ring, crest));
    }
  }
}
