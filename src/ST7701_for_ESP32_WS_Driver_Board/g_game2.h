#pragma once
#include "g_game1.h"
#include <lvgl.h>
#include "game_parent.h"
#include "multiplayer_overlay.h"

namespace game2{
    struct Phys
    {
        float x{0};
        float y{0};
        float vx{0};
        float vy{0};
        Phys(float nx = 0, float ny = 0, float nvx = 0, float nvy = 0) : x(nx), y(ny), vx(nvx),  vy(nvy) {}
    };

    class g_game2 : public game_parent
    {
    private:
        int tick = 0;
        //Object player;
        //Object test_obj;
        //Object bg;
        //Object landingpad;

        lv_obj_t* bg;
        bool running = false;

        void CreateBall(int x, int y, int i);
        void DestroyBall(int index);

        void CreateCrowd(int x, int count_offset);

    public:
        g_game2(int* exp_change);
        ~g_game2() override;
        void PlayerInput(uint16_t x, uint16_t iny) override;
        void Update() override;
        void Setup() override;
        void render() override;
        bool setup = false;

        int flag = 0; // 0 - ready, 1 - busy
        int score = 0;
        int difficulty = 1; // 0-7

        float NORM_GRAV = 0.007;
        float DOWN_GRAV = 0.011;

        const static int obj_count = 5; // 3 balls + 2 crowds
        int active_objs[3]; // cut out the other objs
        bool hit[3];
        lv_obj_t *objs[obj_count];
        lv_obj_t *score_label;
        Phys ps[obj_count];
        int* e_change;

        multi_scn* m;
        int change = 0;
    };
}