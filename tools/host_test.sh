#!/bin/bash
# Compile the shared LED code on this computer (with a fake NeoPixel library) and run checks.
# Usage: tools/host_test.sh
set -e
cd "$(dirname "$0")/.."
# The newest Command Line Tools SDK can mismatch its own linker; use the newest SDK that links.
for sdk in $(ls -dr /Library/Developer/CommandLineTools/SDKs/MacOSX[0-9]*.sdk 2>/dev/null) ""; do
  if clang++ ${sdk:+-isysroot "$sdk"} -std=c++17 -Itest/host -Isrc \
      test/host/test.cpp src/shared/LedStrip.cpp src/shared/ColorUtils.cpp \
      -o .pio/host_test 2>/dev/null; then
    exec .pio/host_test
  fi
done
echo "Could not build host tests with any SDK" >&2
exit 1
