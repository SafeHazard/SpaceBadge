#include "avatar_unlocks.h"
#include "custom.h"
#include "pngs.h"
#include "json.h"
#include <algorithm>
#include <vector>
#include <string>

extern Config config;
extern size_t int_avatar_82;
extern CachedImage* img_avatar_82;

// Global vector to track unlocked avatar filenames for the callback
static std::vector<std::string, PsramAllocator<std::string>> g_unlocked_filenames;

// Callback for when avatar roller selection changes
void avatarRollerChanged(lv_event_t* e) {
    lv_obj_t* roller = (lv_obj_t*)lv_event_get_target(e);
    if (!roller) return;
    
    uint32_t selected_index = lv_roller_get_selected(roller);
    LV_LOG_TRACE("[TRACE] avatarRollerChanged: selected_index=%u, total_unlocked=%zu\n", selected_index, g_unlocked_filenames.size());
    
    if (selected_index >= g_unlocked_filenames.size()) {
        LV_LOG_ERROR("[ERROR] Selected index %u out of range (max: %zu)\n", selected_index, g_unlocked_filenames.size());
        return;
    }
    
    const std::string& selected_name = g_unlocked_filenames[selected_index];
    LV_LOG_INFO("[INFO] Avatar selected: %s\n", selected_name.c_str());
    
    // Find the full filename (with hex prefix and .bin extension) to look up the image
    std::string full_filename;
    bool found = false;
    
    for (size_t i = 0; i < int_avatar_82; i++) {
        if (!img_avatar_82[i].name) continue;
        
        std::string filename(img_avatar_82[i].name);
        std::string clean_name = filename;
        
        // Remove "XX_" prefix if present
        if (clean_name.length() > 3 && clean_name[2] == '_') {
            clean_name = clean_name.substr(3);
        }
        
        // Remove .bin suffix
        if (clean_name.length() > 4 && clean_name.substr(clean_name.length() - 4) == ".bin") {
            clean_name = clean_name.substr(0, clean_name.length() - 4);
        }
        
        if (clean_name == selected_name) {
            full_filename = filename;
            found = true;
            config.user.avatar = (int)i;  // Set avatar index in config
            LV_LOG_INFO("[INFO] Found avatar at index %zu: %s\n", i, full_filename.c_str());
            break;
        }
    }
    
    if (!found) {
        LV_LOG_ERROR("[ERROR] Could not find full filename for selected avatar: %s\n", selected_name.c_str());
        return;
    }
    
    // Update both avatar screen and main screen images
    const lv_img_dsc_t* img = get_cached_image(img_avatar_82, int_avatar_82, full_filename.c_str());
    if (img && objects.img_avatar_avatar && lv_obj_is_valid(objects.img_avatar_avatar)) {
        lv_image_set_src(objects.img_avatar_avatar, img);
        LV_LOG_INFO("[INFO] Updated avatar screen image to: %s\n", full_filename.c_str());
    } else {
        LV_LOG_ERROR("[ERROR] Failed to update avatar screen image - img=%p, obj=%p\n", img, objects.img_avatar_avatar);
    }
    
    // Also update main screen avatar immediately
    if (img && objects.img_main_avatar && lv_obj_is_valid(objects.img_main_avatar)) {
        // Validate image before setting
        if (img->header.w > 0 && img->header.h > 0) {
            lv_image_set_src(objects.img_main_avatar, img);
            
            // Debug: verify the source was actually set
            const void* current_src = lv_image_get_src(objects.img_main_avatar);
            LV_LOG_INFO("[INFO] Updated main screen avatar to: %s (src verify: %p vs %p, size: %dx%d)\n", 
                          full_filename.c_str(), img, current_src, img->header.w, img->header.h);
        } else {
            LV_LOG_ERROR("[ERROR] Invalid image dimensions for %s: %dx%d\n", 
                          full_filename.c_str(), img->header.w, img->header.h);
        }
    } else {
        LV_LOG_ERROR("[ERROR] Failed to update main screen avatar - img=%p, obj=%p, valid=%s\n", 
                      img, objects.img_main_avatar, 
                      objects.img_main_avatar ? (lv_obj_is_valid(objects.img_main_avatar) ? "YES" : "NO") : "NULL");
    }
    
    // Also update badge screen avatar immediately
    if (img && objects.img_badge_avatar && lv_obj_is_valid(objects.img_badge_avatar)) {
        // Validate image before setting
        if (img->header.w > 0 && img->header.h > 0) {
            lv_image_set_src(objects.img_badge_avatar, img);
            
            // Debug: verify the source was actually set
            const void* current_src = lv_image_get_src(objects.img_badge_avatar);
            LV_LOG_INFO("[INFO] Updated badge screen avatar to: %s (src verify: %p vs %p, size: %dx%d)\n", 
                          full_filename.c_str(), img, current_src, img->header.w, img->header.h);
        } else {
            LV_LOG_ERROR("[ERROR] Invalid image dimensions for badge %s: %dx%d\n", 
                          full_filename.c_str(), img->header.w, img->header.h);
        }
    } else {
        LV_LOG_ERROR("[ERROR] Failed to update badge screen avatar - img=%p, obj=%p, valid=%s\n", 
                      img, objects.img_badge_avatar,
                      objects.img_badge_avatar ? (lv_obj_is_valid(objects.img_badge_avatar) ? "YES" : "NO") : "NULL");
    }
    
    // Note: Config save and main screen update are delayed until roller is released
    LV_LOG_INFO("[INFO] Avatar selection updated in memory (index: %d) - will save and apply on release\n", config.user.avatar);
}

