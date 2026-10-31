#ifndef LIVE_DRAW_H
#define LIVE_DRAW_H

#include <string>

// Live draw from the phone. Messages (see RaveCapeMobile/cape/protocol.ts):
//   "p:<finger>,<x 0-1000>,<y 0-1000>,<rrggbb>;..."  brush points; x across strips, y shoulders to hem
//   "u:<finger>"                                     finger lifted, its next point starts a new stroke
void handleDrawPoints(const std::string &points);
void handleDrawFingerUp(const std::string &finger);
void stepLiveDraw(float t);

#endif
