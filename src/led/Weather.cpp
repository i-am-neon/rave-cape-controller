#include "Spooky.h"
#include "shared/Frame.h"

#define S NUM_STRIPS
#define L NUM_LEDS_PER_STRIP

// These fill every LED every frame, so anything that doesn't depend on time (distances,
// angles, curves) is computed once, and sines come from fastSin, to stay inside the 20 ms
// frame budget alongside the ~14 ms it takes to send 480 LEDs.

// Distance of each LED from the heart (upper middle of the back), in LEDs
static float heartDistance[S][L];

static void computeHeartDistances()
{
  static bool ready = false;
  if (ready)
    return;
  for (int s = 0; s < S; s++)
    for (int l = 0; l < L; l++)
      heartDistance[s][l] = hypotf((s - 3.5f) * 3, l - 12);
  ready = true;
}

void stepHeartbeat(float t)
{
  // One beat (lub at 0, dub at 0.28 of the cycle), sampled once
  #define BEAT_STEPS 1024
  static float beat[BEAT_STEPS];
  static float falloff[S][L];
  static bool ready = false;
  if (!ready)
  {
    computeHeartDistances();
    auto thump = [](float x) { return expf(-(x / 0.06f) * (x / 0.06f)); };
    for (int i = 0; i < BEAT_STEPS; i++)
    {
      float ph = i / (float)BEAT_STEPS;
      beat[i] = min(1.0f, thump(ph) + thump(ph - 1) + 0.65f * thump(ph - 0.28f));
    }
    for (int s = 0; s < S; s++)
      for (int l = 0; l < L; l++)
        falloff[s][l] = max(0.25f, 1 - heartDistance[s][l] / 60 * 1.2f);
    ready = true;
  }

  for (int s = 0; s < S; s++)
  {
    for (int l = 0; l < L; l++)
    {
      float ph = t * 1.05f - heartDistance[s][l] / 60; // the beat ripples out from the heart
      ph -= floorf(ph);
      float e = beat[(int)(ph * BEAT_STEPS) & (BEAT_STEPS - 1)];
      framePut(s, l, mixColor(rgb(14, 0, 0), rgb(255, 0, 10), e * falloff[s][l]));
    }
  }
}

void stepGhostBreath(float t)
{
  float breath = 0.5f - 0.5f * fastCos(t * 2 * PI / 6);
  for (int s = 0; s < S; s++)
  {
    for (int l = 0; l < L; l++)
    {
      float wisp = 0.5f + 0.5f * fastSin(l * 0.22f - t * 1.4f + s * 1.7f) * fastSin(l * 0.09f + t * 0.6f + s * 0.9f);
      framePut(s, l, scaleColor(rgb(150, 200, 255), 0.05f + 0.55f * breath * wisp));
    }
  }
}

void stepSpectralVeil(float t)
{
  for (int s = 0; s < S; s++)
  {
    float stripWave = fastSin(s * 0.7f + t * 0.5f);
    for (int l = 0; l < L; l++)
    {
      float n = stripWave + fastSin(l * 0.09f - t * 0.8f + s * 0.3f) + fastSin((s + l * 0.2f) * 0.5f + t * 0.3f);
      float v = max(0.0f, fastSin(l * 0.15f + t * 0.9f + s * 0.5f + n));
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
    float angle = s / (float)S * 2 * PI;
    for (int l = 0; l < L; l++)
    {
      // Two interlocking swirls spiralling up the cape; troughs stay deep crimson
      float swirl = fastSin(angle + l * 0.16f + t * 3.2f);
      float swirl2 = fastSin(angle * 2 - l * 0.07f + t * 1.7f + 1.3f);
      float flicker = 0.85f + 0.15f * fastSin(t * 23 + s * 7.1f + l * 1.3f);
      float heat = 0.5f + 0.5f * (0.55f * swirl + 0.45f * swirl2);
      framePut(s, l, paletteColor(positions, colors, 4, heat * sqrtf(heat) * flicker)); // heat^1.5
    }
  }
}

void stepEctoplasm(float t)
{
  static const float positions[4] = {0, 0.4f, 0.75f, 1};
  static const RGB colors[4] = {rgb(30, 0, 50), rgb(120, 0, 200), rgb(40, 255, 80), rgb(200, 255, 200)};
  #define GOO_STEPS 256
  static float curve[GOO_STEPS + 1]; // x^1.3
  static float radius[S][L];
  static bool ready = false;
  if (!ready)
  {
    for (int i = 0; i <= GOO_STEPS; i++)
      curve[i] = powf(i / (float)GOO_STEPS, 1.3f);
    for (int s = 0; s < S; s++)
      for (int l = 0; l < L; l++)
        radius[s][l] = hypotf(s * 0.9f - 3, l * 0.12f - 3.5f) * 1.5f;
    ready = true;
  }

  float rowWave[L];
  for (int l = 0; l < L; l++)
    rowWave[l] = fastSin(l * 0.12f * 1.5f - t * 2.1f);
  for (int s = 0; s < S; s++)
  {
    float x = s * 0.9f;
    float stripWave = fastSin(x + t * 1.3f);
    for (int l = 0; l < L; l++)
    {
      // Classic plasma: four interfering waves, one radiating from the middle of the back
      float y = l * 0.12f;
      float v = stripWave + rowWave[l] + fastSin((x + y) * 0.7f + t * 1.7f) + fastSin(radius[s][l] - t * 2);
      float n = constrain((v + 4) / 8, 0.0f, 1.0f) * GOO_STEPS;
      int i = (int)n;
      float shaped = i >= GOO_STEPS ? curve[GOO_STEPS] : curve[i] + (curve[i + 1] - curve[i]) * (n - i);
      framePut(s, l, paletteColor(positions, colors, 4, shaped));
    }
  }
}

void stepBanshee(float t)
{
  static float falloff[S][L];
  static bool ready = false;
  if (!ready)
  {
    computeHeartDistances();
    for (int s = 0; s < S; s++)
      for (int l = 0; l < L; l++)
        falloff[s][l] = max(0.3f, 1 - heartDistance[s][l] / 80);
    ready = true;
  }

  RGB ring = hsvColor(260 + 60 * fastSin(t * 0.5f), 0.45f, 1);
  for (int s = 0; s < S; s++)
  {
    for (int l = 0; l < L; l++)
    {
      // Shockwave rings racing outward from the heart over a swirling violet storm
      float wave = max(0.0f, fastSin(heartDistance[s][l] * 0.35f - t * 6));
      float crest = wave * wave * wave * falloff[s][l];
      float swirl = 0.5f + 0.5f * fastSin(s * 0.8f - l * 0.1f + t * 1.5f);
      framePut(s, l, mixColor(mixColor(rgb(25, 0, 45), rgb(70, 0, 130), swirl), ring, crest));
    }
  }
}
