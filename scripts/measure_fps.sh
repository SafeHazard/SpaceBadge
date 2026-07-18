#!/bin/bash
# measure_fps.sh — capture the [FPS] serial telemetry the perf build emits.
#
#   scripts/measure_fps.sh                # just capture COM8 for ~25s
#   scripts/measure_fps.sh --seconds 30   # capture for 30s
#   scripts/measure_fps.sh --port COM8
#
# The perf firmware (SPACEBADGE_FPS_LOG=1) prints once/sec over USB-CDC:
#   [FPS] render=NN flush/s=MMM MB/s=X.X (bufrows=R)
# render = display refreshes/sec (true visible frame rate).
ACLI="C:/Users/data/OneDrive/esp/ui_test/tools/arduino-cli.exe"
PORT="COM8"
SECONDS_CAP=25
while [[ $# -gt 0 ]]; do
  case "$1" in
    --port) PORT="$2"; shift 2 ;;
    --seconds) SECONDS_CAP="$2"; shift 2 ;;
    *) echo "Unknown option: $1"; exit 1 ;;
  esac
done

OUT="$(dirname "$0")/../build/fps_capture.txt"
echo "[MEASURE] capturing $PORT for ${SECONDS_CAP}s -> $OUT"
# arduino-cli monitor streams until killed; run it, kill after the window.
timeout "${SECONDS_CAP}" "$ACLI" monitor -p "$PORT" -c baudrate=115200 --quiet > "$OUT" 2>/dev/null
echo "[MEASURE] --- [FPS] lines ---"
grep "\[FPS\]" "$OUT" || echo "(no [FPS] lines seen — is the perf firmware flashed? is a game/animation on screen to force refreshes?)"
