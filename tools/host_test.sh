#!/bin/bash
# Compile the LED code on this computer (with a fake NeoPixel library) and run checks.
# Usage: tools/host_test.sh            unit checks for the power cap and color helpers
#        tools/host_test.sh parity     spooky animation power/coverage stats as JSON, and
#                                      frames for the emulator's C++ replay (?compare)
set -e
cd "$(dirname "$0")/.."
mkdir -p .pio
if [ "$1" = parity ]; then
  sources="test/host/spooky_parity.cpp src/shared/LedStrip.cpp src/shared/ColorUtils.cpp src/shared/Frame.cpp src/led/Spooky.cpp src/led/Fire.cpp src/led/Weather.cpp src/led/Creatures.cpp src/led/Patterns.cpp src/led/LiveDraw.cpp"
  args="tools/emulator/cpp_frames.json"
else
  sources="test/host/test.cpp src/shared/LedStrip.cpp src/shared/ColorUtils.cpp"
  args=""
fi
# The newest Command Line Tools SDK can mismatch its own linker; use the newest SDK that links.
for sdk in $(ls -dr /Library/Developer/CommandLineTools/SDKs/MacOSX[0-9]*.sdk 2>/dev/null) ""; do
  if clang++ ${sdk:+-isysroot "$sdk"} -std=c++17 -O2 -Itest/host -Isrc $sources -o .pio/host_test 2>.pio/host_test_build.log; then
    exec .pio/host_test $args
  fi
done
echo "Could not build host tests with any SDK:" >&2
cat .pio/host_test_build.log >&2
exit 1
