#include "flavor_animations.h"
#include "./src/ui/styles.h"
#include "badgeMode.h"
#include <stdlib.h>

#define NUM_FLAVOR_OBJECTS 7
#define CASCADE_DELAY_MS 100
#define TRANSITION_DELAY_MS 100
#define ORANGE_STAY_TIME_MS 3000
#define MIN_PAUSE_MS 1000
#define MAX_PAUSE_MS 5000

// Black period timing
#define INITIAL_BLACK_TIME_MS 500      // Short black period during initial cascade
#define NORMAL_BLACK_TIME_MS 2000      // Longer black period after cascade complete
#define RE_CASCADE_INTERVAL_MS 30000   // Re-cascade every 30 seconds

// Text generation constraints
#define MAX_TEXT_LENGTH 23  // Length of "00 00 04 08 15 16 23 42"
#define DIGIT_WIDTH_PX 6    // Average digit width (5-7px)
#define SPACE_WIDTH_PX 3    // Space width

// Special sequence probability (out of 100)
#define SPECIAL_SEQUENCE_CHANCE 25

// Special sequences and easter eggs
static const char* hex_sequences[] = {
    "0xDEADBEEF",
    "0x1337",
    "0xCAFEBABE", 
    "0xFEEDFACE",
    "0xBADC0DE",
    "0x8BADF00D",
    "0x01701D",      // NCC-1701D Enterprise
    "0xC0FFEE",
    "0xDECAFBAD",
    "0x5CA1AB1E",    // Scalable
    "0xBEEF",        // Beef
    "0xFACE",        // Face
    "0xACE",         // Ace
    "0xBAD",         // Bad
    "0xFEED",        // Feed
    "0xA11C0DE",     // All Code
    "0x1CE",         // Ice
    "0xC0DE"         // Code
};

// Geeky text strings to convert to ASCII codes (easily expandable)
// TO ADD NEW STRINGS: Just add them to this array with a comment!
// They will be automatically converted to ASCII codes like "HAL" -> "72 65 76"
static const char* ascii_strings[] = {
    "HAL",           // HAL 9000
    "42",            // Hitchhiker's Guide
    "TRON",          // TRON
    "NEO",           // The Matrix
    "R2D2",          // Star Wars
    "BORG",          // Star Trek
    "FLUX",          // Back to the Future
    "GORT",          // The Day the Earth Stood Still
    "EVE",           // Wall-E
    "DATA",          // Star Trek TNG
    "JARVIS",        // Iron Man
    "SKYNET",        // Terminator
    "GLaDOS",        // Portal
    "SHODAN",        // System Shock
    "CORTANA"        // Halo
};

static const char* special_number_sequences[] = {
    "04 08 15 16 23 42",        // LOST numbers
    "00 04 08 15 16 23 42",     // Padded LOST
    "00 00 04 08 15 16 23",     // Partial LOST
    "11 22 33 44 55 66",        // Pattern
    "01 01 02 03 05 08 13",     // Fibonacci
    "31 41 59 26 53 58 97"      // Pi digits
};

#define NUM_HEX_SEQUENCES (sizeof(hex_sequences) / sizeof(hex_sequences[0]))
#define NUM_ASCII_STRINGS (sizeof(ascii_strings) / sizeof(ascii_strings[0]))
#define NUM_SPECIAL_SEQUENCES (sizeof(special_number_sequences) / sizeof(special_number_sequences[0]))

// Animation instances
static flavor_animation_t animations[NUM_FLAVOR_OBJECTS];
static bool animations_enabled = false;
static lv_timer_t* update_timer = NULL;
static lv_timer_t* recascade_timer = NULL;
static bool all_cascade_complete = false;

// Forward declarations
static void schedule_flavor_update(flavor_animation_t* anim, flavor_animation_state_t state);

// Get random pause duration between min and max
static uint32_t get_random_pause_duration(void) {
    return MIN_PAUSE_MS + (rand() % (MAX_PAUSE_MS - MIN_PAUSE_MS + 1));
}

// Re-cascade timer callback - triggers a new cascade effect
static void recascade_timer_cb(lv_timer_t* timer) {
    (void)timer;
    
    if (!animations_enabled || !is_main_screen_active()) return;
    
    // Reset all animations to start a new cascade
    all_cascade_complete = false;
    
    for (int i = 0; i < NUM_FLAVOR_OBJECTS; i++) {
        if (animations[i].timer) {
            // Reset cascade state
            animations[i].is_cascade_complete = false;
            animations[i].is_in_cascade = true;
            
            // Reset to black state with cascade timing
            schedule_flavor_update(&animations[i], FLAVOR_STATE_BLACK);
            
            // Restart with cascade delay
            uint32_t cascade_delay = animations[i].start_delay + INITIAL_BLACK_TIME_MS;
            lv_timer_set_period(animations[i].timer, cascade_delay);
        }
    }
}

