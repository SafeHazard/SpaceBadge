#include "convos.h"
#include "json.h"
#include "global.hpp"
#include "./src/ui/ui.h"
#include <string>
#include <vector>
#include "session.h"
#include "custom.h"
#include "game_parent.h"
#include "g_game1.h"

// Global variable to preserve player count across game initialization
int preservedPlayerCount = 0;
#include "g_game2.h"
#include "g_game3.h"
#include "g_game4.h"
#include "g_game5.h"
#include "g_game6.h"

JsonDocument* chat_doc = nullptr;  // Use pointer for PSRAM allocation
QChat chat;
Line* foundLines;
int chatArrayLen;
int curLine;

extern CachedImage* queue_images;
extern CachedImage* exp_images;
extern size_t queue_images_size;
extern size_t exp_images_size;
extern Config config;

// Global countdown variables
static int briefingCountdown = 0;
static lv_timer_t* countdownTimer = nullptr;
static int pendingGameNumber = 0;
static bool isHostMode = false;
static bool inCountdownMode = false;
static bool soloMode = false;
static bool soloCountdownShown = false;

void LoadConvoCall(lv_event_t* e);
void Skip(lv_event_t* e);
void NextLine(lv_event_t* e);
void LoadConvo(char* name);
void FreeConvo();
void startGameAfterBriefing(int gameNumber, bool hostMode);

void FreeConvo()
{
    if (chat.lines != NULL) {
        for (size_t i = 0; i < chat.chatCount; i++)
        {
            if (chat.lines[i].text != NULL)
            {
                heap_caps_free(chat.lines[i].text);
                chat.lines[i].text = NULL;
            }
        }

        heap_caps_free(chat.lines);
        chat.lines = NULL;
    }
    
    chat.chatCount = 0;
    chat.nextScreen = NULL;

    // Properly free PSRAM-allocated JsonDocument
    if (chat_doc) {
        chat_doc->clear();
        chat_doc->~JsonDocument();  // Call destructor
        heap_caps_free(chat_doc);   // Free PSRAM memory
        chat_doc = nullptr;
    }
}

void LoadConvo(char* name) // input as game|subset   CHARACTER COUNT IS 150 MAX
{
    LV_LOG_INFO("[MEMORY] Pre-LoadConvo: %u bytes free", ESP.getFreeHeap());
    
    // Clean up any previous conversation first
    FreeConvo();
    curLine = 0;

    // Allocate JsonDocument in PSRAM with memory logging
    chat_doc = (JsonDocument*)heap_caps_malloc(sizeof(JsonDocument), MALLOC_CAP_SPIRAM);
    if (!chat_doc) {
        LV_LOG_ERROR("Failed to allocate PSRAM for JsonDocument, free RAM: %u", ESP.getFreeHeap());
        return;
    }
    
    // Use placement new to construct the JsonDocument in PSRAM
    new(chat_doc) JsonDocument();
    
    // Load JSON with memory monitoring
    uint32_t preReadMem = ESP.getFreeHeap();
    *chat_doc = readJson("L:/chats.json");
    uint32_t postReadMem = ESP.getFreeHeap();
    LV_LOG_INFO("[MEMORY] JSON read used %u bytes, now %u free", preReadMem - postReadMem, postReadMem);
    
    if (*chat_doc == nullptr)
    {
        LV_LOG_ERROR("Failed to load chats.json, free RAM: %u", ESP.getFreeHeap());
        return;
    }

    // attempt to load chat
    JsonObject parent;

    // split the input
    char* buffer = (char*)malloc(strlen(name) + 1);
    strcpy(buffer, name);
    char* a = strtok(buffer, "|");
    char* b = strtok(NULL, "|");
    free(buffer);

    if (!a || !b)
    {
        LV_LOG_ERROR("Invalid JSON string.\n");
        return;
    }

    // get the doc for it
    LV_LOG_INFO("Strings: %s|%s\n", a, b);
    parent = (*chat_doc)[a][b];

    if (parent.isNull())
    {
        LV_LOG_ERROR("Invalid JSON identifier.\n");
        return;
    }

    JsonArray arr = parent["lines"];
    size_t arrSize = arr.size();

    // create struct for each line using PSRAM
    foundLines = (Line*)heap_caps_malloc(arrSize * sizeof(Line), MALLOC_CAP_SPIRAM);
    if (!foundLines) {
        LV_LOG_ERROR("Failed to allocate PSRAM for conversation lines\n");
        return;
    }
    
    for (size_t i = 0; i < arrSize; i++)
    {
        const char* temp = parent["lines"][i];
        foundLines[i].text = (char*)heap_caps_malloc(strlen(temp) + 1, MALLOC_CAP_SPIRAM);
        if (foundLines[i].text) {
            strcpy(foundLines[i].text, temp);
        } else {
            LV_LOG_ERROR("Failed to allocate PSRAM for line %zu text\n", i);
            foundLines[i].text = nullptr;
        }

        foundLines[i].imgIdx = parent["images"][i] | 0;
        foundLines[i].expImgIdx = parent["exp_img"][i] | 0;
    }

	uint32_t nextScreen = parent["next_scn"] | 1;
    uint32_t length = arrSize;
    chat = { foundLines, length, nextScreen};
    
    // CRITICAL MEMORY OPTIMIZATION: Free the large JSON document immediately after extraction
    if (chat_doc) {
        chat_doc->clear();  // Clear the document to free its internal memory
        LV_LOG_INFO("[MEMORY] Freed JSON document, now %u bytes free", ESP.getFreeHeap());
    }
    
    if (!chat.lines)
    {
        LV_LOG_ERROR("No conversation lines loaded");
        return;
    }

    NextLine(NULL);
}

