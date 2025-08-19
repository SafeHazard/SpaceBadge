#pragma once
#include "g_game6.h"
#include <lvgl.h>
#include <Arduino.h>
#include "pngs.h"
#include "multiplayer_overlay.h"
#include "mp3s.h"

using namespace game6;
#define DEATH_RANGE -.04
CachedImage* game6_images;
size_t g6_images_size;

extern CachedMP3* game_sfx;
extern size_t mp3_sfxCount;

g_game6::g_game6(int* exp_change) : game_parent(exp_change), e_change(exp_change)
{
    // printf("\n\nCreated new g6 class\n");
    lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);
    score_label = nullptr;  // Safe init
    player = nullptr;

    const char* path = "/images/game6/lvl1";
    m = new multi_scn();
    difficulty = m->calc_difficulty(6);
    switch(difficulty)
    {
        case 1:
            path = "/images/game6/lvl1";
            break;
        case 2:
            path = "/images/game6/lvl2";
            break;
        case 3:
            path = "/images/game6/lvl3";
            break;
        case 4:
            path = "/images/game6/lvl4";
            break;
        case 5:
            path = "/images/game6/lvl5";
            break;
        case 6:
            path = "/images/game6/lvl6";
            break;
        case 7:
            path = "/images/game6/lvl7";
            break;
    }

    if(!preload_image_directory(path, &game6_images, &g6_images_size))
    {
        LV_LOG_ERROR("error loading game images\n");
    }

    for(int i = 0; i < OBJ_BUFFER; i++)
        obs[i] = nullptr;

    bg = nullptr;

    m->create_ui();
    start_timer();
}

int g_game6::CreateObstacle() // -1, 0, 1
{
    int lane = random(-1, 2);

    // find empty index
    int safe_idx = -1;
    for(int i = 0; i < OBJ_BUFFER; i++)
    {
        if(obs[i] == nullptr)
        {
            safe_idx = i;
            break;
        }
    }

    if(safe_idx != -1)
    {
        Pos p = Pos(VANISHING_X, VANISHING_Y, VANISHING_Z + 1, 0, lane);
        p += 1; // do position math
        lv_obj_t *t = lv_img_create(lv_screen_active());
        int randomIndex = random(0, 10);
        if (game6_images && g6_images_size > randomIndex && game6_images[randomIndex].img) {
            lv_img_set_src(t, game6_images[randomIndex].img);
        } else {
            LV_LOG_ERROR("[ERROR] game6_images[%d].img is null\n", randomIndex);
        }
        lv_obj_set_size(t, 0, 0);
        lv_obj_align(t, LV_ALIGN_TOP_LEFT, p.x, p.y);
        lv_obj_set_style_bg_color(t, lv_color_hex(0x00FF00), 0);
        lv_obj_set_style_border_color(t, lv_color_black(), 0);
        lv_obj_set_style_border_side(t, LV_BORDER_SIDE_FULL, 0);
        lv_obj_set_style_pad_all(t, 0, 0);
        lv_obj_clear_flag(t, LV_OBJ_FLAG_SCROLLABLE);

        obs[safe_idx] = new Obstacle(t, p);
    }

    return safe_idx;
}