// Deferred update timer callback - safe to modify UI here
static void deferred_update_timer_cb(lv_timer_t* timer) {
    (void)timer;
    
    if (!animations_enabled || !is_main_screen_active()) return;
    
    // Process pending updates for all animations
    for (int i = 0; i < NUM_FLAVOR_OBJECTS; i++) {
        if (animations[i].needs_update && animations[i].obj) {
            // Update text content
            lv_label_set_text(animations[i].obj, animations[i].pending_text);
            
            // Apply style safely
            remove_style_text_flavor_black(animations[i].obj);
            remove_style_text_flavor_gray_dark(animations[i].obj);
            remove_style_text_flavor_gray_light(animations[i].obj);
            remove_style_text_flavor_white(animations[i].obj);
            remove_style_text_flavor_orange(animations[i].obj);
            
            switch (animations[i].state) {
                case FLAVOR_STATE_BLACK:
                    add_style_text_flavor_black(animations[i].obj);
                    break;
                case FLAVOR_STATE_GRAY_DARK:
                    add_style_text_flavor_gray_dark(animations[i].obj);
                    break;
                case FLAVOR_STATE_GRAY_LIGHT:
                    add_style_text_flavor_gray_light(animations[i].obj);
                    break;
                case FLAVOR_STATE_WHITE:
                    add_style_text_flavor_white(animations[i].obj);
                    break;
                case FLAVOR_STATE_ORANGE:
                case FLAVOR_STATE_PAUSE:
                    add_style_text_flavor_orange(animations[i].obj);
                    break;
            }
            
            animations[i].needs_update = false;
        }
    }
}

// Convert string to ASCII codes with spaces
static void string_to_ascii_codes(const char* str, char* buffer, size_t buffer_size) {
    if (!str || !buffer || buffer_size < 1) return;
    
    buffer[0] = '\0';
    bool first_char = true;
    
    for (const char* c = str; *c && strlen(buffer) < buffer_size - 4; c++) {
        if (!first_char) {
            strcat(buffer, " ");
        }
        first_char = false;
        
        char ascii_code[4];
        if (*c >= 32 && *c <= 126) {  // Printable ASCII
            snprintf(ascii_code, sizeof(ascii_code), "%02d", (int)*c);
        } else {
            snprintf(ascii_code, sizeof(ascii_code), "%02d", (int)*c);
        }
        strcat(buffer, ascii_code);
    }
}

// Generate a special sequence (hex, ASCII, or number pattern)
static bool generate_special_sequence(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size < MAX_TEXT_LENGTH + 1) return false;
    
    // Decide what type of special sequence to generate
    int sequence_type = rand() % 3;
    
    switch (sequence_type) {
        case 0: {
            // Hex sequence
            int hex_index = rand() % NUM_HEX_SEQUENCES;
            strncpy(buffer, hex_sequences[hex_index], buffer_size - 1);
            buffer[buffer_size - 1] = '\0';
            return true;
        }
        
        case 1: {
            // ASCII conversion
            int ascii_index = rand() % NUM_ASCII_STRINGS;
            string_to_ascii_codes(ascii_strings[ascii_index], buffer, buffer_size);
            return true;
        }
        
        case 2: {
            // Special number sequence
            int seq_index = rand() % NUM_SPECIAL_SEQUENCES;
            strncpy(buffer, special_number_sequences[seq_index], buffer_size - 1);
            buffer[buffer_size - 1] = '\0';
            return true;
        }
    }
    
    return false;
}

