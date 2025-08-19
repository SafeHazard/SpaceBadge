#pragma once
#include "multiplayer_overlay.h"
#include <lvgl.h>
#include "global.hpp"
#include "json.h"
#include "pngs.h"
#include <Arduino.h>

#if 1
#include "./src/ui/styles.h"
#else
#define add_style_text_flavor_white(obj) lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT)
#endif

extern GameState gameState;
CachedImage* mo_images;
size_t mo_images_size;

multi_scn::multi_scn()
{
    //printf("[MULTI_OVERLAY] Initializing multiplayer overlay\n");
    
    // Use globally preloaded overlay images (loaded during startup to avoid fragmentation)
    extern CachedImage* overlay_images;
    extern size_t overlay_images_size;
    
    mo_images = overlay_images;
    mo_images_size = overlay_images_size;
    
    if (mo_images && mo_images_size > 0) {
        //printf("[MULTI_OVERLAY] Using globally preloaded overlay images (%zu images)\n", mo_images_size);
        
        // Verify images are actually loaded
        bool has_bad = false, has_good = false;
        for (size_t i = 0; i < mo_images_size; i++) {
            if (mo_images[i].name) {
                if (strstr(mo_images[i].name, "bad")) has_bad = (mo_images[i].img != nullptr);
                if (strstr(mo_images[i].name, "good")) has_good = (mo_images[i].img != nullptr);
            }
        }
        
        if (has_bad && has_good) {
            //printf("[MULTI_OVERLAY] Both bad and good overlay images successfully loaded\n");
        } else {
            //printf("[MULTI_OVERLAY] Some overlay images missing - will use color fallbacks\n");
        }
    } else {
        //printf("[MULTI_OVERLAY] No overlay images available - will use color-based effects\n");
    }
}

