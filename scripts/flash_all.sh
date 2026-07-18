#!/bin/bash
# flash_all.sh — flash the full image (bootloader + custom partition table +
# boot_app0 + app + LittleFS data) to the badge in ONE esptool call.
#
# WHY: `arduino-cli upload` only writes the app; it does NOT restore the LittleFS
# data partition. Flashing the app with the wrong partition scheme orphans the
# badge's config (LittleFS mount fails -> default config -> crash in applyConfig).
# This script flashes the project's OWN partitions.csv layout AND the data image,
# so the badge boots clean.
#
# PREREQ: the board must be in DOWNLOAD MODE, or a running/crash-looping firmware
# will make esptool time out ("Write timeout"). To enter download mode:
#   hold BOOT, tap RESET, release BOOT   (then run this).
#
# Build the artifacts first with:  scripts/build_perf.sh            (production, CDC)
#   or for serial-readable [FPS]:  a CDCOnBoot=default build into build/perf_uart
# and the data image with mklittlefs (see docs/perf-frame-rate.md).
#
#   scripts/flash_all.sh [--dir build/perf] [--port COM8] [--lfs build/spacebadge-littlefs.bin]

DIR="build/perf"
PORT="COM8"
LFS="build/spacebadge-littlefs.bin"
while [[ $# -gt 0 ]]; do
  case "$1" in
    --dir)  DIR="$2"; shift 2 ;;
    --port) PORT="$2"; shift 2 ;;
    --lfs)  LFS="$2"; shift 2 ;;
    *) echo "Unknown option: $1"; exit 1 ;;
  esac
done

REPO="$(cd "$(dirname "$0")/.." && pwd)"
# Locate esptool + boot_app0 dynamically (core version varies: 2.0.10 / 2.0.17 / ...).
ESPTOOL="$(ls "C:/Users/data/AppData/Local/Arduino15/packages/esp32/tools/esptool_py/"*/esptool.exe 2>/dev/null | head -1)"
BOOTAPP0="$(ls "C:/Users/data/AppData/Local/Arduino15/packages/esp32/hardware/esp32/"*/tools/partitions/boot_app0.bin 2>/dev/null | head -1)"
[ -z "$ESPTOOL" ] && { echo "esptool.exe not found under Arduino15"; exit 1; }
D="$REPO/$DIR"
BOOT="$D/ST7701_for_ESP32_WS_Driver_Board.ino.bootloader.bin"
PART="$D/ST7701_for_ESP32_WS_Driver_Board.ino.partitions.bin"
APP="$D/ST7701_for_ESP32_WS_Driver_Board.ino.bin"

for f in "$BOOT" "$PART" "$APP"; do
  [[ -f "$f" ]] || { echo "MISSING: $f (build first)"; exit 1; }
done

ARGS=(0x0 "$BOOT" 0x8000 "$PART" 0xe000 "$BOOTAPP0" 0x10000 "$APP")
if [[ -f "$REPO/$LFS" ]]; then
  ARGS+=(0x5F0000 "$REPO/$LFS")
  echo "[FLASH] including LittleFS data image ($LFS) @ 0x5F0000"
else
  echo "[FLASH] WARN: no LittleFS image at $LFS — badge will boot with default config"
fi

echo "[FLASH] port=$PORT  dir=$DIR"
echo "[FLASH] NOTE: board must be in DOWNLOAD MODE (hold BOOT, tap RESET, release BOOT)."
"$ESPTOOL" --chip esp32s3 --port "$PORT" --baud 460800 \
  --before default_reset --after hard_reset --connect-attempts 10 \
  write_flash -z --flash_mode keep --flash_freq keep --flash_size keep "${ARGS[@]}"
