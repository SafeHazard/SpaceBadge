#include "mission.h"
#include "session.h"
#include "./src/ui/ui.h"
#include "./src/ui/images.h"
#include <painlessMesh.h>

#include "game_parent.h"
#include "g_game1.h"
#include "g_game2.h"
#include "g_game3.h"
#include "g_game4.h"
#include "g_game5.h"
#include "g_game6.h"
#include "multiplayer_overlay.h"
#include "convos.h"

int32_t earnedXP_mission = 0;
extern game_parent* game;

// used to evaluate if host or join screens should have 'ready' or 'engage' enabled
void setMissionReadyState(lv_event_t* e)
{
	// for the host screen
	if (lv_scr_act() == lv_obj_get_screen(objects.host))
	{
		// Use comprehensive check that considers game selection AND player ready states
		extern void checkHostStartButtonState();
		checkHostStartButtonState();
	}

	// for the join screen
	if (lv_scr_act() == lv_obj_get_screen(objects.join))
	{
		extern SessionManager* sessionManager;
		
		// Check if we're actually in a session (more reliable than roller text)
		bool inSession = sessionManager && sessionManager->isInLobby();
		
		// Check if a valid game is selected (index > 0, since index 0 might be "No Game" or similar)
		bool gameSelected = lv_dropdown_get_selected(objects.ddl_join_games) > 0;
		
		// Also check the roller to make sure we're not on "No Crews Found"
		char selectedHost[64];
		bool validHost = false;
		if (objects.roller_join_games) {
			lv_roller_get_selected_str(objects.roller_join_games, selectedHost, sizeof(selectedHost));
			validHost = (strcmp(selectedHost, "No Crews Found") != 0 && strlen(selectedHost) > 0);
		}
		
		LV_LOG_INFO("[MISSION] Join ready state check: inSession=%s, gameSelected=%s, validHost=%s (host='%s')\n", 
		             inSession ? "yes" : "no", gameSelected ? "yes" : "no", validHost ? "yes" : "no", selectedHost);
		
		// Enable ready button if we're in a session AND have selected a game AND have a valid host
		if (inSession && gameSelected && validHost)
		{
			lv_obj_clear_state(objects.btn_join_ready, LV_STATE_DISABLED);
			lv_obj_clear_state(objects.cnt_join_ready, LV_STATE_DISABLED);
		}
		else
		{
			lv_obj_add_state(objects.btn_join_ready, LV_STATE_DISABLED);
			lv_obj_add_state(objects.cnt_join_ready, LV_STATE_DISABLED);
		}
	}
}

// Manual selection tracking (accessible from other files)
lv_obj_t* selectedPlayerButton = nullptr;
uint32_t selectedPlayerNodeId = 0;

