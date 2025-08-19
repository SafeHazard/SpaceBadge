#include <Arduino.h>
//#include "LVGL_Driver.h"
#include "Display_Driver.h"
#include "global.hpp"
#include "custom.h"
#include "json.h"
#include "./src/ui/ui.h"
#include <lvgl.h>
#include <LittleFS.h>
#include "badgeMode.h"
#include "mesh.h"
//#include "Audio_PCM5101.h"
#include "mp3s.h"
#include "BAT_Driver.h"
#include "lightweight_tasks.h"
#include "session.h"
#include "convos.h"
#include "avatar_unlocks.h"
#include "./src/ui/images.h"

#include "game_parent.h"
#include "g_game1.h"
#include "g_game2.h"
#include "g_game3.h"
#include "g_game4.h"
#include "g_game5.h"
#include "g_game6.h"

// Global components
unsigned long last_tick = 0;
Config config;
GameState gameState;
lv_display_t* disp = NULL;
ContactManager* scanResults = nullptr;
extern bool meshInitialized; // Track mesh state
game_parent* game = nullptr;

// XP tracking  
extern int32_t earnedXP;        // From session.cpp
extern int32_t earnedXP_mission; // From mission.cpp
int currentGameNumber = 0;      // Set when game starts, used for post-game summary
 

