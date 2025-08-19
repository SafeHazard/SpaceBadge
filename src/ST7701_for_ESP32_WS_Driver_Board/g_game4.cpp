#pragma once
#include "g_game4.h"
#include <lvgl.h>
#include <Arduino.h>
#include "pngs.h"
#include "multiplayer_overlay.h"
#include "mp3s.h"

using namespace game4;
#define C 30

CachedImage* game4_images;
size_t g4_images_size;

extern CachedMP3* game_sfx;
extern size_t mp3_sfxCount;

int g_game4::active_color = 1;
bool g_game4::changed = false;
int g_game4::colors = 0;
lv_color_t lv_color_yellow() { return lv_color_make(255, 255, 0); }
lv_color_t lv_color_dark_gray() { return lv_color_make(64, 64, 64); }
lv_color_t lv_color_red() { return lv_color_make(255, 0, 0); }

static void increase_color_cb(lv_event_t* e) {

    if(++g_game4::active_color > g_game4::colors)
    {
        g_game4::active_color = 0;
    }

    g_game4::changed = true;
}

static void decrease_color_cb(lv_event_t* e) {

    if(--g_game4::active_color < 0)
    {
        g_game4::active_color = g_game4::colors;
    }

    g_game4::changed = true;
}

static void set_color_cb(lv_event_t* e) 
{

    int new_color = (int)(intptr_t)lv_event_get_user_data(e);  // Cast user data to int
    g_game4::active_color = new_color;
    g_game4::changed = true;
}

g_game4::g_game4(int* exp_change) : game_parent(exp_change), e_change(exp_change)
{
    LV_LOG_INFO("\n\nCreated new g4 class\n");
    lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);
    score_label = nullptr;  // Safe init
    selected_text = nullptr;

    const char* path = "/images/game4/lvl1";
    m = new multi_scn();
    difficulty = m->calc_difficulty(4);
    switch(difficulty)
    {
        case 1:
            path = "/images/game4/lvl1";
            break;
        case 2:
            path = "/images/game4/lvl2";
            break;
        case 3:
            path = "/images/game4/lvl3";
            break;
        case 4:
            path = "/images/game4/lvl4";
            break;
        case 5:
            path = "/images/game4/lvl5";
            break;
        case 6:
            path = "/images/game4/lvl6";
            break;
        case 7:
            path = "/images/game4/lvl7";
            break;
    }

    if(!preload_image_directory(path, &game4_images, &g4_images_size))
    {
        LV_LOG_ERROR("failed to load game images\n");
    }

    // initialize color table for renderer
    color_lookup[0] = lv_color_hex(0xFFFFFF); // empty
    color_lookup[1] = lv_color_hex(0xFF0000); // red
    color_lookup[2] = lv_color_hex(0x00FF00); // blue
    color_lookup[3] = lv_color_hex(0x0000FF); // green
    color_lookup[4] = lv_color_hex(0x800080); // purple
    color_lookup[5] = lv_color_hex(0xFFFF00); // yellow
    color_lookup[6] = lv_color_hex(0x00FFFF); // cyan
    color_lookup[7] = lv_color_hex(0xFF00FF); // pink
    color_lookup[8] = lv_color_hex(0xB87333); // brown

    bg = nullptr;

    m->create_ui();
    start_timer();
}

void g_game4::ResetMap()
{
    for(int i = 0; i < grid_size * grid_size; i++)
    {
        lv_obj_set_style_bg_color(grid[i], color_lookup[0], 0);
        cmap[i] = 0;
    }
}