void g_game6::Setup()
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
            speed = 4;
            spawnInterval = 350Ul;
            break;
        case 6:
            speed = 3;
            spawnInterval = 300Ul;
            break;
        case 5:
            speed = 3;
            spawnInterval = 400Ul;
            break;
        case 4:
            speed = 3;
            spawnInterval = 550Ul;
            break;
        case 3:
            speed = 2.5;
            spawnInterval = 750Ul;
            break;
        case 2:
            speed = 2;
            spawnInterval = 800Ul;
            break;
        case 1:
            speed = 1.5;
            spawnInterval = 900Ul;
            break;
        default:
        break;
    }

    // Render player object
    player = new Player();

    player->obj = lv_img_create(lv_screen_active());
    if (game6_images && g6_images_size > 10 && game6_images[10].img) {
        lv_img_set_src(player->obj, game6_images[10].img);
    } else {
        LV_LOG_ERROR("[ERROR] game6_images[10].img is null\n");
    }
    lv_obj_set_size(player->obj, PLAYER_SIZE, PLAYER_SIZE);
    lv_obj_align(player->obj, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(player->obj, lv_color_hex(0x0000FF), 0);
    lv_obj_set_style_border_color(player->obj, lv_color_black(), 0);
    lv_obj_set_style_border_side(player->obj, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_pad_all(player->obj, 0, 0);
    lv_obj_clear_flag(player->obj, LV_OBJ_FLAG_SCROLLABLE);
    player->pos = Pos(0, 0, .5, PLAYER_SIZE, 0); // or whatever initial Pos you want

    player->pos += 0.01;
    lv_obj_set_pos(player->obj, player->pos.x - (PLAYER_SIZE / 2), player->pos.y - (PLAYER_SIZE / 2));

    if (load_game_background(6, difficulty))
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
        LV_LOG_ERROR("failure loading game images - using fallback\n");
        // Fallback: solid black background
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    setup = true;
    running = true;
}

void g_game6::PlayerInput(uint16_t inx, uint16_t iny)
{
    float rx = inx - 120;
    if(rx < 0) // turn left
    {
        if(player->pos.lane != -1)
            player->pos.lane--;
    }

    if(rx >= 0) // turn right
    {
        if(player->pos.lane != 1)
            player->pos.lane++;
    }
};

void g_game6::Update()
{
    if (!setup) return;
    if (!running) return;

    if(m != nullptr)
    {
        int scorePercent = (int(((float)score / 9850.0) * 100));
        m->update_ui(time_left(), scorePercent);
        
        // Broadcast score to other players in multiplayer games
        broadcastScore(scorePercent);
    }

    // Spawn after delay
    unsigned long now = millis();
    unsigned long diff = now - lastSpawnTime;

    if (diff > spawnInterval)
    {
        int idx = CreateObstacle();
        if (idx != -1)
        {
            lastSpawnTime = now;
        }
    }

    render();
}

// It does what it says it does
void g_game6::render()
{
    //if (score_label != nullptr)
    //{
    //    char buf[64];
    //    snprintf(buf, sizeof(buf), "Score: %d\n", score);
    //    lv_label_set_text(score_label, buf);
    //}
    //else
    //    printf("Score label missing\n");

    
    for(int i = 0; i < OBJ_BUFFER; i++)
    {
        if(obs[i] == nullptr) continue;

        if(obs[i]->obj != nullptr)
        {
            lv_obj_set_pos(obs[i]->obj, obs[i]->pos.x - (obs[i]->pos.scale / 2), obs[i]->pos.y - obs[i]->pos.scale);
            lv_obj_set_size(obs[i]->obj, obs[i]->pos.scale, obs[i]->pos.scale);
        }
    }

    if(player != nullptr)
    {
        player->pos += 0;
        lv_obj_set_pos(player->obj, player->pos.x - (PLAYER_SIZE / 2), player->pos.y);
    }

    for (int i = 0; i < OBJ_BUFFER; i++)
    {
        Obstacle* oTemp = obs[i];
        if (oTemp == nullptr) continue;

        float t = oTemp->pos.z / VANISHING_Z;
        t = constrain(t, 0.0f, 1.0f);

        float adjustedDelta = speed * (t * t * .8f * .2f);
        oTemp->pos += adjustedDelta;
        oTemp->pos.scale = 50 * powf(1.0f - t, 3.0f); // Cubic ease-out

        bool shouldDelete = false;

        if (oTemp->pos.z <= 0.32f) // went off screen
        {
            score += 100;
            m->flicker_good();

            if(game_sfx != nullptr)
            {
                Play_Cached_Mp3(game_sfx[3]);
            }

            if (oTemp->obj != nullptr)
                lv_obj_set_pos(oTemp->obj, -50, -50);
            shouldDelete = true;
        }

        // check for player collision
        if (!oTemp->scored &&
            player != nullptr &&
            player->pos.lane == oTemp->pos.lane &&
            oTemp->pos.z - player->pos.z < DEATH_RANGE && oTemp->pos.z - player->pos.z > DEATH_RANGE - 0.065)
        {
            oTemp->scored = true;
            score -= 500;
            m->flicker_bad();

            if(game_sfx != nullptr)
            {
                Play_Cached_Mp3(game_sfx[0]);
            }
            // printf("PY: %f -- OY: %f  --  DIFF: %f\n", player->pos.y, oTemp->pos.y, oTemp->pos.z - player->pos.z);
            oTemp->pos.z = 50;
            if (oTemp->obj != nullptr)
                lv_obj_set_pos(oTemp->obj, -50, -50);
            shouldDelete = true;
        }

        if (shouldDelete)
        {
            delete oTemp;
            obs[i] = nullptr;
        }
    }
}

g_game6::~g_game6()
{
    running = false;
    stopPlaying();

    float team_mult = 1 + (0/*successful teammates*/ * .1);
    float exp_gain = ((float)score / (33.5 - ((float)(difficulty - 1)))) * team_mult;
    if(exp_gain < -35)
        exp_gain = -35;

    (*e_change) = exp_gain;

    if(player != nullptr)
    {
        delete player;
        player = nullptr;
    }

    for(int i = 0; i < OBJ_BUFFER; i++)
    {
        delete obs[i];
        obs[i] = nullptr;
    }

    free_cached_images(game6_images, g6_images_size);

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