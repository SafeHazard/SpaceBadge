#pragma once
#include "g_game1.h"
#include <lvgl.h>
#include "pngs.h"
#include <Arduino.h>
#include "multiplayer_overlay.h"
#include "memory_manager.h"
#include "mp3s.h"

using namespace game1;
CachedImage* game1_images;
size_t g1_images_size;
extern CachedMP3* game_sfx;
extern size_t mp3_sfxCount;
g_game1::g_game1(int* exp_change) : game_parent(exp_change), e_change(exp_change)
{
    LV_LOG_INFO("Created new g1 class\n");
    memoryManager.logMemoryStats("Game1 Constructor");
    lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

    const char* path = "/images/game1/lvl1";
    m = new multi_scn();
    difficulty = m->calc_difficulty(1);
    switch(difficulty)
    {
        case 1:
            path = "/images/game1/lvl1";
            break;
        case 2:
            path = "/images/game1/lvl2";
            break;
        case 3:
            path = "/images/game1/lvl3";
            break;
        case 4:
            path = "/images/game1/lvl4";
            break;
        case 5:
            path = "/images/game1/lvl5";
            break;
        case 6:
            path = "/images/game1/lvl6";
            break;
        case 7:
            path = "/images/game1/lvl7";
            break;
    }

    if(!preload_image_directory(path, &game1_images, &g1_images_size))
    {
        LV_LOG_ERROR("[ERROR] Failed to preload game1 images from %s\n", path);
        // Set safe defaults
        game1_images = nullptr;
        g1_images_size = 0;
    }

    // Validate images before using them
    if (game1_images && g1_images_size >= 2) {
        objs[0] = lv_img_create(lv_screen_active());
        if (game1_images[1].img) {
            lv_img_set_src(objs[0], game1_images[1].img);
        } else {
            LV_LOG_ERROR("[ERROR] game1_images[1].img is null\n");
        }
        lv_obj_set_size(objs[0], 32, 32);
        lv_obj_set_style_bg_color(objs[0], lv_color_hex(0xFF0000), 0); // red
        lv_obj_align(objs[0], LV_ALIGN_CENTER, 0, 0);  // Center it
        lv_obj_set_style_border_width(objs[0], 0, 0);
        lv_obj_set_style_border_color(objs[0], lv_color_black(), 0);
        lv_obj_set_pos(objs[0], 120, 0);

        objs[1] = lv_img_create(lv_screen_active());
        if (game1_images[0].img) {
            lv_img_set_src(objs[1], game1_images[0].img);
        } else {
            LV_LOG_ERROR("[ERROR] game1_images[0].img is null\n");
        }
        lv_obj_set_size(objs[1], 40, 16);
        lv_obj_set_style_bg_color(objs[1], lv_color_hex(0x00FF00), 0); // green
        lv_obj_align(objs[1], LV_ALIGN_CENTER, 0, 0);  // Center it
        lv_obj_set_style_border_width(objs[1], 0, 0);
        lv_obj_set_style_border_color(objs[1], lv_color_black(), 0);
        lv_obj_align(objs[1], LV_ALIGN_CENTER, 0, 0);  // Center it
        lv_obj_set_pos(objs[1], 0, 150);  // Fine-tune position*/
    } else {
        LV_LOG_ERROR("[ERROR] game1_images not properly loaded (ptr=%p, size=%zu)\n", game1_images, g1_images_size);
        // Create objects without images as fallback
        objs[0] = lv_img_create(lv_screen_active());
        lv_obj_set_size(objs[0], 32, 32);
        lv_obj_set_style_bg_color(objs[0], lv_color_hex(0xFF0000), 0); // red
        lv_obj_set_pos(objs[0], 120, 0);
        
        objs[1] = lv_img_create(lv_screen_active());
        lv_obj_set_size(objs[1], 40, 16);
        lv_obj_set_style_bg_color(objs[1], lv_color_hex(0x00FF00), 0); // green
        lv_obj_set_pos(objs[1], 0, 150);
    }

    objs[2] = nullptr;
    objs[3] = nullptr;

    bg = nullptr;
    score_label = nullptr;

    score = 0;
	tap_counter = 0;

    m->create_ui();
}

void g_game1::PlayerInput(uint16_t inx, uint16_t iny)
{
    if(player_paused)
        return;

    p1.vy += -1.125;

    if(inx < 120)
    {
        p1.vx -= .32;
    }
    else
    {
        p1.vx += .32;
    }

    if(game_sfx != nullptr)
    {
        Play_Cached_Mp3(game_sfx[4]);
    }
    score += 25;
    tap_counter++;
};

