#pragma once
#include "g_game2.h"
#include <lvgl.h>
#include "pngs.h"
#include "multiplayer_overlay.h"
#include "mp3s.h"

using namespace game2;
#define BALL_SIZE 20
#define PAD_WIDTH 40

CachedImage* game2_images;
size_t g2_images_size;

extern CachedMP3* game_sfx;
extern size_t mp3_sfxCount;

g_game2::g_game2(int* exp_change) : game_parent(exp_change), e_change(exp_change)
{
    LV_LOG_INFO("Created new g2 class\n");
    lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);
    
    if(!preload_image_directory("/images/game2", &game2_images, &g2_images_size))
    {
        LV_LOG_ERROR("game 2 images did not load\n");
    }

    for(int i = 0; i < obj_count; i++)
    {
        ps[i] = Phys(0, 0, 0, 0);
    }

    // score_label = lv_label_create(lv_screen_active());
    // lv_label_set_long_mode(score_label, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    // lv_obj_set_width(score_label, 200);  /*Set smaller width to make the lines wrap*/
    // lv_obj_set_style_text_align(score_label, LV_TEXT_ALIGN_LEFT, 0);
    // lv_obj_set_style_text_font(score_label, &lv_font_montserrat_18, 0); // Change to 24px font
    // lv_obj_align(score_label, LV_ALIGN_TOP_LEFT, 0, 0);
    // lv_obj_set_style_text_color(score_label, lv_color_hex(0x00FF00), 0); // Red text

    bg = nullptr;

    m = new multi_scn();
    m->create_ui();
    difficulty = m->calc_difficulty(2);
    start_timer();
}

void g_game2::CreateBall(int x, int y, int i)
{
    objs[i] = lv_img_create(lv_screen_active());
    if (game2_images && g2_images_size > 0 && game2_images[0].img) {
        lv_img_set_src(objs[i], game2_images[0].img);
    } else {
        LV_LOG_ERROR("[ERROR] game2_images[0].img is null\n");
    }
    lv_obj_set_size(objs[i], BALL_SIZE, BALL_SIZE);
    lv_obj_set_style_bg_color(objs[i], lv_color_hex(0xFF0000), 0); // red
    lv_obj_set_style_border_width(objs[i], 0, 0);
    lv_obj_set_style_border_color(objs[i], lv_color_black(), 0);

    ps[i] = Phys(x, y, 0, 0);
    active_objs[i] = 1;

    lv_obj_set_pos(objs[i], ps[i].x, ps[i].y);
}

void g_game2::DestroyBall(int index)
{
    // if obj exists, destroy it
    score -= 175;
    if(objs[index] != NULL)
    {
        lv_obj_del(objs[index]);
        active_objs[index] = 0;
    }

    // Make sure there isn't an imposter obj...
    objs[index] = NULL;
}

void g_game2::CreateCrowd(int x, int idx)
{
    objs[idx] = lv_img_create(lv_screen_active());
    if (game2_images && g2_images_size > 1 && game2_images[1].img) {
        lv_img_set_src(objs[idx], game2_images[1].img);
    } else {
        LV_LOG_ERROR("[ERROR] game2_images[1].img is null\n");
    }
    lv_obj_set_size(objs[idx], PAD_WIDTH, 16);
    lv_obj_set_style_bg_color(objs[idx], lv_color_hex(0xFFFFFF), 0); // white
    lv_obj_set_style_border_width(objs[idx], 0, 0);
    lv_obj_set_style_border_color(objs[idx], lv_color_black(), 0);

    ps[idx] = Phys(x, 270, 0, 0);

    lv_obj_align(objs[idx], LV_ALIGN_CENTER, ps[idx].x, ps[idx].y);  // Center it
    lv_obj_set_pos(objs[idx], ps[idx].x, ps[idx].y);  // Fine-tune position*/
}

