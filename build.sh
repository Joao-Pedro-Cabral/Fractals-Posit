#!/usr/bin/env bash

# Purpose: reproducible build/run of Fractals-Posit-Fork with Boost cpp_dec_float_1000 baseline
#
# Usage examples:
#   ./build.sh                         # clean + build only
#   RUN_BASELINE=1 ./build.sh          # also run baseline image with timing
#   RUN_ALL=1 ./build.sh               # run all dtypes for the default view (can be slow!)
#
# Environment variables you can override:
#   UNIVERSAL_INC  -> path that contains sw/universal/... headers (default set below)
#   SOFTPOSIT_ROOT -> path to soft-posit-cpp (must contain include/ and src/)
#   BOOST_INC      -> path to Boost headers if not in default include path
#   TIME_CMD       -> timing tool; defaults to '/usr/bin/time -l' on macOS, 'time' elsewhere

set -euo pipefail

# --- Detect platform timing command ---
if [[ "$(uname -s)" == "Darwin" ]]; then
  : "${TIME_CMD:=/usr/bin/time -l}"
else
  : "${TIME_CMD:=/usr/bin/time}"
fi

# --- Absolute paths (defaults for this workspace) ---
REPO_ROOT="/Users/inbasekaranperumal/Developer/OpenSource/LFX/jp-fractals-fork/Fractals-Posit-Fork"
UNIVERSAL_INC_DEFAULT="/Users/inbasekaranperumal/Developer/OpenSource/LFX/universal/include/sw"
SOFTPOSIT_ROOT_DEFAULT="/Users/inbasekaranperumal/Developer/OpenSource/LFX/soft-posit-cpp"
BOOST_INC_DEFAULT="/opt/homebrew/include"

# Allow overrides via environment variables
: "${UNIVERSAL_INC:=${UNIVERSAL_INC_DEFAULT}}"
: "${SOFTPOSIT_ROOT:=${SOFTPOSIT_ROOT_DEFAULT}}"
: "${BOOST_INC:=${BOOST_INC_DEFAULT}}"

echo "[info] Repository:            $REPO_ROOT"
echo "[info] UNIVERSAL_INC:         $UNIVERSAL_INC"
echo "[info] SOFTPOSIT_ROOT:        $SOFTPOSIT_ROOT"
echo "[info] BOOST_INC:             $BOOST_INC"

cd "$REPO_ROOT"

echo "[step] Clean build directory"
make clean || true

echo "[step] Compile fractal binary (with SoftPosit static lib)"
make BOOST_INC="$BOOST_INC" UNIVERSAL_INC="$UNIVERSAL_INC" SOFTPOSIT_ROOT="$SOFTPOSIT_ROOT" compile

echo "[done] Build completed: $REPO_ROOT/build/fractal"

# --- Optional runs ---
if [[ "${RUN_BASELINE:-0}" == "1" ]]; then
  echo "[run] Baseline cpp_dec_float_1000 mandelbrot -0.759 0 2.5 (this can be slow)"
  (cd build && $TIME_CMD ./fractal cpp_dec_float_1000 mandelbrot -0.759 0 2.5)
fi

if [[ "${RUN_ALL:-0}" == "1" ]]; then
  echo "[run] All dtypes for default view (can take a long time depending on baseline)"
  (cd build && $TIME_CMD ./fractal all mandelbrot -0.759 0 2.5)
fi

echo "[hint] To compare images, run: make compare"