int g_game4::Step(int realIdx, int colorIdx)
{
    int next = -1;
    bool checkU = false;
    bool checkR = false;
    bool checkD = false;
    bool checkL = false;
    bool done = false;

    while(!done)
    {
        int dir = random(0, 4); // 0 - up, 1 - right, 2 - down, 3 - left
        if(checkU && checkR && checkD && checkL) // if all directions have been checked, but no cell is empty, failout
        {
            next = -1;
            done = true;
            break;
        }

        switch(dir)
        {
            case 0:
                if(checkU) break;
                checkU = true;
                if(realIdx - grid_size < 0) break; // check if out of the top of the grid
                next = realIdx - grid_size;
                if(cmap[next] != 0 /*&& cmap[next] != colorIdx*/) break; // check if the square is occupied
                done = true;
                break;
            case 1:
            right:
                if(checkR) break;
                checkR = true;
                //if((int)(realIdx + 1) / grid_size != (int)realIdx / grid_size) break; // check if wrapping the right side of the grid
                if(abs(((realIdx + 1) % grid_size) - ((realIdx) % grid_size)) > 1) break;
                next = realIdx + 1;
                if(cmap[next] != 0 /*&& cmap[next] != colorIdx*/) break; // check if the square is occupied
                done = true;
                break;
            case 2:
            down:
                if(checkD) break;
                checkD = true;
                if(realIdx + grid_size > (grid_size * grid_size) - 1) break; // check if out of the bottom of the grid
                next = realIdx + grid_size;
                if(cmap[next] != 0 /*&& cmap[next] != colorIdx*/) break; // check if the square is occupied
                done = true;
                break;
            case 3:
            left:
                if(checkL) break;
                checkL = true;
                if(abs(((realIdx - 1) % grid_size) - ((realIdx) % grid_size)) > 1) break; // check if wrapping the left side of the grid
                next = realIdx - 1;
                if(cmap[next] != 0 /*&& cmap[next] != colorIdx*/) break; // check if the square is occupied
                done = true;
                break;
            default: // make sure that the random number generater doesn't have an aneurysm
                done = false;
                next = -1;
                LV_LOG_WARN("\nGenerated false number\n\n");
                break;
        }
    }
    

    if(next >= 0 && next < grid_size * grid_size && cmap[next] < 1) // if the cell is clear, return it
    {
        return next;
    }
    else // if the number is wacky, return -1 (null)
    {
        return -1;
        crashout = true;
    }
}

void g_game4::GenerateMap()
{
    major_issue = false;
    
    for(int i = 0; i < colors; i++) // do this for each color
    {
        head_indexes[i] = -1;
        tail_indexes[i] = -1;
        bool unique = false;

        while(!unique) // while the selected grid square is not empty
        {

            int rand = random(0, (grid_size * grid_size)); // pick random grid square
            unique = true; // assume unique
            
            if (cmap[rand] != 0) // check if the cell is already taken
            {
                unique = false;
            }
            else
            {
                for(int c = 0; c < colors; c++) // loop through every color
                {
                    if(i != c && head_indexes[c] == rand) // check all the heads -- if the cell chosen is NOT unique, set the loop to pick a new cell
                    {
                        unique = false;
                        break;
                    }
                }
            }
            
            if(unique) // if the cell is completely unique
            {
                head_indexes[i] = rand;
                int next = Step(rand, i + 1); // get a neighboring cell
                if(next == -1) // if no neightboring cell is empty, failout
                {
                    LV_LOG_ERROR("\nMAJOR ISSUE --- MAJOR ISSUE WE CANNOT GET A SAFE STARTING BASE GRID ------ KILLING PROGRAM\n\n");
                    major_issue = true;
                    return;
                }

                tail_indexes[i] = next; // assuming the neighbor cell is empty, take over that cell
                cmap[head_indexes[i]] = i + 1; // assuming the head cell is empty, take over that cell 
                cmap[tail_indexes[i]] = i + 1; // color the new cell
            }
        }
    }

    for(int col = 0; col < colors; col++) // for each color
    {
        for(int i = random((float)(max_steps / 2) + 0.5f, max_steps); i >= 0; i--) // move each tail a couple cells
        {
            if(crashout)
            {
                crashout = false;
                break;
            }

            int a = Step(tail_indexes[col], col + 1); // get the next cell
            if(a != -1) // if the function says a cell is empty
            {
                cmap[a] = col + 1; // set grid color
                tail_indexes[col] = a; // notate tail index
                // printf("Extending color line. idx - %d, color - %d\n", a, col + 1);
            }
            else // if no cells are empty, end the line
            {
                // printf("NO SAFE DIRECTION FOUND -- ENDING LINE.\n");
                break;
            }
        }
    }

    for(int c = 0; c < grid_size * grid_size; c++)
    {
        bool reset = true;
        for(int i = 0; i < colors; i++)
        {
            // printf("IDX: %d  -- HEAD: %d  -- TAIL: %d\n", c, head_indexes[i], tail_indexes[i]);
            if(head_indexes[i] == c || tail_indexes[i] == c) // if the cell is not found in the head or tail list, reset the color on cmap
            {
                reset = false;
                break;
            }
        }

        if(reset)
        {
            cmap[c] = 0;
            // printf("Resetting %d\n", c);
        }
    }
}

