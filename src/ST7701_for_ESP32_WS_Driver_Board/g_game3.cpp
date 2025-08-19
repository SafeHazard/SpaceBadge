#pragma once
#include "g_game3.h"
#include <lvgl.h>
#include <Arduino.h>
#include "pngs.h"
#include "multiplayer_overlay.h"
#include "mp3s.h"

using namespace game3;
#define HEIGHT 25
#define WIDTH 40
#define PAD_WIDTH 40

CachedImage* game3_images;
size_t g3_images_size;

extern CachedMP3* game_sfx;
extern size_t mp3_sfxCount;

g_game3::g_game3(int* exp_change) : game_parent(exp_change), e_change(exp_change)
{
    LV_LOG_INFO("Created new g3 class\n");
    lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);
    score_label = nullptr;  // Safe init

    count = 0;

    const char* path = "/images/game3/lvl1";
    m = new multi_scn();
    difficulty = m->calc_difficulty(3);
    switch(difficulty)
    {
        case 1:
            path = "/images/game3/lvl1";
            break;
        case 2:
            path = "/images/game3/lvl2";
            break;
        case 3:
            path = "/images/game3/lvl3";
            break;
        case 4:
            path = "/images/game3/lvl4";
            break;
        case 5:
            path = "/images/game3/lvl5";
            break;
        case 6:
            path = "/images/game3/lvl6";
            break;
        case 7:
            path = "/images/game3/lvl7";
            break;
    }

    if(!preload_image_directory(path, &game3_images, &g3_images_size))
    {
       LV_LOG_ERROR("error loading backgrounds\n");
    }
    
    bg = nullptr;
    // printf("spawn interval: %d\n", spawnInterval);
    // printf("max: %d\n", max_cnt);
    // printf("grav: %f\n", GRAV);
    // printf("grav: %d\n", count);

    m->create_ui();
    start_timer();
}

void g_game3::CreatePkt(int idx)
{
    // printf("SPAWNING\n");
    ps[idx] = Phys(0, -100, 0, 0);

    objs[idx] = lv_img_create(lv_screen_active());
    if (game3_images && g3_images_size > 0 && game3_images[0].img) {
        lv_img_set_src(objs[idx], game3_images[0].img);
    } else {
        LV_LOG_ERROR("[ERROR] game3_images[0].img is null\n");
    }
    lv_obj_set_size(objs[idx], WIDTH, HEIGHT);

    lv_obj_align(objs[idx], LV_ALIGN_TOP_MID, ps[idx].x, ps[idx].y);  // Center it
    lv_obj_set_style_border_width(objs[idx], 0, 0);
    lv_obj_set_style_border_color(objs[idx], lv_color_black(), 0);

    alive[idx] = 0;
}

void g_game3::RespawnPkt(int lane, bool safe)
{
    // printf("STARTING\n");
    // find dead obj
    int i = -1;
    for(int j = 0; j < COUNT; j++)
    {
        if(alive[j] == 0)
        {
            i = j;
            break;
        }
    }
    
    if(i == -1)
    {
        LV_LOG_ERROR("No free object. Not activating object\n");
        return;
    }

    if(safe) {
        if (game3_images && g3_images_size > 1 && game3_images[1].img) {
            lv_img_set_src(objs[i], game3_images[1].img); // "green"
        } else {
            LV_LOG_ERROR("[ERROR] game3_images[1].img is null\n");
        }
    } else {
        if (game3_images && g3_images_size > 0 && game3_images[0].img) {
            lv_img_set_src(objs[i], game3_images[0].img); // "red"
        } else {
            LV_LOG_ERROR("[ERROR] game3_images[0].img is null\n");
        }
    }

    int x = (lane * 100);
    int y = -HEIGHT; //(random(0, 4) * -30);
    ps[i].x = x;
    ps[i].y = y;
    ps[i].vy = GRAV;
    lv_obj_move_to(objs[i], ps[i].x, ps[i].y);

    alive[i] = 1;
    count += 1;
}

