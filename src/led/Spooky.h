#ifndef SPOOKY_H
#define SPOOKY_H

#include <string>

// Halloween animations, ported 1:1 from tools/emulator/index.html. Each step function
// draws one frame into the frame buffer; t is seconds since the animation was chosen.
#define SPOOKY_TICK 0.02f // the emulator and firmware both step at 50 fps

struct SpookyAnimation
{
  const char *name;
  void (*step)(float t);
};

const SpookyAnimation *findSpookyAnimation(const std::string &name);
void runSpookyAnimation(const SpookyAnimation *animation); // call every loop()

void stepBonfire(float t);
void stepGhostFire(float t);
void stepWitchFire(float t);
void stepInferno(float t);
void stepHeartbeat(float t);
void stepGhostBreath(float t);
void stepBloodDrip(float t);
void stepEctoplasm(float t);
void stepCauldron(float t);
void stepBanshee(float t);
void stepGlitch(float t);
void stepReaper(float t);
void stepDigitalRain(float t);
void stepPumpkinSwirl(float t);
void stepCandyCorn(float t);
void stepSpectralVeil(float t);

#endif
