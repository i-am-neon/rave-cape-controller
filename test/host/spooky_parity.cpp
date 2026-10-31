// Runs every spooky animation through the real frame -> power cap pipeline and prints
// average power and coverage as JSON, to compare against the emulator's JavaScript.
// Optionally dumps 100 frames per animation so the emulator can replay the C++ output.
#include <cstdio>
#include <cstdlib>
#include "led/Spooky.h"
#include "shared/Frame.h"
#include "led/LiveDraw.h"

static const char *ids[] = {"bonfire", "ghostfire", "witchfire", "inferno", "heartbeat", "ghost", "drip", "ecto",
                            "cauldron", "banshee", "glitch", "reaper", "rain", "pumpkin", "candycorn", "aurora"};
static const int TICKS = 1500;

int main(int argc, char **argv)
{
  std::srand(1);
  FILE *frames = argc > 1 ? std::fopen(argv[1], "w") : nullptr;
  if (frames)
    std::fprintf(frames, "{");
  std::printf("{");
  for (int i = 0; i < 16; i++)
  {
    const SpookyAnimation *animation = findSpookyAnimation(ids[i]);
    if (!animation)
    {
      std::fprintf(stderr, "missing animation %s\n", ids[i]);
      return 1;
    }
    for (auto &strip : frame)
      for (auto &p : strip)
        p = RGB{0, 0, 0};
    double load = 0, lit = 0;
    if (frames)
      std::fprintf(frames, "%s\"%s\":[", i ? "," : "", ids[i]);
    for (int tick = 0; tick < TICKS; tick++)
    {
      animation->step(tick * SPOOKY_TICK);
      showFrame();
      bool dump = frames && tick >= 1000 && tick < 1100;
      if (dump)
        std::fprintf(frames, "%s[", tick > 1000 ? "," : "");
      for (int s = 0; s < NUM_STRIPS; s++)
      {
        uint8_t *px = strips[s].getPixels();
        for (int l = 0; l < NUM_LEDS_PER_STRIP; l++)
        {
          int sum = px[l * 3] + px[l * 3 + 1] + px[l * 3 + 2];
          load += sum;
          lit += sum > 8;
          if (dump)
            std::fprintf(frames, "%s%d,%d,%d", s || l ? "," : "", px[l * 3], px[l * 3 + 1], px[l * 3 + 2]);
        }
      }
      if (dump)
        std::fprintf(frames, "]");
    }
    if (frames)
      std::fprintf(frames, "]");
    std::printf("%s\"%s\":{\"load\":%.3f,\"lit\":%.3f}", i ? "," : "", ids[i],
                load / TICKS / (NUM_STRIPS * MAX_CHANNEL_SUM_PER_STRIP),
                lit / TICKS / (NUM_STRIPS * NUM_LEDS_PER_STRIP));
  }
  std::printf("}\n");

  // Live draw checks, written to stderr so stdout stays JSON
  int failures = 0;
  auto check = [&](const char *name, bool ok) {
    std::fprintf(stderr, "%s draw: %s\n", ok ? "PASS" : "FAIL", name);
    failures += !ok;
  };
  auto clear = [] {
    for (auto &strip : frame)
      for (auto &p : strip)
        p = RGB{0, 0, 0};
  };
  clear();
  handleDrawPoints("0,0,0,ff0000"); // top of the leftmost strip
  check("point lights its LED at full color", frame[0][0].r == 255 && frame[0][0].g == 0);
  check("brush falls off with distance", frame[0][2].r > 0 && frame[0][2].r < frame[0][1].r);
  check("brush stays narrow across strips", frame[2][0].r == 0);
  handleDrawFingerUp("0");
  clear();
  handleDrawPoints("1,0,0,00ff00;1,1000,0,00ff00"); // one finger, left to right along the shoulders
  bool continuous = true;
  for (int s = 0; s < NUM_STRIPS; s++)
    continuous &= frame[s][0].g > 100;
  check("fast stroke is continuous across all strips", continuous);
  handleDrawFingerUp("1");
  clear();
  handleDrawPoints("2,500,500,ffffff;garbage;9,1,1,ffffff"); // bad entries are ignored
  float before = frame[3][30].r + frame[4][30].r;
  check("garbage and out-of-range fingers ignored", before > 0);
  for (int i = 0; i < 50; i++)
    stepLiveDraw(i * SPOOKY_TICK);
  float after = frame[3][30].r + frame[4][30].r;
  check("trail fades to ~5% (0.942^50) in 1 second", after < before * 0.06f && after > 0);
  if (failures)
    return 1;

  if (frames)
  {
    std::fprintf(frames, "}");
    std::fclose(frames);
  }
  return 0;
}
