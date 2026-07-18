# SpaceBadge frame-rate optimization

Branch: `perf/lovyangfx-dma-flush`

## TL;DR (measured on hardware)

The display driver was **not** the frame-rate bottleneck. The raw driver and a LovyanGFX
DMA rewrite both land at the same fps — because both were already pinned at LVGL's refresh
cap (`LV_DEF_REFR_PERIOD = 33 ms ≈ 30 Hz`). **The single change that raises the frame rate is
`LV_DEF_REFR_PERIOD 33 → 16` (≈60 Hz), which took the running badge from ~30 to ~50 fps —
and it works with the existing driver.** The LovyanGFX swap is optional (cleaner driver,
headroom, a path to async flush) but by itself buys ~0 fps. Tradeoff: 60 Hz ≈ 2× render CPU;
validate mesh/audio/games/battery before adopting it. See the measured table below.

## Original hypothesis (partially wrong — kept for the record)

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

## Measurements (on hardware, COM8, `[FPS]` telemetry)

`render` = display refreshes/sec (visible fps); `us/flush` = mean CPU time per flush.

**Real-world fps (badge running its normal UI):**

| flush driver | `LV_DEF_REFR_PERIOD` | render fps |
|---|---|---|
| raw (original) | 33 ms (~30 Hz cap) | **~28.5** |
| LovyanGFX DMA | 33 ms (~30 Hz cap) | **~30** |
| LovyanGFX DMA | 16 ms (~60 Hz cap) | **~50** |
| raw (original) | 16 ms (~60 Hz cap) | **~50** |

**Per-flush (bench = forced full-screen redraw):** raw ~1503 µs / 9600 B ≈ **6.1 MB/s**;
LovyanGFX ~2748 µs / 19200 B ≈ **6.8 MB/s**. Both are **SPI-clock-bound** (80 MHz); DMA is
only ~10 %/byte faster — it does not make the SPI clock faster. A full complex-screen redraw
is **render-bound** (~1 fps for BOTH drivers), i.e. limited by LVGL's software drawing, not
the flush — which is why the badge relies on partial updates.

### What this means (honest)

- **The display flush was NOT the frame-rate bottleneck.** Swapping the hand-rolled raw
  driver for LovyanGFX DMA changes real-world fps by ~1–2 fps — both were already pinned at
  LVGL's refresh-period cap.
- **The real fps lever is `LV_DEF_REFR_PERIOD`** (one line in `lv_conf.h`): 33 → 16 ms took
  the UI from ~30 to **~50 fps** (not the full 60 — the loop/render cadence caps it ~50).
- **LovyanGFX's actual value:** a cleaner/maintained driver, a modest per-flush speedup, DMA
  + a double buffer that give **headroom** to sustain a higher refresh rate, and the
  prerequisite for a future **async flush** (which *would* overlap render with transfer and
  free the CPU — the current flush is blocking, so the 2nd buffer buys no concurrency yet).
- **Tradeoff of 60 Hz:** ~2× the render CPU. On a badge that also runs painlessMesh, audio,
  and games, this can steal CPU from them (audio memory-pressure warnings already appear at
  30 Hz). Validate mesh/audio/gameplay before shipping 60 Hz; 33 ms stays the safe default.

> Serial-capture gotchas: (1) this board's app console is on the native USB, and under
> `CDCOnBoot=cdc` `Serial` and `printf` route to *different* sinks — the `[FPS]` meter uses
> `printf` to match LVGL's own log. (2) Measurement builds use `CDCOnBoot=default`; production
> ships `cdc`. (3) A crash-looping firmware wedges the USB — recover via download mode.

## Validation status

- Compiles + links clean (esp32 core 2.0.17; the machine's 2.0.10 arduino-cli core was
  broken for WiFi linking and was reinstalled — IDE toolchain untouched).
- Expert adversarial review: no Critical/High display-correctness bugs; color + 0/180
  rotation verified against the reference; buffers/DMA/bus/backlight/touch all safe.
- On-hardware colors need a human eyeball (I can't see the screen) — please confirm.
