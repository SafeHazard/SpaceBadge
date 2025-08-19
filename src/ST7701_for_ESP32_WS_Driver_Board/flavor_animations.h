#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>
#include "./src/ui/ui.h"

// Animation states
typedef enum {
    FLAVOR_STATE_BLACK = 0,
    FLAVOR_STATE_GRAY_DARK,
    FLAVOR_STATE_GRAY_LIGHT,
    FLAVOR_STATE_WHITE,
    FLAVOR_STATE_ORANGE,
    FLAVOR_STATE_PAUSE
} flavor_animation_state_t;

// Animation structure for each flavor text
typedef struct {
    lv_obj_t* obj;
    flavor_animation_state_t state;
    lv_timer_t* timer;
    uint32_t start_delay;
    bool is_cascade_complete;
    uint32_t pause_duration;
    bool needs_update;
    char pending_text[24];  // MAX_TEXT_LENGTH + 1
    bool is_in_cascade;     // True during cascade, false during normal cycling
} flavor_animation_t;

// Public functions
void flavor_animations_init(void);
void flavor_animations_start(void);
void flavor_animations_stop(void);
void flavor_animations_deinit(void);
bool is_main_screen_active(void);

#ifdef __cplusplus
}
#endif