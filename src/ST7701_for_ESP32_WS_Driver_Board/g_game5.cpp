#pragma once
#include "g_game5.h"
#include <lvgl.h>
#include <cmath>
#include <algorithm>
#include <Arduino.h>
#include "pngs.h"
#include "multiplayer_overlay.h"
#include "mp3s.h"

using namespace game5;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define ARROW_COUNT 10
#define ARROW_SIZE 15

CachedImage* game5_images;
size_t g5_images_size;

extern CachedMP3* game_sfx;
extern size_t mp3_sfxCount;

g_game5::g_game5(int* exp_change) : game_parent(exp_change), e_change(exp_change)
{
    LV_LOG_INFO("\n\nCreated new g5 class\n");
    lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);
    score_label = nullptr;  // Safe init
    player = nullptr;

    const char* path = "/images/game5/lvl1";
    m = new multi_scn();
    difficulty = m->calc_difficulty(5);
    switch(difficulty)
    {
        case 1:
            path = "/images/game5/lvl1";
            break;
        case 2:
            path = "/images/game5/lvl2";
            break;
        case 3:
            path = "/images/game5/lvl3";
            break;
        case 4:
            path = "/images/game5/lvl4";
            break;
        case 5:
            path = "/images/game5/lvl5";
            break;
        case 6:
            path = "/images/game5/lvl6";
            break;
        case 7:
            path = "/images/game5/lvl7";
            break;
    }

    if(!preload_image_directory(path, &game5_images, &g5_images_size))
    {
        LV_LOG_ERROR("failed to load game images\n");
    }

    bg = nullptr;

    m->create_ui();
    start_timer();
}

int g_game5::SpawnRandArrow()
{
    int freeIndices[ARROW_COUNT];
    int freeCount = 0;

    for (int i = 0; i < ARROW_COUNT; i++) 
    {
        if(objs[i].obj == nullptr)
            continue;

        if (!objs[i].active)
        {

            freeIndices[freeCount++] = i;
            if(objs[i].obj == nullptr)
                {
                    break;
                }

            lv_obj_set_pos(objs[i].obj, -50, -50);
            
            break;
        }
    }

    if (freeCount > 0)
    {
        int idx = freeIndices[0];
        int dir = random(0, 4);
        int x = 0; 
        int y = 0;

        switch(dir)
        {
            case 0:  // bottom (moving up)
                x = 120;
                y = 320 - ARROW_SIZE;
                break;
            case 1:  // left (moving right)
                x = 0 - ARROW_SIZE - 40;
                y = 160;
                break;
            case 2:  // top (moving down)
                x = 120;
                y = 0;
                break;
            case 3:  // right (moving left)
                x = 240 + ARROW_SIZE + 40;
                y = 160;
                break;
            default:
                LV_LOG_ERROR("ERROR. NO DIRECTION\n");
                x = -50;
                y = -50;
                return -1;
            break;
        }

        lv_obj_move_to(objs[idx].obj, x, y);
        objs[idx].x = x;
        objs[idx].y = y;
        
        objs[idx].dir = dir;
        lv_obj_set_style_transform_angle(objs[idx].obj, 900 * (dir+3), 0);
        return idx;
    }
    else
    {
        return -1;
    }
}

void g_game5::DeleteArrow(int i)
{
    // printf("Delete %d\n", i);
    objs[i].x = -50;
    objs[i].y = -50;
    objs[i].dir = -1;
    objs[i].active = false;

    lv_obj_move_to(objs[i].obj, objs[i].x, objs[i].y);
}