void NextLine(lv_event_t* e)
{
    if (curLine >= chat.chatCount)
    {
        // During countdown mode, check if solo mode allows proceeding
        if (inCountdownMode) {
            if (soloMode && briefingCountdown == 0) {
                // In solo mode, only start game if countdown message has already been shown
                if (soloCountdownShown) {
                    LV_LOG_INFO("[BRIEFING] Solo mode: player tapped to start game\n");
                    inCountdownMode = false; // Exit countdown mode
                    LV_LOG_INFO(">>>>>>>>>>>>>>>>>> startGameAfterBriefing fired from convos.cpp/NextLine\n");
                    startGameAfterBriefing(pendingGameNumber, isHostMode);
                    return;
                } else {
                    LV_LOG_INFO("[BRIEFING] Solo mode: countdown message shown, waiting for tap to start\n");
                    soloCountdownShown = true;
                    return; // Don't start game yet, wait for next tap
                }
            } else {
                LV_LOG_INFO("[BRIEFING] At countdown screen - no further advance allowed\n");
                return;
            }
        }
        Skip(e);
        return;
    }
    
    // Safety check for chat data
    if (!chat.lines) {
        LV_LOG_ERROR("[ERROR] chat.lines is null\n");
        return;
    }

    uint32_t idx = curLine++;
    lv_label_set_text(objects.lbl_queue_speech, chat.lines[idx].text);
    
    // If we've reached the countdown line during countdown mode, hide advance controls
    if (inCountdownMode && curLine >= chat.chatCount) {
        // Hide skip button and tap area once we reach countdown (last line)
        if (objects.btn_queue_skip) {
            lv_obj_add_flag(objects.btn_queue_skip, LV_OBJ_FLAG_HIDDEN);
        }
        if (objects.cnt_queue_tap) {
            lv_obj_add_flag(objects.cnt_queue_tap, LV_OBJ_FLAG_HIDDEN);
        }
        LV_LOG_INFO("[BRIEFING] Reached countdown screen - controls hidden\n");
        
        // In solo mode, keep controls visible so player can manually start when ready
        if (soloMode && briefingCountdown == 0) {
            LV_LOG_INFO("[BRIEFING] Solo mode: player can tap to start game when ready\n");
            // Keep tap controls visible in solo mode for manual game start
            if (objects.btn_queue_skip) {
                lv_obj_clear_flag(objects.btn_queue_skip, LV_OBJ_FLAG_HIDDEN);
            }
            if (objects.cnt_queue_tap) {
                lv_obj_clear_flag(objects.cnt_queue_tap, LV_OBJ_FLAG_HIDDEN);
            }
        }
    }

    uint32_t imgIndex = chat.lines[idx].imgIdx;
    uint32_t expImgIndex = chat.lines[idx].expImgIdx;
    
    // Add safety checks for queue_images
    if (queue_images && imgIndex < queue_images_size && queue_images[imgIndex].img && objects.img_queue_portrait) {
        lv_img_set_src(objects.img_queue_portrait, queue_images[imgIndex].img);
    } else {
       LV_LOG_ERROR("Queue image access failed: queue_images=%p, imgIndex=%u, queue_images_size=%zu\n", 
               queue_images, imgIndex, queue_images_size);
        // Don't set NULL image source - instead, hide the image or use a default
        if (objects.img_queue_portrait) {
            // Use index 0 as default Q portrait if available, otherwise hide the image
            if (queue_images && queue_images_size > 0 && queue_images[0].img) {
                lv_img_set_src(objects.img_queue_portrait, queue_images[0].img);
            } else {
                lv_obj_add_flag(objects.img_queue_portrait, LV_OBJ_FLAG_HIDDEN);
            }
        }
    }

    if (expImgIndex <= 0)
    {
        // Hide explanation image instead of setting NULL source
        if (objects.img_queue_explanation)
            lv_obj_add_flag(objects.img_queue_explanation, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    // Add safety checks for exp_images
    uint32_t expIndex = expImgIndex - 1; // Convert to 0-based index
    if (exp_images && expIndex < exp_images_size && exp_images[expIndex].img && objects.img_queue_explanation) {
        lv_img_set_src(objects.img_queue_explanation, exp_images[expIndex].img);
        lv_obj_clear_flag(objects.img_queue_explanation, LV_OBJ_FLAG_HIDDEN);
    } else {
        LV_LOG_ERROR("Exp image access failed: exp_images=%p, expIndex=%u, exp_images_size=%zu\n", 
               exp_images, expIndex, exp_images_size);
        // Hide explanation image instead of setting NULL source
        if (objects.img_queue_explanation)
            lv_obj_add_flag(objects.img_queue_explanation, LV_OBJ_FLAG_HIDDEN);
    }
}

void Skip(lv_event_t* e)
{
	// Prevent skipping during countdown mode, except in solo mode
	if (inCountdownMode) {
		if (soloMode && briefingCountdown == 0) {
			// In solo mode, only start game if countdown message has already been shown
			if (soloCountdownShown) {
				LV_LOG_INFO("[BRIEFING] Solo mode: skip pressed to start game\n");
				inCountdownMode = false; // Exit countdown mode
                LV_LOG_INFO(">>>>>>>>>>>>>>>>>> startGameAfterBriefing fired from convos.cpp/Skip\n");
				startGameAfterBriefing(pendingGameNumber, isHostMode);
				return;
			} else {
				LV_LOG_INFO("[BRIEFING] Solo mode: countdown message shown via skip, waiting for next input\n");
				soloCountdownShown = true;
				return; // Don't start game yet, wait for next input
			}
		} else {
			LV_LOG_INFO("[BRIEFING] Skip blocked - countdown in progress\n");
			return;
		}
	}
	
	FreeConvo();
	
	// If we're in countdown mode (briefing), always return to main screen when skipped
	// This prevents the game from starting in the background
	if (inCountdownMode) {
		LV_LOG_INFO("[INFO] Briefing skipped, returning to main screen\n");
		inCountdownMode = false; // Clear countdown mode
		soloCountdownShown = false; // Reset solo countdown flag
		
		// Clean up countdown timer if it exists
		if (countdownTimer) {
			lv_timer_del(countdownTimer);
			countdownTimer = nullptr;
		}
		
		loadScreen(SCREEN_ID_MAIN); // Always go to main screen
		return;
	}
	
	// Check for special outro trigger
	if (chat.nextScreen == 999) {
		LV_LOG_INFO("[INFO] Post-game summary completed - showing outro for RANK_CAPT achievement\n");
		LoadConvo("misc|outro");
		config.board.outroWatched = true;  // Mark outro as seen
		saveBoardConfig(config, "L:/default.json");
		return;
	}
	
	// Normal conversation flow - validate nextScreen value before loading
	if (chat.nextScreen < 1 || chat.nextScreen > 11) { // Valid screen IDs are 1-11
		LV_LOG_ERROR("[ERROR] Invalid nextScreen value: %u, defaulting to SCREEN_ID_MAIN (1)\n", chat.nextScreen);
		loadScreen((ScreensEnum)1); // Default to main screen
	} else {
		LV_LOG_INFO("[INFO] Loading screen %u\n", chat.nextScreen);
		loadScreen((ScreensEnum)chat.nextScreen);
	}
}

void LoadConvoCall(lv_event_t* e)
{
    // reset text box
    lv_label_set_text(objects.lbl_queue_speech, "");

    // change const input to a passable string
    const char* const_in = (const char*)lv_event_get_user_data(e);
    char JSON_in[strlen(const_in) + 1];
    strcpy(JSON_in, const_in);
    LV_LOG_INFO("String passed in callback: %s\n", JSON_in);
    LoadConvo(JSON_in);
}

void showQScreen(char* convo)
{
    loadScreen(SCREEN_ID_QUEUE);
	LoadConvo(convo);
}

void showOutro()
{
    showQScreen("misc|outro");
}

void showIntroAtBoot()
{
    if(config.board.introWatched) return;
    
    // MEMORY OPTIMIZATION: Free any unused resources before showing intro
    LV_LOG_INFO("[MEMORY] Pre-intro optimization: %u bytes free", ESP.getFreeHeap());
    
    // Force garbage collection and cleanup before intro loads
    extern void cleanup_lightweight_tasks();
    extern void init_lightweight_tasks();
    
    // Brief cleanup to free any stale memory
    heap_caps_malloc_extmem_enable(1);  // Ensure PSRAM preference
    
    // Log memory before intro  
    uint32_t preIntroMem = ESP.getFreeHeap();
    LV_LOG_WARN("[MEMORY] Starting intro with %u bytes free - monitoring for crash", preIntroMem);
    
    showIntro(NULL);
}

void showOutroAtWin()
{
    showOutro(NULL);
}

void showIntro(lv_event_t* e)
{
    if(e) play_random_beep();

    showQScreen("misc|intro");
    config.board.introWatched = true;
    saveBoardConfig(config, "L:/default.json");
}

void showOutro(lv_event_t* e)
{
    if(e) play_random_beep();

    showQScreen("misc|outro");
    config.board.outroWatched = true;  // Mark outro as seen
    saveBoardConfig(config, "L:/default.json");
}

// Check if user has reached RANK_CAPT and enable outro UI elements if so
void checkOutroUnlocks()
{
    int totalXP = 0;
    for (int i = 0; i < 6; i++) {
        if (config.games[i].XP != -1) {  
            totalXP += config.games[i].XP;
        }
    }
    
    if (totalXP >= RANK_CAPT) {
        LV_LOG_INFO("[INFO] User has reached RANK_CAPT (totalXP: %d >= %d) - enabling outro UI elements\n", totalXP, RANK_CAPT);
        
        // Enable the outro UI elements
        if (objects.cnt_info_outro && lv_obj_is_valid(objects.cnt_info_outro)) {
            lv_obj_clear_state(objects.cnt_info_outro, LV_STATE_DISABLED);
        }
        if (objects.btn_info_outro && lv_obj_is_valid(objects.btn_info_outro)) {
            lv_obj_clear_state(objects.btn_info_outro, LV_STATE_DISABLED);
        }
    } else {
        LV_LOG_INFO("[INFO] User has not reached RANK_CAPT yet (totalXP: %d < %d) - outro UI elements remain disabled\n", totalXP, RANK_CAPT);
    }
}

void showGameConvo(int gameId, int difficulty)
{
    char convo[32];
    snprintf(convo, sizeof(convo), "game%d|difficulty %d", gameId, difficulty);
    showQScreen(convo);
}

// Determine difficulty subset based on player's XP for a specific game
static int getDifficultyFromXP(int gameNumber, int gameXP) {
    // Use the same thresholds as avatar unlocks to determine difficulty
    if (gameXP >= AVATAR_UNLOCK_6) return 7;      // 28800+ XP - Expert
    if (gameXP >= AVATAR_UNLOCK_5) return 6;      // 14400+ XP - Advanced
    if (gameXP >= AVATAR_UNLOCK_4) return 5;      // 9600+ XP  - Intermediate
    if (gameXP >= AVATAR_UNLOCK_3) return 4;      // 4800+ XP  - Novice+
    if (gameXP >= AVATAR_UNLOCK_2) return 3;      // 2400+ XP  - Novice
    if (gameXP >= AVATAR_UNLOCK_1) return 2;      // 1200+ XP  - Beginner+
	if (gameXP >= AVATAR_UNLOCK_0) return 1;      // 0+ XP     - Beginner
    return 0; // 0 XP - First time (tutorial)
}

// Start game after pre-game briefing countdown completes
void startGameAfterBriefing(int gameNumber, bool hostMode) {
    extern Config config;
    extern game_parent* game;
    extern int currentGameNumber;
    extern int32_t earnedXP;
    extern int32_t earnedXP_mission;
    
    // SAFETY: Validate game number before proceeding
    if (gameNumber < 1 || gameNumber > 6) {
        LV_LOG_ERROR("[GAME] Invalid game number %d, aborting game start\n", gameNumber);
        return;
    }
    
    // SAFETY: Check if sessionManager exists before accessing
    extern SessionManager* sessionManager;
    if (!sessionManager) {
        LV_LOG_ERROR("[GAME] SessionManager is null, aborting game start\n");
        return;
    }
    
    // Check if game is already running - prevent restart
    if (game != nullptr && game->time_left() > 0) {
        LV_LOG_INFO("[GAME] Game already running, ignoring restart request\n");
        return;
    }
    
    LV_LOG_INFO("[GAME] Starting game %d after briefing (%s mode)\n", gameNumber, hostMode ? "host" : "player");
    
    // Restore skip controls before leaving queue screen
    if (objects.btn_queue_skip) {
        lv_obj_clear_flag(objects.btn_queue_skip, LV_OBJ_FLAG_HIDDEN);
    }
    if (objects.cnt_queue_tap) {
        lv_obj_clear_flag(objects.cnt_queue_tap, LV_OBJ_FLAG_HIDDEN);
    }
    
    // Set current game number for post-game summary
    currentGameNumber = gameNumber;
    
    // Load game screen
    loadScreen(SCREEN_ID_GAME_SCREEN);
    
    // Clean up any existing game instance
    if (game != nullptr) {
        delete game;
        game = nullptr;
    }
    
    // Choose correct XP variable based on mode
    int32_t* xpPtr = hostMode ? &earnedXP_mission : &earnedXP;
    
    // Create game instance with error handling
    try {
        switch (gameNumber) {
            case 1: game = new game1::g_game1(xpPtr); break;
            case 2: game = new game2::g_game2(xpPtr); break;
            case 3: game = new game3::g_game3(xpPtr); break;
            case 4: game = new game4::g_game4(xpPtr); break;
            case 5: game = new game5::g_game5(xpPtr); break;
            case 6: game = new game6::g_game6(xpPtr); break;
            default: game = new game1::g_game1(xpPtr); break;
        }
        
        if (!game) {
            LV_LOG_ERROR("[GAME] Failed to create game %d instance\n", gameNumber);
            return;
        }
    } catch (const std::exception& e) {
        LV_LOG_ERROR("[GAME] Exception creating game %d: %s\n", gameNumber, e.what());
        return;
    } catch (...) {
        LV_LOG_ERROR("[GAME] Unknown exception creating game %d\n", gameNumber);
        return;
    }
    
    // Initialize gameState for multiplayer
    extern GameState gameState;
    extern SessionManager* sessionManager;
    
    // Preserve playerCount if it was set by timed game start packet
    preservedPlayerCount = gameState.playerCount;
    LV_LOG_INFO("[GAME] Current gameState.playerCount before reset: %d\n", preservedPlayerCount);
    LV_LOG_INFO("[GAME] gameState.sessionID before reset: %u\n", gameState.sessionID);
    
    // Check if playerIDs were already set by TimedGameStart packet (for clients)
    bool hasExistingPlayerIDs = false;
    int existingNonZeroCount = 0;
    for (int i = 0; i < 5; i++) {
        if (gameState.playerIDs[i] != 0) {
            existingNonZeroCount++;
        }
    }
    if (existingNonZeroCount >= 2) { // Host + at least one other player
        hasExistingPlayerIDs = true;
        LV_LOG_ERROR("[GAME_RESET] Found existing playerIDs structure with %d players - preserving it\n", existingNonZeroCount);
        for (int i = 0; i < 5; i++) {
            if (gameState.playerIDs[i] != 0) {
                LV_LOG_ERROR("[GAME_RESET]   playerIDs[%d] = %u (preserved)\n", i, gameState.playerIDs[i]);
            }
        }
    } else {
        LV_LOG_ERROR("[GAME_RESET] No existing playerIDs structure found - will build new one\n");
    }
    
    // Reset gameState (but preserve playerIDs if they exist from TimedGameStart)
    if (!hasExistingPlayerIDs) {
        memset(gameState.playerIDs, 0, sizeof(gameState.playerIDs));
        LV_LOG_ERROR("[GAME_RESET] Cleared playerIDs array\n");
    }
    memset(gameState.scores, 0, sizeof(gameState.scores));
    memset(gameState.playerStatus, 0, sizeof(gameState.playerStatus));
    memset(gameState.timeSinceLastMessage, 0, sizeof(gameState.timeSinceLastMessage));
    
    gameState.myScore = 0;
    uint32_t preservedSessionID = gameState.sessionID; // Preserve session ID
    if (!hasExistingPlayerIDs) {
        gameState.sessionID = 0;
        gameState.playerCount = 0;
    } else {
        // Keep existing playerCount if we preserved playerIDs
        LV_LOG_ERROR("[GAME_RESET] Preserved playerCount = %d and sessionID = %u\n", gameState.playerCount, gameState.sessionID);
    }
    
    // If in a session, populate playerIDs and session info
    // Check for active session (either in lobby or in game)
    if (sessionManager && sessionManager->getMySessionId() != 0) {
        gameState.sessionID = sessionManager->getMySessionId();
        
        // Get lobby players (for host) or create minimal multiplayer setup (for clients)
        if (hostMode) {
            PsramVector<LobbyPlayer> lobbyPlayers = sessionManager->getLobbyPlayers();
            LV_LOG_INFO("[GAME] Host mode: found %zu lobby players\n", lobbyPlayers.size());
            
            // Add host (self) as first player
            extern Config config;
            gameState.playerIDs[0] = config.user.nodeId;
            gameState.playerCount = 1;
            
            // Add lobby players (FIXED: correct loop bounds)
            for (size_t i = 0; i < lobbyPlayers.size(); i++) {
                if (gameState.playerCount < 5) { // Safety check: max 5 players
                    gameState.playerIDs[gameState.playerCount] = lobbyPlayers[i].nodeId;
                    gameState.playerCount++;
                } else {
                    LV_LOG_WARN("[GAME] Max players reached, skipping lobby player %u\n", lobbyPlayers[i].nodeId);
                    break;
                }
            }
            
            LV_LOG_ERROR("[HOST_DEBUG] Host final playerCount = %d (should be 3 for 3-player game)\n", gameState.playerCount);
            LV_LOG_INFO("[GAME] Host gameState initialized: sessionID=%u, playerCount=%d\n", 
                         gameState.sessionID, gameState.playerCount);
            for (int i = 0; i < gameState.playerCount; i++) {
                LV_LOG_INFO("[GAME]   Player %d: nodeID=%u\n", i, gameState.playerIDs[i]);
            }
        } else {
            // Client mode - restore playerCount that was set by timed game start packet
            if (preservedPlayerCount > 0) {
                gameState.playerCount = preservedPlayerCount;
                LV_LOG_INFO("[GAME] Client restored playerCount to %d from preserved value\n", gameState.playerCount);
            } else {
                // Fallback to minimal multiplayer setup
                gameState.playerCount = 1;
                LV_LOG_WARN("[GAME] Client: no preserved playerCount, defaulting to 1\n");
            }
            
            // CRITICAL FIX: Clients need to initialize their own nodeId in the correct slot
            // The slot positions should match what the host established
            extern Config config;
            
            // For now, put ourselves in a placeholder slot - the score processing will handle dynamic assignment
            // The important thing is that we initialize our own nodeId somewhere so score broadcasting works
            bool foundMySlot = false;
            for (int i = 0; i < 5; i++) {
                if (gameState.playerIDs[i] == config.user.nodeId) {
                    foundMySlot = true;
                    LV_LOG_INFO("[GAME] Client found self already in playerIDs[%d] = %u\n", i, config.user.nodeId);
                    break;
                }
            }
            
            if (!foundMySlot) {
                // Put ourselves in first available slot (this will be corrected by score processing)
                for (int i = 0; i < 5; i++) {
                    if (gameState.playerIDs[i] == 0) {
                        gameState.playerIDs[i] = config.user.nodeId;
                        LV_LOG_INFO("[GAME] Client initialized self as playerIDs[%d] = %u\n", i, config.user.nodeId);
                        break;
                    }
                }
            }
            
            // Note: Proper playerIDs structure will be established through score packet exchanges
        }
    } else {
        LV_LOG_INFO("[GAME] Solo mode - no multiplayer gameState initialization\n");
        /*gameState.playerCount = sessionManager->lobbyPlayers.size();*/
    }
    
    // Setup and start the game with safety checks
    LV_LOG_INFO("[GAME] Starting game setup with %u players\n", gameState.playerCount);
    
    // SAFETY: Final validation before game setup
    if (!game) {
        LV_LOG_ERROR("[GAME] Game object is null before setup, aborting\n");
        return;
    }
    
    try {
        game->Setup();
        LV_LOG_INFO("[GAME] Game %d started successfully\n", gameNumber);
    } catch (const std::exception& e) {
        LV_LOG_ERROR("[GAME] Exception during game setup: %s\n", e.what());
        // Clean up failed game
        delete game;
        game = nullptr;
        return;
    } catch (...) {
        LV_LOG_ERROR("[GAME] Unknown exception during game setup\n");
        // Clean up failed game
        delete game;
        game = nullptr;
        return;
    }
}

// Timer callback for countdown
// Clear briefing state to prevent game restart bugs
void clearBriefingState() {
    LV_LOG_INFO("[BRIEFING] Clearing briefing state to prevent restart bugs\n");
    
    // Clean up countdown timer if it exists
    if (countdownTimer) {
        lv_timer_del(countdownTimer);
        countdownTimer = nullptr;
    }
    
    // Reset all static briefing variables
    briefingCountdown = 0;
    pendingGameNumber = 0;
    inCountdownMode = false;
    soloCountdownShown = false;
}

static void countdownTimerCallback(lv_timer_t* timer) {
    briefingCountdown--;
    
    if (briefingCountdown <= 0) {
        // Time's up - start the game
        lv_timer_del(countdownTimer);
        countdownTimer = nullptr;
        inCountdownMode = false; // Exit countdown mode
        
        // SAFETY CHECK: Only start game if we have a valid pending game
        if (pendingGameNumber > 0 && pendingGameNumber <= 6) {
            // Call the appropriate game start function
            LV_LOG_INFO(">>>>>>>>>>>>>>>>>> startGameAfterBriefing fired from convos.cpp/countdownTimerCallback\n");
            startGameAfterBriefing(pendingGameNumber, isHostMode);
            
            LV_LOG_INFO("[BRIEFING] Countdown finished, starting game %d (%s mode)\n", 
                          pendingGameNumber, isHostMode ? "host" : "player");
        } else {
            LV_LOG_ERROR("[BRIEFING] Invalid pendingGameNumber %d, not starting game\n", pendingGameNumber);
        }
        return;
    }
    
    // Update countdown display - countdown is always the last line
    if (chat.lines && chat.chatCount > 0) {
        Line* countdownLine = &chat.lines[chat.chatCount - 1]; // Countdown is the last line
        if (countdownLine->text) {
            // Update the countdown text  
            char countdownText[50];
            snprintf(countdownText, sizeof(countdownText), "Game begins in: %d seconds", briefingCountdown);
            
            // Reallocate and update the text (use heap_caps_free and heap_caps_malloc for PSRAM)
            if (countdownLine->text) {
                heap_caps_free(countdownLine->text);
                countdownLine->text = nullptr;
            }
            countdownLine->text = (char*)heap_caps_malloc(strlen(countdownText) + 1, MALLOC_CAP_SPIRAM);
            if (countdownLine->text) {
                strcpy(countdownLine->text, countdownText);
            } else {
                LV_LOG_ERROR("[HEAP] Failed to allocate memory for countdown text\n");
            }
            
            // If we're currently showing the countdown line, update the display
            if (curLine >= chat.chatCount) {
                lv_label_set_text(objects.lbl_queue_speech, countdownText);
            }
        }
    }
}

void showPreGameBriefing(int gameNumber, int countdownSeconds) {
    LV_LOG_INFO("[MEM] Before briefing - Free RAM: %u bytes, Free PSRAM: %u bytes\n", 
                  ESP.getFreeHeap(), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    
    // Clean up any existing conversation and timer first to prevent memory leaks
    if (countdownTimer) {
        lv_timer_del(countdownTimer);
        countdownTimer = nullptr;
    }
    FreeConvo();
    
    LV_LOG_INFO("[MEM] After FreeConvo - Free RAM: %u bytes, Free PSRAM: %u bytes\n", 
                  ESP.getFreeHeap(), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    
    // Reset solo countdown flag for new briefing
    soloCountdownShown = false;
    
    // Get player's XP for this game to determine difficulty/subset
    extern Config config;
    int gameXP = config.games[gameNumber - 1].XP;
    int difficulty = getDifficultyFromXP(gameNumber, gameXP);
    if (gameXP <= -1)
    {
        config.games[gameNumber - 1].XP = 0; // Reset XP to stop tutorial from playing again.
    }
    
    LV_LOG_INFO("[BRIEFING] Game %d briefing: XP=%d, difficulty=%d, countdown=%ds\n", 
                  gameNumber, gameXP, difficulty, countdownSeconds);
    
    // Set up countdown
    briefingCountdown = countdownSeconds;
    pendingGameNumber = gameNumber;
    
    // Determine if this is host mode by checking if we have lobby players (only hosts have lobby players)
    extern SessionManager* sessionManager;
    bool hasLobbyPlayers = false;
    if (sessionManager) {
        PsramVector<LobbyPlayer> lobbyPlayers = sessionManager->getLobbyPlayers();
        hasLobbyPlayers = (lobbyPlayers.size() > 0);
    }
    isHostMode = hasLobbyPlayers;
    
    LV_LOG_ERROR("[HOST_MODE_DEBUG] sessionManager exists: %s\n", sessionManager ? "YES" : "NO");
    if (sessionManager) {
        LV_LOG_INFO("[HOST_MODE_DEBUG] sessionManager->getMyStatus() = %d\n", (int)sessionManager->getMyStatus());
        LV_LOG_INFO("[HOST_MODE_DEBUG] lobbyPlayers.size() = %zu\n", hasLobbyPlayers ? sessionManager->getLobbyPlayers().size() : 0);
        LV_LOG_INFO("[HOST_MODE_DEBUG] hasLobbyPlayers = %s\n", hasLobbyPlayers ? "TRUE" : "FALSE");
    }
    LV_LOG_INFO("[HOST_MODE_DEBUG] Final isHostMode = %s\n", isHostMode ? "TRUE" : "FALSE");
    
    // Check if host is playing solo (ONLY for actual hosts, not clients)
    soloMode = false;
    if (sessionManager && sessionManager->getMyStatus() == Hosting) {
        // Only true hosts (with Hosting status) can trigger solo mode
        PsramVector<LobbyPlayer> lobbyPlayers = sessionManager->getLobbyPlayers();
        soloMode = lobbyPlayers.empty();
        if (soloMode) {
            LV_LOG_INFO("[BRIEFING] Solo mode detected - host has no lobby players, will skip countdown\n");
        } else {
            LV_LOG_INFO("[BRIEFING] Multiplayer mode - host has %zu lobby players, using full countdown\n", lobbyPlayers.size());
        }
    } else {
        LV_LOG_INFO("[BRIEFING] Client mode - using standard countdown duration\n");
    }
    
    // Load the queue screen first
    loadScreen(SCREEN_ID_QUEUE);
    
    // Build conversation key
    char convo[32];
    snprintf(convo, sizeof(convo), "game%d|difficulty %d", gameNumber, difficulty);
    LV_LOG_INFO("[BRIEFING] Loading conversation: %s\n", convo);
    
    LV_LOG_INFO("[MEM] Before LoadConvo - Free RAM: %u bytes, Free PSRAM: %u bytes\n",
                  ESP.getFreeHeap(), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    
    // Use the existing LoadConvo function to avoid code duplication and memory leaks
    LoadConvo(convo);
    
    LV_LOG_INFO("[MEM] After LoadConvo - Free RAM: %u bytes, Free PSRAM: %u bytes\n",
                  ESP.getFreeHeap(), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    
    // Force garbage collection and memory cleanup after loading conversation
    if (chat_doc) {
        chat_doc->clear();  // Clear the large JSON document to free memory
        LV_LOG_INFO("[MEM] After clearing chat_doc - Free RAM: %u bytes, Free PSRAM: %u bytes\n",
                      ESP.getFreeHeap(), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    }
    
    // Check if conversation loaded successfully
    if (!chat.lines || chat.chatCount == 0) {
        LV_LOG_ERROR("[ERROR] Failed to load conversation %s, using fallback\n", convo);
        
        // Create minimal fallback conversation in PSRAM
        chat.chatCount = 1;
        chat.lines = (Line*)heap_caps_malloc(sizeof(Line), MALLOC_CAP_SPIRAM);
        if (chat.lines) {
            const char* fallbackText = "Prepare for the upcoming challenge.";
            chat.lines[0].text = (char*)heap_caps_malloc(strlen(fallbackText) + 1, MALLOC_CAP_SPIRAM);
            if (chat.lines[0].text) {
                strcpy(chat.lines[0].text, fallbackText);
            }
            chat.lines[0].imgIdx = 0;
            chat.lines[0].expImgIdx = 0;
            chat.nextScreen = SCREEN_ID_QUEUE;
        }
    }
    
    // Add countdown text as the final line, but only if we have valid chat data
    if (chat.lines && chat.chatCount > 0) {
        // Safely expand the chat array
        size_t newCount = chat.chatCount + 1;
        Line* newLines = (Line*)heap_caps_realloc(chat.lines, newCount * sizeof(Line), MALLOC_CAP_SPIRAM);
        if (newLines) {
            chat.lines = newLines;
            chat.chatCount = newCount;
            
            // Create countdown line
            Line* countdownLine = &chat.lines[chat.chatCount - 1];
            char countdownText[50];
            snprintf(countdownText, sizeof(countdownText), "Game begins in: %d seconds", briefingCountdown);
            
            countdownLine->text = (char*)heap_caps_malloc(strlen(countdownText) + 1, MALLOC_CAP_SPIRAM);
            if (countdownLine->text) {
                strcpy(countdownLine->text, countdownText);
            }
            countdownLine->imgIdx = 0; // Default Q portrait
            countdownLine->expImgIdx = 0; // No explanation image
        } else {
            LV_LOG_ERROR("[ERROR] Failed to reallocate chat lines for countdown\n");
        }
    }
    
    // Reset to first line (LoadConvo already advanced to line 1)
    curLine = 0;
    NextLine(NULL);
    
    // Enable countdown mode but allow tapping through conversation
    inCountdownMode = true;
    
    // Hide skip button in pregame briefs - user must tap through conversation
    if (objects.btn_queue_skip) {
        lv_obj_add_flag(objects.btn_queue_skip, LV_OBJ_FLAG_HIDDEN);
    }
    if (objects.cnt_queue_tap) {
        lv_obj_clear_flag(objects.cnt_queue_tap, LV_OBJ_FLAG_HIDDEN);
    }
    
    LV_LOG_INFO("[BRIEFING] Conversation loaded - %zu lines total\n", chat.chatCount);
    
    // Adjust countdown for solo mode
    if (soloMode) {
        // In solo mode, skip countdown entirely
        briefingCountdown = 0;
        LV_LOG_INFO("[BRIEFING] Solo mode: skipping countdown, game starts immediately\n");
        
        // Update the countdown text in the last line if it exists
        if (chat.lines && chat.chatCount > 0) {
            Line* countdownLine = &chat.lines[chat.chatCount - 1];
            if (countdownLine->text) {
                heap_caps_free(countdownLine->text);
                const char* soloText = "Tap to begin solo game";
                countdownLine->text = (char*)heap_caps_malloc(strlen(soloText) + 1, MALLOC_CAP_SPIRAM);
                if (countdownLine->text) {
                    strcpy(countdownLine->text, soloText);
                }
            }
        }
    }
    
    // Start countdown timer (1 second intervals) - unless in solo mode
    if (soloMode) {
        // In solo mode, no countdown - user taps through conversation to start
        countdownTimer = nullptr;
        LV_LOG_INFO("[BRIEFING] Solo mode: no countdown timer, tap through conversation to start\n");
    } else {
        countdownTimer = lv_timer_create(countdownTimerCallback, 1000, NULL);
    }
}

void showPostGameSummary(int gameNumber, int baseXP, int bonusXP, int previousAvatarCount, int newAvatarCount, const char* previousRank, const char* newRank, TeamBonusResult teamBonusDetails)
{
    LV_LOG_INFO("[POST_GAME] Starting post-game summary, free RAM: %u", ESP.getFreeHeap());
    
    // CRITICAL SAFETY: Ensure LVGL is in a stable state before manipulating objects
    lv_timer_handler();  // Process any pending timer operations
    
    // Clean up any existing conversation
    FreeConvo();
    curLine = 0;
    
    // Create dynamic conversation structure
    const char* gameNames[] = {"Assimilation", "LineCon", "Packet Filter", "Wiring", "Defender", "Speeder"};
    const char* gameName = (gameNumber >= 1 && gameNumber <= 6) ? gameNames[gameNumber - 1] : "that last game";
    
    // Calculate new totals
    int newTotalXP = config.user.totalXP; //updated in the custom.cpp function that calls this one
    int avatarsUnlocked = newAvatarCount - previousAvatarCount;
    bool rankPromoted = (strcmp(previousRank, newRank) != 0);
    
    // Build dynamic dialogue with Q's characteristic arrogance
    std::vector<std::string> dialogueLines;
    
    // Opening line - always dismissive about the test completion
    const char* openings[] = {
        "Another trial completed. How... pedestrian.",
        "Well, well. Another mortal stumbles through my tests.",
        "Finished, are we? How delightfully... adequate.",
        "Another test concluded. I suppose congratulations are in order?",
        "My, my. You've managed to complete yet another examination."
    };
    dialogueLines.push_back(openings[rand() % 5]);
    
    // XP announcement with team bonus logic and Q's characteristic arrogance
    char xpLine[250];
    int totalXPEarned = baseXP + bonusXP;
    
    if (totalXPEarned > 0) {
        if (bonusXP > 0) {
            // Had team bonus - praise teamwork sarcastically
            snprintf(xpLine, sizeof(xpLine), 
                "You've earned %d XP from %s, plus %d bonus from your... competent colleagues, for %d total.", 
                baseXP, gameName, bonusXP, totalXPEarned);
        } else {
            // No team bonus - standard XP announcement
            snprintf(xpLine, sizeof(xpLine), 
                "You've added %d XP to %s for a total of %d.", 
                baseXP, gameName, newTotalXP);
        }
    } else {
        snprintf(xpLine, sizeof(xpLine), 
            "No experience gained in %s? How utterly... predictable.\nYour total remains a modest %d.", 
            gameName, newTotalXP);
    }
    dialogueLines.push_back(xpLine);
    
    // Team bonus commentary - detailed Q-style analysis
    if (teamBonusDetails.totalTeammates > 0) {
        char teamLine[300];
        
        if (bonusXP > 0) {
            // Successful team bonus - sarcastic praise
            const char* teamSuccessComments[] = {
                "Your teammates actually performed adequately - %d of them scored 99%% and stayed connected. How... refreshing.",
                "Impressive! %d teammates both achieved excellence AND remained present. A rare combination of competence and commitment.",
                "Well, well. %d of your associates managed to exceed expectations while maintaining connectivity. Miracles do happen.",
                "How delightfully unexpected - %d teammates proved worthy of the bonus they've earned you. Choose your allies wisely, indeed.",
                "Your %d qualifying teammates have elevated your performance through their own. A testament to... adequate social networking."
            };
            snprintf(teamLine, sizeof(teamLine), teamSuccessComments[rand() % 5], teamBonusDetails.eligibleTeammates);
            dialogueLines.push_back(teamLine);
            
        } else if (teamBonusDetails.disconnectedCount > 0 && teamBonusDetails.lowScoreCount == 0) {
            // High scores but disconnected - connectivity commentary
            const char* disconnectComments[] = {
                "Your crew scored well but abandoned you when it mattered. Perhaps work on... loyalty?",
                "Ah, the classic disappearing act. %d crewmates had the skill but lacked the staying power.",
                "Your associates achieved excellence then vanished. One must question their commitment to the cause.",
                "How very... typical. Your crew performed well and then left. Choose friends who stay, perhaps?",
                "Your crew performed, then departed. A masterclass in tactical abandonment. How... illuminating."
            };
            snprintf(teamLine, sizeof(teamLine), disconnectComments[rand() % 5], teamBonusDetails.disconnectedCount);
            dialogueLines.push_back(teamLine);
            
        } else if (teamBonusDetails.lowScoreCount > 0 && teamBonusDetails.disconnectedCount == 0) {
            // Connected but low scores - performance commentary
            const char* lowScoreComments[] = {
                "Your crew remained present but underperformed. Loyalty without competence is... quaint.",
                "Your crew stayed connected yet failed to excel. Perhaps choose associates with higher... aspirations?",
                "Regarding your crew: present but not particularly productive. Your social circle needs some... curation.",
                "Your crew has connectivity without capability. How wonderfully... pedestrian.",
                "Your crew showed up but didn't show off. Consider raising your standards in companionship."
            };
            snprintf(teamLine, sizeof(teamLine), lowScoreComments[rand() % 5], teamBonusDetails.lowScoreCount);
            dialogueLines.push_back(teamLine);
            
        } else {
            // Mixed failures or complete failure
            const char* mixedFailureComments[] = {
                "Your teammates provided a fascinating study in mediocrity - neither excellent scores nor reliable connections.",
                "What a delightful collection of shortcomings your associates displayed. How very... educational.",
                "Your companions achieved the remarkable feat of failing in multiple dimensions simultaneously.",
                "Between disconnections and underperformance, your team covered all bases of disappointment.",
                "Perhaps it's time to... reconsider your social networking strategy. Your crew's results speak volumes."
            };
            snprintf(teamLine, sizeof(teamLine), mixedFailureComments[rand() % 5]);
            dialogueLines.push_back(teamLine);
        }
    }
    
    // Avatar unlock commentary - only if avatars were unlocked
    if (avatarsUnlocked > 0) {
        char avatarLine[200];
        const char* avatarComments[] = {
            "Oh, and you've unlocked %d new avatars. How... collectible of you.",
            "Congratulations! %d more digital personas to hide behind. Charming.",
            "You've earned %d new avatars. Because apparently, variety is the spice of mediocrity.",
            "Another %d avatars added to your collection. Building quite the menagerie, aren't we?",
            "%d new avatars unlocked. I'm sure they'll serve you well in your... endeavors."
        };
        snprintf(avatarLine, sizeof(avatarLine), avatarComments[rand() % 5], avatarsUnlocked);
        dialogueLines.push_back(avatarLine);
    }
    
    // Rank promotion commentary - only if promoted
    if (rankPromoted) {
        char rankLine[200];
        const char* promotionComments[] = {
            "And a promotion to %s! How... upwardly mobile of you.",
            "Elevated to %s, I see. Don't let it go to your head.",
            "Congratulations on your advancement to %s. Try not to disappoint.",
            "A new rank: %s. I suppose even mortals can climb ladders.",
            "Promoted to %s! How delightfully... hierarchical."
        };
        snprintf(rankLine, sizeof(rankLine), promotionComments[rand() % 5], newRank);
        dialogueLines.push_back(rankLine);
    }
    
    // Closing dismissal
    const char* closings[] = {
        "Now run along. There are more tests awaiting.",
        "Until next time... if there is a next time.",
        "Dismissed. Try not to get too excited by your 'achievements'.",
        "Off with you. I have more important matters to attend to.",
        "Go forth and... continue being adequately ordinary."
    };
    dialogueLines.push_back(closings[rand() % 5]);
    
    // Create the chat structure manually using PSRAM since we're not loading from JSON
    size_t lineCount = dialogueLines.size();
    foundLines = (Line*)heap_caps_malloc(lineCount * sizeof(Line), MALLOC_CAP_SPIRAM);
    
    for (size_t i = 0; i < lineCount; i++) {
        foundLines[i].text = (char*)heap_caps_malloc(dialogueLines[i].length() + 1, MALLOC_CAP_SPIRAM);
        strcpy(foundLines[i].text, dialogueLines[i].c_str());
        foundLines[i].imgIdx = 0; // Default Q portrait
        foundLines[i].expImgIdx = 0; // No explanation images
    }
    
    // Set up chat structure - return to main screen after summary
    chat = { foundLines, lineCount, SCREEN_ID_MAIN };
    
    // SAFETY: Ensure LVGL is stable before loading new screen
    lv_timer_handler();  // Process any pending operations
    
    // Load queue screen and start the conversation with safety delay
    loadScreen(SCREEN_ID_QUEUE);
    
    // Small delay to allow screen transition to complete
    delay(20);
    lv_timer_handler();  // Process screen load operations
    
    NextLine(NULL);
    
	checkOutroUnlocks(); // Check if user has reached RANK_CAPT and enable outro UI elements if so

    if (newTotalXP >= RANK_CAPT && !config.board.outroWatched) {
        LV_LOG_INFO("[INFO] User reached RANK_CAPT (totalXP: %d >= %d) and hasn't seen outro - will show automatically after summary\n", newTotalXP, RANK_CAPT);
        
        // Set up a flag to show outro after this conversation ends
        // We'll use a special nextScreen value to indicate outro should be shown
        //chat = { foundLines, lineCount, 999 }; // Special code for outro
        
        // Load queue screen and start the conversation
        //loadScreen(SCREEN_ID_QUEUE);
        //NextLine(NULL);

        showOutroAtWin();

        return; // Exit early since we set up the chat structure above
    }
}