void g_game3::DestroyPkt(int index, bool bottom)
{
    // printf("KILLING\n");
    // if obj exists, destroy it
    const void* src = lv_img_get_src(objs[index]);
    if(bottom)
    {
        if (game3_images && g3_images_size > 0 && src == game3_images[0].img)
        {
            m->flicker_bad();
            if(game_sfx != nullptr)
            {
                Play_Cached_Mp3(game_sfx[0]);
            }
            score -= 100 * mult;
        }
        else
        {
            if(game_sfx != nullptr)
            {
                Play_Cached_Mp3(game_sfx[2]);
            }
            m->flicker_good();
            score += 10;
        }
    }

    ps[index].x = 0;
    ps[index].y = -100;
    ps[index].vy = 0;
    alive[index] = 0;
    lv_obj_move_to(objs[index], ps[index].x, ps[index].y);

    count -= 1;
}

void g_game3::Setup()
{
    for (int i = 0; i < COUNT; i++) 
    {
        objs[i] = nullptr;
        ps[i] = Phys(); // default to 0s
        alive[i] = 0;
    }

    // MAKE DUMMY PACKETS
    for(int i = 0; i < COUNT; i++)
        CreatePkt(i);

    switch(difficulty)
    {
        case 7:
            spawnInterval = 250Ul;
            max_cnt = 7;
            GRAV = 2;
            mult = 4;
            bad_chance = .6;
            break;
        case 6:
            spawnInterval = 300Ul;
            max_cnt = 6;
            GRAV = 2;
            mult = 3;
            bad_chance = .6;
            break;
        case 5:
            spawnInterval = 400Ul;
            max_cnt = 6;
            GRAV = 2;
            mult = 2;
            bad_chance = .5;
            break;
        case 4:
            spawnInterval = 500Ul;
            max_cnt = 5;
            GRAV = 2;
            mult = 2;
            bad_chance = .5;
            break;
        case 3:
            spawnInterval = 600Ul;
            max_cnt = 5;
            GRAV = 1.5;
            mult = 1;
            bad_chance = .45;
            break;
        case 2:
            spawnInterval = 700Ul;
            max_cnt = 5;
            GRAV = 1;
            mult = 1;
            bad_chance = .4;
            break;
        case 1:
            spawnInterval = 800Ul;
            max_cnt = 4;
            GRAV = 1;
            mult = 1;
            bad_chance = .35;
            break;
        default:
            spawnInterval = 800Ul;
            max_cnt = 3;
            GRAV = 1;
            mult = 1;
            bad_chance = .35;
        break;
    }

    // printf("interval %lu\n", spawnInterval);
    // printf("max count %d\n", max_cnt);
    // printf("GRAV %f\n", GRAV);
    // printf("mult %d\n", mult);

    // SCORE LABEL
    // score_label = lv_label_create(lv_screen_active());
    // lv_label_set_long_mode(score_label, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    // lv_obj_set_width(score_label, 200);  /*Set smaller width to make the lines wrap*/
    // lv_obj_set_style_text_align(score_label, LV_TEXT_ALIGN_LEFT, 0);
    // lv_obj_set_style_text_font(score_label, &lv_font_montserrat_18, 0); // Change to 24px font
    // lv_obj_align(score_label, LV_ALIGN_TOP_LEFT, 0, 0);
    // lv_obj_set_style_text_color(score_label, lv_color_hex(0x00FF00), 0); // Green text

    if (load_game_background(3, difficulty))
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
        LV_LOG_ERROR("failure on bg - using fallback\n");
        // Fallback: solid black background
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    setup = true;
    running = true;
}