void g_game5::Setup()
{
    // SCORE LABEL
    // score_label = lv_label_create(lv_screen_active());
    // lv_label_set_long_mode(score_label, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    // lv_obj_set_width(score_label, 200);  /*Set smaller width to make the lines wrap*/
    // lv_obj_set_style_text_align(score_label, LV_TEXT_ALIGN_LEFT, 0);
    // lv_obj_set_style_text_font(score_label, &lv_font_montserrat_18, 0); // Change to 24px font
    // lv_obj_align(score_label, LV_ALIGN_TOP_LEFT, 0, 0);
    // lv_obj_set_style_text_color(score_label, lv_color_hex(0x00FF00), 0); // Green text

    switch(difficulty)
    {
        case 7:
            speed = 1.4;
            spawnInterval = 600Ul;
            break;
        case 6:
            speed = 1.3;
            spawnInterval = 600Ul;
            break;
        case 5:
            speed = 1.2;
            spawnInterval = 650Ul;
            break;
        case 4:
            speed = 1.1;
            spawnInterval = 650Ul;
            break;
        case 3:
            speed = 1;
            spawnInterval = 700Ul;
            break;
        case 2:
            speed = .9;
            spawnInterval = 700Ul;
            break;
        case 1:
            speed = .7;
            spawnInterval = 800Ul;
            break;
        default:
        break;
    }

    // Setup Shield
    // shield_dir = 0;
    shield = lv_img_create(lv_screen_active());
    if (game5_images && g5_images_size > 3 && game5_images[3].img) {
        lv_img_set_src(shield, game5_images[3].img);
    } else {
        LV_LOG_ERROR("[ERROR] game5_images[3].img is null\n");
    }
    lv_obj_set_size(shield, 40, 10);
    lv_obj_align(shield, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_style_bg_color(shield, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_border_width(shield, 0, 0);
    lv_obj_set_style_border_color(shield, lv_color_black(), 0);
    
    // Base direction
    lv_obj_set_style_transform_pivot_x(shield, 20, 0);  // Center X
    lv_obj_set_style_transform_pivot_y(shield, 5, 0);   // Center Y
    lv_obj_set_style_transform_angle(shield, 900 * shield_dir, 0);
    PlayerInput(0, 1);

    // Render player object
    player = lv_img_create(lv_screen_active());
    if (game5_images && g5_images_size > 2 && game5_images[2].img) {
        lv_img_set_src(player, game5_images[2].img);
    } else {
        LV_LOG_ERROR("[ERROR] game5_images[2].img is null\n");
    }
    lv_obj_set_size(player, 25, 25);
    lv_obj_align(player, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(player, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_border_color(player, lv_color_black(), 0);
    lv_obj_set_style_border_side(player, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_pad_all(player, 0, 0);
    lv_obj_clear_flag(player, LV_OBJ_FLAG_SCROLLABLE);

    // create arrow buffer
    for(int i = 0; i < ARROW_COUNT; i++)
    {
        lv_obj_t* tmp = lv_img_create(lv_screen_active());
        int randomIndex = random(0, 2);
        if (game5_images && g5_images_size > randomIndex && game5_images[randomIndex].img) {
            lv_img_set_src(tmp, game5_images[randomIndex].img);
        } else {
            LV_LOG_ERROR("[ERROR] game5_images[%d].img is null\n", randomIndex);
        }
        lv_obj_set_size(tmp, 15, 15);
        lv_obj_set_style_bg_color(tmp, lv_color_hex(0xFF0000), 0);
        lv_obj_set_style_border_width(tmp, 0, 0);
        lv_obj_set_style_border_color(tmp, lv_color_black(), 0);

        lv_obj_set_style_transform_pivot_x(tmp, 0, 0);  // Center X
        lv_obj_set_style_transform_pivot_y(tmp, 0, 0);   // Center Y
        lv_obj_set_style_transform_angle(tmp, 900 * -1, 0);
        lv_obj_set_pos(tmp, -50, -50);

        objs[i] = O(tmp, -1, false);
    }

    if (load_game_background(5, difficulty))
    {
        LV_LOG_INFO("success on bg\n");
        const lv_img_dsc_t* bg_img = get_current_game_background();

        if (bg_img)
        {
            lv_obj_set_style_bg_image_src(lv_screen_active(), bg_img, LV_PART_MAIN | LV_STATE_DEFAULT);
            LV_LOG_INFO("load canary\n");
        }
        else
        {
            LV_LOG_WARN("warning: background loaded but get_current_game_background() returned null\n");
            // Fallback: solid black background
            lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    else
    {
        LV_LOG_ERROR("error loading game images - using fallback\n");
        // Fallback: solid black background
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    setup = true;
    running = true;
}

void g_game5::PlayerInput(uint16_t inx, uint16_t iny)
{
    float rx = inx - 120;
    float ry = iny - 160;
    float length = std::sqrt(rx * rx + ry * ry);
    if (length == 0) // Zero input vector, ignoring
        return;

    float nx = rx / length;
    float ny = ry / length;
    float angle = std::atan2(ny, nx);
    float angle_degrees = angle * (180.0f / M_PI);
    if(angle_degrees < 0) // it doesnt work without this. idk why. there was a reason
        angle_degrees += 360;

    if(angle_degrees > 135 && angle_degrees < 225) // shield left
    {
        shield_dir = 1;
        lv_obj_set_pos(shield, -20, 0);
    }
    if ((angle_degrees > 315 && angle_degrees <= 360) || (angle_degrees < 45 && angle_degrees >= 0)) // shield right
    {
        shield_dir = 3;
        lv_obj_set_pos(shield, 20, 0);
    }
    if(angle_degrees > 45 && angle_degrees < 135) // shield down
    {
        shield_dir = 0;
        lv_obj_set_pos(shield, 0, 20);
    }
    if(angle_degrees > 225 && angle_degrees < 315) // shield up
    {
        shield_dir = 2;
        lv_obj_set_pos(shield, 0, -20);
    }

    // Move the shield
    lv_obj_set_style_transform_angle(shield, 900 * shield_dir, 0);
};

int g_game5::GetArrowCount() {
    int active = 0;
    for(int i = 0; i < ARROW_COUNT; i++) {
        if(!objs[i].active)
            active++;
    }
    return active;
}

void g_game5::Update()
{
    if (!setup) return;
    if (!running) return;

    if(m != nullptr)
    {
        int scorePercent = (int(((float)score / 2800.0) * 100));
        m->update_ui(time_left(), scorePercent);
        
        // Broadcast score to other players in multiplayer games
        broadcastScore(scorePercent);
    }

    // Spawn after delay
    unsigned long now = millis();
    unsigned long diff = now - lastSpawnTime;

    // printf("now: %lu | lastSpawnTime: %lu | diff: %lu | spawnInterval: %lu\n", now, lastSpawnTime, diff, spawnInterval);
    if (diff > spawnInterval && GetArrowCount() < 11)
    {
        int idx = SpawnRandArrow();
        if(idx != -1)
        {
            objs[idx].active = true;
            lastSpawnTime = now;
        }
    }

    // Look for arrows doing things they shouldn't be
    for(int i = 0; i < ARROW_COUNT; i++)
    {
        if(!objs[i].active)
            continue;

        int x = objs[i].x;
        int y = objs[i].y;
        float arrow_cx = (float)x + ARROW_SIZE / 2;
        float arrow_cy = (float)y + ARROW_SIZE / 2;
        float rx = (x - 120);
        float ry = (y - 160);
        float dist = std::sqrt(rx * rx + ry * ry); // dist from center

        if(dist <= 20 && dist > 5)// shield hit
        {
            if(objs[i].dir == shield_dir)
            {
                DeleteArrow(i);
                score += 25;
                if(game_sfx != nullptr)
                {
                    Play_Cached_Mp3(game_sfx[3]);
                }
                m->flicker_good();
                continue;
            }
        }
        
        if(dist < 5) // player hit
        {
            DeleteArrow(i);
            score -= 100;
            if(game_sfx != nullptr)
            {
                Play_Cached_Mp3(game_sfx[1]);
            }
            m->flicker_bad();
            continue;
        }

        if ( x > (ARROW_SIZE + 400) ||
             x < (-400 - ARROW_SIZE) ||  
             y > (ARROW_SIZE + 400) ||
             y < (-400 - ARROW_SIZE)) // edge check (idk how this would happen, but just in case)
        {
            DeleteArrow(i);
            continue;
        }
    }

    render();
}

// It does what it says it does
void g_game5::render()
{
    //if (score_label != nullptr)
    //{
    //    char buf[64];
    //    snprintf(buf, sizeof(buf), "Score: %d", score);
    //    lv_label_set_text(score_label, buf);
    //}
    //else
    //    printf("Score label missing\n");

    for(int i = 0; i < ARROW_COUNT; i++)
    {
        if(!objs[i].active)
            continue;
        
        switch(objs[i].dir)
        {
            case 0:  // go up
                objs[i].y -= speed;
            break;
            case 1:  // go right
                objs[i].x += speed;
            break;
            case 2:  // go down
                objs[i].y += speed;
            break;
            case 3:  // go left
                objs[i].x -= speed;
            break;
            default:
                LV_LOG_ERROR("ERROR. NO DIRECTION");
            break;
        }

        lv_obj_move_to(objs[i].obj, objs[i].x, objs[i].y);
    }
}

g_game5::~g_game5()
{
    //printf("THIS IS ABOUT TO BE A NIGHTMARE (5)\n");
    
    running = false;
    stopPlaying();

    float team_mult = 1 + (0/*successful teammates*/ * .1);
    float exp_gain = ((float)score / (9.375 - ((float)(difficulty - 1) * .125))) * team_mult;
    if(exp_gain < -35)
        exp_gain = -35;

    (*e_change) = exp_gain;

    for (int i = 0; i < ARROW_COUNT; i++)
    {
        if (objs[i].obj != nullptr)
        {
            lv_obj_del(objs[i].obj);
            objs[i].obj = nullptr;
        }
    }

    free_cached_images(game5_images, g5_images_size);

    if(player != nullptr)
    {
        lv_obj_del(player);
        player = nullptr;
    }

    if(shield != nullptr)
    {
        lv_obj_del(shield);
        shield = nullptr;
    }

    if(score_label != nullptr)
    {
        lv_obj_del(score_label);
        score_label = nullptr;
    }

    if(bg)
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