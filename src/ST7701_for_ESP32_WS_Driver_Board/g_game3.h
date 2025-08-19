#pragma once
#include <lvgl.h>
#include "game_parent.h"
#include "multiplayer_overlay.h"

namespace game3{
    #define COUNT 7

    struct Phys
    {
        float x{0};
        float y{0};
        float vx{0};
        float vy{0};
        Phys(float nx = 0, float ny = 0, float nvx = 0, float nvy = 0) : x(nx), y(ny), vx(nvx),  vy(nvy) {}
    };

    class g_game3 : public game_parent
    {
    private:
        int count = 0;

        lv_obj_t* bg;
        bool running = false;

        void CreatePkt(int idx); // spawns a placeholder object that can be re-used
        void RespawnPkt(int lane, bool safe); // activates object
        void DestroyPkt(int index, bool bottom); //hide and deactivate object

    public:
        g_game3(int* exp_change);
        ~g_game3() override;
        void PlayerInput(uint16_t inx, uint16_t iny) override;
        void Update() override;
        void Setup() override;
        void render() override;
        bool setup = false;

        int flag = 0; // 0 - ready, 1 - busy
        int score = 0;
        int difficulty = 1; // 0-7
        float GRAV = 1;
        float bad_chance = .3;
        int mult = 1;

        lv_obj_t *objs[COUNT];
        int alive[COUNT];
        Phys ps[COUNT];
        multi_scn* m;

        int max_cnt = 3;
        int* e_change;

        unsigned long lastSpawnTime = 0;
        unsigned long spawnInterval; // spawn every 1000 ms
        lv_obj_t *score_label;
    };
}