void g_game3::PlayerInput(uint16_t inx, uint16_t iny)
{
    // printf("Score: %d\n", score);
    for(int i = 0; i < COUNT; i++)
    {
        // PARANOIA CHECKS
        if(i >= 10) return;
        if (alive[i] == 0) continue;
        if (objs[i] == nullptr) continue; 
        if (!lv_obj_is_valid(objs[i])) continue;

        // Bounds checks
        if(inx < ps[i].x || inx > ps[i].x + (WIDTH)) continue;
        if(iny < ps[i].y - (HEIGHT) || iny > ps[i].y + (HEIGHT*1.5)) continue;
        
        // Compare img (determain safe / malicious)
        const void* src = lv_img_get_src(objs[i]);
        if (game3_images && g3_images_size > 0 && src == game3_images[0].img)
        {
            m->flicker_good();
            if(game_sfx != nullptr)
            {
                Play_Cached_Mp3(game_sfx[3]);
            }
            score += 100;
        }
        else if (game3_images && g3_images_size > 1 && src == game3_images[1].img)
        {
            m->flicker_bad();
            if(game_sfx != nullptr)
            {
                Play_Cached_Mp3(game_sfx[1]);
            }
            score -= 200 * mult;
        }

        // Destroy the packet
        DestroyPkt(i, false);
        return;
    }
};

void g_game3::Update()
{
    if (!setup) return;
    if (!running) return;
    
    if(m != nullptr)
    {
        int scorePercent = (int(((float)score / 3800.0) * 100));
        m->update_ui(time_left(), scorePercent);
        
        // Broadcast score to other players in multiplayer games
        broadcastScore(scorePercent);
    }

    for(int i = 0; i < COUNT; i++)
    {
        if (alive[i] == 0) continue;

        ps[i].y = ps[i].y + ps[i].vy;
        if(ps[i].y /*removed height add to go offscreen*/ > 320)
        {
            DestroyPkt(i, true);
            // RespawnPkt(random(0, 3), (bool)random(0, 2));
        }
    }

    // Spawn after delay
    unsigned long now = millis();
    unsigned long diff = now - lastSpawnTime;
    // spawnInterval = 5000Ul;

    // printf("now: %lu | lastSpawnTime: %lu | diff: %lu | spawnInterval: %lu\n", now, lastSpawnTime, diff, spawnInterval);
    // printf("COUNT %d -- \n");
    if (diff > spawnInterval && count < max_cnt)
    {
        int lane = random(0, 3);
        float rnd = (float)random(0, 101) / 100.0;
        bool safe = (bool)(rnd >= bad_chance);
        RespawnPkt(lane, safe);
        // printf("RESPAWNING--------------------------------------------\n");
        lastSpawnTime = now;
    }

    render();
}

// It does what it says it does
void g_game3::render()
{
    // printf("---------------------- RENDER ----------------------\n");
    for(int i = 0; i < COUNT; i++)
    {
        if(alive[i] == 0) continue;
        // printf("X: %f, Y: %f\n", ps[i].x, ps[i].y);
        lv_obj_move_to(objs[i], ps[i].x, ps[i].y);
    }

    //if (score_label != nullptr)
    //{
    //    char buf[64];
    //    snprintf(buf, sizeof(buf), "Score: %d", score);
    //    lv_label_set_text(score_label, buf);
    //}
    //else
    //    printf("AFNAUEIHBFUGAESBGUBSEUBGIUSEABGYUAESBYUGBASEOI\n");
    
}

g_game3::~g_game3()
{
    running = false;
    stopPlaying();

    float team_mult = 1 + (0/*successful teammates*/ * .1);
    float exp_gain = ((float)score / (15.625 - ((float)(difficulty - 1) * 0.25))) * team_mult;
    if(exp_gain < -35)
        exp_gain = -35;

    (*e_change) = exp_gain;

    for(int i = 0; i < COUNT; i++)
    {
        if(objs[i] != nullptr)
        {
            lv_obj_del(objs[i]);
            objs[i] = nullptr;
        }
    }

    free_cached_images(game3_images, g3_images_size);

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