// Callback for when avatar roller is released - saves config to disk only
void avatarRollerReleased(lv_event_t* e) {
    LV_LOG_TRACE("[TRACE] avatarRollerReleased: Saving config to disk\n");
    
    // Save config only - don't reapply everything (avoids mesh restart crashes)
    if (saveBoardConfig(config, "L:/default.json")) {
        LV_LOG_INFO("[INFO] Avatar selection saved (index: %d)\n", config.user.avatar);
    } else {
        LV_LOG_ERROR("[ERROR] Failed to save avatar selection\n");
    }
}

void populate_avatar_roller() {
    int avatars_unlocked = calculateUnlockedAvatars();
    LV_LOG_TRACE("[TRACE] populate_avatar_roller() - avatars_unlocked: %d, total_avatars: %zu\n", avatars_unlocked, int_avatar_82);
    
    // Safety check for valid roller object
    if (!objects.roller_avatar_component || !lv_obj_is_valid(objects.roller_avatar_component)) {
        LV_LOG_ERROR("[ERROR] Invalid roller_avatar_component object\n");
        return;
    }
    
    // Safety check for img_avatar_82 array
    if (!img_avatar_82 || int_avatar_82 <= 0) {
        LV_LOG_ERROR("[ERROR] img_avatar_82 array not loaded or empty (count: %zu)\n", int_avatar_82);
        lv_roller_set_options(objects.roller_avatar_component, "No avatars loaded", LV_ROLLER_MODE_NORMAL);
        return;
    }
    
    // Debug: List all loaded avatar filenames
    LV_LOG_INFO("[DEBUG] Loaded avatars:\n");
    for (size_t i = 0; i < int_avatar_82; i++) {
        if (img_avatar_82[i].name) {
            LV_LOG_INFO("  [%zu]: %s\n", i, img_avatar_82[i].name);
        } else {
            LV_LOG_INFO("  [%zu]: (null)\n", i);
        }
    }
    
    // Extract unlocked filenames
    std::vector<std::string, PsramAllocator<std::string>> unlocked_names;
    
    for (size_t i = 0; i < int_avatar_82; i++) {
        if (!img_avatar_82[i].name) continue;
        
        std::string filename(img_avatar_82[i].name);
        bool is_unlocked = false;
        
        // Check if filename has hex prefix format (XX_name.bin)
        if (filename.length() >= 3 && filename[2] == '_') {
            // Extract hex prefix (first 2 characters)
            std::string hex_str = filename.substr(0, 2);
            
            // Convert hex to int
            int hex_value;
            try {
                hex_value = std::stoi(hex_str, nullptr, 16);
                is_unlocked = (hex_value < avatars_unlocked);
                //LV_LOG_INFO("[DEBUG] Avatar %s: hex=%02X, unlocked=%s\n", filename.c_str(), hex_value, is_unlocked ? "YES" : "NO");
            } catch (...) {
                LV_LOG_WARN("[WARN] Invalid hex prefix in filename: %s, treating as legacy\n", filename.c_str());
                is_unlocked = ((int)i < avatars_unlocked);  // Fallback: unlock first N files
            }
        } else {
            // Legacy naming (no hex prefix) - unlock first N files
            is_unlocked = ((int)i < avatars_unlocked);
            //LV_LOG_INFO("[DEBUG] Avatar %s: legacy naming, index=%zu, unlocked=%s\n", filename.c_str(), i, is_unlocked ? "YES" : "NO");
        }
        
        if (is_unlocked) {
            // Clean up the name for display
            std::string clean_name = filename;
            
            // Remove "XX_" prefix if present
            if (clean_name.length() > 3 && clean_name[2] == '_') {
                clean_name = clean_name.substr(3);
            }
            
            // Remove .bin suffix
            if (clean_name.length() > 4 && clean_name.substr(clean_name.length() - 4) == ".bin") {
                clean_name = clean_name.substr(0, clean_name.length() - 4);
            }
            
            unlocked_names.push_back(clean_name);
            //LV_LOG_INFO("[INFO] Unlocked avatar: %s\n", clean_name.c_str());
        }
    }
    
    if (unlocked_names.empty()) {
        //LV_LOG_WARN("[WARN] No avatars unlocked\n");
        lv_roller_set_options(objects.roller_avatar_component, "No avatars unlocked", LV_ROLLER_MODE_NORMAL);
        return;
    }
    
    // Sort alphabetically
    std::sort(unlocked_names.begin(), unlocked_names.end());
    
    // Update global vector for the callback
    g_unlocked_filenames = unlocked_names;
    //LV_LOG_INFO("[DEBUG] Updated g_unlocked_filenames with %zu entries\n", g_unlocked_filenames.size());
    
    // Build roller options string
    std::string options_str;
    for (size_t i = 0; i < unlocked_names.size(); i++) {
        if (i > 0) options_str += "\n";
        options_str += unlocked_names[i];
    }
    
    // Set roller options (normal mode)
    lv_roller_set_options(objects.roller_avatar_component, options_str.c_str(), LV_ROLLER_MODE_NORMAL);
    
    // Set roller to current avatar selection with validation and fallback
    bool avatar_found = false;
    
    // Validate current avatar index
    if (config.user.avatar >= 0 && config.user.avatar < (int)int_avatar_82 && 
        img_avatar_82 && img_avatar_82[config.user.avatar].name) {
        
        std::string current_avatar_filename(img_avatar_82[config.user.avatar].name);
        
        // Clean up the current avatar filename for comparison
        std::string current_clean_name = current_avatar_filename;
        if (current_clean_name.length() > 3 && current_clean_name[2] == '_') {
            current_clean_name = current_clean_name.substr(3);
        }
        if (current_clean_name.length() > 4 && current_clean_name.substr(current_clean_name.length() - 4) == ".bin") {
            current_clean_name = current_clean_name.substr(0, current_clean_name.length() - 4);
        }
        
        // Find the index in g_unlocked_filenames (check if current avatar is still unlocked)
        for (size_t i = 0; i < g_unlocked_filenames.size(); i++) {
            if (g_unlocked_filenames[i] == current_clean_name) {
                lv_roller_set_selected(objects.roller_avatar_component, i, LV_ANIM_OFF);
                //LV_LOG_INFO("[INFO] Set roller to current avatar: %s (roller index: %zu, avatar index: %d)\n", 
                //              current_clean_name.c_str(), i, config.user.avatar);
                avatar_found = true;
                break;
            }
        }
        
        if (!avatar_found) {
            //LV_LOG_WARN("[WARN] Current avatar '%s' (index: %d) is no longer unlocked - falling back to first unlocked\n", 
            //              current_clean_name.c_str(), config.user.avatar);
        }
    } else {
        //LV_LOG_ERROR("[ERROR] Invalid avatar index: %d (max: %zu, array valid: %s) - falling back to first unlocked\n", 
        //              config.user.avatar, int_avatar_82, img_avatar_82 ? "YES" : "NO");
    }
    
    // Fallback: set to first unlocked avatar if current selection is invalid/locked
    if (!avatar_found && !g_unlocked_filenames.empty()) {
        lv_roller_set_selected(objects.roller_avatar_component, 0, LV_ANIM_OFF);
        
        // Update config to point to the first unlocked avatar
        const std::string& fallback_name = g_unlocked_filenames[0];
        for (size_t i = 0; i < int_avatar_82; i++) {
            if (!img_avatar_82[i].name) continue;
            
            std::string filename(img_avatar_82[i].name);
            std::string clean_name = filename;
            
            // Clean filename for comparison
            if (clean_name.length() > 3 && clean_name[2] == '_') {
                clean_name = clean_name.substr(3);
            }
            if (clean_name.length() > 4 && clean_name.substr(clean_name.length() - 4) == ".bin") {
                clean_name = clean_name.substr(0, clean_name.length() - 4);
            }
            
            if (clean_name == fallback_name) {
                config.user.avatar = (int)i;
                //LV_LOG_INFO("[INFO] Reset avatar to first unlocked: %s (index: %d)\n", fallback_name.c_str(), (int)i);
                break;
            }
        }
    }
    
    //LV_LOG_INFO("[INFO] Populated avatar roller with %zu unlocked avatars\n", unlocked_names.size());
}

