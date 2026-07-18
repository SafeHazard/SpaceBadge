# SpaceBadge frame-rate optimization — LovyanGFX DMA flush

Branch: `perf/lovyangfx-dma-flush`

## Problem

The badge's display path was a hand-rolled raw-SPI ST7789 driver (`Display_ST7789.cpp`)
driven by LVGL through `Display_Driver.cpp`. Three things capped the frame rate:

1. **Blocking, non-DMA flush.** `LCD_addWindow` → `LCD_WriteData_nbyte` →
   `SPIClass::transferBytes`, which polls the SPI FIFO 64 bytes at a time with the CPU
   fully blocked for the whole transfer (~150 KB for a full frame). No DMA.
2. **Per-byte `digitalWrite` CS/DC** in the command/cursor path (Arduino slow-path GPIO).
3. **Single 1/16-screen draw buffer**, so LVGL stalls on the blocking flush with no
   second buffer to work ahead into.

Net effect: ~24 fps (the shipping sketchbook is literally named `dc33-24fps`), and the
CPU is pinned inside the flush instead of doing game/mesh/audio work.

Note: the user's hunch was "swap TFT_eSPI for LovyanGFX." The badge never used TFT_eSPI —
but the instinct was right: LovyanGFX's DMA path is the fix.

## Why LovyanGFX is the right move (and low-risk here)

The sibling project `ui_test` (`C:\Users\data\OneDrive\esp\ui_test`) runs the **identical
hardware** — Waveshare ESP32-S3-Touch-LCD-2.8, ST7789, same pins (SCLK 40 / MOSI 45 /
DC 41 / CS 42 / RST 39 / BL 5), CST328 touch — and already uses LovyanGFX with DMA. Its
LVGL color config is **byte-identical** to this project's `lv_conf.h` (16 bpp, no
`LV_COLOR_16_SWAP`), so its known-good panel config (`invert=true`, `rgb_order=false`)
ports over with colors guaranteed correct; only the rotation changes (portrait 0 here vs
landscape 3 there).

## The change (one file: `Display_Driver.cpp`)

- **DMA flush** via LovyanGFX: `startWrite` / `setAddrWindow` / `writePixelsDMA` /
  `endWrite`. The SPI transfer runs on the DMA engine; CS/DC are register-driven.
- **Double-buffered** partial draw buffers in **internal DMA RAM** (2 × 240×40×2 = 2×19200 B).
  Internal-RAM render + DMA-out is the FPS-optimal layout on ESP32-S3 (avoids the
  DMA-from-PSRAM pitfall the ui_test reference never measured around).
- **Rotation** handed entirely to LovyanGFX (`setRotation`), matching the reference and
  removing a latent 90/270 double-rotation footgun.
- Raw driver (`Display_ST7789.cpp`) left in the tree, now off the render path, reachable
  via the `USE_LOVYAN_FLUSH 0` compile switch (instant revert / A-B baseline).

### Compile switches (top of `Display_Driver.cpp`)
| macro | default | meaning |
|---|---|---|
| `USE_LOVYAN_FLUSH` | 1 | 1 = LovyanGFX DMA; 0 = original raw-SPI flush (baseline / prod fallback) |
| `SPACEBADGE_FPS_LOG` | 1 | 1 Hz `[FPS]` serial meter (set 0 for production) |
| `SPACEBADGE_FPS_BENCH` | 0 | force continuous full-screen redraw for max-throughput A/B |

## Further levers (not yet applied — measure first)

1. **Async flush** — `pushImageDMA` + defer `lv_display_flush_ready` to DMA-done. Only this
   makes the second buffer actually overlap render with transfer (the blocking flush does
   not). Biggest remaining win if the 30 Hz cap isn't already the limit.
2. **Raise `LV_DEF_REFR_PERIOD` 33 → ~16 ms** in `lv_conf.h` — lifts LVGL's ~30 Hz refresh
   cap toward 60. Only helps once the flush is fast enough to keep up (it now is).
3. **`DISP_BUF_ROWS`** sweep — larger buffers = fewer flush calls / less per-flush overhead.

## Measurements (COM8, `[FPS]` telemetry)

Metric legend: `render` = display refreshes/sec (true visible fps, capped by
`LV_DEF_REFR_PERIOD`); `us/flush` = mean CPU time blocked per flush (uncapped — this is
where the DMA speedup shows directly); `MB/s` = flush throughput.

| build | render fps | us/flush | MB/s | notes |
|---|---|---|---|---|
| baseline raw, bench | _pending_ | _pending_ | _pending_ | `USE_LOVYAN_FLUSH=0 BENCH=1` |
| lovyan DMA, bench | _pending_ | _pending_ | _pending_ | `USE_LOVYAN_FLUSH=1 BENCH=1` |
| lovyan DMA, real UI | _pending_ | _pending_ | _pending_ | as shipped (no bench) |

> Serial-capture gotcha: this board exposes the app console on the native USB CDC, not the
> COM port used for flashing. Measurement builds use `CDCOnBoot=default` to route `Serial`
> to the flashing port; production ships `CDCOnBoot=cdc`. Also monitor with `dtr=off rts=off`.

## Validation status

- Compiles + links clean (esp32 core 2.0.17; the machine's 2.0.10 arduino-cli core was
  broken for WiFi linking and was reinstalled — IDE toolchain untouched).
- Expert adversarial review: no Critical/High display-correctness bugs; color + 0/180
  rotation verified against the reference; buffers/DMA/bus/backlight/touch all safe.
- On-hardware colors need a human eyeball (I can't see the screen) — please confirm.