// fires when a player entry on the Host screen is clicked
void hostPlayerListClick(lv_event_t* e)
{
	play_random_beep();

	static uint32_t lastClickTime = 0;
	uint32_t now = millis();
	LV_LOG_INFO("[HOST] Player list clicked at %u ms (delta: %u ms)\n", now, now - lastClickTime);
	lastClickTime = now;
	
	lv_obj_t* clickedButton = lv_event_get_current_target_obj(e);
	uint32_t nodeId = (uint32_t)(uintptr_t)lv_obj_get_user_data(clickedButton);
	LV_LOG_INFO("[HOST] Button clicked (nodeId: %u)\n", nodeId);

	// Check if this is the same button that was already selected
	bool wasAlreadySelected = (clickedButton == selectedPlayerButton && nodeId == selectedPlayerNodeId);
	LV_LOG_INFO("[HOST] Was already selected: %s (selectedNodeId: %u)\n", 
	             wasAlreadySelected ? "yes" : "no", selectedPlayerNodeId);

	// Clear visual selection from all buttons
	LV_LOG_INFO("[HOST] Clearing all visual selections\n");
	for (uint32_t i = 0; i < lv_obj_get_child_count(objects.list_host_players); i++)
	{
		lv_obj_t* button = lv_obj_get_child(objects.list_host_players, i);
		lv_obj_remove_state(button, LV_STATE_CHECKED);
		lv_obj_clear_state(button, LV_STATE_CHECKED);
	}
	
	// Reset our tracking
	selectedPlayerButton = nullptr;
	selectedPlayerNodeId = 0;

	// If this button was NOT already selected, select it
	if (!wasAlreadySelected)
	{
		LV_LOG_INFO("[HOST] Selecting new player button\n");
		lv_obj_add_state(clickedButton, LV_STATE_CHECKED);
		
		// Update our manual tracking
		selectedPlayerButton = clickedButton;
		selectedPlayerNodeId = nodeId;
		
		// Verify the state was actually set
		bool newCheckState = lv_obj_has_state(clickedButton, LV_STATE_CHECKED);
		LV_LOG_INFO("[HOST] After setting checked state: %s\n", newCheckState ? "checked" : "unchecked");

		// Enable kick button when a player is selected
		LV_LOG_INFO("[HOST] Enabling kick button\n");
		lv_obj_clear_state(objects.btn_host_kick, LV_STATE_DISABLED);
		lv_obj_clear_state(objects.cnt_host_kick, LV_STATE_DISABLED);
		
		// Store selected player nodeId for kick function
		lv_obj_set_user_data(objects.btn_host_kick, (void*)(uintptr_t)nodeId);

		LV_LOG_INFO("[HOST] Selected player %u for potential kick\n", nodeId);
	}
	else
	{
		LV_LOG_INFO("[HOST] Same button clicked again, deselecting and disabling kick\n");
		// Same button clicked again, disable kick
		lv_obj_add_state(objects.btn_host_kick, LV_STATE_DISABLED);
		lv_obj_add_state(objects.cnt_host_kick, LV_STATE_DISABLED);
		lv_obj_set_user_data(objects.btn_host_kick, nullptr);
	}
}

// Host kick button click - remove selected player from lobby
void hostKickClicked(lv_event_t* e)
{
	if (!sessionManager || !sessionManager->isHosting())
	{
		LV_LOG_WARN("[HOST] Kick clicked but not hosting\n");
		return;
	}

	// Get selected player ID from button user data
	uint32_t playerToKick = (uint32_t)(uintptr_t)lv_obj_get_user_data(objects.btn_host_kick);
	if (playerToKick == 0)
	{
		LV_LOG_WARN("[HOST] No player selected for kick\n");
		return;
	}

	play_random_beep();

	// Send rejection packet to the player
	extern painlessMesh mesh;
	String rejectPacket = createJSONSessionPacket(sessionManager->getMySessionId(), Hosting, Reject);
	bool sent = mesh.sendSingle(playerToKick, rejectPacket);

	if (sent)
	{
		LV_LOG_INFO("[HOST] Sent kick packet to player %u\n", playerToKick);
	}
	else
	{
		LV_LOG_INFO("[HOST] Failed to send kick packet to player %u\n", playerToKick);
	}

	// Remove player from our lobby (this will also update the UI)
	sessionManager->removeLobbyPlayer(playerToKick);

	// Reset manual selection tracking
	selectedPlayerButton = nullptr;
	selectedPlayerNodeId = 0;

	// Disable kick button since no player is selected now
	lv_obj_add_state(objects.btn_host_kick, LV_STATE_DISABLED);
	lv_obj_add_state(objects.cnt_host_kick, LV_STATE_DISABLED);
	lv_obj_set_user_data(objects.btn_host_kick, nullptr);
}

// Host screen loading event - start hosting
void hostScreenLoaded(lv_event_t* e)
{
	if (!sessionManager) return;

	sessionManager->lobbyPlayers.clear();
	extern Config config;

	// Start hosting with our node ID as session ID
	uint32_t sessionId = config.user.nodeId;
	
	// Add delay to prevent mesh overload during rapid screen transitions
	static uint32_t lastHostStart = 0;
	uint32_t now = millis();
	if (now - lastHostStart < 2000) {
		LV_LOG_WARN("[HOST] Throttling host start, waiting...\n");
		return;
	}
	lastHostStart = now;
	
	sessionManager->startHosting(sessionId);

	// Initialize the UI state 
	extern void updateHostPlayerList();
	extern void checkHostStartButtonState();
	updateHostPlayerList();
	checkHostStartButtonState();

	LV_LOG_INFO("[HOST] Started hosting session: %u\n", sessionId);
}