void multi_scn::create_ui()
{
    parent_obj = lv_obj_create(lv_screen_active());
    lv_obj_set_pos(parent_obj, 0, 0);
    lv_obj_set_size(parent_obj, 240, 320);
    //lv_obj_set_style_bg_color(parent_obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(parent_obj, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(parent_obj, LV_OBJ_FLAG_SCROLLABLE);
    LV_LOG_INFO("################ Player count: %d\n", gameState.playerCount);
    lv_obj_move_background(parent_obj);
    {
        {
            lbl_1 = lv_label_create(parent_obj);
            lv_obj_set_pos(lbl_1, 0, 0);
            lv_obj_set_size(lbl_1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_white(lbl_1);
            lv_label_set_text(lbl_1, "1");
        }
        {
            lbl_2 = lv_label_create(parent_obj);
            lv_obj_set_pos(lbl_2, 70, 0);
            lv_obj_set_size(lbl_2, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_white(lbl_2);
            lv_label_set_text(lbl_2, "2");
        }
        {
            lbl_3 = lv_label_create(parent_obj);
            lv_obj_set_pos(lbl_3, 0, 14);
            lv_obj_set_size(lbl_3, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_white(lbl_3);
            lv_label_set_text(lbl_3, "3");
        }
        {
            lbl_4 = lv_label_create(parent_obj);
            lv_obj_set_pos(lbl_4, 70, 14);
            lv_obj_set_size(lbl_4, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_white(lbl_4);
            lv_label_set_text(lbl_4, "4");
        }
        {
            // sld_game_p1
            lv_obj_t *obj = lv_slider_create(parent_obj);
            sld_game_p1 = obj;
            lv_obj_set_pos(obj, 6, 4);
            lv_obj_set_size(obj, 40, 10);
            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_set_style_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK);
        }
        {
            // sld_game_p2
            lv_obj_t *obj = lv_slider_create(parent_obj);
            sld_game_p2 = obj;
            lv_obj_set_pos(obj, 76, 4);
            lv_obj_set_size(obj, 40, 10);
            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_set_style_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00e122), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK);
        }
        {
            // sld_game_p3
            lv_obj_t *obj = lv_slider_create(parent_obj);
            sld_game_p3 = obj;
            lv_obj_set_pos(obj, 6, 18);
            lv_obj_set_size(obj, 40, 10);
            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_set_style_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffd15704), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK);
        }
        {
            // sld_game_p4
            lv_obj_t *obj = lv_slider_create(parent_obj);
            sld_game_p4 = obj;
            lv_obj_set_pos(obj, 76, 18);
            lv_obj_set_size(obj, 40, 10);
            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_set_style_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffd104ce), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK);
        }
        {
            // sld_game_score
            lv_obj_t *obj = lv_slider_create(parent_obj);
            sld_game_score = obj;
            lv_obj_set_pos(obj, 171, 4);
            lv_obj_set_size(obj, 40, 10);
            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_set_style_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK);
        }
        {
            // sld_game_timer
            lv_obj_t *obj = lv_slider_create(parent_obj);
            sld_game_timer = obj;
            lv_obj_set_pos(obj, 171, 18);
            lv_obj_set_size(obj, 40, 10);
            lv_slider_set_value(obj, 55, LV_ANIM_OFF);
            lv_obj_set_style_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK);
        }
        {
            lbl_score = lv_label_create(parent_obj);
            lv_obj_set_pos(lbl_score, 140, 0);
            lv_obj_set_size(lbl_score, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_white(lbl_score);
            lv_label_set_text(lbl_score, "Score:");
        }
        {
            lbl_time = lv_label_create(parent_obj);
            lv_obj_set_pos(lbl_time, 140, 14);
            lv_obj_set_size(lbl_time, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_white(lbl_time);
            lv_label_set_text(lbl_time, "Time:");
        }
        {
            // Check if mo_images loaded successfully before accessing
            if (mo_images && mo_images_size > 0 && mo_images[0].img) {
                // Use image-based effect
                bad_effect = lv_img_create(parent_obj);
                lv_obj_set_pos(bad_effect, 0, 0);
                lv_obj_center(bad_effect);
                lv_obj_set_size(bad_effect, 240, 320);
                lv_img_set_src(bad_effect, mo_images[0].img);
            } else {
                // Use color-based effect as fallback
                //printf("[MULTI_OVERLAY] Using color-based bad effect (no image available)\n");
                bad_effect = lv_obj_create(parent_obj);
                lv_obj_set_pos(bad_effect, 0, 0);
                lv_obj_set_size(bad_effect, 240, 320);
                lv_obj_set_style_bg_color(bad_effect, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_opa(bad_effect, LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT); // 30% opacity red
                lv_obj_set_style_border_width(bad_effect, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            lv_obj_clear_flag(bad_effect, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(bad_effect, LV_OBJ_FLAG_PRESS_LOCK);
            lv_obj_add_flag(bad_effect, LV_OBJ_FLAG_HIDDEN);
        }
        {
            // Check if mo_images loaded successfully before accessing
            if (mo_images && mo_images_size > 1 && mo_images[1].img) {
                // Use image-based effect
                good_effect = lv_img_create(parent_obj);
                lv_obj_set_pos(good_effect, 0, 0);
                lv_obj_center(good_effect);
                lv_obj_set_size(good_effect, 240, 320);
                lv_img_set_src(good_effect, mo_images[1].img);
            } else {
                // Use color-based effect as fallback
                //printf("[MULTI_OVERLAY] Using color-based good effect (no image available)\n");
                good_effect = lv_obj_create(parent_obj);
                lv_obj_set_pos(good_effect, 0, 0);
                lv_obj_set_size(good_effect, 240, 320);
                lv_obj_set_style_bg_color(good_effect, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_opa(good_effect, LV_OPA_30, LV_PART_MAIN | LV_STATE_DEFAULT); // 30% opacity green
                lv_obj_set_style_border_width(good_effect, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            lv_obj_clear_flag(good_effect, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(good_effect, LV_OBJ_FLAG_PRESS_LOCK);
            lv_obj_add_flag(good_effect, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void multi_scn::update_ui(int t_percent, int s_percent)
{
    //GameState state = gameState;
    // gameState.scores[] contains OTHER players' scores (not including self)
    // gameState.myScore contains our own score
    int p1_per = gameState.scores[0];  // First other player -> sld_game_p1
    int p2_per = gameState.scores[1];  // Second other player -> sld_game_p2
    int p3_per = gameState.scores[2];  // Third other player -> sld_game_p3
    int p4_per = gameState.scores[3];  // Fourth other player -> sld_game_p4
    int self_score_percent = s_percent;
    int timer_percent = t_percent;

    // Debug logging for scores (reduced frequency)
    static uint32_t lastScoreDebug = 0;
    uint32_t now = millis();
    if (now - lastScoreDebug > 5000) { // Every 5 seconds
        LV_LOG_INFO("[OVERLAY] Scores: p1=%d, p2=%d, p3=%d, p4=%d, self=%d, timer=%d\n", 
                     p1_per, p2_per, p3_per, p4_per, self_score_percent, timer_percent);
        lastScoreDebug = now;
    }
    
    // Update sliders for OTHER players only (scores[] already excludes self)
    lv_slider_set_value(sld_game_p1, p1_per, LV_ANIM_OFF);
    lv_slider_set_value(sld_game_p2, p2_per, LV_ANIM_OFF);
    lv_slider_set_value(sld_game_p3, p3_per, LV_ANIM_OFF);
    lv_slider_set_value(sld_game_p4, p4_per, LV_ANIM_OFF);
    lv_slider_set_value(sld_game_score, self_score_percent, LV_ANIM_ON);
    lv_slider_set_value(sld_game_timer, timer_percent, LV_ANIM_ON);

    // Update player labels based on total playerCount
    // lbl_1 shows '1' if gameState.playerCount > 1, else 'X'
    // lbl_2 shows '2' if gameState.playerCount > 2, else 'X'  
    // lbl_3 shows '3' if gameState.playerCount > 3, else 'X'
    // lbl_4 shows '4' if gameState.playerCount > 4, else 'X'
    
    static int lastPlayerCount = -1;
    
    // Log when player count changes
    if (gameState.playerCount != lastPlayerCount) {
        LV_LOG_INFO("[OVERLAY] Player count: %d\n", gameState.playerCount);
        lastPlayerCount = gameState.playerCount;
    }
    
    lv_obj_t* labels[] = {lbl_1, lbl_2, lbl_3, lbl_4};
    
    for (int i = 0; i < 4; i++) {
        if (labels[i]) {
            if (gameState.playerCount > (i + 1)) {
                // Active player slot - show player number
                char playerNum[2];
                snprintf(playerNum, sizeof(playerNum), "%d", i + 1);
                lv_label_set_text(labels[i], playerNum);
            } else {
                // Empty slot - show X
                lv_label_set_text(labels[i], "X");
            }
        }
    }

	gameState.myScore = self_score_percent;
}

// static lv_timer_t* bad_timer = nullptr;
// static lv_timer_t* good_timer = nullptr;

void hide_effect_cb(lv_timer_t* timer)
{
    lv_obj_t* effect = (lv_obj_t*) lv_timer_get_user_data(timer);
    if (lv_obj_is_valid(effect))
    {
        lv_obj_add_flag(effect, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
		LV_LOG_ERROR("timer broke.\n");
    }
    lv_timer_del(timer);  // makes it a oneshot
}

void multi_scn::flicker_bad()
{
    lv_obj_clear_flag(bad_effect, LV_OBJ_FLAG_HIDDEN);
    lv_timer_t* t = lv_timer_create(hide_effect_cb, 50, bad_effect);
}


void multi_scn::flicker_good()
{
    lv_obj_clear_flag(good_effect, LV_OBJ_FLAG_HIDDEN);
    lv_timer_t* t = lv_timer_create(hide_effect_cb, 50, good_effect);
}

extern Config config;
int multi_scn::calc_difficulty(int gameIdx) // returns difficulty for a game
{
    if(--gameIdx < 0)
    {
        LV_LOG_ERROR("WRONG IDX\n");
        return 1;
    }

    int xp = config.games[gameIdx].XP;
    if(xp >= AVATAR_UNLOCK_6) return 7;
    if(xp >= AVATAR_UNLOCK_5) return 6;
    if(xp >= AVATAR_UNLOCK_4) return 5;
    if(xp >= AVATAR_UNLOCK_3) return 4;
    if(xp >= AVATAR_UNLOCK_2) return 3;
    if(xp >= AVATAR_UNLOCK_1) return 2;
    if(xp >= 0) return 1;
    
    return 1;
}

multi_scn::~multi_scn()
{
    // Don't free mo_images - they're globally managed and shared
    // free_cached_images(mo_images, mo_images_size);

    if(lbl_1 != nullptr)
    {
        lv_obj_del(lbl_1);
        lbl_1 = nullptr;
    }
    if(lbl_2 != nullptr)
    {
        lv_obj_del(lbl_2);
        lbl_2 = nullptr;
    }
    if(lbl_3 != nullptr)
    {
        lv_obj_del(lbl_3);
        lbl_3 = nullptr;
    }
    if(lbl_4 != nullptr)
    {
        lv_obj_del(lbl_4);
        lbl_4 = nullptr;
    }

    if(sld_game_p1 != nullptr)
    {
        lv_obj_del(sld_game_p1);
        sld_game_p1 = nullptr;
    }
    if(sld_game_p2 != nullptr)
    {
        lv_obj_del(sld_game_p2);
        sld_game_p2 = nullptr;
    }
    if(sld_game_p3 != nullptr)
    {
        lv_obj_del(sld_game_p3);
        sld_game_p3 = nullptr;
    }
    if(sld_game_p4 != nullptr)
    {
        lv_obj_del(sld_game_p4);
        sld_game_p4 = nullptr;
    }
    if(sld_game_score != nullptr)
    {
        lv_obj_del(sld_game_score);
        sld_game_score = nullptr;
    }
    if(sld_game_timer != nullptr)
    {
        lv_obj_del(sld_game_timer);
        sld_game_timer = nullptr;
    }

    if(lbl_score != nullptr)
    {
        lv_obj_del(lbl_score);
        lbl_score = nullptr;
    }
    if(lbl_time != nullptr)
    {
        lv_obj_del(lbl_time);
        lbl_time = nullptr;
    }

    if(parent_obj != nullptr)
    {
        lv_obj_del(parent_obj);
        parent_obj = nullptr;
    }

    if(bad_effect != nullptr)
    {
        lv_obj_del(bad_effect);
        bad_effect = nullptr;
    }
    if(good_effect != nullptr)
    {
        lv_obj_del(good_effect);
        good_effect = nullptr;
    }

    // if(bad_timer != nullptr)
    // {
    //     lv_timer_del(bad_timer);
    //     bad_timer = nullptr;
    // }
    // if(good_timer != nullptr)
    // {
    //     lv_timer_del(good_timer);
    //     good_timer = nullptr;
    // }

    // if(bu_effect != nullptr) { lv_obj_del(bu_effect); bu_effect = nullptr; }
    // if(bd_effect != nullptr) { lv_obj_del(bd_effect); bd_effect = nullptr; }
    // if(bl_effect != nullptr) { lv_obj_del(bl_effect); bl_effect = nullptr; }
    // if(br_effect != nullptr) { lv_obj_del(br_effect); br_effect = nullptr; }
}