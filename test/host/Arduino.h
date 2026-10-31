#pragma once
#include <cstdint>
#include <cstdlib>
typedef uint8_t byte;
#include <cmath>
#include <algorithm>
using std::max;
using std::min;
#define PI 3.14159265358979f
inline long random(long n) { return n <= 0 ? 0 : std::rand() % n; }
inline unsigned long millis() { return 0; }
inline unsigned long micros() { return 0; }
template <typename T> T constrain(T v, T lo, T hi) { return v < lo ? lo : v > hi ? hi : v; }
struct { template <typename... A> void printf(A...) {} } Serial;
