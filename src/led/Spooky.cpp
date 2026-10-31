#include "Spooky.h"
#include "shared/Frame.h"
#include "LiveDraw.h"

static const SpookyAnimation spookyAnimations[] = {
    {"bonfire", stepBonfire},
    {"ghostfire", stepGhostFire},
    {"witchfire", stepWitchFire},
    {"inferno", stepInferno},
    {"heartbeat", stepHeartbeat},
    {"ghost", stepGhostBreath},
    {"drip", stepBloodDrip},
    {"ecto", stepEctoplasm},
    {"cauldron", stepCauldron},
    {"banshee", stepBanshee},
    {"glitch", stepGlitch},
    {"reaper", stepReaper},
    {"rain", stepDigitalRain},
    {"pumpkin", stepPumpkinSwirl},
    {"candycorn", stepCandyCorn},
    {"aurora", stepSpectralVeil},
    {"draw", stepLiveDraw}, // live draw from the phone; points arrive via handleDrawPoints
};

const SpookyAnimation *findSpookyAnimation(const std::string &name)
{
  for (const SpookyAnimation &animation : spookyAnimations)
  {
    if (name == animation.name)
      return &animation;
  }
  return nullptr;
}

void runSpookyAnimation(const SpookyAnimation *animation)
{
  static const SpookyAnimation *running = nullptr;
  static unsigned long startMillis = 0;
  static unsigned long lastStep = 0;

  unsigned long now = millis();
  if (animation != running)
  {
    running = animation;
    startMillis = now;
    lastStep = now - 1000; // draw the first frame right away
  }
  if (now - lastStep < SPOOKY_TICK * 1000)
    return;

  lastStep = now;
  unsigned long frameStart = micros();
  animation->step((now - startMillis) / 1000.0f);
  showFrame();

  // Every 5 seconds, log how long a frame takes; the budget is SPOOKY_TICK (20 ms)
  static unsigned long lastReport = 0;
  if (now - lastReport > 5000)
  {
    lastReport = now;
    Serial.printf("%s: frame took %lu us\n", animation->name, micros() - frameStart);
  }
}
