#include "Display_Driver.h"
#include <esp_timer.h>
#include "game_parent.h"

// Display buffer size (adapt based on available RAM)
#define BUFFER_ROWS 32
#define BUFFER_SIZE (LCD_WIDTH * BUFFER_ROWS)

// Display buffers (CPU-driven)
// static lv_color_t buf1[BUFFER_SIZE];
// static lv_color_t buf2[BUFFER_SIZE];

int pixelCount = (LCD_WIDTH * LCD_HEIGHT * 2) / 16; // Even smaller buffers for more RAM
int bufSizeBytes = pixelCount * sizeof(lv_color_t);

static lv_color_t* buf1 = (lv_color_t*)heap_caps_aligned_alloc(32, bufSizeBytes, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
// buf2 removed to save RAM - using single buffer only

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

// Display rotation
void set_display_rotation(lv_display_t* disp, lv_display_rotation_t rotation)
{
	lv_display_set_rotation(disp, rotation);                    // Updates LVGL's layout engine
	LCD_SetRotation(static_cast<uint8_t>(rotation));            // Updates ST7789's MADCTL register
	lv_obj_invalidate(lv_screen_active());                      // Forces redraw of current screen
}

// Flush function for LVGL
static void my_disp_flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map)
{
	// flush timing
	//int64_t start = esp_timer_get_time();

	uint32_t w = (area->x2 - area->x1 + 1);
	uint32_t h = (area->y2 - area->y1 + 1);

	// Set the drawing window
	//LCD_SetCursor(area->x1, area->y1, area->x2, area->y2); //LCD_addWindow already calls this function

	// Send pixels to display
	LCD_addWindow(area->x1, area->y1, area->x2, area->y2, (uint16_t*)px_map);

	//int64_t end = esp_timer_get_time();
	//lv_logf("Flush time: %lld us (%d x %d)\n", end - start, w, h);

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
	LCD_Init();  // This function includes pin and SPI initialization
	LV_LOG_INFO("LCD initialized\n");

	// Initialize backlight
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

	// Check buffer allocation
	//if (!buf1 || !buf2) {
	//	LV_LOG_ERROR("Buffer allocation failed: buf1=%p buf2=%p\n", buf1, buf2);
	//	return false;
	//}
	//LV_LOG_INFO("Buffers allocated: buf1=%p buf2=%p\n", buf1, buf2);

	// Set the flush callback
	LV_LOG_INFO("flush setup...");
	lv_display_set_flush_cb(disp, my_disp_flush);
	LV_LOG_INFO("done.\n");

	// Set up the buffers (buf2 = double buffer, replace with NULL for single buffer)
	LV_LOG_INFO("buffer setup...");
	lv_display_set_buffers(disp, buf1, NULL, pixelCount, LV_DISPLAY_RENDER_MODE_PARTIAL); // Single buffer to save RAM
	LV_LOG_INFO("done.\n");

	// Set display default
	LV_LOG_INFO("display setup...");
	lv_display_set_default(disp);
	LV_LOG_INFO("done.\n");

	LV_LOG_INFO("Display initialization complete!\n");

	LV_LOG_INFO("Initializing touch input device...");
	lv_indev_t* indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
	lv_indev_set_read_cb(indev, read_touchpad);
	lv_indev_set_long_press_time(indev, 2000); //set 'long press' time for touchscreen (used to exit badge mode)
	LV_LOG_INFO("done.\n");

	return true;
}
