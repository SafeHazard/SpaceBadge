#include <cstdio>
#include "custom.h"
#include "mesh.h"
#include "avatar_unlocks.h"
#include "mission.h"
#include "contacts.h"
#include "info.h"
#include "mp3s.h"
#include "lightweight_tasks.h"
#include <chrono>
#include <unordered_map>
#include "./src/ui/images.h"
#include "./src/ui/styles.h"
#include "session.h"
#include "convos.h"
#include "game_parent.h"
#include "g_game1.h"
#include "g_game2.h"
#include "g_game3.h"
#include "g_game4.h"
#include "g_game5.h"
#include "g_game6.h"

#ifdef _WIN32
#include <thread>
#include <atomic>
static std::thread _rthr;
static std::atomic<bool> _rrun{ false };
#else
//#include <lvgl.h>
static lv_timer_t* _rtr = nullptr;
#endif

// globals
static void (*_rcb)() = nullptr; //repeating callback function pointer
uint64_t tickDelta = 0;
uint64_t tickThreshold = 100;

int32_t int_ms_since_touch = 0;

extern objects_t objects;                   // LVGL root screens object
extern Config config;                       // Global configuration object
extern std::vector<IDPacket> g_idPackets;   // global ID packet repo
extern ContactManager* scanResults;         // global ID packet repo (converts from IDPacket to ContactData)
extern bool badgeMode_triggered;            // global 'is badge mode triggered?' bool
extern lv_display_t* disp;                  // global reference to the LVGL display

// Repeating callback task 
#ifdef _WIN32
static void _thread_loop(uint32_t ms)
{
    while (_rrun)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        if (_rrun && _rcb) _rcb();
    }
}
#endif

// repeat the given callback function every ms milliseconds
void repeat_on(uint32_t ms, void (*cb)())
{
    repeat_off();
    _rcb = cb;
#ifdef _WIN32
    _rrun = true;
    _rthr = std::thread(_thread_loop, ms);
#else
    // no-capture lambda converts to lv_timer_cb_t
    _rtr = lv_timer_create(
        [](lv_timer_t* t) { if (_rcb) _rcb(); },
        ms, nullptr
    );
#endif
}

// stop the repeating callback
void repeat_off()
{
#ifdef _WIN32
    _rrun = false;
    if (_rthr.joinable()) _rthr.join();
#else
    if (_rtr)
    {
        lv_timer_del(_rtr);
        _rtr = nullptr;
    }
#endif
    _rcb = nullptr;
}

char* avatarIDToFilename(int32_t avatarID)
{
    // Convert the avatar ID to a filename
    static char filename[64];
    snprintf(filename, sizeof(filename), "L:/avatars/%d.png", avatarID);
    LV_LOG_INFO("Avatar ID: %d, Filename: %s\n", avatarID, filename);
    // DEBUG VERSION - CHANGE FOR PRODUCTION
    snprintf(filename, sizeof(filename), "L:/images/16x16.png");
    return filename;
}

// return the name/board ID of a contact, depending on the user's settings
//  Display Names   -> Everyone     = always show username
//                  -> None         = always show board ID
//                  -> Not Blocked  = show DisplayName unless isBlocked
//                  -> Crew         = only show DisplayName if isCrew
char* getNameFromContact(ContactData* contact)
{
    static char retVal[64]; // Static to maintain lifetime

    switch (config.board.displayNameOptions.showNamesFrom)
    {
    case Everyone:
        return contact->displayName;
        break;

    case None:
        snprintf(retVal, sizeof(retVal), "%u", contact->nodeId);
        return retVal;
        break;

    case NotBlocked:
        if (contact->isBlocked)
        {
            snprintf(retVal, sizeof(retVal), "%u", contact->nodeId);
            return retVal;
        }
        else
            return contact->displayName;
        break;

    case Crew:
        if (contact->isCrew)
            return contact->displayName;
        else
        {
            snprintf(retVal, sizeof(retVal), "%u", contact->nodeId);
            return retVal;
        }
        break;

    default:
        snprintf(retVal, sizeof(retVal), "%u", contact->nodeId);
        return retVal;
        break;
    }
}

// Force a reload of Crew & Scan contact data
void reloadContactLists()
{
    populate_crew_list(NULL);
    populate_scan_list(NULL);
}

void screenContactsLoaded(lv_event_t* e)
{
    // if the contacts screen is loaded, populate the crew and scan lists
	reloadContactLists();

    // set the current tab to scan
    int_contacts_tab_current = 0;
    lv_obj_set_state(objects.tab_contacts_scan, LV_STATE_CHECKED, true);
    lv_obj_set_state(objects.tab_contacts_crew, LV_STATE_CHECKED, false);
    
    // Refresh contact details for the scan tab
    refreshCurrentTabContactDisplay();
}

void screenInfoLoaded(lv_event_t* e)
{
    // Start info screen updates when info screen is displayed
    start_info_screen_updates();
}

void screenInfoUnloaded(lv_event_t* e)
{
    // Stop info screen updates when leaving info screen
    stop_info_screen_updates();
}