#include "./src/ui/screens.h"
void g_game1::Setup()
{
    // Add objects to game
    /*bg = Object(::objects.game1, *game_master, "L:/images/g1/bg.png");
    bg.SetSize(240, 320);*/



	score = 0;
	tap_counter = 0;

    p1 = Phys(120, 0, 0, 0);
    lv_obj_move_to(objs[0], p1.x, p1.y);

    switch(difficulty)
    {
        case 7:
            vel_threshold = 0.8;
            grav_mult = 1.35;
            break;
        case 6:
            vel_threshold = 0.8;
            grav_mult = 1.3;
            break;
        case 5:
            vel_threshold = 0.85;
            grav_mult = 1.2;
            break;
        case 4:
            vel_threshold = .9;
            grav_mult = 1.1;
            break;
        case 3:
            vel_threshold = 1;
            grav_mult = 1;
            break;
        case 2:
            vel_threshold = 1;
            grav_mult = 0.9;
            break;
        case 1:
            vel_threshold = 1.1;
            grav_mult = 0.8;
            break;
        default:
            vel_threshold = 1.2;
            grav_mult = 0.7;
            break;
    }

    // SCORE LABEL
    // score_label = lv_label_create(lv_screen_active());
    // lv_label_set_long_mode(score_label, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    // lv_obj_set_width(score_label, 200);  /*Set smaller width to make the lines wrap*/
    // lv_obj_set_style_text_align(score_label, LV_TEXT_ALIGN_LEFT, 0);
    // lv_obj_set_style_text_font(score_label, &lv_font_montserrat_18, 0); // Change to 24px font
    // lv_obj_align(score_label, LV_ALIGN_TOP_LEFT, 0, 0);
    // lv_obj_set_style_text_color(score_label, lv_color_hex(0x00FF00), 0); // Green text
    // lv_obj_set_style_bg_opa(score_label, LV_OPA_TRANSP, 0);

    start_timer();

    if (load_game_background(1, difficulty))
    {
        const lv_img_dsc_t* bg_img = get_current_game_background();
        
        if (bg_img) {
            lv_obj_set_style_bg_image_src(lv_screen_active(), bg_img, LV_PART_MAIN | LV_STATE_DEFAULT);
        } else {
            LV_LOG_WARN("[WARN] warning: background loaded but get_current_game_background() returned null\n");
            // Fallback: solid black background
            lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    else
    {
        LV_LOG_ERROR("[ERROR] failure on bg - using fallback\n");
        // Fallback: solid black background
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    //bg = lv_img_create(lv_screen_active());
    //lv_obj_set_size(bg, 240, 320);
    //lv_obj_center(bg);
    //lv_obj_move_to_index(bg, 0);
    //lv_img_set_src(objects.game_screen, game_background->img_desc);
    setup = true;
    running = true;
    player_paused = false;
}

void g_game1::Update()
{
    if (!running || nullptr == objs[0])
        return;
    flag = 1;

    if(m != nullptr)
    {
        int scorePercent = (int)(((float)score / 5600.0) * 100);
        m->update_ui(time_left(), scorePercent);
        
        // Broadcast score to other players in multiplayer games
        broadcastScore(scorePercent);
    }

    if(player_paused)
    {
        unsigned long now = millis();
        unsigned long diff = now - deathTimestamp;
        if (diff >= respawnTime)
        {
            tap_counter = 0;
            p1.x = 120;
            p1.y = 0;
            p1.vx = 0;
            p1.vy = 0;
            lv_obj_move_to(objs[0], p1.x, p1.y);
            player_paused = false;

            // lv_obj_del(lose_text);
            // lose_text = nullptr;
        }
        return;
    }


    if (p1.vy > 0)
    {
        p1.vy = p1.vy + (.015 * grav_mult);
    }
    else
    {
        p1.vy = p1.vy + (.0085 * grav_mult);
    }
    p1.vx = p1.vx * 0.995;
    p1.vy = p1.vy * 0.998;


    p1.x = p1.x + p1.vx;
    p1.y = p1.y + p1.vy;

    // check border
    if(p1.x > 208)
    {
        p1.x = 208;
        p1.vx = 0;
    }
    else if(p1.x < 0)
    {
        p1.x = 0;
        p1.vx = 0;
    }
    if((p1.x > 62 && p1.x < 140) && (p1.y > 270))
    {
        p1.y = 270;
        lv_obj_move_to(objs[0], p1.x, p1.y);
        // printf("vy = %f\n", p1.vy);

        // lose_text = lv_label_create(lv_screen_active());
        // lv_label_set_long_mode(lose_text, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
        // lv_label_set_text(lose_text, "");
        // lv_obj_set_width(lose_text, 250);  /*Set smaller width to make the lines wrap*/
        // lv_obj_set_style_text_align(lose_text, LV_TEXT_ALIGN_CENTER, 0);
        // lv_obj_set_style_text_font(lose_text, &lv_font_montserrat_24, 0); // Change to 24px font
        // lv_obj_align(lose_text, LV_ALIGN_CENTER, 0, -40);

        if(p1.vy <= vel_threshold)
        {
            // printf("win\n");
            // lv_obj_set_style_text_color(lose_text, lv_color_hex(0x00FF00), 0); // Red text
            // lv_label_set_text(lose_text, "victory has been achieved");
            m->flicker_good();
            if(game_sfx != nullptr)
            {
                Play_Cached_Mp3(game_sfx[3]);
            }            score += 250;
        }
        else
        {
            // printf("lose\n");
            // lv_obj_set_style_text_color(lose_text, lv_color_hex(0xFF0000), 0); // Red text
            // lv_label_set_text(lose_text, "you lose. sucks");
            m->flicker_bad();
            if(game_sfx != nullptr)
            {
                Play_Cached_Mp3(game_sfx[0]);
            }            score -= ((tap_counter * 25) * 1.2) + 80;
        }

        //printf("%f\n", p1.vy);
        p1.vy = 0;
        player_paused = true;
        deathTimestamp = millis();
    }
    else if(p1.y < 0)
    {
        p1.y = 0;
        p1.vy = 0;
    }
    if(p1.y > 288)
    {
        p1.y = 288;

        //lose_text = lv_label_create(lv_screen_active());
        //lv_label_set_long_mode(lose_text, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
        //lv_label_set_text(lose_text, "");
        //lv_obj_set_width(lose_text, 250);  /*Set smaller width to make the lines wrap*/
        //lv_obj_set_style_text_align(lose_text, LV_TEXT_ALIGN_CENTER, 0);
        //lv_obj_set_style_text_font(lose_text, &lv_font_montserrat_24, 0); // Change to 24px font
        //lv_obj_align(lose_text, LV_ALIGN_CENTER, 0, -40);

        //// printf("lose\n");
        //lv_obj_set_style_text_color(lose_text, lv_color_hex(0xFF0000), 0); // Red text
        //lv_label_set_text(lose_text, "you lose. sucks");
        if(game_sfx != nullptr)
        {
            Play_Cached_Mp3(game_sfx[0]);
        }
        m->flicker_bad();

        score -= ((tap_counter * 25) * 1.2) + 80;

        p1.vy = 0;
        player_paused = true;
        deathTimestamp = millis();
    }

    flag = 0;
    render();
}

void g_game1::render()
{
    // while (flag == 1)
    //     delay(5);
    //if (score_label != nullptr)
    //{
    //    char buf[64];
    //    snprintf(buf, sizeof(buf), "Score: %d", score);
    //    lv_label_set_text(score_label, buf);
    //}
    //else
    //    printf("AFNAUEIHBFUGAESBGUBSEUBGIUSEABGYUAESBYUGBASEOI\n");

    lv_obj_move_to(objs[0], p1.x, p1.y);
    //lv_obj_move_to(obj2, xpos1, ypos1);
}

g_game1::~g_game1()
{
    //printf("THIS IS ABOUT TO BE A NIGHTMARE\n");
    memoryManager.logMemoryStats("Game1 Destructor Start");
    stopPlaying();
    
    running = false;

    float team_mult = 1 + (0/*successful teammates*/ * .1);
    float exp_gain = ((float)score / (18 - ((float)(difficulty - 1) * 0.125))) * team_mult;
    if(exp_gain < -35)
        exp_gain = -35;
    
    if (e_change) *e_change = exp_gain;

    free_cached_images(game1_images, g1_images_size);

    for(int i = 0; i < 4; i++)
    {
        if(objs[i] != nullptr)
        {
            lv_obj_del(objs[i]);
            objs[i] = NULL;
        }
    }

    if(score_label != nullptr)
    {
        lv_obj_del(score_label);
        score_label = nullptr;
    }

    // if(lose_text != nullptr)
    // {
    //     lv_obj_del(lose_text);
    //     lose_text = NULL;
    // }

    if(bg != nullptr)
    {
        lv_obj_del(bg);
        bg = NULL;
    }

    if(m != nullptr)
    {
        delete m;
        m = nullptr;
    }
}