#pragma once
#include <lvgl.h>
#include "game_parent.h"
#include "multiplayer_overlay.h"

namespace game4{
    class g_game4 : public game_parent
    {
    private:
        lv_obj_t* bg;
        bool running = false;

    public:
        g_game4(int* exp_change);
        ~g_game4() override;
        void PlayerInput(uint16_t inx, uint16_t iny) override;
        void Update() override;
        void Setup() override;
        void render() override;

        static constexpr int grid_size = 8;
        static bool changed;

        void GenerateMap();
        void ResetMap();
        int Step(int realIdx, int colorIdx); // returns next cell index
        bool setup = false;

        int score = 0;
        int difficulty = 1; // 0-7
        static int colors;
        int max_steps = 0;
        
        bool crashout = false;
        bool major_issue = false;

        lv_obj_t *grid[grid_size * grid_size];
        lv_color_t color_lookup[9]; // -1 - uninitialized, 0 - empty, 1 - red, 2 - blue, 3 - green, 4 - purple, 5 - yellow, 6 - cyan, 7 - pink, 8 - brown

        int offset = 20;
        // lv_obj_t *selected_indicator;
        // lv_obj_t *left_arrow;
        // lv_obj_t *right_arrow;

        //input
        lv_obj_t *sel;
        lv_obj_t *cs[8];

        int head_indexes[9];
        int tail_indexes[9];

        int cmap[grid_size * grid_size]; // -1 - uninitialized, 0 - empty, 1 - red, 2 - blue, 3 - green, 4 - purple, 5 - yellow, 6 - cyan, 7 - pink, 8 - brown
        static int active_color;
        lv_obj_t *score_label;
        lv_obj_t *selected_text;
        int* e_change;
        multi_scn* m;

        bool CheckForWin();
        bool DFS(int current, int target, int color, bool visited[]);
    };
}