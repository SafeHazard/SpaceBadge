#pragma once
#include <lvgl.h>

// Lightweight timer-based task system to replace heavy FreeRTOS tasks
void init_lightweight_tasks();
void cleanup_lightweight_tasks();
void start_info_screen_updates();
void stop_info_screen_updates();
void saveConfigAsync(); // Keep for compatibility