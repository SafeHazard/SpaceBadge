#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include "json.h"
#include "global.hpp"
#include "./src/ui/screens.h"
#include "Display_Driver.h"

extern Config config; // Global configuration object

extern bool badgeMode_triggered;

void badgeModeCheck();
void end_badge_mode(lv_event_t* e);