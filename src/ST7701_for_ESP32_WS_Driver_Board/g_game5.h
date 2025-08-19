#pragma once
#include <lvgl.h>
#include "game_parent.h"
#include "multiplayer_overlay.h"

namespace game5{

    struct O
    {
        lv_obj_t *obj;
        int dir;
        float x;
        float y;
        bool active;

        // Default constructor (needed for array)
        O() : obj(nullptr), dir(0), active(false) {}

        // Parameters
        O(lv_obj_t *nobj, int ndir = 0, bool nactive = false) : obj(nobj), dir(ndir), active(nactive) {}

        // ~O()
        // {
        //     if(obj != nullptr)
        //     {
        //         lv_obj_del(obj);
        //         obj = nullptr;
        //     }
        // }
    };

    class g_game5 : public game_parent
    {
    private:
        lv_obj_t* bg;
        bool running = false;

    public:
        g_game5(int* exp_change);
        ~g_game5() override;
        void PlayerInput(uint16_t inx, uint16_t iny) override;
        void Update() override;
        void Setup() override;
        void render() override;
        
        int SpawnRandArrow(); // return idx of new obj
        void DeleteArrow(int i);
        void KillArrow(int i);
        int GetArrowCount();

        int score = 0;
        int difficulty = 1; // 0-7

        float speed;

        lv_obj_t *shield;
        int shield_dir = 0; // direction that the shield is DEFENDING from
        lv_obj_t *player;
        O objs[10]; //  list + buffer
        lv_obj_t *score_label;
        int* e_change;
        multi_scn* m;

        unsigned long lastSpawnTime = 0;
        unsigned long spawnInterval; // spawn every 1000 ms

        bool setup = false;
    };
}