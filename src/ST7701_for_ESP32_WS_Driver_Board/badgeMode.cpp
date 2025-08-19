#include "badgeMode.h"
#include "custom.h"

bool badgeMode_triggered = false;
volatile unsigned long badgeMode_lastActivity = 0;

void safeLoadXP(lv_obj_t* barGraph, int gameIndex, lv_obj_t* label)
{
if (barGraph == NULL || !lv_obj_is_valid(barGraph)) {
		LV_LOG_ERROR("[ERROR] Invalid object for game %d XP bar\n", gameIndex + 1);
		return;
	}
	
	if (gameIndex < 0 || gameIndex >= config.games.size()) {
		LV_LOG_ERROR("[ERROR] Invalid game index %d for XP bar\n", gameIndex);
		return;
	}

	if(config.games[gameIndex].XP < 0) {
		LV_LOG_ERROR("[ERROR] Invalid XP value %d for game %d\n", config.games[gameIndex].XP, gameIndex + 1);
		return;
	}

	// Set the XP value for the game bar
	lv_bar_set_value(barGraph, config.games[gameIndex].XP, LV_ANIM_OFF);
	if (config.games[gameIndex].XP > lv_bar_get_max_value(barGraph))
	{
		lv_obj_set_style_text_color(label, lv_color_hex(0x00FF00), 0); // set to green if XP exceeds max
	}
}

// load the badge screen and rotate the display
void load_screen_badge()
{
	LV_LOG_INFO("[BADGE] Loading badge mode screen...\n");
	
#ifndef _WIN32
	lv_display_t* disp = lv_display_get_default();
	if (disp == NULL)
	{
		LV_LOG_ERROR("No default display found\n");
		badgeMode_triggered = false; // Reset flag if we can't load
		return;
	}

	// Verify badge screen exists before attempting to load
	if (!objects.badge || !lv_obj_is_valid(objects.badge)) {
		LV_LOG_ERROR("[ERROR] Badge screen object is invalid\n");
		badgeMode_triggered = false;
		return;
	}

	set_display_rotation(disp, LV_DISPLAY_ROTATION_180); //doesn't work on Windows.
	delay(750); // Increased delay for screen stabilization
#endif
	
	lv_screen_load_anim(objects.badge, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
	
	// Verify screen loaded successfully
	if (lv_screen_active() == objects.badge) {
		LV_LOG_INFO("[BADGE] Badge screen loaded successfully\n");
	} else {
		LV_LOG_ERROR("[ERROR] Failed to load badge screen\n");
		badgeMode_triggered = false;
	}

	safeLoadXP(objects.bar_badge_game1, 0, objects.lbl_badge_game1);
	safeLoadXP(objects.bar_badge_game2, 1, objects.lbl_badge_game2);
	safeLoadXP(objects.bar_badge_game3, 2, objects.lbl_badge_game3);
	safeLoadXP(objects.bar_badge_game4, 3, objects.lbl_badge_game4);
	safeLoadXP(objects.bar_badge_game5, 4, objects.lbl_badge_game5);
	safeLoadXP(objects.bar_badge_game6, 5, objects.lbl_badge_game6);
}

// end badge mode and go back to the main screen
void end_badge_mode(lv_event_t* e)
{
#ifndef _WIN32
	lv_display_t* disp = lv_display_get_default();
	if (disp == NULL)
	{
		LV_LOG_ERROR("No default display found\n");
		return;
	}
	set_display_rotation(disp, LV_DISPLAY_ROTATION_0);
#endif
	badgeMode_triggered = false;

	play_random_beep();
	
	// Ensure flavor text restarts when exiting badge mode
	extern void startRandomLabelUpdates();
	
	startRandomLabelUpdates();
	LV_LOG_INFO("[BADGE] Exiting badge mode - flavor text and animations restarted\n");
	
	lv_screen_load_anim(objects.main, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
}

void badgeModeCheck()
{
	if (badgeMode_triggered)
	{
		badgeMode_lastActivity = millis();
		return;
	}

	// Only trigger badge mode from specific screens
	lv_obj_t* active_screen = lv_screen_active();
	if (active_screen != objects.main && 
	    active_screen != objects.mission &&
	    active_screen != objects.contacts &&
	    active_screen != objects.settings &&
	    active_screen != objects.profile &&
	    active_screen != objects.avatar &&
	    active_screen != objects.info) {
		badgeMode_lastActivity = millis(); // Reset timer if not on eligible screen
		return;
	}

	unsigned long now = millis();
	if (now - badgeMode_lastActivity >= config.board.badgeMode.delay * 1000 && config.board.badgeMode.enabled)
	{
		LV_LOG_INFO("[BADGE] Triggering badge mode after %lu seconds of inactivity\n", (now - badgeMode_lastActivity) / 1000);
		badgeMode_triggered = true;
		load_screen_badge();
	}
}