int calculateUnlockedAvatars() {
    // Base unlocked avatars (hex 00-18, which is 25 avatars: 0x00 to 0x18)
    const int BASE_UNLOCKED = 25;
    
    // Unlock thresholds per game (from global.hpp)
    const int UNLOCK_THRESHOLDS[] = {
        AVATAR_UNLOCK_1,  // 1200
        AVATAR_UNLOCK_2,  // 2400  
        AVATAR_UNLOCK_3,  // 4800
        AVATAR_UNLOCK_4,  // 9600
        AVATAR_UNLOCK_5,  // 14400
        AVATAR_UNLOCK_6   // 28800
    };
    const int NUM_UNLOCK_STAGES = sizeof(UNLOCK_THRESHOLDS) / sizeof(UNLOCK_THRESHOLDS[0]);
    const int AVATARS_PER_STAGE = 6;
    
    int total_unlocked = BASE_UNLOCKED;
    
    // Check each game's XP against unlock thresholds
    for (int game = 0; game < 6; game++) {  // 6 games now (was 7)
        int game_xp = config.games[game].XP;
        
        // Count how many unlock stages this game has reached
        for (int stage = 0; stage < NUM_UNLOCK_STAGES; stage++) {
            if (game_xp >= UNLOCK_THRESHOLDS[stage]) {
                total_unlocked += AVATARS_PER_STAGE;
                LV_LOG_INFO("[DEBUG] Game %d (XP: %d) unlocked stage %d (threshold: %d) - added %d avatars\n", 
                              game + 1, game_xp, stage + 1, UNLOCK_THRESHOLDS[stage], AVATARS_PER_STAGE);
            } else {
                break; // Stop at first unmet threshold for this game
            }
        }
    }
    
    LV_LOG_INFO("[INFO] Total avatars unlocked: %d (base: %d + earned: %d)\n", 
                  total_unlocked, BASE_UNLOCKED, total_unlocked - BASE_UNLOCKED);
    return total_unlocked;
}