// Join screen loading event - initialize join state
void joinScreenLoaded(lv_event_t* e)
{
	if (!sessionManager) return;

	// Reset session state when entering join screen after game completion
	// This ensures clean state if coming from a game
	if (sessionManager->getMyStatus() == InGame) {
		LV_LOG_INFO("[JOIN] Resetting session state after game completion\n");
		sessionManager->leaveSession();
	}

	// Update the join roller with current hosts
	updateJoinRoller();

	// Reset ready state image to match session state
	if (objects.img_join_readystate)
	{
		// Set image based on actual session state
		if (sessionManager->isReady()) {
			lv_image_set_src(objects.img_join_readystate, &img_ui___ready);
		} else {
			lv_image_set_src(objects.img_join_readystate, &img_ui___not_ready);
		}
	}

	// Properly evaluate ready button state based on current conditions
	setMissionReadyState(e);

	LV_LOG_INFO("[JOIN] Join screen loaded - status: %d\n", sessionManager->getMyStatus());
}

// Screen unloading event - cleanup session state when leaving host/join screens  
void screenUnloaded(lv_event_t* e)
{
	if (!sessionManager) return;

	lv_obj_t* screen = (lv_obj_t*)lv_event_get_target(e);

	if (screen == objects.host)
	{
		// Only stop hosting if we're not in the middle of an active game
		// If we're InGame, we're likely going to briefing/game screen and should maintain session
		if (sessionManager->getMyStatus() != InGame)
		{
			// Leaving host screen - stop hosting
			sessionManager->stopHosting();
			LV_LOG_INFO("[HOST] Stopped hosting (left screen)\n");
		}
		else
		{
			LV_LOG_INFO("[HOST] Left host screen but staying InGame - maintaining session\n");
		}
	}
	else if (screen == objects.join)
	{
		// Leaving join screen - leave any current session
		if (sessionManager->isInLobby())
		{
			// Send leave message to host
			uint32_t targetHost = sessionManager->getTargetHost();
			if (targetHost != 0)
			{
				String leavePacket = createJSONSessionPacket(sessionManager->getMySessionId(), Idle, Request);
				extern painlessMesh mesh;
				mesh.sendSingle(targetHost, leavePacket);
				LV_LOG_INFO("[JOIN] Sent leave message to host %u\n", targetHost);
			}
			sessionManager->leaveSession();
		}
		LV_LOG_INFO("[JOIN] Left join screen\n");
	}
}