void memoryInit()
{
	LV_LOG_INFO("Initializing PSRAM...");
	
	// Enable heap corruption detection before enabling PSRAM
	heap_caps_malloc_extmem_enable(1024 * 1024); //more RAM

	if (!psramInit())
	{
		LV_LOG_ERROR("PSRAM Init failed\n");
	}
	else
	{
		LV_LOG_INFO("success\n");
	}
	
	// Test allocation to verify heap integrity
	void* testPtr = heap_caps_malloc(sizeof(lv_style_t), MALLOC_CAP_SPIRAM);
	if (testPtr) {
		heap_caps_free(testPtr);
		testPtr = nullptr;
	} else {
		LV_LOG_ERROR("PSRAM test allocation failed - heap may be corrupted\n");
	}
	
	LV_LOG_INFO("Free internal RAM: %u bytes\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
	LV_LOG_INFO("Free PSRAM: %u bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
}

void littleFSInit()
{
	lv_fs_arduino_esp_littlefs_init();

	if (!LittleFS.begin(false))
	{
		LV_LOG_ERROR("LittleFS mounting...Failed\n");
		return;
	}
	else
	{
		LV_LOG_INFO("LittleFS mounting...done.\n");
	}
}



void setup()
{
	Serial.begin(115200);

	esp_log_level_set("*", ESP_LOG_VERBOSE);

	// File system initialization
	littleFSInit();

	// PSRAM initialization
	memoryInit();
	
	// Load configuration from JSON file
	if (loadConfig(config))
	{
		LV_LOG_INFO("Config loaded.");
		LV_LOG_INFO("Loaded %zu contacts\n", config.contacts.size());
	}
	else
	{
		LV_LOG_ERROR("Error loading config. Using defaults.");
	}
	
	// initialize scanning results
	scanResults = new ContactManager();

	// set up audio
	init_audio(config.board.volume);

	// start battery monitoring
	BAT_Init();

	// touch screen initialization
	Touch_Init();

	// display initialization
	init_display();

	// Create loading screen with stars background
	lv_obj_t* loading_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(loading_screen, lv_color_black(), 0);
	
	// Add stars background image
	lv_obj_t* stars_bg = lv_img_create(loading_screen);
	lv_img_set_src(stars_bg, &img_stars);
	lv_obj_set_pos(stars_bg, 0, 0);
	
	// Add "Loading..." text (centered)
	lv_obj_t* loading_label = lv_label_create(loading_screen);
	lv_label_set_text(loading_label, "Quantum rift\nopening...");
	lv_obj_set_style_text_color(loading_label, lv_color_white(), 0);
	lv_obj_set_style_text_font(loading_label, &lv_font_montserrat_24, 0);
	lv_obj_center(loading_label);
	
	// Add shuttle image as progress indicator below the text
	lv_obj_t* loading_progress = lv_bar_create(loading_screen);
	lv_obj_set_size(loading_progress, 200, 20);
	lv_obj_set_pos(loading_progress, 20, 200); // Centered horizontally, below text
	lv_obj_set_style_bg_opa(loading_progress, 0, LV_PART_MAIN); // Make background transparent
	
	// Set up gradient for the indicator part
	lv_obj_set_style_bg_color(loading_progress, lv_color_hex(0x000000), LV_PART_INDICATOR);
	lv_obj_set_style_bg_grad_color(loading_progress, lv_color_hex(0x00FFFF), LV_PART_INDICATOR);
	lv_obj_set_style_bg_grad_dir(loading_progress, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
	lv_bar_set_value(loading_progress, 0, LV_ANIM_OFF); // Start at 0%
	
	// Add shuttle image on top of the progress bar
	lv_obj_t* shuttle_img = lv_img_create(loading_screen);
	lv_img_set_src(shuttle_img, &img_ui___shuttle);
	lv_obj_set_pos(shuttle_img, 20, 195); // Start at left edge of progress bar
	
	// Add percentage label below progress bar
	lv_obj_t* progress_label = lv_label_create(loading_screen);
	lv_label_set_text(progress_label, "0%");
	lv_obj_set_style_text_color(progress_label, lv_color_white(), 0);
	lv_obj_set_style_text_font(progress_label, &lv_font_montserrat_14, 0);
	lv_obj_set_style_text_align(progress_label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_pos(progress_label, 105, 230); // Centered below progress bar
	
	lv_scr_load(loading_screen);
	
	// Process LVGL to show loading screen
	lv_tick_inc(5);
	lv_timer_handler();

	// SAFETY: Process LVGL events before heavy memory operations
	lv_timer_handler();
	
	// Preload all images with unified progress feedback and shuttle animation
	preload_all_image_directories_with_progress(loading_progress, progress_label, shuttle_img);
	
	// SAFETY: Process LVGL events after memory-intensive operations
	lv_timer_handler();

	// Reserve PSRAM for game backgrounds (after hit effects, before avatars)
	LV_LOG_INFO("[BOOT] Reserving PSRAM for game backgrounds...\n");
	if (!init_game_background_psram()) {
		LV_LOG_ERROR("[ERROR] Failed to initialize game background PSRAM reservation\n");
		// Continue boot process - this is not critical for basic functionality
	}

	// Now initialize the full UI
	ui_init();
	delay(100); // Allow UI to settle
	lv_obj_delete(progress_label); // Remove progress label
	lv_obj_delete(loading_progress); // Remove progress bar
	lv_obj_delete(loading_screen); // Remove loading screen

	// Initialize UI callbacks
	setup_cb();

	// Verify heap integrity before starting mesh (most likely source of corruption)
	size_t freeBefore = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
	LV_LOG_INFO("[HEAP] Pre-mesh heap check: %u bytes free internal\n", freeBefore);
	
	// Apply the config to the UI and the board (volume, mesh init, etc)
	// Note that this INCLUDES the mesh initialization
	applyConfig(config);
	
	// Verify heap integrity after mesh initialization
	size_t freeAfter = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
	LV_LOG_INFO("[HEAP] Post-mesh heap check: %u bytes free internal (delta: %d)\n", 
	            freeAfter, (int)(freeAfter - freeBefore)); 
	
	// Initialize avatar unlocks based on current game XP
	updateAvatarUnlocks();
	
	// Check for RANK_CAPT achievement and enable outro UI elements if reached
	checkOutroUnlocks();
	
	// Initialize session manager
	init_session_manager();
	
	// set up lightweight tasks (saves ~40KB RAM vs FreeRTOS tasks)
	init_lightweight_tasks();

	// preload beeps
	preload_all_beeps();

	// Initialize game state - both sessionManager and gameState will be set to Idle
	// This will be synchronized when sessionManager is created
	gameState.myStatus = Idle; // Direct assignment is OK here since sessionManager isn't created yet
	LV_LOG_INFO("[INFO] Initialized gameState.myStatus to Idle\n");
	
	// show the intro and set the intro watched flag
	showIntroAtBoot();
	//printConfig(config);

	preload_all_game_sfx();
}

void loop()
{
	unsigned long now = millis();

	if (now - last_tick >= 5)
	{
		lv_tick_inc(5);
		last_tick = now;
		ui_tick();
	}

	 //Check if any serial input is available
	 // ** ONLY WORKS IF USB CDC ON BOOT IS ENABLED AND CRLF LINE ENDINGS USED **
	//static String input;
	//while (Serial.available())
	//{
	//	char c = Serial.read();
	//	if (c == '\n' || c == '\r')
	//	{
	//		if (input.length() > 0)
	//		{
	//			lv_log("working...");
	//			handleSimulatorCommand(input);  // <- call the simulator
	//			input = "";  // clear for next command
	//		}
	//	}
	//	else
	//	{
	//		input += c;
	//	}
	//}

	lv_timer_handler(); 


		try
		{
			if(meshInitialized)
				mesh.update();
		}
		catch (const std::exception& e)
		{
			LV_LOG_ERROR("[ERROR] Mesh update failed (attempt): %s\n", e.what());
		}
		catch (...)
		{
			LV_LOG_ERROR("[ERROR] Unknown mesh update error (attempt)\n");
		}

	if (game != nullptr)
	{
		game->Update();
		
		// Check game time_left() every second
		static uint32_t lastTimeCheck = 0;
		if (millis() - lastTimeCheck >= 1000) {
			lastTimeCheck = millis();
			if (game->time_left() <= 0) {
				LV_LOG_INFO("[GAME] Game time expired, cleaning up\n");
				
				// CRITICAL: Process LVGL timer queue before deleting game objects
				// This prevents crashes from timers accessing deleted objects
				lv_timer_handler();
				
				// Clean up game instance (this calculates XP in destructor)
				// Make sure the XP counters are 0
				earnedXP = 0;
				earnedXP_mission = 0;
				delete game;
				game = nullptr;
				
				// Clear any residual briefing state to prevent game restart bugs
				extern void clearBriefingState();
				clearBriefingState();
				
				// Give LVGL one more cycle to clean up any pending operations
				lv_timer_handler();
				
				// Read BOTH XP values and use whichever one is non-zero
				LV_LOG_INFO("[DEBUG] earnedXP = %d, earnedXP_mission = %d\n", earnedXP, earnedXP_mission);
				
				// Use whichever XP value is non-zero (mission for host/solo, regular for joined players)
				int32_t finalXP = (earnedXP_mission != 0) ? earnedXP_mission : earnedXP;
				const char* gameMode = (earnedXP_mission != 0) ? "host/solo" : "joined player";
				
				Serial.printf("[GAME] Game %d completed, earned %d XP (%s mode)\n", 
							  currentGameNumber, finalXP, gameMode);
				
				// Gracefully end session with proper cleanup
				if (sessionManager) {
					// If we were hosting, notify players before stopping
					if (sessionManager->isHosting()) {
						sessionManager->stopHosting(); // This sends disconnect notifications
					} else if (sessionManager->isInLobby()) {
						sessionManager->leaveSession(); // This sends leave notifications
					} else {
						// Just reset status if not in active session
						sessionManager->setMyStatus(Idle);
					}
					LV_LOG_INFO("[GAME] Session properly cleaned up after game completion\n");
				}
				
				// Show post-game summary with Q's debrief
				if (currentGameNumber > 0) {
					// SAFETY: Delay before showing post-game summary to allow LVGL to stabilize
					delay(50);  // Small delay to ensure all LVGL operations complete
					lv_timer_handler();  // Final cleanup of timer queue
					
					extern void processGameCompletion(int gameNumber, int32_t earnedXP);
					processGameCompletion(currentGameNumber, finalXP);
					
					// Clear game tracking
					currentGameNumber = 0;
				} else {
					// Fallback to main screen if no game data
					lv_screen_load_anim(objects.main, LV_SCR_LOAD_ANIM_NONE, 200, 0, false);
					LV_LOG_INFO("[GAME] Returned to main screen after game timeout\n");
				}
			}
		}
		
		vTaskDelay(pdMS_TO_TICKS(5));
	}
}