void updateAvatarUnlocks() {
    static int last_unlock_count = -1; // Track the last calculated count to detect changes
    int new_unlock_count = calculateUnlockedAvatars();
    
    if (last_unlock_count != new_unlock_count) {
        LV_LOG_INFO("[INFO] Avatar unlock count changed: %d -> %d\n", 
                      last_unlock_count, new_unlock_count);
        last_unlock_count = new_unlock_count;
        
        // If we're currently on the avatar screen, refresh the roller
        if (objects.roller_avatar_component && lv_obj_is_valid(objects.roller_avatar_component) && 
            lv_scr_act() == objects.avatar) {
            populate_avatar_roller();
        }
        
        // Update the unlock label if visible
        if (objects.lbl_avatar_unlocks && lv_obj_is_valid(objects.lbl_avatar_unlocks)) {
            char unlock_text[32];
            snprintf(unlock_text, sizeof(unlock_text), "%d/%d Unlocked", 
                     new_unlock_count, (int)int_avatar_82);
            lv_label_set_text(objects.lbl_avatar_unlocks, unlock_text);
        }
    }
}

void screenAvatarLoaded(lv_event_t* e) {
    //LV_LOG_TRACE("[TRACE] screenAvatarLoaded()\n");
    
    // Update avatar unlock count based on current game XP
    updateAvatarUnlocks();
    
    // Update unlock count label
    if (objects.lbl_avatar_unlocks && lv_obj_is_valid(objects.lbl_avatar_unlocks)) {
        int avatars_unlocked = calculateUnlockedAvatars();
        char unlock_text[32];
        snprintf(unlock_text, sizeof(unlock_text), "%d/%d Unlocked", 
                 avatars_unlocked, (int)int_avatar_82);
        lv_label_set_text(objects.lbl_avatar_unlocks, unlock_text);
        //LV_LOG_INFO("[INFO] Updated unlock label: %s\n", unlock_text);
    } else {
        LV_LOG_WARN("[WARN] lbl_avatar_unlocks object not found or invalid\n");
    }
    
    populate_avatar_roller();
}