// Join roller selection changed - join/leave sessions based on selection
void joinRollerChanged(lv_event_t* e)
{
	LV_LOG_INFO("[JOIN] roller changed\n");
	if (!sessionManager) return;

	char selectedHost[64];
	lv_roller_get_selected_str(objects.roller_join_games, selectedHost, sizeof(selectedHost));

	LV_LOG_INFO("[JOIN] Selected: '%s', current status: %d, target: %u\n", 
	             selectedHost, sessionManager->getMyStatus(), sessionManager->getTargetHost());

	// If "No Crews Found" or empty, leave current session ONLY if user manually selected it
	// Don't leave if this was triggered by an automatic UI update
	if (strcmp(selectedHost, "No Crews Found") == 0 || strlen(selectedHost) == 0)
	{
		// Only leave session if this was a user-initiated change (e != nullptr)
		if (e != nullptr && sessionManager->isInLobby())
		{
			LV_LOG_INFO("[JOIN] User selected 'No Crews Found', leaving session\n");
			sessionManager->leaveSession();
			
			// Reset ready state image to not ready
			if (objects.img_join_readystate)
			{
				lv_image_set_src(objects.img_join_readystate, &img_ui___not_ready);
			}
		}

		setMissionReadyState(e); // Update button states
		return;
	}

	// Find the host node ID based on selected name
	PsramVector<uint32_t> hosts = sessionManager->getActiveHosts();
	uint32_t selectedIndex = lv_roller_get_selected(objects.roller_join_games);

	if (selectedIndex < hosts.size())
	{
		uint32_t hostNodeId = hosts[selectedIndex];
		LV_LOG_INFO("[JOIN] Session selected: %u, my status: %d\n", hostNodeId, sessionManager->getMyStatus());

		// Leave current session if we're in a different one
		if (sessionManager->isInLobby() && sessionManager->getTargetHost() != hostNodeId)
		{
			sessionManager->leaveSession();

			// Reset ready state when switching hosts
			if (objects.img_join_readystate)
			{
				lv_image_set_src(objects.img_join_readystate, &img_ui___not_ready);
			}
		}

		// Join the new session
		if (!sessionManager->isInLobby())
		{
			sessionManager->joinSession(hostNodeId); // hostNodeId is also the sessionId
			LV_LOG_INFO("[JOIN] Joined session hosted by %u, new status: %d\n", hostNodeId, sessionManager->getMyStatus());
		}
		else
		{
			LV_LOG_INFO("[JOIN] Already in lobby for host %u, current status: %d\n", hostNodeId, sessionManager->getMyStatus());
		}
	}
	else
	{
		// Selected index is out of bounds (stale selection after host list update)
		LV_LOG_WARN("[JOIN] Selected host index %u is out of bounds (only %zu hosts available)\n",
			selectedIndex, hosts.size());

		// Leave current session since selection is invalid
		if (sessionManager->isInLobby())
		{
			sessionManager->leaveSession();
		}

		// Reset ready state image
		if (objects.img_join_readystate)
		{
			lv_image_set_src(objects.img_join_readystate, &img_ui___not_ready);
		}
	}

	setMissionReadyState(e); // Update button states
}

// Enhanced join ready button click - toggle ready state and update UI
void joinReadyClicked(lv_event_t* e)
{
	if (!sessionManager)
	{
		LV_LOG_WARN("[JOIN] Ready clicked but sessionManager is null\n");
		return;
	}

	PlayerStatus currentStatus = sessionManager->getMyStatus();
	LV_LOG_INFO("[JOIN] Ready clicked - current status: %d, isInLobby: %s\n",
		currentStatus, sessionManager->isInLobby() ? "true" : "false");

	if (!sessionManager->isInLobby())
	{
		LV_LOG_WARN("[JOIN] Ready clicked but not in lobby\n");
		return;
	}

	play_random_beep();

	bool currentlyReady = sessionManager->isReady();
	sessionManager->setReadyStatus(!currentlyReady);

	// Update UI image
	if (objects.img_join_readystate)
	{
		if (sessionManager->isReady())
		{
			lv_image_set_src(objects.img_join_readystate, &img_ui___ready);
			LV_LOG_INFO("[JOIN] Status changed to Ready\n");
		}
		else
		{
			lv_image_set_src(objects.img_join_readystate, &img_ui___not_ready);
			LV_LOG_INFO("[JOIN] Status changed to InLobby\n");
		}
	}
	
	// Send immediate status update to host instead of waiting for timer
	uint32_t targetHost = sessionManager->getTargetHost();
	uint32_t sessionId = sessionManager->getMySessionId();
	if (targetHost != 0 && sessionId != 0) {
		extern painlessMesh mesh;
		String statusPacket = createJSONSessionPacket(sessionId, sessionManager->getMyStatus(), Request);
		if (!statusPacket.isEmpty()) {
			bool sent = mesh.sendSingle(targetHost, statusPacket);
			LV_LOG_INFO("[JOIN] Sent immediate ready status update to host %u: %s\n", 
			             targetHost, sent ? "SUCCESS" : "FAILED");
		}
	}
}

void hostGameChanged(lv_event_t* e)
{
	play_random_beep();
}

