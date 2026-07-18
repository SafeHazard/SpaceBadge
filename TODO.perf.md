# SpaceBadge — Frame Rate Optimization (branch: perf/lovyangfx-dma-flush)

Goal: raise the badge UI frame rate. Current display path is a hand-rolled raw-SPI
ST7789 driver with a **blocking, non-DMA flush**, **single 1/16-screen LVGL buffer**,
and **slow `digitalWrite` CS/DC**. Reference: `C:\Users\data\OneDrive\esp\ui_test`
runs the **identical hardware** (Waveshare ESP32-S3-Touch-LCD-2.8, ST7789, same pins)
using **LovyanGFX DMA + double-buffering** via `libs/LovyanInit-Waveshare/ws_lcd_setup.cpp`.

Hardware test: **COM8 available for flashing** — can measure real before/after FPS.

## Findings (confirmed)
- Bottleneck = `Display_ST7789.cpp::LCD_WriteData_nbyte` → blocking `transferBytes`,
  per-flush `digitalWrite` CS/DC, single partial buffer (`Display_Driver.cpp`).
- Same board as ui_test → LovyanGFX config is drop-in (pins match exactly).
- SpaceBadge runs **portrait 240x320** (EEZ UI); ui_test runs landscape 320x240.
  Migration must keep rotation 0 / 240x320.

## Queue
- [x] Create branch, confirm bottleneck, confirm hardware match
- [x] Harvest ui_test agent notes (research agent) — see findings below
- [x] Confirm lv_conf color format / byte-swap — IDENTICAL to ui_test (16bpp, no swap) => ui_test flush is copy-safe
- [x] Implement LovyanGFX DMA flush + double-buffer (portrait) in Display_Driver.cpp — DONE
- [x] Add FPS instrumentation (serial, REFR_READY render-rate + flush throughput) — DONE (SPACEBADGE_FPS_LOG)
- [~] Build+flash toolchain: arduino-cli's esp32 2.0.10 core is BROKEN for WiFi link
      (bare WiFi.begin() sketch also fails: undefined `__wrap_esp_wifi_*`). Repairing via
      `core install --force` (in progress). User's Visual Micro build is unaffected (separate toolchain).
- [ ] Build changed firmware → flash COM8 → measure FPS delta
- [ ] Expert review of the change before merge
- [ ] Document; push branch (do NOT merge to main until hardware-validated)

## Key facts harvested from ui_test
- Same hardware (Waveshare ESP32-S3-Touch-LCD-2.8, ST7789, same pins). ui_test uses
  LovyanGFX + DMA. Known-good color: invert=true, rgb_order=false, no LVGL byte swap.
- ui_test "143 fps" = lv_timer_handler CALL RATE, not display refresh. Real display
  refresh is capped at ~30 Hz by `LV_DEF_REFR_PERIOD 33`. This badge's baseline ~24 fps
  (sketchbook literally named "dc33-24fps") means the blocking flush can't even hit the cap.
- ui_test uses FULL double-buffer in PSRAM but NEVER measured it vs internal-RAM partial.
  Agent flagged DMA-from-PSRAM as a known pitfall => I chose internal-RAM partial double
  buffer (likely faster). Unexplored territory = a chance to beat the reference.
- ui_test flush is BLOCKING (endWrite waits). Async flush_ready-on-DMA-done is an
  un-exploited further lever.

## Levers (in priority order)
1. DMA flush + double buffer  ....... DONE (this change) — frees CPU, overlaps render/transfer
2. Raise LV_DEF_REFR_PERIOD 33->~16 .. one-line lv_conf; only helps IF flush keeps up. MEASURE FIRST.
3. Async flush (pushImageDMA + immediate flush_ready) .. deeper pipelining. MEASURE FIRST.
4. Buffer size tuning (DISP_BUF_ROWS) ................... quick sweep once measurable.

## Toolchain fix (done)
- arduino-cli's esp32 core 2.0.10 was BROKEN (bare WiFi.begin() wouldn't link: missing
  `__wrap_esp_wifi_*`). Installed 2.0.17 -> links clean. arduino-cli private dir only;
  Visual Micro toolchain untouched. Build+flash of full firmware to COM8 works (verified).

## Expert review outcome (clean)
- NO Critical/High display-correctness bugs. Color verified correct vs ui_test reference.
  Rotation correct for the badge's actual 0/180 usage. Buffers/DMA/bus/backlight/touch all safe.
