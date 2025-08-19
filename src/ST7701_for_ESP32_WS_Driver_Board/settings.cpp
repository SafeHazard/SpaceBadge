#include "settings.h"

// en/disable airplane mode
void set_airplane_mode(lv_event_t* e)
{
    config.board.airplaneMode = (lv_obj_get_state(objects.check_settings_airplanemode) & LV_STATE_CHECKED);
    saveAndApplyBoardConfig(config);
    play_random_beep();
}

// en/disable badge mode
void set_badge_mode(lv_event_t* e)
{
    config.board.badgeMode.enabled = (lv_obj_get_state(objects.check_settings_badgemode) & LV_STATE_CHECKED);
    saveAndApplyBoardConfig(config);
    play_random_beep();
}

// increase badge mode delay by 5 seconds, but not above 900 (15 min)
void increase_delay(lv_event_t* e)
{
    config.board.badgeMode.delay = min(900, config.board.badgeMode.delay + 5);
    saveAndApplyBoardConfig(config);
    play_random_beep();
}

// decrease badge mode delay by 5 seconds, but not below 5 seconds
void decrease_delay(lv_event_t* e)
{
    config.board.badgeMode.delay = max(5, config.board.badgeMode.delay - 5);
    saveAndApplyBoardConfig(config);
    play_random_beep();
}

// rotate through usernames you allow to display (all/none/friends/not blocked)
void set_usernames(lv_event_t* e)
{
    char* usernames = lv_label_get_text(objects.lbl_settings_usernames);
    if (strcmp(usernames, "Everyone") == 0)
        config.board.displayNameOptions.showNamesFrom = NotBlocked;
    else if (strcmp(usernames, "Not Blocked") == 0)
        config.board.displayNameOptions.showNamesFrom = Crew;
    else if (strcmp(usernames, "Crew") == 0)
        config.board.displayNameOptions.showNamesFrom = None;
    else
    {
        config.board.displayNameOptions.showNamesFrom = Everyone;
    }
    reloadContactLists();
    saveAndApplyBoardConfig(config);
    play_random_beep();
}

// rotate through who is allowed to join your games (all/none/friends/not blocked)
void set_gamehosts(lv_event_t* e)
{
    char* gameHosts = lv_label_get_text(objects.lbl_settings_gamehosts);
    if (strcmp(gameHosts, "Everyone") == 0)
        config.board.displayNameOptions.gameHosts = NotBlocked;
    else if (strcmp(gameHosts, "Not Blocked") == 0)
        config.board.displayNameOptions.gameHosts = Crew;
    else if (strcmp(gameHosts, "Crew") == 0)
        config.board.displayNameOptions.gameHosts = None;
    else
    {
        config.board.displayNameOptions.gameHosts = Everyone;
    }
    saveAndApplyBoardConfig(config);
    play_random_beep();
}

// change the badge volume
void set_volume(lv_event_t* e)
{
    config.board.volume = lv_slider_get_value(objects.sld_settings_volume);
    saveAndApplyBoardConfig(config);
    play_random_beep();
}