void g_game4::Setup()
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
            colors = 8;
            max_steps = 8;
            break;
        case 6:
            colors = 7;
            max_steps = 10;
            break;
        case 5:
            colors = 7;
            max_steps = 9;
            break;
        case 4:
            colors = 6;
            max_steps = 9;
            break;
        case 3:
            colors = 5;
            max_steps = 12;
            break;
        case 2:
            colors = 4;
            max_steps = 11;
            break;
        case 1:
            colors = 3;
            max_steps = 7;
            break;
        default:
            colors = 2;
            max_steps = 10;
        break;
    }

    //left_arrow = lv_btn_create(lv_screen_active());
    //right_arrow = lv_btn_create(lv_screen_active());

    //lv_obj_set_size(left_arrow, 40, 40);
    //lv_obj_set_size(right_arrow, 40, 40);

    //lv_obj_align(left_arrow, LV_ALIGN_BOTTOM_MID, -50, -10);
    //lv_obj_align(right_arrow, LV_ALIGN_BOTTOM_MID, 50, -10);

    //lv_obj_add_event_cb(left_arrow, decrease_color_cb, LV_EVENT_RELEASED, NULL);
    //lv_obj_add_event_cb(right_arrow, increase_color_cb, LV_EVENT_RELEASED, NULL);

    //lv_obj_t* left_label = lv_label_create(left_arrow);
    //lv_obj_t* right_label = lv_label_create(right_arrow);

    //lv_label_set_text(left_label, "<");
    //lv_label_set_text(right_label, ">");

    //lv_obj_set_style_text_font(left_label, &lv_font_montserrat_24, 0);
    //lv_obj_set_style_text_font(right_label, &lv_font_montserrat_24, 0);


    //// Color indicator box
    //selected_indicator = lv_img_create(lv_screen_active());
    //lv_img_set_src(selected_indicator, game4_images[8].img);
    //lv_obj_set_size(selected_indicator, 40, 40);
    //lv_obj_align(selected_indicator, LV_ALIGN_BOTTOM_MID, 0, -10);
    //lv_obj_set_style_bg_color(selected_indicator, color_lookup[active_color + 1], 0);
    //lv_obj_set_style_border_width(selected_indicator, 2, 0);
    //lv_obj_set_style_border_color(selected_indicator, lv_color_black(), 0);


    // create base variables (it will kill me otherwise)
    for(int i = 0; i < grid_size * grid_size; i++)
    {
        grid[i] = lv_img_create(lv_screen_active());
        if (game4_images && g4_images_size > 8 && game4_images[8].img) {
            lv_img_set_src(grid[i], game4_images[8].img);
        } else {
            LV_LOG_ERROR("[ERROR] game4_images[8].img is null\n");
        }
        lv_obj_set_size(grid[i], C, C);

        float x = ((i % grid_size) - ((float)grid_size / 2.0f) + 0.5f) * C;
        float y = (((i / grid_size) - ((float)grid_size / 2.0f) + 0.5f) * C) + offset;

        // printf("IDX: %d    X: %d -- Y: %d\n", i, x, y);
        lv_obj_align(grid[i], LV_ALIGN_CENTER, x, y - 15);  // Center it
        lv_obj_set_style_border_width(grid[i], 0, 0);
        lv_obj_set_style_border_color(grid[i], lv_color_black(), 0);
        lv_obj_set_style_bg_color(grid[i], color_lookup[0], 0);

        cmap[i] = 0;
    }

    for(int i = 0; i < colors; i++)
    {
        cs[i] = lv_btn_create(lv_screen_active());
        float x = ((i % colors) - ((float)colors / 2.0f) + 0.5f) * C;
        lv_obj_align(cs[i], LV_ALIGN_BOTTOM_MID, x, -5);  // Center it

        lv_obj_set_style_bg_color(cs[i], color_lookup[active_color + 1], 0);
        lv_obj_set_style_border_width(cs[i], 5, 0);
        lv_obj_clear_flag(cs[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_size(cs[i], C, C);

        lv_obj_t *img = lv_img_create(cs[i]);
        lv_obj_set_size(img, C-10, C-10);
        lv_obj_center(img);
        if (game4_images && g4_images_size > i && game4_images[i].img) {
            lv_img_set_src(img, game4_images[i].img);
        } else {
            LV_LOG_ERROR("[ERROR] game4_images[%d].img is null\n", i);
        }


        lv_obj_add_flag(cs[i], LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_border_color(cs[i], lv_color_dark_gray(), 0);
        lv_obj_add_event_cb(cs[i], set_color_cb, LV_EVENT_CLICKED, (void*)(intptr_t)(i + 1));
    }

    major_issue = false;
    GenerateMap();
    while(major_issue)
    {
        ResetMap();
        GenerateMap();
    }

    if (load_game_background(4, difficulty))
    {
        LV_LOG_INFO("success on bg\n");
        lv_obj_set_style_bg_image_src(lv_screen_active(), NULL, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else
    {
        LV_LOG_ERROR("failure on bg - using fallback\n");
        // Fallback: solid black background  
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    changed = true;
    setup = true;
    running = true;
}

bool g_game4::CheckForWin()
{
    for (int c = 1; c <= colors; c++) {
        bool visited[grid_size * grid_size] = { false };
        if (!DFS(head_indexes[c - 1], tail_indexes[c - 1], c, visited)) {
            // printf("Color %d path not found\n", c);
            return false;
        }
        // printf("Color %d path OK\n", c);
    }

    m->flicker_good();
    if(game_sfx != nullptr)
    {
        Play_Cached_Mp3(game_sfx[3]);
    }
    return true;
}

bool g_game4::DFS(int current, int target, int color, bool visited[])
{
    if (current == target) return true;
    visited[current] = true;

    int row = current / grid_size;
    int col = current % grid_size;

    // Directions: up, right, down, left
    const int drow[] = { -1, 0, 1, 0 };
    const int dcol[] = { 0, 1, 0, -1 };

    for (int i = 0; i < 4; i++) {
        int new_row = row + drow[i];
        int new_col = col + dcol[i];

        if (new_row < 0 || new_row >= grid_size || new_col < 0 || new_col >= grid_size)
            continue;

        int next = new_row * grid_size + new_col;

        if (!visited[next] && cmap[next] == color) {
            if (DFS(next, target, color, visited))
                return true;
        }
    }

    return false;
}


void g_game4::PlayerInput(uint16_t inx, uint16_t iny)
{
    // printf("Score: %d\n", score);
    int screen_w = lv_obj_get_width(lv_screen_active());
    int screen_h = lv_obj_get_height(lv_screen_active());
    int origin_x = (screen_w / 2) - (grid_size * C) / 2;
    int origin_y = ((screen_h / 2) - (grid_size * C) / 2) - 15;

    // Convert coordinates to grid indices
    int col = (inx - origin_x) / C;
    int row = (iny - offset - origin_y) / C;

    if (col >= 0 && col < grid_size && row >= 0 && row < grid_size)
    {
        int index = row * grid_size + col;
        // printf("You clicked square at index %d (row %d, col %d)\n", index, row, col);

        for(int i = 0; i < colors; i++)
        {
            if(index == head_indexes[i] || index == tail_indexes[i])
            {
                //LV_LOG_ERROR("DO NOT DELETE THE HEADS.\n");
                return;
            }
        }

        cmap[index] = active_color;
        lv_obj_set_style_bg_color(grid[index], color_lookup[active_color], 0);
        changed = true;
    }
    // else
    // {
    //     printf("Click outside grid\n");
    // }

    // printf("DID I WIN???? %d\n", CheckForWin());

    if(CheckForWin())
    {
        score += 100 * difficulty;
        major_issue = false;
        ResetMap();
        GenerateMap();
        while(major_issue)
        {
            ResetMap();
            GenerateMap();
        }
    }

    changed = true;
};

void g_game4::Update()
{
    if (!setup) return;
    if (!running) return;

    if(m != nullptr)
    {
        int scorePercent = (int(((float)score / 1200.0) * 100));
        m->update_ui(time_left(), scorePercent);
        
        // Broadcast score to other players in multiplayer games
        broadcastScore(scorePercent);
    }

    if (!changed) return;
    // printf("map changed passed --- moving to render\n");
    render();
}

// It does what it says it does
void g_game4::render()
{
    // switch(active_color)
    // {
    //     case 0: lv_img_set_src(selected_indicator, game4_images[8].img); break;
    //     case 1: lv_img_set_src(selected_indicator, game4_images[0].img); break;
    //     case 2: lv_img_set_src(selected_indicator, game4_images[1].img); break;
    //     case 3: lv_img_set_src(selected_indicator, game4_images[2].img); break;
    //     case 4: lv_img_set_src(selected_indicator, game4_images[3].img); break;
    //     case 5: lv_img_set_src(selected_indicator, game4_images[4].img); break;
    //     case 6: lv_img_set_src(selected_indicator, game4_images[5].img); break;
    //     case 7: lv_img_set_src(selected_indicator, game4_images[6].img); break;
    //     case 8: lv_img_set_src(selected_indicator, game4_images[7].img); break;
    // }
    // lv_img_set_src(grid[i], game4_images[cmap[i] - 1].img);

    // printf("---------------------- RENDER ----------------------\n");
    //if (score_label != nullptr)
    //{
    //    char buf[64];
    //    snprintf(buf, sizeof(buf), "Score: %d", score);
    //    lv_label_set_text(score_label, buf);
    //}
    //else
    //    printf("Score label missing\n");

    for(int i = 0; i < grid_size * grid_size; i++)
    {
        if(cmap[i] > -1)
        {
            switch(cmap[i])
            {
                case 0: if (game4_images && g4_images_size > 8 && game4_images[8].img) lv_img_set_src(grid[i], game4_images[8].img); break;
                case 1: if (game4_images && g4_images_size > 0 && game4_images[0].img) lv_img_set_src(grid[i], game4_images[0].img); break;
                case 2: if (game4_images && g4_images_size > 1 && game4_images[1].img) lv_img_set_src(grid[i], game4_images[1].img); break;
                case 3: if (game4_images && g4_images_size > 2 && game4_images[2].img) lv_img_set_src(grid[i], game4_images[2].img); break;
                case 4: if (game4_images && g4_images_size > 3 && game4_images[3].img) lv_img_set_src(grid[i], game4_images[3].img); break;
                case 5: if (game4_images && g4_images_size > 4 && game4_images[4].img) lv_img_set_src(grid[i], game4_images[4].img); break;
                case 6: if (game4_images && g4_images_size > 5 && game4_images[5].img) lv_img_set_src(grid[i], game4_images[5].img); break;
                case 7: if (game4_images && g4_images_size > 6 && game4_images[6].img) lv_img_set_src(grid[i], game4_images[6].img); break;
                case 8: if (game4_images && g4_images_size > 7 && game4_images[7].img) lv_img_set_src(grid[i], game4_images[7].img); break;
            }
            
            lv_obj_set_style_bg_color(grid[i], color_lookup[cmap[i]], 0); // get color of cell, get hex color using looku
        }
    }

    for(int i = 0; i < colors; i++)
    {
        if(cs[i] != nullptr)
        {
            if(active_color - 1 == i)
            {
                lv_obj_set_style_border_color(cs[i], lv_color_yellow(), 0);
            }
            else
            {
                lv_obj_set_style_border_color(cs[i], lv_color_dark_gray(), 0);
            }
        }
    }

    changed = false;
}

g_game4::~g_game4()
{
    running = false;
    stopPlaying();

    float team_mult = 1 + (0/*successful teammates*/ * .1);
    float exp_gain = ((float)score / (4 - ((float)(difficulty - 1) * .125))) * team_mult;
    if(exp_gain < -35)
        exp_gain = -35;

    (*e_change) = exp_gain;

    for(int i = 0; i < grid_size * grid_size; i++)
    {
        if(grid[i] != nullptr)
        {
            lv_obj_del(grid[i]);
            grid[i] = NULL;
        }
    }
    free_cached_images(game4_images, g4_images_size);

    // if(selected_indicator != nullptr)
    // {
    //     lv_obj_del(selected_indicator);
    //     selected_indicator = nullptr;
    // }

    // if(left_arrow != nullptr)
    // {
    //     lv_obj_del(left_arrow);
    //     left_arrow = nullptr;
    // }

    // if(right_arrow != nullptr)
    // {
    //     lv_obj_del(right_arrow);
    //     right_arrow = nullptr;
    // }

    for(int i = 0; i < colors; i++)
    {
        if(cs[i] != nullptr)
        {
            lv_obj_del(cs[i]);
            cs[i] = nullptr;
        }
    }

    if(selected_text != nullptr)
    {
        lv_obj_del(selected_text);
        selected_text = nullptr;
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