void joinGameChanged(lv_event_t* e)
{
	play_random_beep();
	
	// If user selects 'Select' (index 0) while ready, clear ready state
	if (sessionManager && sessionManager->isReady()) {
		int32_t selectedGameIndex = lv_dropdown_get_selected(objects.ddl_join_games);
		if (selectedGameIndex <= 0) {
			LV_LOG_INFO("[JOIN] Game changed to 'Select', clearing ready state\n");
			sessionManager->setMyStatus(InLobby); // Clear ready state
			
			// Update ready state image immediately
			if (objects.img_join_readystate) {
				lv_image_set_src(objects.img_join_readystate, &img_ui___not_ready);
			}
			
			// Send immediate status update to host
			uint32_t targetHost = sessionManager->getTargetHost();
			if (targetHost != 0) {
				uint32_t sessionId = sessionManager->getMySessionId();
				extern painlessMesh mesh;
				String statusPacket = createJSONSessionPacket(sessionId, sessionManager->getMyStatus(), Request);
				if (!statusPacket.isEmpty()) {
					bool sent = mesh.sendSingle(targetHost, statusPacket);
					LV_LOG_INFO("[JOIN] Sent ready state clear to host %u: %s\n", 
					             targetHost, sent ? "SUCCESS" : "FAILED");
				}
			}
		}
	}
}

// Host start button click - begin the game
void hostStartClicked(lv_event_t* e)
{
	if (!sessionManager || !sessionManager->isHosting())
	{
		LV_LOG_WARN("[HOST] Start clicked but not hosting\n");
		return;
	}

	// Get selected game from dropdown (1-indexed)
	int32_t selectedGameIndex = lv_dropdown_get_selected(objects.ddl_host_games);
	if (selectedGameIndex <= 0) {
		LV_LOG_WARN("[HOST] No game selected, cannot start\n");
		return;
	}

	play_random_beep();

	LV_LOG_INFO("[HOST] Starting game %d\n", selectedGameIndex);

	// Get all lobby players for game initialization
	PsramVector<LobbyPlayer> lobbyPlayers = sessionManager->getLobbyPlayers();
	
	LV_LOG_INFO("[HOST] Starting time-synchronized game %d\n", selectedGameIndex);

	// Check if host is playing solo (no lobby players)
	bool isSolo = lobbyPlayers.empty();
	
	if (isSolo) {
		// Solo mode: start briefing immediately with no countdown
		LV_LOG_INFO("[HOST] NEW CODE: Solo mode - starting briefing immediately\n");
		// Set status to InGame before changing screens
		sessionManager->setMyStatus(InGame);
		extern void showPreGameBriefing(int gameNumber, int countdownSeconds);
		showPreGameBriefing(selectedGameIndex, 15); // Give solo players 15 seconds to read the briefing
	} else {
		// Multiplayer mode: start timed game BEFORE changing status, then start briefing
		LV_LOG_INFO("[HOST] NEW CODE: Multiplayer mode - starting briefing now, syncing game start for %zu players\n", lobbyPlayers.size());
		
		// Calculate when games should start (30 seconds from now to allow for briefing + mesh stability)
		uint32_t gameStartDelay = 30; // 30 seconds for briefing + buffer + mesh stability
		sessionManager->startTimedGame(selectedGameIndex, gameStartDelay);
		
		// Now set status to InGame AFTER starting timed game but BEFORE changing screens
		sessionManager->setMyStatus(InGame);
		
		// Start host briefing immediately with countdown to game start
		extern void showPreGameBriefing(int gameNumber, int countdownSeconds);
		showPreGameBriefing(selectedGameIndex, gameStartDelay);
		
		LV_LOG_INFO("[HOST] Host briefing started, game will start in %u seconds\n", gameStartDelay);
	}
	
	// NOTE: Don't set status to Idle here! Game will handle session cleanup when it ends.
	// sessionManager->setMyStatus(Idle); // REMOVED - this was breaking sessions immediately
	LV_LOG_INFO("[HOST] Game session active - status remains InGame until completion\n");
	return;
}