// Generate random grouped numbers that fit within the text width constraints
static void generate_random_text(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size < MAX_TEXT_LENGTH + 1) return;
    
    // Check if we should generate a special sequence instead
    if ((rand() % 100) < SPECIAL_SEQUENCE_CHANCE) {
        if (generate_special_sequence(buffer, buffer_size)) {
            return;  // Special sequence generated successfully
        }
        // If special sequence failed, fall through to regular generation
    }
    
    buffer[0] = '\0';
    size_t current_length = 0;
    bool first_group = true;
    
    while (current_length < MAX_TEXT_LENGTH - 1) {
        // Determine random group size (1-4 digits, weighted towards 2-3)
        int group_size;
        int rand_val = rand() % 10;
        if (rand_val < 3) group_size = 1;       // 30% chance
        else if (rand_val < 7) group_size = 2;  // 40% chance  
        else if (rand_val < 9) group_size = 3;  // 20% chance
        else group_size = 4;                    // 10% chance
        
        // Calculate space needed for this group
        int space_needed = group_size;  // digits
        if (!first_group) space_needed += 1;  // space before group
        
        // Check if we have room for this group
        if (current_length + space_needed > MAX_TEXT_LENGTH) {
            break;
        }
        
        // Add space before group (except first)
        if (!first_group) {
            strcat(buffer, " ");
            current_length++;
        }
        first_group = false;
        
        // Generate random digits for this group
        for (int i = 0; i < group_size; i++) {
            char digit = '0' + (rand() % 10);
            size_t len = strlen(buffer);
            buffer[len] = digit;
            buffer[len + 1] = '\0';
            current_length++;
        }
        
        // Stop if we're getting close to the limit
        if (current_length >= MAX_TEXT_LENGTH - 4) {
            break;
        }
    }
}

// Schedule a deferred style and text update
static void schedule_flavor_update(flavor_animation_t* anim, flavor_animation_state_t state) {
    if (!anim || !anim->obj) return;
    
    anim->state = state;
    
    // Generate new text when switching to black (user won't see the change)
    if (state == FLAVOR_STATE_BLACK) {
        generate_random_text(anim->pending_text, sizeof(anim->pending_text));
    } else {
        // Keep existing text for other states
        const char* current_text = lv_label_get_text(anim->obj);
        if (current_text) {
            strncpy(anim->pending_text, current_text, sizeof(anim->pending_text) - 1);
            anim->pending_text[sizeof(anim->pending_text) - 1] = '\0';
        }
    }
    
    anim->needs_update = true;
}

// Check if all cascades are complete and update global state
static void check_all_cascades_complete(void) {
    if (all_cascade_complete) return;
    
    bool all_complete = true;
    for (int i = 0; i < NUM_FLAVOR_OBJECTS; i++) {
        if (!animations[i].is_cascade_complete) {
            all_complete = false;
            break;
        }
    }
    
    if (all_complete) {
        all_cascade_complete = true;
        // Mark all animations as no longer in cascade mode
        for (int i = 0; i < NUM_FLAVOR_OBJECTS; i++) {
            animations[i].is_in_cascade = false;
        }
    }
}

// Timer callback for flavor animations
static void flavor_animation_timer_cb(lv_timer_t* timer) {
    flavor_animation_t* anim = (flavor_animation_t*)lv_timer_get_user_data(timer);
    if (!anim || !anim->obj || !animations_enabled) return;
    
    // Only animate if main screen is active
    if (!is_main_screen_active()) return;
    
    switch (anim->state) {
        case FLAVOR_STATE_BLACK:
            schedule_flavor_update(anim, FLAVOR_STATE_GRAY_DARK);
            lv_timer_set_period(timer, TRANSITION_DELAY_MS);
            break;
            
        case FLAVOR_STATE_GRAY_DARK:
            schedule_flavor_update(anim, FLAVOR_STATE_GRAY_LIGHT);
            lv_timer_set_period(timer, TRANSITION_DELAY_MS);
            break;
            
        case FLAVOR_STATE_GRAY_LIGHT:
            schedule_flavor_update(anim, FLAVOR_STATE_WHITE);
            lv_timer_set_period(timer, TRANSITION_DELAY_MS);
            break;
            
        case FLAVOR_STATE_WHITE:
            schedule_flavor_update(anim, FLAVOR_STATE_ORANGE);
            lv_timer_set_period(timer, ORANGE_STAY_TIME_MS);
            anim->is_cascade_complete = true;
            check_all_cascades_complete();
            break;
            
        case FLAVOR_STATE_ORANGE:
            anim->state = FLAVOR_STATE_PAUSE;
            anim->pause_duration = get_random_pause_duration();
            lv_timer_set_period(timer, anim->pause_duration);
            break;
            
        case FLAVOR_STATE_PAUSE:
            // Restart animation sequence with appropriate black period
            schedule_flavor_update(anim, FLAVOR_STATE_BLACK);
            uint32_t black_time = anim->is_in_cascade ? INITIAL_BLACK_TIME_MS : NORMAL_BLACK_TIME_MS;
            lv_timer_set_period(timer, black_time);
            break;
    }
}