void g_game2::Setup()
{
    // Add objects to game
    // CreateBall(60, 0, 0);
    // CreateBall(120, 0, 1);
    // CreateBall(180, 0, 2);
    for(int i = 0; i < 3; i++)
    {
        objs[i] = nullptr;
    }

    active_objs[0] = 0;
    active_objs[1] = 0;
    active_objs[2] = 0;

    // for(int i = 0; i < 3; i++)
    // {
    //     printf("ACTIVE? {%f}\n", active_objs[i]);
    // }

    // Create crowds
    CreateCrowd(0, 3);
    if(difficulty != 7) {CreateCrowd(120, 4);}

    switch(difficulty)
    {
        case 7:
            // 1 pad (implemented above)
            NORM_GRAV = 0.023;
            DOWN_GRAV = 0.028;
        case 6:
            // TODO - INCREASE PENALTY
            goto obj_spawn;
        case 5:
            NORM_GRAV = 0.023;
            DOWN_GRAV = 0.028;
            goto obj_spawn;
        case 4:
            NORM_GRAV = 0.018;
            DOWN_GRAV = 0.021;
            goto obj_spawn;
        case 3:
            NORM_GRAV = 0.012;
            DOWN_GRAV = 0.016;
        case 2:
        obj_spawn:
            if(objs[2] == nullptr)
                CreateBall(rand() % (241 - BALL_SIZE), rand() % 80, 2);
        case 1:
            if(objs[1] == nullptr)
                CreateBall(rand() % (241 - BALL_SIZE), rand() % 80, 1);
            if(objs[0] == nullptr)
                CreateBall(rand() % (241 - BALL_SIZE), rand() % 80, 0);
            break;
        default:
            if(objs[0] == nullptr)
                CreateBall(rand() % (241 - BALL_SIZE), rand() % 80, 0);
            break;
    }

    if (load_game_background(2, difficulty))
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

void g_game2::PlayerInput(uint16_t inx, uint16_t iny)
{
    // printf("Score: %d\n", score);
    if(difficulty != 7)
    {
        if(change == 0)
        {
            change = 1;
            ps[3].x = inx - (PAD_WIDTH / 2);
        }
        else
        {
            change = 0;
            ps[4].x = inx - (PAD_WIDTH / 2);
        }
    }
    else
    {
        change = 1;
        ps[3].x = inx - (PAD_WIDTH / 2);
    }
    
    ps[3].x = (ps[3].x < 0) ? 0 : ps[3].x;
    ps[3].x = (ps[3].x + PAD_WIDTH > 240) ? 240 - PAD_WIDTH : ps[3].x;
    ps[4].x = (ps[4].x < 0) ? 0 : ps[4].x;
    ps[4].x = (ps[4].x + PAD_WIDTH > 240) ? 240 - PAD_WIDTH : ps[4].x;
    // if(abs(inx - (ps[3].x + (PAD_WIDTH/2))) < abs(inx - (ps[4].x + (PAD_WIDTH/2))))
    // {
    //     ps[3].x = inx - 20;
    // }
    // else //if(abs(inx - ps[3].x + 20) > abs(inx - ps[4].x + 20))
    // {
    //     ps[4].x = inx - 20;
    // }
};

void g_game2::Update()
{
    if (!running)
        return;
    
    if(m != nullptr)
    {
        int scorePercent = (int(((float)score / 1050.0) * 100));
        m->update_ui(time_left(), scorePercent);
        
        // Broadcast score to other players in multiplayer games
        broadcastScore(scorePercent);
    }

    // printf("---------------------- UPDATE ----------------------\n");
    
    flag = 1;
    
    for(int i = 0; i < 3; i++)
    {
        // skip popped balls
        if(active_objs[i] == 0) continue;
        if(objs[i] == NULL) continue;
        // if(ps[i].x == NULL) continue;

        // if left of ball or right of ball is within the bounding box
        bool over_pad_1 = (ps[i].x >= ps[3].x && ps[i].x <= ps[3].x + PAD_WIDTH) || (ps[i].x + BALL_SIZE >= ps[3].x && ps[i].x + BALL_SIZE <= ps[3].x + PAD_WIDTH);
        bool over_pad_2 = false;
        if(difficulty != 7) over_pad_2 = (ps[i].x >= ps[4].x && ps[i].x <= ps[4].x + PAD_WIDTH) || (ps[i].x + BALL_SIZE >= ps[4].x && ps[i].x + BALL_SIZE <= ps[4].x + PAD_WIDTH);

        // printf("Pad 2: %d\n", over_pad_2);

        if(over_pad_1 || over_pad_2)
        {
            if(ps[i].y + BALL_SIZE >= 260 && !hit[i])
            {
                float n = 0-(1.5 + (float)((rand() % 501) / 1000.0));
                // printf("HELP ME %f\n", n);
                ps[i].vy = n;//-1.5;
                bool left = (rand() % 2);
                int mod = (left) ? -1 : 1;
                // printf("left? - %d\n", left);
                // printf("First half of math - %f\n", (-2 * (int)(left)) + 1);
                ps[i].vx = mod * ((rand() % 2501) / 1000);
                score = score + 25;
                m->flicker_good();
                if(game_sfx != nullptr)
                {
                    Play_Cached_Mp3(game_sfx[3]);
                }

                hit[i] = true;
            }
        }

        if (ps[i].vy > -.2)
        {
            ps[i].vy = ps[i].vy + NORM_GRAV;
        }
        else
        {
            ps[i].vy = ps[i].vy + DOWN_GRAV;
        }

        // ps[i].vy = (ps[i].vy < -999) ? -999 : ps[i].vy;
        // ps[i].vy = (ps[i].vy > 1) ? 1 : ps[i].vy;

        if(ps[i].y + BALL_SIZE > 320)
        {
            // ps[i].y = 320 - BALL_SIZE;
            // ps[i].vy = -.56 * ps[i].vy;

            DestroyBall(i);
            CreateBall(rand() % 240, 0, i);
            m->flicker_bad();
            if(game_sfx != nullptr)
            {
                Play_Cached_Mp3(game_sfx[0]);
            }
        }
        else if(ps[i].y < 0)
        {
            ps[i].y = 0;
            ps[i].vy = -.56 * ps[i].vy;
        }

        ps[i].vx = ps[i].vx * 0.998;
        ps[i].vy = ps[i].vy * 0.999;

        ps[i].x = ps[i].x + ps[i].vx;
        ps[i].y = ps[i].y + ps[i].vy;

        // check border
        if(ps[i].x + BALL_SIZE > 240)
        {
            ps[i].x = 240 - BALL_SIZE;
            ps[i].vx = -.56 * ps[i].vx;
        }
        else if(ps[i].x < 0)
        {
            ps[i].x = 1;
            ps[i].vx = -.56 * ps[i].vx;
        }

        if (ps[i].y + BALL_SIZE < 260)
        {
            hit[i] = false;
        }
    }
    flag = 0;
    render();
}

void g_game2::render()
{
    // printf("---------------------- RENDER ----------------------\n");
    for(int i = 0; i < obj_count; i++)
    {
        // skip popped balls
        if(i < 3 && active_objs[i] != 1) continue;
        if(objs[i] == nullptr || (difficulty == 7 && i == 4)) continue;

        lv_obj_move_to(objs[i], ps[i].x, ps[i].y);
    }
    
    // char buf[64];
    // snprintf(buf, sizeof(buf), "Score: %d", score);
    // lv_label_set_text(score_label, buf);

    if(difficulty != 7)
    {
        if(change == 0)
        {
            if (game2_images && g2_images_size > 2 && game2_images[2].img) {
                lv_img_set_src(objs[3], game2_images[2].img);
            }
            if (game2_images && g2_images_size > 1 && game2_images[1].img) {
                lv_img_set_src(objs[4], game2_images[1].img);
            }
        }
        else
        {
            if (game2_images && g2_images_size > 1 && game2_images[1].img) {
                lv_img_set_src(objs[3], game2_images[1].img);
            }
            if (game2_images && g2_images_size > 2 && game2_images[2].img) {
                lv_img_set_src(objs[4], game2_images[2].img);
            }
        }
    }
}

g_game2::~g_game2()
{
    running = false;
    stopPlaying();

    float team_mult = 1 + (0/*successful teammates*/ * .1);
    float exp_gain = ((float)score / (3.5 - ((float)(difficulty - 1) * 0.0625))) * team_mult;
    if(exp_gain < -35)
        exp_gain = -35;
    (*e_change) = exp_gain;

    // SAFETY: Process LVGL timer queue before deleting objects
    // This prevents crashes from timers accessing objects during deletion
    lv_timer_handler();
    //
    //// SAFETY: Small delay to allow any pending LVGL operations to complete
    vTaskDelay(pdMS_TO_TICKS(10));

    // SAFETY: Check if objects are still valid before deletion
    // Make sure all LVGL objects are deleted safely
    for(int i = 0; i < obj_count; i++)
    {
        if(objs[i] != nullptr)
        {
            // Verify object is still valid before deletion
            if (lv_obj_is_valid(objs[i])) {
                lv_obj_del(objs[i]);
            }
            objs[i] = nullptr;
        }
    }

    if(score_label != nullptr)
    {
        // Verify object is still valid before deletion
        if (lv_obj_is_valid(score_label)) {
            lv_obj_del(score_label);
        }
        score_label = nullptr;
    }

    if(bg != nullptr)
    {
        // Verify object is still valid before deletion
        if (lv_obj_is_valid(bg)) {
            lv_obj_del(bg);
        }
        bg = nullptr;
    }

    // SAFETY: Final timer handler call after object deletions
    lv_timer_handler();

    if(m != nullptr)
    {
        delete m;
        m = nullptr;
    }

    // Free cached images allocated by preload_image_directory
    free_cached_images(game2_images, g2_images_size);

    // No need to delete active_objs or ps because:
    // - active_objs is a fixed-size int array on the stack (not dynamically allocated)
    // - ps is also a fixed-size array of Phys structs (no dynamic memory inside)
}
