#ifndef FLOW_H
#define FLOW_H

#include <vector>
#include <stdint.h>

void setFlow(const std::vector<uint32_t> &colors);
uint32_t interpolateGradient(const std::vector<uint32_t> &colors, float position);

#endif