// Check if main screen is currently active and badge mode is not triggered
bool is_main_screen_active(void) {
    if (!objects.main) return false;
    if (badgeMode_triggered) return false;  // Don't animate during badge mode
    return lv_scr_act() == objects.main;
}

// Initialize the animation system
void flavor_animations_init(void) {
    // Get flavor object pointers
    lv_obj_t* flavor_objects[NUM_FLAVOR_OBJECTS] = {
        objects.flavor_main1,
        objects.flavor_main2, 
        objects.flavor_main3,
        objects.flavor_main4,
        objects.flavor_main5,
        objects.flavor_main6,
        objects.flavor_main7
    };
    
    // Initialize cascade state
    all_cascade_complete = false;
    
    // Initialize each animation
    for (int i = 0; i < NUM_FLAVOR_OBJECTS; i++) {
        animations[i].obj = flavor_objects[i];
        animations[i].state = FLAVOR_STATE_BLACK;
        animations[i].timer = NULL;
        animations[i].start_delay = i * CASCADE_DELAY_MS;
        animations[i].is_cascade_complete = false;
        animations[i].pause_duration = 0;
        animations[i].needs_update = false;
        animations[i].pending_text[0] = '\0';
        animations[i].is_in_cascade = true;  // Start in cascade mode
        
        // Set initial style and random text
        if (animations[i].obj) {
            schedule_flavor_update(&animations[i], FLAVOR_STATE_BLACK);
        }
    }
    
    // Create deferred update timer (runs every 10ms for smooth updates)
    if (!update_timer) {
        update_timer = lv_timer_create(deferred_update_timer_cb, 10, NULL);
        if (update_timer) {
            lv_timer_set_repeat_count(update_timer, -1);
        }
    }
    
    // Create re-cascade timer
    if (!recascade_timer) {
        recascade_timer = lv_timer_create(recascade_timer_cb, RE_CASCADE_INTERVAL_MS, NULL);
        if (recascade_timer) {
            lv_timer_set_repeat_count(recascade_timer, -1);
        }
    }
}

// Start the cascade animation
void flavor_animations_start(void) {
    if (!is_main_screen_active()) return;
    
    animations_enabled = true;
    
    // Resume re-cascade timer
    if (recascade_timer) {
        lv_timer_resume(recascade_timer);
        lv_timer_reset(recascade_timer);  // Reset the timer to start fresh
    }
    
    // Create timers for each animation with cascade delay
    for (int i = 0; i < NUM_FLAVOR_OBJECTS; i++) {
        if (animations[i].obj && !animations[i].timer) {
            // Start with cascade delay + initial black time
            uint32_t initial_delay = animations[i].start_delay + INITIAL_BLACK_TIME_MS;
            
            animations[i].timer = lv_timer_create(
                flavor_animation_timer_cb,
                initial_delay,
                &animations[i]
            );
            
            if (animations[i].timer) {
                lv_timer_set_repeat_count(animations[i].timer, -1); // Infinite repeat
            }
        }
    }
}

// Stop all animations
void flavor_animations_stop(void) {
    animations_enabled = false;
    
    // Stop re-cascade timer
    if (recascade_timer) {
        lv_timer_pause(recascade_timer);
    }
    
    for (int i = 0; i < NUM_FLAVOR_OBJECTS; i++) {
        if (animations[i].timer) {
            lv_timer_del(animations[i].timer);
            animations[i].timer = NULL;
        }
        
        // Reset to black state
        animations[i].state = FLAVOR_STATE_BLACK;
        animations[i].is_cascade_complete = false;
        animations[i].needs_update = false;
        animations[i].is_in_cascade = true;  // Reset to cascade mode
        if (animations[i].obj) {
            schedule_flavor_update(&animations[i], FLAVOR_STATE_BLACK);
        }
    }
    
    // Reset cascade state
    all_cascade_complete = false;
}

// Clean up the animation system
void flavor_animations_deinit(void) {
    flavor_animations_stop();
    
    // Clean up timers
    if (update_timer) {
        lv_timer_del(update_timer);
        update_timer = NULL;
    }
    
    if (recascade_timer) {
        lv_timer_del(recascade_timer);
        recascade_timer = NULL;
    }
    
    // Clear all references
    for (int i = 0; i < NUM_FLAVOR_OBJECTS; i++) {
        animations[i].obj = NULL;
        animations[i].timer = NULL;
        animations[i].needs_update = false;
    }
}