- FIXED M1: removed redundant lv_display_set_rotation() (90/270 footgun; LovyanGFX owns rotation).
- FIXED L2: corrected the false "double buffer overlaps render/DMA" comment. Blocking flush =>
  2nd buffer gives NO overlap today; real win is DMA + batched SPI vs bit-banged CS/DC.
  => ASYNC flush (pushImageDMA + deferred flush_ready) is the next real lever.
- FIXED L3: static_assert on buffer alignment.
- L4: ship production with SPACEBADGE_FPS_LOG 0.

## Measurement harness (done)
- Display_Driver.cpp now has: USE_LOVYAN_FLUSH (1=lovyan/0=raw baseline, also a prod revert),
  1 Hz heartbeat [FPS] meter via lv_timer (render fps / flush-per-sec / MB/s / us-per-flush),
  SPACEBADGE_FPS_BENCH (force full-screen redraw for max-throughput A/B).
- scripts/build_perf.sh (--upload --port --def X=Y), scripts/measure_fps.sh (capture COM8; DTR/RTS off).
- Measure gotcha: monitor MUST use dtr=off rts=off or the S3 native-USB-CDC resets and emits nothing.

## BLOCKED — needs user physical action
- The badge's USB port is WEDGED (esptool/arduino-cli "Write timeout" on connect). Cause:
  I flashed with the wrong partition scheme (built-in app3M_fat9M_16MB instead of the
  project's custom partitions.csv), which orphaned the LittleFS data -> default config ->
  a PRE-EXISTING null-deref crash in calculateUnlockedAvatars (avatar_unlocks.cpp:319,
  reads config.games[0..5] when default config has fewer). The crash-loop resets the
  USB every ~500ms so the OUT endpoint is never serviced -> writes time out. (Reads work;
  I captured the crash backtrace to prove it's NOT the display change.)
- RECOVERY (user): put the board in DOWNLOAD MODE (hold BOOT, tap RESET, release BOOT),
  then either:
    * `bash scripts/flash_all.sh --dir build/perf_uart`   (flashes app + correct
      partition table + the rebuilt LittleFS data image -> boots clean, serial [FPS] on COM8)
    * or restore the shipping firmware from Binaries/spacebadge_1.1.1-3m.bin.
  Everything to flash is already built (build/perf_uart) and the data image is at
  build/spacebadge-littlefs.bin.

## What's DONE and validated
- Code committed (485d447). Compiles + links (esp32 2.0.17). Adversarial review clean
  (no High/Critical; color + 0/180 rotation verified vs ui_test reference). BOOT-VERIFIED
  on hardware (saw boot logs before the unrelated config crash).
- FIXED build config: build_perf.sh now uses the project's partitions.csv (build.partitions).

## MEASURED RESULTS (on hardware, COM8) — honest, and they revise the premise
- Raw driver (baseline), real-world: **~28.5 fps** (pinned at LVGL's 30 Hz cap).
- LovyanGFX DMA, real-world: **~29-30 fps** (same 30 Hz cap).
  => The flush was NOT the frame-rate bottleneck. Swapping to LovyanGFX alone does
     essentially nothing to fps; both hit the LV_DEF_REFR_PERIOD=33ms (~30 Hz) ceiling.
- Per-flush (bench, forced full-screen): raw ~1503us/9600B (~6.1 MB/s) vs lovyan
  ~2748us/19200B (~6.8 MB/s). Both are SPI-clock-bound (80 MHz); DMA is only ~10%/byte
  faster. Full-screen complex redraw is RENDER-bound (~1 fps both), not flush-bound.
- REAL lever = lower LV_DEF_REFR_PERIOD (33->16 = 60 Hz cap). Testing lovyan+refr16 now.
  The DMA flush's value is HEADROOM to sustain a higher refresh rate + future async flush.
- The "dc33-24fps" baseline in the sketchbook name doesn't match my ~28.5 measurement
  (old/heavier scenario, or measured differently).

## STILL PENDING (needs the badge reflashable)
- Read the actual [FPS] delta (render fps + us/flush) — LovyanGFX vs raw baseline (A/B via
  USE_LOVYAN_FLUSH), and the SPACEBADGE_FPS_BENCH max-throughput numbers. Fill in docs table.
- Human eyeball: confirm on-screen colors look right (I can't see the screen).
- Then: LV_DEF_REFR_PERIOD 33->16 experiment; consider async flush for real render/DMA overlap.
