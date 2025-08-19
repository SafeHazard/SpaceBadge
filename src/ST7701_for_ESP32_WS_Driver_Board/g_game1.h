#pragma once
#include <lvgl.h>
#include "game_parent.h"
#include "multiplayer_overlay.h"

namespace game1{
    struct Phys
    {
        float x{0};
        float y{0};
        float vx{0};
        float vy{0};
        Phys(float nx = 0, float ny = 0, float nvx = 0, float nvy = 0) : x(nx), y(ny), vx(nvx),  vy(nvy) {}
    };


    class g_game1 : public game_parent
    {
    private:
        int tick = 0;
        lv_obj_t* bg;
        bool running = false;


    public:
        g_game1(int* exp_change);
        ~g_game1() override;

        void PlayerInput(uint16_t x, uint16_t iny) override;
        void Update() override;
        void Setup() override;
        void render() override;
        bool setup = false;

        Phys p1;

        float xpos1;
        float ypos1;
        float xvel1;
        float yvel1;
        int flag = 0; // 0 - ready, 1 - busy
        int tap_counter;

        int difficulty = 7;
        int score = 0;
        float grav_mult;
        float vel_threshold;

        lv_obj_t *objs[4];

        lv_obj_t* lose_text;
        lv_obj_t* score_label;

        int* e_change;

        multi_scn* m;

        bool player_paused;
        unsigned long deathTimestamp = 0;
        unsigned long respawnTime = 3000Ul;
    };
}