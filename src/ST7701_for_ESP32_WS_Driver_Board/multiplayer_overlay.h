#pragma once
#include <lvgl.h>
#include "global.hpp"

class multi_scn
{
private:
	lv_obj_t* lbl_1;
	lv_obj_t* lbl_2;
	lv_obj_t* lbl_3;
	lv_obj_t* lbl_4;
	
	lv_obj_t* sld_game_p1;
	lv_obj_t* sld_game_p2;
	lv_obj_t* sld_game_p3;
	lv_obj_t* sld_game_p4;
	lv_obj_t* sld_game_score;
	lv_obj_t* sld_game_timer;
	
	lv_obj_t* lbl_score;
	lv_obj_t* lbl_time;

    lv_obj_t* parent_obj;

    lv_obj_t* bad_effect;
    lv_obj_t* good_effect;

	int playerCount = 0;

public:
    multi_scn();
    ~multi_scn();
	void create_ui();
	void update_ui(int t_percent, int s_percent);
    void flicker_good();
    void flicker_bad();
    int calc_difficulty(int gameIdx);
};
