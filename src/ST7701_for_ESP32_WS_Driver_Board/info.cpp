#include "info.h"

void info_credits(lv_event_t* e)
{ 
	play_random_beep();
	lv_obj_clear_flag(objects.pnl_info_credits, LV_OBJ_FLAG_HIDDEN);
}

void info_docs(lv_event_t* e)
{
	play_random_beep();
	lv_obj_clear_flag(objects.cnt_info_qr, LV_OBJ_FLAG_HIDDEN);
}

void info_disclaimers(lv_event_t* e)
{ 
	play_random_beep();
	lv_obj_clear_flag(objects.pnl_info_disclaimers, LV_OBJ_FLAG_HIDDEN);
}

void info_clear_credits(lv_event_t* e)
{
	play_random_beep();
	lv_obj_add_flag(objects.pnl_info_credits, LV_OBJ_FLAG_HIDDEN);
}

void info_clear_docs(lv_event_t* e)
{
	play_random_beep();
	lv_obj_add_flag(objects.cnt_info_qr, LV_OBJ_FLAG_HIDDEN);
}

void info_clear_disclaimers(lv_event_t* e)
{
	play_random_beep();
	lv_obj_add_flag(objects.pnl_info_disclaimers, LV_OBJ_FLAG_HIDDEN);
}