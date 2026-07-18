#include "Display_Driver.h"
#include <esp_timer.h>
#include "game_parent.h"

// ─────────────────────────────────────────────────────────────────────────────
// Display backend (perf/lovyangfx-dma-flush)
//
// USE_LOVYAN_FLUSH = 1 (default): LovyanGFX DMA flush + double buffer. The SPI
//   transfer runs on the DMA engine and CS/DC are driven at register level,
//   replacing the original hand-rolled path (Display_ST7789.cpp) that pushed the
//   whole frame with a CPU-polled `transferBytes` and toggled CS/DC per byte with
//   digitalWrite. This is the frame-rate win.
//
// USE_LOVYAN_FLUSH = 0: the original raw-SPI blocking flush, unchanged. Kept as a
//   one-flag fallback (instant revert to the proven driver) and as the A/B
//   baseline for the FPS meter below.
//
// The LovyanGFX panel/bus/color config is copied VERBATIM from the sibling ui_test
// project (Waveshare ESP32-S3-Touch-LCD-2.8 — identical hardware + identical
// lv_conf color handling), so colors are known-good; only the rotation is portrait
// (0) to match this badge's 240x320 UI.
// ─────────────────────────────────────────────────────────────────────────────

#ifndef USE_LOVYAN_FLUSH
#define USE_LOVYAN_FLUSH 1
#endif

// Serial [FPS] telemetry (once/sec via an lv_timer, so it prints even when the
// screen is idle). Set 0 for production builds.
#ifndef SPACEBADGE_FPS_LOG
#define SPACEBADGE_FPS_LOG 1
#endif

// Benchmark stimulus: continuously invalidate the whole active screen so the
// display pipeline (render + flush) is the bottleneck — an apples-to-apples A/B
// of max throughput. MUST be 0 in production (it pins the CPU redrawing).
#ifndef SPACEBADGE_FPS_BENCH
#define SPACEBADGE_FPS_BENCH 0
#endif

#if USE_LOVYAN_FLUSH
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// Pins — identical to Display_ST7789.h (Waveshare ESP32-S3-Touch-LCD-2.8)
#define LGFX_PIN_SCLK   40
#define LGFX_PIN_MOSI   45
#define LGFX_PIN_MISO   -1
#define LGFX_PIN_DC     41
#define LGFX_PIN_CS     42
#define LGFX_PIN_RST    39

class LGFX : public lgfx::LGFX_Device {
	lgfx::Panel_ST7789 _panel;
	lgfx::Bus_SPI      _bus;
public:
	LGFX(void) {
		{   // SPI bus (SPI2/FSPI, 80 MHz, DMA auto)
			auto cfg        = _bus.config();
			cfg.spi_host    = SPI2_HOST;
			cfg.spi_mode    = 0;
			cfg.freq_write  = 80000000;
			cfg.freq_read   = 16000000;
			cfg.pin_sclk    = LGFX_PIN_SCLK;
			cfg.pin_mosi    = LGFX_PIN_MOSI;
			cfg.pin_miso    = LGFX_PIN_MISO;
			cfg.pin_dc      = LGFX_PIN_DC;
			cfg.dma_channel = SPI_DMA_CH_AUTO;
			_bus.config(cfg);
			_panel.setBus(&_bus);
		}
		{   // ST7789 panel, native 240x320 portrait
			auto cfg            = _panel.config();
			cfg.pin_cs          = LGFX_PIN_CS;
			cfg.pin_rst         = LGFX_PIN_RST;
			cfg.pin_busy        = -1;
			cfg.memory_width    = 240;
			cfg.memory_height   = 320;
			cfg.panel_width     = 240;
			cfg.panel_height    = 320;
			cfg.offset_x        = 0;
			cfg.offset_y        = 0;
			cfg.offset_rotation = 0;
			cfg.readable        = false;
			cfg.invert          = true;   // ST7789 IPS — matches ui_test known-good
			cfg.rgb_order       = false;
			cfg.bus_shared      = false;
			_panel.config(cfg);
		}
		setPanel(&_panel);
	}
};