// change UI settings and apply them to the board
// only call this function after the config has been loaded and UI objects are initialized
bool applyConfig(Config& config)
{
    //username
    lv_textarea_set_text(objects.txt_profile_username, config.user.displayName);

    //volume
    lv_slider_set_value(objects.sld_settings_volume, config.board.volume, LV_ANIM_OFF);
#ifndef _WIN32
    setVolume((float)config.board.volume);
#else
    // Windows does not have a volume adjustment function, so we skip this
	LV_LOG_WARN("Volume adjustment is not supported on Windows.\n");
#endif // !_WIN32

	extern bool meshInitialized; // global mesh initialization state
    if (!meshInitialized && !config.board.airplaneMode)
    {
        exitAirplaneMode();
    }

	// airplane mode is slow/expensive, so we only change it if it user changed it
	bool checkState = lv_obj_has_state(objects.check_settings_airplanemode, LV_STATE_CHECKED);
	if (checkState != config.board.airplaneMode)
    {
        lv_obj_set_state(objects.check_settings_airplanemode, LV_STATE_CHECKED, config.board.airplaneMode);
        if (config.board.airplaneMode)
        {
            enterAirplaneMode(); // comprehensive airplane mode activation
            lv_img_set_src(objects.img_main_wifi, &img_ui_element___wi_fi_disabled); // set the wifi icon to off
        }
        else
        {
            exitAirplaneMode(); // comprehensive airplane mode deactivation
            lv_img_set_src(objects.img_main_wifi, &img_ui_element___wi_fi_enabled);
        }
    }

    //badge mode
    lv_obj_set_state(objects.check_settings_badgemode, LV_STATE_CHECKED, config.board.badgeMode.enabled);
    if (config.board.badgeMode.enabled)
    {
        lv_img_set_src(objects.img_main_badge, &img_ui_element___badge_enabled);
    }
    else
    {
        lv_img_set_src(objects.img_main_badge, &img_ui_element___badge_disabled);
	}
    lv_label_set_text(objects.lbl_settings_badgedelay, std::to_string(config.board.badgeMode.delay).c_str());

    String displayUsernames;
    switch (config.board.displayNameOptions.showNamesFrom)
    {
    case Everyone:
        displayUsernames = "Everyone";
        break;
    case NotBlocked:
        displayUsernames = "Not Blocked";
        break;
    case Crew:
        displayUsernames = "Crew";
        break;
    case None:
        displayUsernames = "None";
        break;
    default:
        break;
    }
    lv_label_set_text(objects.lbl_settings_usernames, displayUsernames.c_str());

    String gameHosts;
    switch (config.board.displayNameOptions.gameHosts)
    {
    case Everyone:
        gameHosts = "Everyone";
        break;
    case NotBlocked:
        gameHosts = "Not Blocked";
        break;
    case Crew:
        gameHosts = "Crew";
        break;
    case None:
        gameHosts = "None";
        break;
    default:
        break;
    }
    lv_label_set_text(objects.lbl_settings_gamehosts, gameHosts.c_str());

    // Load user's selected avatar with validation and fallback
    if (objects.img_main_avatar && lv_obj_is_valid(objects.img_main_avatar)) {
        bool avatar_applied = false;
        
        // Get number of unlocked avatars to check bounds
        extern int calculateUnlockedAvatars();
        int unlocked_count = calculateUnlockedAvatars();
        
        // Validate and try to apply current avatar (must be within unlocked range)
        if (img_avatar_82 && config.user.avatar >= 0 && config.user.avatar < unlocked_count && 
            config.user.avatar < (int)int_avatar_82 && img_avatar_82[config.user.avatar].name) {
            
            const lv_img_dsc_t* avatar_img = get_cached_image(img_avatar_82, int_avatar_82, img_avatar_82[config.user.avatar].name);
            if (avatar_img && avatar_img->header.w > 0 && avatar_img->header.h > 0) {
                lv_image_set_src(objects.img_main_avatar, avatar_img);
                LV_LOG_INFO("[INFO] Applied user avatar: %s (index: %d, size: %dx%d)\n", 
                              img_avatar_82[config.user.avatar].name, config.user.avatar,
                              avatar_img->header.w, avatar_img->header.h);
                avatar_applied = true;
            } else {
                LV_LOG_ERROR("[ERROR] Failed to load avatar image for index: %d (img=%p, dims=%dx%d)\n",
                              config.user.avatar, avatar_img, 
                              avatar_img ? avatar_img->header.w : 0, 
                              avatar_img ? avatar_img->header.h : 0);
            }
        } else {
            LV_LOG_WARN("[WARN] Avatar index %d outside unlocked range (0-%d) or invalid (max: %zu, array valid: %s)\n",
                          config.user.avatar, unlocked_count - 1, int_avatar_82, img_avatar_82 ? "YES" : "NO");
        }
        
        // Fallback: select random avatar from unlocked range if current selection failed
        if (!avatar_applied && img_avatar_82 && int_avatar_82 > 0 && unlocked_count > 0) {
            // Select random avatar from unlocked range
            int random_avatar = rand() % unlocked_count;
            
            // Find the random avatar within unlocked range
            if (random_avatar < (int)int_avatar_82 && img_avatar_82[random_avatar].name) {
                const lv_img_dsc_t* fallback_img = get_cached_image(img_avatar_82, int_avatar_82, img_avatar_82[random_avatar].name);
                if (fallback_img) {
                    lv_image_set_src(objects.img_main_avatar, fallback_img);
                    config.user.avatar = random_avatar; // Update config to valid unlocked avatar
                    LV_LOG_INFO("[INFO] Applied random unlocked avatar: %s (index: %d/%d unlocked)\n",
                                  img_avatar_82[random_avatar].name, random_avatar, unlocked_count);
                    avatar_applied = true;
                }
            }
        }
        
        if (!avatar_applied) {
            LV_LOG_ERROR("[ERROR] No valid avatar could be applied - img_main_avatar will show default\n");
        }
    } else {
        LV_LOG_ERROR("[ERROR] img_main_avatar object is null or invalid\n");
    }

    // Also update badge avatar with same logic (already validated unlocked range above)
    if (objects.img_badge_avatar && lv_obj_is_valid(objects.img_badge_avatar)) {
        bool badge_avatar_applied = false;
        
        // Use the same validated avatar from above (already within unlocked range)
        if (img_avatar_82 && config.user.avatar >= 0 && config.user.avatar < (int)int_avatar_82 && 
            img_avatar_82[config.user.avatar].name) {
            
            const lv_img_dsc_t* avatar_img = get_cached_image(img_avatar_82, int_avatar_82, img_avatar_82[config.user.avatar].name);
            if (avatar_img && avatar_img->header.w > 0 && avatar_img->header.h > 0) {
                lv_image_set_src(objects.img_badge_avatar, avatar_img);
                LV_LOG_INFO("[INFO] Applied user badge avatar: %s (index: %d, size: %dx%d)\n",
                              img_avatar_82[config.user.avatar].name, config.user.avatar,
                              avatar_img->header.w, avatar_img->header.h);
                badge_avatar_applied = true;
            } else {
                LV_LOG_ERROR("[ERROR] Failed to load badge avatar image for index: %d (img=%p, dims=%dx%d)\n", 
                              config.user.avatar, avatar_img,
                              avatar_img ? avatar_img->header.w : 0, 
                              avatar_img ? avatar_img->header.h : 0);
            }
        } else {
            LV_LOG_WARN("[WARN] Invalid badge avatar index: %d (max: %zu, array valid: %s)\n", 
                          config.user.avatar, int_avatar_82, img_avatar_82 ? "YES" : "NO");
        }
        
        // Fallback: use first available avatar if current selection failed
        if (!badge_avatar_applied && img_avatar_82 && int_avatar_82 > 0) {
            // Find first valid avatar
            for (size_t i = 0; i < int_avatar_82; i++) {
                if (img_avatar_82[i].name) {
                    const lv_img_dsc_t* fallback_img = get_cached_image(img_avatar_82, int_avatar_82, img_avatar_82[i].name);
                    if (fallback_img) {
                        lv_image_set_src(objects.img_badge_avatar, fallback_img);
                        LV_LOG_INFO("[INFO] Applied fallback badge avatar: %s (index: %d)\n", 
                                      img_avatar_82[i].name, (int)i);
                        badge_avatar_applied = true;
                        break;
                    }
                }
            }
        }
        
        if (!badge_avatar_applied) {
            LV_LOG_ERROR("[ERROR] No valid badge avatar could be applied - img_badge_avatar will show default\n");
        }
    } else {
        LV_LOG_ERROR("[ERROR] img_badge_avatar object is null or invalid\n");
    }

    return true;
}

