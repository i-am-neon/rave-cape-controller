#include "Spooky.h"
#include "shared/Frame.h"

#define L NUM_LEDS_PER_STRIP

// Classic Fire2012 per strip. heat index 0 is the hem, so flames rise up the cape.
static void fireStep(float heat[NUM_STRIPS][L], int cooling, int sparking)
{
  for (int s = 0; s < NUM_STRIPS; s++)
  {
    for (int i = 0; i < L; i++)
      heat[s][i] = max(0.0f, heat[s][i] - (int)randFloat(0, (cooling * 10.0f) / L + 2));
    for (int k = L - 1; k >= 2; k--)
      heat[s][k] = (heat[s][k - 1] + heat[s][k - 2] * 2) / 3;
    if (randInt(255) < sparking)
    {
      int y = randInt(7);
      heat[s][y] = min(255.0f, heat[s][y] + randFloat(160, 255));
    }
  }
}

static void drawFire(float heat[NUM_STRIPS][L], const RGB colors[4])
{
  static const float positions[4] = {0, 0.35f, 0.7f, 1};
  for (int s = 0; s < NUM_STRIPS; s++)
    for (int i = 0; i < L; i++)
      framePut(s, L - 1 - i, paletteColor(positions, colors, 4, heat[s][i] / 255));
}

void stepBonfire(float t)
{
  static float heat[NUM_STRIPS][L];
  static const RGB colors[4] = {rgb(0, 0, 0), rgb(200, 0, 0), rgb(255, 110, 0), rgb(255, 220, 90)};
  fireStep(heat, 55, 120);
  drawFire(heat, colors);
}

void stepGhostFire(float t)
{
  static float heat[NUM_STRIPS][L];
  static const RGB colors[4] = {rgb(0, 0, 0), rgb(0, 10, 160), rgb(0, 170, 255), rgb(200, 240, 255)};
  fireStep(heat, 60, 110);
  drawFire(heat, colors);
}

void stepWitchFire(float t)
{
  static float heat[NUM_STRIPS][L];
  static const RGB colors[4] = {rgb(0, 0, 0), rgb(70, 0, 110), rgb(40, 200, 0), rgb(190, 255, 90)};
  // Witch fire's palette has its purple stop at 0.3 and green at 0.65
  static const float positions[4] = {0, 0.3f, 0.65f, 1};
  fireStep(heat, 50, 130);
  for (int s = 0; s < NUM_STRIPS; s++)
    for (int i = 0; i < L; i++)
      framePut(s, L - 1 - i, paletteColor(positions, colors, 4, heat[s][i] / 255));
}
