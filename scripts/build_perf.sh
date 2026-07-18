#!/bin/bash
# build_perf.sh — Compile (and optionally flash) the SpaceBadge firmware from CLI,
# reusing the arduino-cli that ships with the sibling ui_test project. Created for
# the perf/lovyangfx-dma-flush branch so frame-rate work can be built + flashed +
# measured autonomously (COM8).
#
# Usage:
#   scripts/build_perf.sh                 # compile only
#   scripts/build_perf.sh --upload        # compile + flash to $PORT (default COM8)
#   scripts/build_perf.sh --upload --port COM8
#
# Board matches the Visual Micro config: ESP32-S3 Dev Module, 16MB flash, OPI PSRAM,
# app3M_fat9M_16MB partitions, CDC-on-boot (so Serial prints reach the USB port).

set -o pipefail
REPO_DIR="$(cd "$(dirname "$0")/.." && pwd)"
UITEST_DIR="C:/Users/data/OneDrive/esp/ui_test"
ACLI="$UITEST_DIR/tools/arduino-cli.exe"
SKETCH="$REPO_DIR/src/ST7701_for_ESP32_WS_Driver_Board/ST7701_for_ESP32_WS_Driver_Board.ino"
LIBS="$REPO_DIR/src/libraries"
BUILD_PATH="$REPO_DIR/build/perf"
# Use the project's OWN partitions.csv (sketch dir) — NOT the built-in app3M scheme.
# The custom table defines the `spiffs` data partition (0x5F0000) that LittleFS mounts;
# the built-in scheme's layout differs and orphans the badge's data (=> default config
# => crash in applyConfig). Overridden below via build.partitions=partitions.
FQBN="esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB,DebugLevel=none"

UPLOAD=0
PORT="COM8"
DEFS=""
while [[ $# -gt 0 ]]; do
  case "$1" in
    --upload) UPLOAD=1; shift ;;
    --port)   PORT="$2"; shift 2 ;;
    --def)    DEFS+=" -D$2"; shift 2 ;;   # e.g. --def SPACEBADGE_FPS_BENCH=1
    *) echo "Unknown option: $1"; exit 1 ;;
  esac
done

mkdir -p "$BUILD_PATH"
echo "[BUILD] arduino-cli: $ACLI"
echo "[BUILD] sketch:      $SKETCH"
echo "[BUILD] libraries:   $LIBS"
echo "[BUILD] fqbn:        $FQBN"
echo "[BUILD] Compiling... (this is a large project; expect several minutes)"

EXTRA=()
if [[ -n "$DEFS" ]]; then
  echo "[BUILD] extra defines:$DEFS"
  EXTRA+=(--build-property "compiler.cpp.extra_flags=${DEFS# }")
  EXTRA+=(--build-property "compiler.c.extra_flags=${DEFS# }")
fi

"$ACLI" compile \
  --fqbn "$FQBN" \
  --libraries "$LIBS" \
  --build-path "$BUILD_PATH" \
  --build-property "build.partitions=partitions" \
  --build-property "upload.maximum_size=3080192" \
  "${EXTRA[@]}" \
  "$SKETCH"
RC=$?
if [[ $RC -ne 0 ]]; then
  echo "[BUILD] Compile FAILED (exit $RC)"
  exit $RC
fi
echo "[BUILD] Compile OK -> $BUILD_PATH"

if [[ $UPLOAD -eq 1 ]]; then
  echo "[BUILD] Uploading to $PORT ..."
  "$ACLI" upload --fqbn "$FQBN" -p "$PORT" --input-dir "$BUILD_PATH" "$SKETCH"
  RC=$?
  [[ $RC -ne 0 ]] && { echo "[BUILD] Upload FAILED (exit $RC)"; exit $RC; }
  echo "[BUILD] Upload OK"
fi