static LGFX lgfx_display;
#endif // USE_LOVYAN_FLUSH

// ─── Display buffers ─────────────────────────────────────────────────────────
#if USE_LOVYAN_FLUSH
// Two DMA-capable partial buffers in internal RAM. Rendering into fast internal
// RAM (not PSRAM) then DMA-ing out is the FPS-optimal layout on ESP32-S3.
// NOTE: with the current blocking flush (endWrite waits for DMA) the second
// buffer does NOT overlap render with transfer — LVGL only reuses it after the
// flush returns. It is kept because it is free insurance and is the prerequisite
// for a future async flush (pushImageDMA + deferred flush_ready) which WOULD
// overlap. The measured win today is DMA + batched SPI vs per-byte bit-bang.
#ifndef DISP_BUF_ROWS
#define DISP_BUF_ROWS 40
#endif
static const uint32_t DISP_BUF_PX    = (uint32_t)LCD_WIDTH * DISP_BUF_ROWS;
static const uint32_t DISP_BUF_BYTES = DISP_BUF_PX * sizeof(lv_color_t);
static_assert(DISP_BUF_BYTES % 32 == 0, "buffer size must be a multiple of the 32-byte alignment for heap_caps_aligned_alloc");

static lv_color_t* buf1 = (lv_color_t*)heap_caps_aligned_alloc(
	32, DISP_BUF_BYTES, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
static lv_color_t* buf2 = (lv_color_t*)heap_caps_aligned_alloc(
	32, DISP_BUF_BYTES, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
#else
// Original single partial buffer (raw driver baseline).
static int    pixelCount   = (LCD_WIDTH * LCD_HEIGHT * 2) / 16;
static uint32_t bufSizeBytes = pixelCount * sizeof(lv_color_t);
static lv_color_t* buf1 = (lv_color_t*)heap_caps_aligned_alloc(32, bufSizeBytes, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
#endif

// ─── On-device FPS / throughput meter (Serial, USB-CDC) ──────────────────────
// Prints once/sec from a 1 Hz lv_timer (runs inside lv_timer_handler, so it
// reports even on an idle screen):
//   [FPS] render=NN flush/s=MMM MB/s=X.X us/flush=UUU (mode=lovyan|raw bufrows=R)
// render = display refreshes/sec (LV_EVENT_REFR_READY) = the true visible frame
// rate. us/flush = mean CPU time blocked per flush (uncapped by the 33 ms LVGL
// refresh period — this is where the DMA speedup shows directly).
#if SPACEBADGE_FPS_LOG
static volatile uint32_t s_flush_count = 0;
static volatile uint64_t s_flush_bytes = 0;
static volatile uint64_t s_flush_us    = 0;
static volatile uint32_t s_refr_count  = 0;
static uint32_t s_fps_last_ms = 0;

static void fps_on_refr_finish(lv_event_t* e) {
	(void)e;
	s_refr_count++;
#if SPACEBADGE_FPS_BENCH
	// force the next refresh so the pipeline stays saturated for benchmarking
	lv_obj_invalidate(lv_screen_active());
#endif
}

// Called at the end of every flush (self-timed). Emits via printf so it lands on
// the same ESP-IDF console as LVGL's LV_LOG (LV_LOG_PRINTF=1) — NOT Serial, which
// under CDCOnBoot routes to a different sink. Firing from the flush (not an
// lv_timer) means it also reports during the boot intro's lv_refr_now rendering.
static void fps_report(void) {
	uint32_t now = millis();
	if (s_fps_last_ms == 0) { s_fps_last_ms = now; return; }
	uint32_t dt = now - s_fps_last_ms;
	if (dt < 1000) return;
	uint32_t refr  = s_refr_count;
	uint32_t flush = s_flush_count;
	uint64_t bytes = s_flush_bytes;
	uint64_t us    = s_flush_us;
	s_refr_count = 0; s_flush_count = 0; s_flush_bytes = 0; s_flush_us = 0;
	s_fps_last_ms = now;
	// Normalize rates by the actual interval (dt may exceed 1000 ms if rendering paused).
	float secs      = (float)dt / 1000.0f;
	float render_fps = (float)refr / secs;
	float flush_ps   = (float)flush / secs;
	float mbps       = (float)bytes / (1024.0f * 1024.0f) / secs;
	uint32_t uspf    = flush ? (uint32_t)(us / flush) : 0;
	printf("[FPS] render=%.1f flush/s=%.1f MB/s=%.2f us/flush=%lu (mode=%s bufrows=%d bench=%d)\n",
		render_fps, flush_ps, mbps, (unsigned long)uspf,
		USE_LOVYAN_FLUSH ? "lovyan" : "raw",
#if USE_LOVYAN_FLUSH
		(int)DISP_BUF_ROWS,
#else
		0,
#endif
		(int)SPACEBADGE_FPS_BENCH);
}
#endif // SPACEBADGE_FPS_LOG

extern volatile unsigned long badgeMode_lastActivity;
extern game_parent* game;

DRAM_ATTR lv_obj_t *buttons[BUTTON_COUNT];
DRAM_ATTR WanderCtx ctxs[BUTTON_COUNT];

bool INPUTWAIT;

/*Read the touchpad*/
void read_touchpad(lv_indev_t* indev, lv_indev_data_t* data)
{
	uint16_t touchpad_x[5] = { 0 };
	uint16_t touchpad_y[5] = { 0 };
	uint16_t strength[5] = { 0 };
	uint8_t touchpad_cnt = 0;
	Touch_Read_Data();
	uint8_t touchpad_pressed = Touch_Get_XY(touchpad_x, touchpad_y, strength, &touchpad_cnt, CST328_LCD_TOUCH_MAX_POINTS);
	if (touchpad_pressed && touchpad_cnt > 0)
	{
		// update last interaction
		badgeMode_lastActivity = millis();

		data->point.x = touchpad_x[0];
		data->point.y = touchpad_y[0];
		data->state = LV_INDEV_STATE_PR;

		LV_LOG_TRACE("LVGL  : X=%u Y=%u num=%d\r\n", touchpad_x[0], touchpad_y[0], touchpad_cnt);

		if (!INPUTWAIT && game != nullptr)
		{
			game->PlayerInput(touchpad_x[0], touchpad_y[0]);
			INPUTWAIT = true;
		}
	}
	else
	{
		data->state = LV_INDEV_STATE_REL;
		INPUTWAIT = false;
	}
}

// Display rotation.
void set_display_rotation(lv_display_t* disp, lv_display_rotation_t rotation)
{
#if USE_LOVYAN_FLUSH
	// LovyanGFX owns the rotation (panel MADCTL). We deliberately do NOT also call
	// lv_display_set_rotation(): in this vendored LVGL build a custom flush_cb does
	// no software rotation, so for the badge's 0/180 usage it is a no-op on output,
	// and for 90/270 it would fight LovyanGFX's transform (resolution swap vs pixel
	// rotation). Panel-side rotation alone is correct and matches the ui_test ref.
	lgfx_display.setRotation(static_cast<uint8_t>(rotation));
#else
	lv_display_set_rotation(disp, rotation);                    // LVGL layout engine
	LCD_SetRotation(static_cast<uint8_t>(rotation));            // ST7789 MADCTL
#endif
	lv_obj_invalidate(lv_screen_active());                     // force redraw
}

// Flush callback for LVGL.
static void my_disp_flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map)
{
	uint32_t w = (area->x2 - area->x1 + 1);
	uint32_t h = (area->y2 - area->y1 + 1);
#if SPACEBADGE_FPS_LOG
	int64_t t0 = esp_timer_get_time();
#endif

#if USE_LOVYAN_FLUSH
	lgfx_display.startWrite();
	lgfx_display.setAddrWindow(area->x1, area->y1, w, h);
	lgfx_display.writePixelsDMA((lgfx::rgb565_t*)px_map, w * h);
	lgfx_display.endWrite();
#else
	LCD_addWindow(area->x1, area->y1, area->x2, area->y2, (uint16_t*)px_map);
#endif

#if SPACEBADGE_FPS_LOG
	s_flush_us    += (uint64_t)(esp_timer_get_time() - t0);
	s_flush_count++;
	s_flush_bytes += (uint64_t)w * h * sizeof(lv_color_t);
	fps_report();
#endif

	// Inform LVGL that flushing is done
	lv_display_flush_ready(disp);
}

// Initialize display for LVGL
bool init_display(void)
{
	LV_LOG_INFO("Starting display initialization...\n");

	// Ensure power is on
	pinMode(PWR_Control_PIN, OUTPUT);
	digitalWrite(PWR_Control_PIN, HIGH);
	delay(100); // Give power time to stabilize
	LV_LOG_INFO("Power enabled\n");

	// Initialize display hardware
#if USE_LOVYAN_FLUSH
	lgfx_display.begin();          // bus + panel + reset + ST7789 init sequence
	lgfx_display.setRotation(0);   // portrait 240x320
	LV_LOG_INFO("LovyanGFX initialized\n");
#else
	LCD_Init();                    // raw driver: pins + SPI + panel init (+ Touch_Init)
	LV_LOG_INFO("LCD initialized (raw)\n");
#endif

	// Initialize backlight (existing LEDC channel; LovyanGFX is NOT given the
	// backlight pin, so there is no PWM-channel conflict on pin 5)
	Backlight_Init();
	LV_LOG_INFO("Backlight initialized\n");

	// Turn on backlight to 50%
	Set_Backlight(50);
	LV_LOG_INFO("Backlight set to 50\%\n");

	LV_LOG_INFO("initializing LVGL...");
	lv_init();
	LV_LOG_INFO("done.\n");

	LV_LOG_INFO("display startup: %u x %u...", LCD_WIDTH, LCD_HEIGHT);
	lv_display_t* disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
	LV_LOG_INFO("done.\n");
	if (!disp)
	{
		LV_LOG_ERROR("Failed to create display\n");
		return false;
	}
	LV_LOG_TRACE("LVGL display created\n");

	// Set the flush callback
	LV_LOG_INFO("flush setup...");
	lv_display_set_flush_cb(disp, my_disp_flush);
	LV_LOG_INFO("done.\n");

	// Set up the draw buffer(s)
	LV_LOG_INFO("buffer setup...");
#if USE_LOVYAN_FLUSH
	if (!buf1 || !buf2) {
		LV_LOG_ERROR("Display buffer allocation failed: buf1=%p buf2=%p (%u bytes each)\n",
			buf1, buf2, (unsigned)DISP_BUF_BYTES);
		return false;
	}
	lv_display_set_buffers(disp, buf1, buf2, DISP_BUF_BYTES, LV_DISPLAY_RENDER_MODE_PARTIAL);
	LV_LOG_INFO("2 x %u bytes internal DMA RAM\n", (unsigned)DISP_BUF_BYTES);
#else
	if (!buf1) {
		LV_LOG_ERROR("Display buffer allocation failed: buf1=%p\n", buf1);
		return false;
	}
	lv_display_set_buffers(disp, buf1, NULL, pixelCount, LV_DISPLAY_RENDER_MODE_PARTIAL);
	LV_LOG_INFO("1 x %u bytes internal DMA RAM\n", (unsigned)pixelCount);
#endif
	LV_LOG_INFO("done.\n");

	// Set display default
	LV_LOG_INFO("display setup...");
	lv_display_set_default(disp);
	LV_LOG_INFO("done.\n");

#if SPACEBADGE_FPS_LOG
	lv_display_add_event_cb(disp, fps_on_refr_finish, LV_EVENT_REFR_READY, NULL);
	// [FPS] is reported from the flush callback (self-timed via printf) so it also
	// covers the boot intro's lv_refr_now rendering, not just the main loop.
#endif

	LV_LOG_INFO("Display initialization complete!\n");

	LV_LOG_INFO("Initializing touch input device...");
	lv_indev_t* indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
	lv_indev_set_read_cb(indev, read_touchpad);
	lv_indev_set_long_press_time(indev, 2000); //set 'long press' time for touchscreen (used to exit badge mode)
	LV_LOG_INFO("done.\n");

	return true;
}