bool saveAndApplyBoardConfig(Config& config)
{
    // Save the configuration to the file
    if (!saveBoardConfig(config))
    {
        LV_LOG_ERROR("Failed to save board config\n");
        return false;
    }
    // Apply the configuration to the board
    return applyConfig(config);
}

// Generic screen loader; requires that the screen to load is passed in as user_data
void load_screen(lv_event_t* e)
{
    play_random_beep();
    // Get the screen object from user_data
    lv_obj_t* target_screen = static_cast<lv_obj_t*>(lv_event_get_user_data(e));
    if (target_screen)
    {
        lv_screen_load_anim(target_screen, LV_SCR_LOAD_ANIM_NONE, 200, 0, false);
        lv_indev_reset(NULL, NULL);
        // Handle game status changes based on screen
        extern SessionManager* sessionManager;
        if (target_screen == objects.main) {
            // DON'T change status if game is actively running
            extern game_parent* game;
            if (game == nullptr && sessionManager) {
                sessionManager->setMyStatus(Idle);
                LV_LOG_INFO("[INFO] Status changed to Idle (main screen)\n");
            } else if (game != nullptr) {
                LV_LOG_INFO("[INFO] Prevented status change - game is running\n");
            }
            // Starting main screen - stop any existing animations first
            flavor_animations_stop();
            // Delay start to allow screen transition to complete
            lv_timer_t* start_timer = lv_timer_create([](lv_timer_t* timer) {
                flavor_animations_start();
                lv_timer_del(timer);
            }, 250, NULL);
            lv_timer_set_repeat_count(start_timer, 1);
        } else if (target_screen == objects.host) {
            if (sessionManager) sessionManager->setMyStatus(Hosting);
            LV_LOG_INFO("[INFO] Status changed to Hosting (host screen)\n");
            // Leaving main screen - stop flavor animations
            flavor_animations_stop();
        } else if (target_screen == objects.join) {
            if (sessionManager) sessionManager->setMyStatus(InLobby);
            LV_LOG_INFO("[INFO] Status changed to InLobby (join screen)\n");
            // Leaving main screen - stop flavor animations
            flavor_animations_stop();
        } else {
            // Leaving main screen - stop flavor animations
            flavor_animations_stop();
        }
    }
}

// Register a button (main UI) to change screens
static void cb_register(lv_obj_t* button, lv_obj_t* screen)
{
    // Safety check for null pointers (lazy loading compatibility)
    if (button && screen) {
        lv_obj_add_event_cb(button, load_screen, LV_EVENT_PRESSED, screen);
    }
}

// convert an lv_event code to a char*
const char* get_lv_event_name(lv_event_code_t event)
{
    switch (event)
    {
    case LV_EVENT_ALL: return "LV_EVENT_ALL";
    case LV_EVENT_PRESSED: return "LV_EVENT_PRESSED";
    case LV_EVENT_PRESSING: return "LV_EVENT_PRESSING";
    case LV_EVENT_PRESS_LOST: return "LV_EVENT_PRESS_LOST";
    case LV_EVENT_SHORT_CLICKED: return "LV_EVENT_SHORT_CLICKED";
    case LV_EVENT_LONG_PRESSED: return "LV_EVENT_LONG_PRESSED";
    case LV_EVENT_LONG_PRESSED_REPEAT: return "LV_EVENT_LONG_PRESSED_REPEAT";
    case LV_EVENT_CLICKED: return "LV_EVENT_CLICKED";
    case LV_EVENT_RELEASED: return "LV_EVENT_RELEASED";
    case LV_EVENT_SCROLL_BEGIN: return "LV_EVENT_SCROLL_BEGIN";
    case LV_EVENT_SCROLL_END: return "LV_EVENT_SCROLL_END";
    case LV_EVENT_SCROLL: return "LV_EVENT_SCROLL";
    case LV_EVENT_GESTURE: return "LV_EVENT_GESTURE";
    case LV_EVENT_KEY: return "LV_EVENT_KEY";
    case LV_EVENT_FOCUSED: return "LV_EVENT_FOCUSED";
    case LV_EVENT_DEFOCUSED: return "LV_EVENT_DEFOCUSED";
    case LV_EVENT_LEAVE: return "LV_EVENT_LEAVE";
    case LV_EVENT_HIT_TEST: return "LV_EVENT_HIT_TEST";
    case LV_EVENT_COVER_CHECK: return "LV_EVENT_COVER_CHECK";
    case LV_EVENT_REFR_EXT_DRAW_SIZE: return "LV_EVENT_REFR_EXT_DRAW_SIZE";
    case LV_EVENT_DRAW_MAIN_BEGIN: return "LV_EVENT_DRAW_MAIN_BEGIN";
    case LV_EVENT_DRAW_MAIN: return "LV_EVENT_DRAW_MAIN";
    case LV_EVENT_DRAW_MAIN_END: return "LV_EVENT_DRAW_MAIN_END";
    case LV_EVENT_DRAW_POST_BEGIN: return "LV_EVENT_DRAW_POST_BEGIN";
    case LV_EVENT_DRAW_POST: return "LV_EVENT_DRAW_POST";
    case LV_EVENT_DRAW_POST_END: return "LV_EVENT_DRAW_POST_END";
    case LV_EVENT_DRAW_TASK_ADDED: return "LV_EVENT_DRAW_TASK_ADDED";
    case LV_EVENT_VALUE_CHANGED: return "LV_EVENT_VALUE_CHANGED";
    case LV_EVENT_INSERT: return "LV_EVENT_INSERT";
    case LV_EVENT_REFRESH: return "LV_EVENT_REFRESH";
    case LV_EVENT_READY: return "LV_EVENT_READY";
    case LV_EVENT_CANCEL: return "LV_EVENT_CANCEL";
    default: return "OTHER_EVENT";
    }
}

