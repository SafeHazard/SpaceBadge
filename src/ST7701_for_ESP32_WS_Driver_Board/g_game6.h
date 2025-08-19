#pragma once
#include <lvgl.h>
#include <cmath>
#include "game_parent.h"
#include "multiplayer_overlay.h"

namespace game6{
    #define VANISHING_X 120
    #define VANISHING_Y 50
    #define VANISHING_Z 6 //6
    #define LANE_WIDTH 30
    #define DEPTH_SCALE 100
    #define PLAYER_SIZE 40

    struct Pos
    {
        float x;
        float y;
        float z; // scaling + pos controller
        int scale;
        int lane;

        Pos() : x(0), y(0), z(0), scale(255), lane(0) {} // 255 = 1 for some reason... thanks guys

        Pos(float nx, float ny, float nz, int nscale, int nlane) : x(nx), y(ny), z(nz), scale(nscale), lane(nlane) {}

        bool operator==(const Pos& other) const 
        {
            return abs((int)x - (int)other.x) < 1 && abs((int)y - (int)other.y) < 1 && abs((int)z - (int)other.z) < 1;
        }

        Pos operator-(const Pos& other) const
        {
            float xs = other.x - x;
            float ys = other.y - y;
            float zs = other.z - z;
            return Pos(xs, ys, zs, scale, lane);
        }

        Pos& operator+=(float delta)
        {
            if(z <= 0.3)
                z = 0.3;
            else
                z -= delta;

            x = VANISHING_X + (lane * LANE_WIDTH) / z;
            y = VANISHING_Y + DEPTH_SCALE / z;
            return *this;
        }
    };

    struct Obstacle
    {
        lv_obj_t *obj;
        Pos pos;
        bool scored;

        Obstacle() : obj(nullptr), pos(Pos()), scored(false) {}

        Obstacle(lv_obj_t *nobj, Pos npos) : obj(nobj), pos(npos), scored(false) {}

        ~Obstacle()
        {
            if (obj != nullptr)
            {
                lv_obj_del(obj);
                obj = nullptr;
            }
        }

    };

    struct Player
    {
        lv_obj_t *obj;
        Pos pos;

        ~Player()
        {
            if(obj != nullptr)
            {
                lv_obj_del(obj);
                obj = nullptr;
            }
        }
    };

    class g_game6 : public game_parent
    {
    private:
        lv_obj_t* bg;
        bool running = false;

    public:
        g_game6(int* exp_change);
        ~g_game6() override;
        void PlayerInput(uint16_t inx, uint16_t iny) override;
        void Update() override;
        void Setup() override;
        void render() override;
        int CreateObstacle(); // -1, 0, 1

        int score = 0;
        int difficulty = 1; // 0-7
        float speed = 1;

        static constexpr int OBJ_BUFFER = 5;
        Obstacle *obs[OBJ_BUFFER];

        unsigned long lastSpawnTime = 0;
        unsigned long spawnInterval; // spawn every 1000 ms
        multi_scn* m;

        Player *player;
        int* e_change;

        lv_obj_t *score_label;
        bool setup = false;
    };
}