// print info about the event; expects string data in the 'user data' field of the event
static void debug_events(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    if (event < 23 || event > 31)
    {
        LV_LOG_INFO("%s: Event: %s\n", static_cast<char*>(lv_event_get_user_data(e)), get_lv_event_name(event));

        if (event == LV_EVENT_CLICKED)
        {
            lv_indev_t* indev = lv_event_get_indev(e); // Get the indev object associated with the event
            if (indev)
            {
                lv_indev_type_t type = lv_indev_get_type(indev); // Get the type of the indev object
                if (type == LV_INDEV_TYPE_POINTER)
                {
                    lv_point_t point;
                    lv_indev_get_point(indev, &point); // Get the point associated with the event
                    LV_LOG_INFO("Pointer event at (%d, %d)\n", point.x, point.y);
                }
            }
        }
    }
}

// set up callbacks for objects
void setup_cb()
{
    // main UI button callbacks
    cb_register(objects.btn_main_mission, objects.mission);
    cb_register(objects.btn_main_contacts, objects.contacts);
    cb_register(objects.btn_main_info, objects.info);
    cb_register(objects.btn_main_settings, objects.settings);
    cb_register(objects.btn_main_profile, objects.profile);
    cb_register(objects.btn_main_avatar, objects.avatar);
    cb_register(objects.btn_avatar_mission, objects.mission);
    cb_register(objects.btn_avatar_contacts, objects.contacts);
    cb_register(objects.btn_avatar_info, objects.info);
    cb_register(objects.btn_avatar_settings, objects.settings);
    cb_register(objects.btn_avatar_profile, objects.profile);
    cb_register(objects.btn_avatar_main, objects.main);
    cb_register(objects.btn_mission_contacts, objects.contacts);
    cb_register(objects.btn_mission_info, objects.info);
    cb_register(objects.btn_mission_settings, objects.settings);
    cb_register(objects.btn_mission_profile, objects.profile);
    cb_register(objects.btn_mission_main, objects.main);
    cb_register(objects.btn_contacts_mission, objects.mission);
    cb_register(objects.btn_contacts_info, objects.info);
    cb_register(objects.btn_contacts_settings, objects.settings);
    cb_register(objects.btn_contacts_profile, objects.profile);
    cb_register(objects.btn_contacts_main, objects.main);
    cb_register(objects.btn_info_contacts, objects.contacts);
    cb_register(objects.btn_info_mission, objects.mission);
    cb_register(objects.btn_info_settings, objects.settings);
    cb_register(objects.btn_info_profile, objects.profile);
    cb_register(objects.btn_info_main, objects.main);
    cb_register(objects.btn_settings_mission, objects.mission);
    cb_register(objects.btn_settings_contacts, objects.contacts);
    cb_register(objects.btn_settings_info, objects.info);
    cb_register(objects.btn_settings_profile, objects.profile);
    cb_register(objects.btn_settings_main, objects.main);
    cb_register(objects.btn_profile_main, objects.main);
    cb_register(objects.btn_host_contacts, objects.contacts);
    cb_register(objects.btn_host_info, objects.info);
    cb_register(objects.btn_host_settings, objects.settings);
    cb_register(objects.btn_host_profile, objects.profile);
    cb_register(objects.btn_host_main, objects.main);
    cb_register(objects.btn_join_contacts, objects.contacts);
    cb_register(objects.btn_join_info, objects.info);
    cb_register(objects.btn_join_settings, objects.settings);
    cb_register(objects.btn_join_profile, objects.profile);
    cb_register(objects.btn_join_main, objects.main);

	// intro & q chat screen callbacks
    lv_obj_add_event_cb(objects.cnt_queue_tap, NextLine, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.btn_queue_skip, Skip, LV_EVENT_CLICKED, NULL);

	// badge mode callback   
    lv_obj_add_event_cb(objects.cnt_badge_tappad, end_badge_mode, LV_EVENT_LONG_PRESSED, NULL); //changed from LV_EVENT_CLICKED to make badge mode only exit after 2000 ms (see Display_Driver.cpp:155)
	// 

    // avatar screen callbacks
    lv_obj_add_event_cb(objects.roller_avatar_component, avatarRollerChanged, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(objects.roller_avatar_component, avatarRollerReleased, LV_EVENT_RELEASED, NULL);

    // mission screen callbacks
    cb_register(objects.btn_mission_host, objects.host);
    cb_register(objects.btn_mission_join, objects.join);
    lv_spinner_set_anim_params(objects.spin_host_placebo, 30000, 200);
    lv_spinner_set_anim_params(objects.spin_join_placebo, 30000, 200);

    // host screen callbacks
    lv_dropdown_set_symbol(objects.ddl_host_games, NULL);
    lv_obj_add_event_cb(objects.ddl_host_games, setMissionReadyState, LV_EVENT_VALUE_CHANGED, (void*)1); // 1 = host
    lv_obj_add_event_cb(objects.ddl_host_games, [](lv_event_t* e) { play_random_beep(); }, LV_EVENT_READY, NULL); // Beep when dropdown opens
    lv_obj_add_event_cb(objects.btn_host_kick, hostKickClicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.btn_host_start, hostStartClicked, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(objects.ddl_host_games, hostGameChanged, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Initialize kick button as disabled (no players selected initially)
    lv_obj_add_state(objects.btn_host_kick, LV_STATE_DISABLED);
    lv_obj_add_state(objects.cnt_host_kick, LV_STATE_DISABLED);

    // join screen callbacks
    lv_dropdown_set_symbol(objects.ddl_join_games, NULL);
    lv_obj_add_event_cb(objects.ddl_join_games, setMissionReadyState, LV_EVENT_VALUE_CHANGED, (void*)2); // 2 = join
    lv_obj_add_event_cb(objects.ddl_join_games, [](lv_event_t* e) { play_random_beep(); }, LV_EVENT_READY, NULL); // Beep when dropdown opens
    lv_obj_add_event_cb(objects.roller_join_games, joinRollerChanged, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(objects.btn_join_ready, joinReadyClicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.ddl_join_games, joinGameChanged, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Host and join screen loading/unloading events
    lv_obj_add_event_cb(objects.host, hostScreenLoaded, LV_EVENT_SCREEN_LOADED, NULL);
    lv_obj_add_event_cb(objects.join, joinScreenLoaded, LV_EVENT_SCREEN_LOADED, NULL);
    lv_obj_add_event_cb(objects.host, screenUnloaded, LV_EVENT_SCREEN_UNLOADED, NULL);
    lv_obj_add_event_cb(objects.join, screenUnloaded, LV_EVENT_SCREEN_UNLOADED, NULL);

    // profile screen callbacks
    lv_obj_add_event_cb(objects.btn_profile_undo, profile_undo, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.btn_profile_save, profile_save, LV_EVENT_CLICKED, NULL);

    // settings screen callbacks
    lv_obj_add_event_cb(objects.check_settings_airplanemode, set_airplane_mode, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(objects.check_settings_badgemode, set_badge_mode, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(objects.btn_settings_delay_down, decrease_delay, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.btn_settings_delay_up, increase_delay, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.btn_settings_usernames, set_usernames, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.btn_settings_gamehosts, set_gamehosts, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.sld_settings_volume, set_volume, LV_EVENT_RELEASED, NULL);

    // contacts screen callbacks
    lv_obj_add_event_cb(objects.check_contacts_block, checkContactsBlockClick, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(objects.check_contacts_crew, checkContactsCrewClick, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(objects.roller_contacts_crew, crewRollerReleased, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(objects.roller_contacts_scan, scanRollerReleased, LV_EVENT_RELEASED, NULL);
    lv_obj_add_state(objects.check_contacts_block, LV_STATE_DISABLED);
    lv_obj_add_state(objects.check_contacts_crew, LV_STATE_DISABLED);
	lv_obj_add_event_cb(objects.contacts, screenContactsLoaded, LV_EVENT_SCREEN_LOADED, NULL);
	lv_obj_add_event_cb(objects.avatar, screenAvatarLoaded, LV_EVENT_SCREEN_LOADED, NULL);
	lv_obj_add_event_cb(objects.info, screenInfoLoaded, LV_EVENT_SCREEN_LOADED, NULL);
	lv_obj_add_event_cb(objects.info, screenInfoUnloaded, LV_EVENT_SCREEN_UNLOADED, NULL);
	
	// Screen load events for pip rank updates
	lv_obj_add_event_cb(objects.main, screenMainLoaded, LV_EVENT_SCREEN_LOADED, NULL);
	lv_obj_add_event_cb(objects.badge, screenBadgeLoaded, LV_EVENT_SCREEN_LOADED, NULL);
	lv_obj_add_event_cb(objects.mission, screenMissionLoaded, LV_EVENT_SCREEN_LOADED, NULL);

    // Global button click event handler for beep sounds
    //lv_display_add_event_cb(lv_display_get_default(), global_button_click_handler, LV_EVENT_CLICKED, NULL);
	//lv_obj_add_event_cb(lv_screen_active(), global_button_click_handler, LV_EVENT_CLICKED, NULL);

    // contact tab button callback & styling
    lv_obj_t* tabview_contacts_buttons = lv_tabview_get_tab_bar(objects.tabview_contacts);
    lv_obj_set_style_bg_color(tabview_contacts_buttons, lv_color_make(0x00, 0x00, 0x00), LV_PART_MAIN);
    for (int i = 0; i < lv_tabview_get_tab_count(objects.tabview_contacts); i++)
    {
        lv_obj_t* button = lv_obj_get_child(tabview_contacts_buttons, i);
        lv_obj_set_style_radius(button, 11, LV_PART_MAIN);
        lv_obj_set_style_text_color(button, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN);
        lv_obj_add_event_cb(button, tabContactsClicked, LV_EVENT_CLICKED, (void*)i); //lets us know what tab has been clicked
    }
    // add contacts
    //objects.list_contacts_crew = nullptr;
    //objects.list_contacts_scan = nullptr;
    populate_crew_list(NULL); // Populate the crew list

    // Info callbacks
	lv_obj_add_event_cb(objects.btn_info_credits, info_credits, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(objects.btn_info_documentation, info_docs, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(objects.btn_info_disclaimers, info_disclaimers, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(objects.lbl_info_credits, info_clear_credits, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(objects.cnt_info_qr, info_clear_docs, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.img_info_doc_qr, info_clear_docs, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(objects.lbl_info_disclaimers, info_clear_disclaimers, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(objects.btn_info_intro, showIntro, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(objects.btn_info_outro, showOutro, LV_EVENT_CLICKED, NULL);

    // other stuff
    //roller_changed(NULL); // Initialize the roller
    //applyConfig(config); // Apply the config to the UI and the board

    // Initialize flavor text animations
    flavor_animations_init();
    
    // Start animations if main screen is already loaded
    if (is_main_screen_active()) {
        flavor_animations_start();
    }
    
    // Start geeky random number updates
    startRandomLabelUpdates();
    
    // Initialize pip ranks based on current XP
    updatePipRanks();

    // badge mode setup
    lv_image_set_inner_align(objects.img_badge_avatar, LV_IMAGE_ALIGN_STRETCH);
}

// Add/update IDPackets received to config.contacts or scanResults
void processIDPacket(IDPacket packet)
{
    // convert to a ContactData object
    ContactData* contact = idPacketToContactData(&packet);
    ContactData* foundContact = config.contacts.findContact(contact->nodeId);

    // if the Node ID is in the freinds or isBlocked list, add/update it in config.contacts
    if (foundContact)
    {
        config.contacts.addOrUpdateContact(*contact);
        LV_LOG_INFO("updated board.contacts with node ID %u\n", contact->nodeId);
        return;
    }

    // the contact wasn't in the config.contacts list
    // add/update it in the scan results list
    scanResults->addOrUpdateContact(*contact);
    LV_LOG_INFO("added or updated %u in scan results\n", contact->nodeId);
	delete contact; // free the memory allocated for the contact
}

// Geeky random number display functions
static lv_timer_t* randomLabelTimer = nullptr;

// Nerdy references for random number patterns
const char* nerdyReferences[] = {
    "42-1337000",      // Answer to everything + leet
    "03-1415926",      // Pi digits
    "27-1828182",      // Euler's number
    "14-1412356",      // Fibonacci sequence start
    "88-8008135",      // Classic calculator spelling
    "16-7770007",      // Binary-inspired
    "42-4242424",      // Hitchhiker's Guide
    "99-9999999",      // Max values
    "11-1010101",      // Binary pattern
    "37-3735928",      // Decimal expansion
    "20-2001001",      // Space Odyssey year
    "19-1984000",      // Orwell reference
    "31-3141592",      // More Pi
    "17-1701000",      // Star Trek Enterprise
    "66-6502000",      // 6502 processor
    "80-8080808",      // Intel 8080
    "65-6510000",      // C64 processor
    "12-1234567",      // Sequential
    "00-0000001",      // Starting up
    "FF-FFFFFF",       // Hex max
    "CA-CAFE000",      // CAFE in hex
    "BE-BEEF000",      // BEEF in hex
    "DE-DEAD000",      // DEAD in hex
    "51-5150000",      // Area 51
    "30-3030303",      // Pattern
    "77-7777777",      // Lucky sevens
    "13-1337133",      // Double leet
    "90-9001000",      // Over 9000 reference
	"FF-BADF00D",      // Bad food in hex
    "12-1221122",      // Pattern
    "55-5555555"       // Pattern
};

void updateRandomLabels() {
    static int refIndex = 0;
    
    // Array of all random label objects
    lv_obj_t* randomLabels[] = {
        objects.lbl_main_random,
        objects.lbl_avatar_random,
        objects.lbl_mission_random,
        objects.lbl_host_random,
        objects.lbl_join_random,
        objects.lbl_contacts_random,
        objects.lbl_settings_random,
		objects.lbl_info_random
    };
    
    const int numLabels = sizeof(randomLabels) / sizeof(randomLabels[0]);
    const int numReferences = sizeof(nerdyReferences) / sizeof(nerdyReferences[0]);
    
    // Update each label with a different reference (cycling through them)
    for (int i = 0; i < numLabels; i++) {
        if (randomLabels[i] != nullptr) {
            int currentRef = (refIndex + i) % numReferences;
            lv_label_set_text(randomLabels[i], nerdyReferences[currentRef]);
        }
    }
    
    // Move to next set of references for next update
    refIndex = (refIndex + 1) % numReferences;
}

static void randomLabelTimerCallback(lv_timer_t* timer) {
    updateRandomLabels();
}

void startRandomLabelUpdates() {
    if (randomLabelTimer == nullptr) {
        // Update every 1.5-2.5 seconds (randomized within this range)
        uint32_t interval = 1500 + (esp_random() % 1000); // 1500-2500ms
        randomLabelTimer = lv_timer_create(randomLabelTimerCallback, interval, nullptr);
        
        // Do initial update
        updateRandomLabels();
        
        LV_LOG_INFO("Started random label updates with %ums interval\n", interval);
    }
}

void stopRandomLabelUpdates() {
    if (randomLabelTimer != nullptr) {
        lv_timer_del(randomLabelTimer);
        randomLabelTimer = nullptr;
        LV_LOG_INFO("Stopped random label updates\n");
    }
}

// Pip rank system implementation
enum PipState {
    PIP_HIDDEN = 0,
    PIP_EMPTY = 1,
    PIP_FILLED = 2
};

void applyPipPattern(lv_obj_t* top, lv_obj_t* top_mid, lv_obj_t* bottom_mid, lv_obj_t* bottom, 
                    int topState, int topMidState, int bottomMidState, int bottomState) {
    
    LV_LOG_INFO("[DEBUG] applyPipPattern called with states: top=%d, top_mid=%d, bottom_mid=%d, bottom=%d\n",
        topState, topMidState, bottomMidState, bottomState);
    LV_LOG_INFO("[DEBUG] Pip objects: top=%p, top_mid=%p, bottom_mid=%p, bottom=%p\n",
        top, top_mid, bottom_mid, bottom);
    
    lv_obj_t* pips[] = {top, top_mid, bottom_mid, bottom};
    int states[] = {topState, topMidState, bottomMidState, bottomState};
    
    // there are FOUR pips
    for (int i = 0; i < 4; i++) {
        if (pips[i] == nullptr) continue;
        
        // Remove all pip styles first
        remove_style_pip_hidden(pips[i]);
        remove_style_pip_empty(pips[i]);
        remove_style_pip_filled(pips[i]);
        
        // Set base color (0x000000 for hidden, 0x00FFFF for empty/filled)
        if (states[i] == PIP_HIDDEN) {
            lv_led_set_color(pips[i], lv_color_hex(0x000000));
            add_style_pip_hidden(pips[i]);
        } else {
            lv_led_set_color(pips[i], lv_color_hex(0xFFFF00));
            if (states[i] == PIP_EMPTY) {
                add_style_pip_empty(pips[i]);
            } else if (states[i] == PIP_FILLED) {
                add_style_pip_filled(pips[i]);
            }
        }
    }
}

void updatePipRanks() {
    extern Config config;
    
    int totalXP = config.user.totalXP;
    
    // Debug output
    LV_LOG_INFO("[DEBUG] updatePipRanks called with totalXP=%d\n", totalXP);
    LV_LOG_INFO("[DEBUG] Rank thresholds: po=%d, ens=%d, ltjg=%d, lt=%d, lcdr=%d, cdr=%d, capt=%d\n",
        config.board.ranks.po, config.board.ranks.ens, config.board.ranks.ltjg, 
        config.board.ranks.lt, config.board.ranks.lcdr, config.board.ranks.cdr, config.board.ranks.capt);
    
    // Determine highest rank achieved (only the highest rank applies)
    PipState topState = PIP_HIDDEN;
    PipState topMidState = PIP_HIDDEN;
    PipState bottomMidState = PIP_HIDDEN;
    PipState bottomState = PIP_HIDDEN;
    
    if (totalXP >= config.board.ranks.capt) {
        // Captain: filled, filled, filled, filled
        LV_LOG_INFO("[DEBUG] Applying Captain rank (totalXP %d >= %d)\n", totalXP, config.board.ranks.capt);
        topState = PIP_FILLED;
        topMidState = PIP_FILLED;
        bottomMidState = PIP_FILLED;
        bottomState = PIP_FILLED;
    } else if (totalXP >= config.board.ranks.cdr) {
        // Commander: hidden, filled, filled, filled
        LV_LOG_INFO("[DEBUG] Applying Commander rank (totalXP %d >= %d)\n", totalXP, config.board.ranks.cdr);
        topState = PIP_HIDDEN;
        topMidState = PIP_FILLED;
        bottomMidState = PIP_FILLED;
        bottomState = PIP_FILLED;
    } else if (totalXP >= config.board.ranks.lcdr) {
        // Lieutenant Commander: hidden, empty, filled, filled
        LV_LOG_INFO("[DEBUG] Applying LCDR rank (totalXP %d >= %d)\n", totalXP, config.board.ranks.lcdr);
        topState = PIP_HIDDEN;
        topMidState = PIP_EMPTY;
        bottomMidState = PIP_FILLED;
        bottomState = PIP_FILLED;
    } else if (totalXP >= config.board.ranks.lt) {
        // Lieutenant: hidden, hidden, filled, filled
        LV_LOG_INFO("[DEBUG] Applying LT rank (totalXP %d >= %d)\n", totalXP, config.board.ranks.lt);
        topState = PIP_HIDDEN;
        topMidState = PIP_HIDDEN;
        bottomMidState = PIP_FILLED;
        bottomState = PIP_FILLED;
    } else if (totalXP >= config.board.ranks.ltjg) {
        // Lieutenant Junior Grade: hidden, hidden, empty, filled
        LV_LOG_INFO("[DEBUG] Applying LTJG rank (totalXP %d >= %d)\n", totalXP, config.board.ranks.ltjg);
        topState = PIP_HIDDEN;
        topMidState = PIP_HIDDEN;
        bottomMidState = PIP_EMPTY;
        bottomState = PIP_FILLED;
    } else if (totalXP >= config.board.ranks.ens) {
        // Ensign: hidden, hidden, hidden, filled
        LV_LOG_INFO("[DEBUG] Applying Ensign rank (totalXP %d >= %d)\n", totalXP, config.board.ranks.ens);
        topState = PIP_HIDDEN;
        topMidState = PIP_HIDDEN;
        bottomMidState = PIP_HIDDEN;
        bottomState = PIP_FILLED;
    } else if (totalXP >= config.board.ranks.po) {
        // Petty Officer: hidden, hidden, hidden, empty
        LV_LOG_INFO("[DEBUG] Applying PO rank (totalXP %d >= %d)\n", totalXP, config.board.ranks.po);
        topState = PIP_HIDDEN;
        topMidState = PIP_HIDDEN;
        bottomMidState = PIP_HIDDEN;
        bottomState = PIP_EMPTY;
    } else {
        LV_LOG_INFO("[DEBUG] No rank achieved (totalXP %d < PO threshold %d), all pips hidden\n", totalXP, config.board.ranks.po);
    }
    
    // Apply pip patterns to both main and badge screens
    applyPipPattern(objects.pip_main_top, objects.pip_main_top_mid, 
                   objects.pip_main_bottom_mid, objects.pip_main_bottom,
                   topState, topMidState, bottomMidState, bottomState);
    
    applyPipPattern(objects.pip_badge_top, objects.pip_badge_top_mid, 
                   objects.pip_badge_bottom_mid, objects.pip_badge_bottom,
                   topState, topMidState, bottomMidState, bottomState);
    
    LV_LOG_INFO("Updated pip ranks for totalXP=%d\n", totalXP);
}

// Screen load event handlers for pip updates
void screenMainLoaded(lv_event_t* e) {
    updatePipRanks();
    
    // Restart flavor text animations when returning to main screen
    startRandomLabelUpdates();
    flavor_animations_start();
    LV_LOG_INFO("[MAIN] Main screen loaded - flavor text animation restarted\n");
}

void screenBadgeLoaded(lv_event_t* e) {
    updatePipRanks();
}

void screenMissionLoaded(lv_event_t* e) {
    LV_LOG_INFO("Mission screen loaded\n");
    updateMissionXPBars();
}

// Function to call when user's XP changes (e.g., after completing a game)
// Update mission XP progress bars
void updateMissionXPBars() {
    // Array of mission XP bars for easy iteration
    lv_obj_t* missionXPLabels[] = {
        objects.lbl_mission_xp1,
        objects.lbl_mission_xp2,
        objects.lbl_mission_xp3,
        objects.lbl_mission_xp4,
        objects.lbl_mission_xp5,
        objects.lbl_mission_xp6
    };
    
    // Update individual game XP bars
    for (int i = 0; i < 6; i++) {
        if (missionXPLabels[i] && i < (int)config.games.size()) {
			char labelText[32];
			int gameXP = config.games[i].XP;
			if (gameXP == -1) gameXP = 0; // Handle uninitialized XP
            snprintf(labelText, sizeof(labelText), "%i/28800", gameXP);
            lv_label_set_text(missionXPLabels[i], labelText);
            LV_LOG_INFO("[MISSION_XP] Updated game %d bar: %d XP\n", i + 1, gameXP);
        }
    }
    
    // Update total XP bar
    if (objects.lbl_mission_xpall) {
        char labelText[32];
        snprintf(labelText, sizeof(labelText), "%i/172200", config.user.totalXP);
        lv_label_set_text(objects.lbl_mission_xpall, labelText);
        LV_LOG_INFO("[MISSION_XP] Updated total XP bar: %d XP\n", config.user.totalXP);
    }
}

void refreshRanksAfterXPChange() {
    updatePipRanks();
    updateAvatarUnlocks();  // Also update avatar unlocks when XP changes
    updateMissionXPBars();  // Update mission XP progress bars
    LV_LOG_INFO("Refreshed ranks, avatar unlocks, and mission XP bars after XP change\n");
}

// Get rank name string from totalXP
static const char* getRankName(int totalXP) {
    if (totalXP >= RANK_CAPT) return "Captain";
    if (totalXP >= RANK_CDR) return "Commander";
    if (totalXP >= RANK_LCDR) return "Lt. Commander";
    if (totalXP >= RANK_LT) return "Lieutenant";
    if (totalXP >= RANK_LTJG) return "Lt. Junior Grade";
    if (totalXP >= RANK_ENS) return "Ensign";
    return "Petty Officer";
}

// Calculate team bonus XP based on teammate performance and connectivity  
static TeamBonusResult calculateTeamBonus(int32_t baseXP) {
    TeamBonusResult result = {0, 0, 0, 0, 0};
    
    extern GameState gameState;
    extern painlessMesh mesh;
    extern Config config;
    
    // Only calculate bonuses for multiplayer games
    if (gameState.sessionID == 0 || gameState.playerCount <= 1) {
        LV_LOG_INFO("[TEAM_BONUS] Solo game - no team bonus applicable");
        return result;
    }
    
    // Get current node list for connectivity checking
    auto nodeList = mesh.getNodeList(false);
    
    // Find our own position in the player array (should be index 0, but let's be safe)
    int myIndex = -1;
    for (int i = 0; i < gameState.playerCount && i < 5; i++) {
        if (gameState.playerIDs[i] == config.user.nodeId) {
            myIndex = i;
            break;
        }
    }
    
    if (myIndex == -1) {
        LV_LOG_WARN("[TEAM_BONUS] Could not find self in gameState.playerIDs");
        return result;
    }
    
    LV_LOG_INFO("[TEAM_BONUS] Calculating team bonus for %d teammates", gameState.playerCount - 1);
    
    // Check each teammate (exclude self)
    for (int i = 0; i < gameState.playerCount && i < 5; i++) {
        if (i == myIndex) continue; // Skip self
        
        uint32_t teammateNodeId = gameState.playerIDs[i];
        int teammateScore = gameState.scores[i];
        result.totalTeammates++;
        
        LV_LOG_INFO("[TEAM_BONUS] Checking teammate %d: nodeID=%u, score=%d%%", 
                   i, teammateNodeId, teammateScore);
        
        // Check if teammate achieved 99%+ score
        bool highScore = (teammateScore >= 99);
        
        // Check if teammate is still connected
        bool isConnected = false;
        for (auto nodeId : nodeList) {
            if (nodeId == teammateNodeId) {
                isConnected = true;
                break;
            }
        }
        
        if (highScore && isConnected) {
            result.eligibleTeammates++;
            LV_LOG_INFO("[TEAM_BONUS] Teammate %u qualifies: %d%% score and connected", 
                       teammateNodeId, teammateScore);
        } else if (highScore && !isConnected) {
            result.disconnectedCount++;
            LV_LOG_INFO("[TEAM_BONUS] Teammate %u had %d%% but disconnected", 
                       teammateNodeId, teammateScore);
        } else if (!highScore && isConnected) {
            result.lowScoreCount++;
            LV_LOG_INFO("[TEAM_BONUS] Teammate %u connected but only %d%% score", 
                       teammateNodeId, teammateScore);
        } else {
            LV_LOG_INFO("[TEAM_BONUS] Teammate %u neither qualified (%d%%) nor connected", 
                       teammateNodeId, teammateScore);
        }
    }
    
    // Calculate bonus: 10% of base XP per qualifying teammate
    if (result.eligibleTeammates > 0) {
        float bonusMultiplier = result.eligibleTeammates * 0.1f;
        result.bonusXP = (int32_t)(baseXP * bonusMultiplier);
        
        LV_LOG_INFO("[TEAM_BONUS] %d qualifying teammates = %.1f%% bonus = %d XP", 
                   result.eligibleTeammates, bonusMultiplier * 100, result.bonusXP);
    } else {
        LV_LOG_INFO("[TEAM_BONUS] No qualifying teammates - no bonus awarded");
    }
    
    return result;
}

// Process game completion: apply XP, save config, and show post-game summary
void processGameCompletion(int gameNumber, int32_t earnedXP) {
    extern Config config;
    
    // Convert 1-based game number to 0-based array index
    int gameIndex = gameNumber - 1;
    
    if (gameIndex < 0 || gameIndex >= (int)config.games.size()) {
        LV_LOG_ERROR("[ERROR] processGameCompletion: Invalid game number %d\n", gameNumber);
        return;
    }
    
    // Get previous state for comparison
    int previousAvatarCount = calculateUnlockedAvatars();
    const char* previousRank = getRankName(config.user.totalXP);
    int previousGameXP = config.games[gameIndex].XP;
    
    // Calculate team bonus based on teammate performance and connectivity
    TeamBonusResult teamBonus = calculateTeamBonus(earnedXP);
    int32_t totalXPToAdd = earnedXP + teamBonus.bonusXP;
    
    // Apply earned XP (including team bonus) to the specific game
    config.games[gameIndex].XP += totalXPToAdd;
    if (config.games[gameIndex].XP < 0) {
        config.games[gameIndex].XP = 0; // Prevent negative XP
    }
    
    // Recalculate total XP as sum of all game XPs
    int newTotalXP = 0;
    for (const auto& game : config.games) {
        int add = game.XP;
        if (add < 0)
            add = 0;

        newTotalXP += add;
    }
    config.user.totalXP = newTotalXP;
    
    if (teamBonus.bonusXP > 0) {
        LV_LOG_INFO("[GAME] Game %d completion: earned %d base XP + %d team bonus = %d total XP, game total now %d, player total now %d\n", 
                      gameNumber, earnedXP, teamBonus.bonusXP, totalXPToAdd, config.games[gameIndex].XP, newTotalXP);
    } else {
        LV_LOG_INFO("[GAME] Game %d completion: earned %d XP (no team bonus), game total now %d, player total now %d\n", 
                      gameNumber, earnedXP, config.games[gameIndex].XP, newTotalXP);
    }
    
    // Update ranks and avatar unlocks
    refreshRanksAfterXPChange();
    
    // Get new state for comparison
    int newAvatarCount = calculateUnlockedAvatars();
    const char* newRank = getRankName(config.user.totalXP);
    
    // Save the updated config
    extern bool saveBoardConfig(Config& config, const char* fileName);
    if (saveBoardConfig(config, "L:/default.json")) {
        LV_LOG_INFO("[GAME] Config saved successfully after game %d completion\n", gameNumber);
    } else {
        LV_LOG_ERROR("[ERROR] Failed to save config after game %d completion\n", gameNumber);
    }
    
    // Show post-game summary with Q's snarky commentary (now includes team bonus info)
    extern void showPostGameSummary(int gameNumber, int baseXP, int bonusXP, int previousAvatarCount, int newAvatarCount, const char* previousRank, const char* newRank, TeamBonusResult teamBonusDetails);
    showPostGameSummary(gameNumber, earnedXP, teamBonus.bonusXP, previousAvatarCount, newAvatarCount, previousRank, newRank, teamBonus);
}

// Comprehensive airplane mode activation - stops mesh and fully shuts down WiFi
void enterAirplaneMode() {
    LV_LOG_INFO("[AIRPLANE] Entering airplane mode - stopping mesh and disabling WiFi...\n");
    
    // First stop the mesh gracefully (this includes session cleanup)
    stop_mesh();
    
    // Allow mesh stop operations to complete
    delay(1000);
    
    // Process any pending LVGL operations before WiFi shutdown
    lv_timer_handler();
    
    // Additional safety: ensure all WiFi operations are complete
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // Fully shut down WiFi subsystem to save power and prevent interference
    LV_LOG_INFO("[AIRPLANE] Shutting down WiFi subsystem...\n");
    WiFi.mode(WIFI_OFF);
    
    // Allow WiFi hardware to fully power down
    delay(2000);
    
    // Process LVGL timers after WiFi shutdown to handle any UI updates
    lv_timer_handler();
    
    LV_LOG_INFO("[AIRPLANE] Airplane mode activated - WiFi fully disabled\n");
}

// Comprehensive airplane mode deactivation - enables WiFi and reinitializes mesh
void exitAirplaneMode() {
    LV_LOG_INFO("[AIRPLANE] Exiting airplane mode - enabling WiFi and mesh...\n");
    
    // Process any pending LVGL operations before WiFi startup
    lv_timer_handler();
    
    // Power up WiFi subsystem
    LV_LOG_INFO("[AIRPLANE] Powering up WiFi subsystem...\n");
    WiFi.mode(WIFI_STA);  // Start in station mode
    
    // Allow WiFi hardware to fully initialize
    delay(3000);  // Longer delay for WiFi hardware initialization
    
    // Process LVGL timers during WiFi startup
    lv_timer_handler();
    
    // Allow additional time for WiFi stack to stabilize
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    // Initialize mesh network
    LV_LOG_INFO("[AIRPLANE] Reinitializing mesh network...\n");
    init_mesh();
    
    // Allow mesh to establish connections
    delay(1000);
    
    // Final LVGL timer processing
    lv_timer_handler();
    
    LV_LOG_INFO("[AIRPLANE] Airplane mode deactivated - WiFi and mesh restored\n");
}

