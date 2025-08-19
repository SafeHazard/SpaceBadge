#include "session.h"
#include "contacts.h"
#include "custom.h"
#include "mesh.h"
#include "convos.h"
#include "./src/ui/screens.h"
#include "./src/ui/ui.h"
#include "./src/ui/images.h"
#include "./src/ui/fonts.h"
#include <lvgl.h>
#include <painlessMesh.h>
#include <vector>
#include <algorithm>

#include "game_parent.h"

// Utility functions for enum-to-string conversion
const char* playerStatusToString(PlayerStatus status) {
	switch (status) {
		case Idle: return "Idle";
		case Hosting: return "Hosting";
		case InLobby: return "InLobby";
		case Ready: return "Ready";
		case InGame: return "InGame";
		case Offline: return "Offline";
		default: return "Unknown";
	}
}

const char* displayOptionsToString(DisplayOptions option) {
	switch (option) {
		case Everyone: return "Everyone";
		case NotBlocked: return "NotBlocked";
		case Crew: return "Crew";
		case None: return "None";
		default: return "Unknown";
	}
}
#include "g_game1.h"
#include "g_game2.h"
#include "g_game3.h"
#include "g_game4.h"
#include "g_game5.h"
#include "g_game6.h"
#include "multiplayer_overlay.h"
#include "convos.h"

// Global session manager instance
SessionManager* sessionManager = nullptr;

// Forward declaration
void processInGameScoreUpdate(const SessionPacket& packet, uint32_t fromNodeId);

extern painlessMesh mesh;
extern Config config;
extern ContactManager* scanResults;
extern GameState gameState;

int32_t earnedXP = 0;
extern game_parent* game;

SessionManager::SessionManager()
{
	currentSessionId = 0;
	myStatus = Idle;
	gameState.myStatus = Idle; // Initialize both in constructor
	targetHost = 0;
	// timedStart is initialized by its own constructor
}

void SessionManager::addOrUpdateSession(const SessionPacket& packet, uint32_t fromNodeId, ContactData* contact)
{
	uint32_t sessionId = packet.sessionID;

	LV_LOG_INFO("[SESSION] Processing packet: sessionId=%u, fromNodeId=%u, status=%s, request=%d (myStatus=%s, currentSessionId=%u)\n",
		sessionId, fromNodeId, playerStatusToString(packet.status), packet.request, 
		playerStatusToString(myStatus), currentSessionId);

	// Update session info
	SessionInfo& info = activeSessions[sessionId];
	info.packet = packet;
	info.lastSeen = millis();
	info.contactData = contact;

	// Track hosting players - add them when they send hosting-related packets
	// Keep hosts in list even when InGame to prevent "No Crews Found" during games
	bool isHostingPacket = ((packet.status == Hosting && 
		(packet.request == HostAdvertising || packet.request == Accept || packet.request == Reject)) ||
		(packet.status == InGame && packet.request == TimedGameStart));

	if (isHostingPacket)
	{
		// Player is hosting (advertising, accepting, or rejecting players)
		auto it = std::find(hostingPlayers.begin(), hostingPlayers.end(), fromNodeId);
		if (it == hostingPlayers.end())
		{
			hostingPlayers.push_back(fromNodeId);
			LV_LOG_INFO("[SESSION] Added hosting player: %u (session: %u, request: %d)\n", fromNodeId, sessionId, packet.request);

			// Update join roller when we add a new host, and throttle it
			static uint32_t lastNewHostRollerUpdate = 0;
			if (lv_scr_act() == objects.join && (millis() - lastNewHostRollerUpdate > 2000))
			{
				updateJoinRoller();
				lastNewHostRollerUpdate = millis();
			}
		}
		else
		{
			// Host already known, just update the session info timestamp (already done above)
			LV_LOG_INFO("[SESSION] Updated known hosting player: %u (session: %u, request: %d)\n", fromNodeId, sessionId, packet.request);
		}
	}
	else
	{
		// Only remove from hosting list if they're truly no longer hosting
		// Don't remove hosts that are InGame - they might still be running a session
		if (packet.status != InGame) {
			auto it = std::find(hostingPlayers.begin(), hostingPlayers.end(), fromNodeId);
			if (it != hostingPlayers.end())
			{
				hostingPlayers.erase(it);
				LV_LOG_INFO("[SESSION] Removed hosting player: %u (status: %d)\n", fromNodeId, packet.status);

				// Update join roller if a host stopped hosting, but throttle updates
				static uint32_t lastRollerUpdate = 0;
				if (lv_scr_act() == objects.join && (millis() - lastRollerUpdate > 3000))
				{
					updateJoinRoller();
					lastRollerUpdate = millis();
				}
			}
		} else {
			LV_LOG_INFO("[SESSION] Keeping host %u in list despite InGame status (active session)\n", fromNodeId);
		}
	}

	// Handle timed game start signals from host
	if (packet.request == TimedGameStart) {
		LV_LOG_INFO("[DEBUG] TimedGameStart packet check: request=%d, sessionId=%u, currentSessionId=%u, fromNodeId=%u, targetHost=%u\n",
		             packet.request, sessionId, currentSessionId, fromNodeId, targetHost);
	}
	
	if (packet.request == TimedGameStart && sessionId == currentSessionId && fromNodeId == targetHost)
	{
		//LV_LOG_ERROR("[ERROR] TimedGameStart packet reached old addOrUpdateSession! Should use JSON overload instead.\n");
		LV_LOG_ERROR("[ERROR] This means the mesh handler routing is broken. Check mesh.cpp TimedGameStart routing.\n");
		return;  // Don't process with old function
	}
	

	// Handle legacy game start packets (InGame status from host with HostAdvertising)
	if (packet.status == InGame && packet.request == HostAdvertising && sessionId == currentSessionId && fromNodeId == targetHost)
	{
		LV_LOG_INFO("[GAME] Received legacy game start signal from host %u\n", fromNodeId);

		// Set our status to InGame
		setMyStatus(InGame);
		
		// Set player count for legacy packets (estimate from session)
		gameState.playerCount = packet.playerCount > 0 ? packet.playerCount : 2; // Default to 2 if not provided
		LV_LOG_INFO("[GAME] Legacy game start: set playerCount=%d\n", gameState.playerCount);

		// Each player starts their individually selected game
		// Get the game selection from our join dropdown
		extern objects_t objects;
		int32_t selectedGameIndex = 0;
		if (objects.ddl_join_games)
		{
			selectedGameIndex = lv_dropdown_get_selected(objects.ddl_join_games);
		}

		if (selectedGameIndex <= 0)
		{
			LV_LOG_INFO("[GAME] No game selected, defaulting to game 1\n");
			selectedGameIndex = 1;
		}

		LV_LOG_INFO("[GAME] Starting pre-game briefing for game %d\n", selectedGameIndex);

		// Clients always use the multiplayer countdown (solo mode only applies to hosts)
		// Show pre-game briefing with countdown (30 seconds for testing, 120 for production)
		extern void showPreGameBriefing(int gameNumber, int countdownSeconds);
		showPreGameBriefing(selectedGameIndex, 30); // 30 seconds countdown for multiplayer clients
		
		// NOTE: Don't set status to Idle here! Player should remain InGame during the session.
		// Status will be reset properly when the game actually ends.
		LV_LOG_INFO("[SESSION] Player remains InGame status during briefing and game\n");
		return;
	}

	// Handle in-game score updates
	if (packet.status == InGame && gameState.myStatus == InGame)
	{
		processInGameScoreUpdate(packet, fromNodeId);
		return; // Don't process further when in game
	}

	// Handle join requests if we're hosting
	if (myStatus == Hosting && sessionId == currentSessionId)
	{
		LV_LOG_INFO("[HOST] Handling join request: myStatus=Hosting, sessionId=%u matches currentSessionId=%u\n",
			sessionId, currentSessionId);
		handleJoinRequest(packet, fromNodeId, contact);
	}
	else if (myStatus == Hosting)
	{
		LV_LOG_WARN("[HOST] Ignoring join request: sessionId=%u != currentSessionId=%u\n", sessionId, currentSessionId);
	}

	// Handle acceptance packets
	if (packet.request == Accept && sessionId == currentSessionId && fromNodeId == targetHost)
	{
		LV_LOG_INFO("[JOIN] SUCCESS! Received acceptance from host %u - confirmed in lobby (myStatus=%s)\n", fromNodeId, playerStatusToString(myStatus));
		// We're already in InLobby status, so just confirm we're properly connected
	}
	else if (packet.request == Accept)
	{
		LV_LOG_WARN("[JOIN] Received acceptance but conditions not met: sessionId=%u vs %u, fromNodeId=%u vs targetHost=%u\n",
			sessionId, currentSessionId, fromNodeId, targetHost);
	}

	// Handle rejection packets
	if (packet.request == Reject)
	{
		if (sessionId == currentSessionId)
		{
			LV_LOG_INFO("[SESSION] Received rejection from host %u\n", fromNodeId);
			leaveSession(); // Leave current session attempt

			// Reset ready state UI immediately when rejected/kicked
			if (objects.img_join_readystate)
			{
				lv_image_set_src(objects.img_join_readystate, &img_ui___not_ready);
				LV_LOG_INFO("[SESSION] Reset ready state UI due to rejection\n");
			}

			// Also update the ready button state (disable it)
			extern void setMissionReadyState(lv_event_t * e);
			setMissionReadyState(nullptr);
		}

		// Remove the host from our hosting list (for both regular rejections and disconnections)
		auto it = std::find(hostingPlayers.begin(), hostingPlayers.end(), fromNodeId);
		if (it != hostingPlayers.end())
		{
			hostingPlayers.erase(it);
			LV_LOG_INFO("[SESSION] Removed host %u from hosting list due to rejection/disconnection\n", fromNodeId);

			// Update join roller if we're on the join screen
			extern int32_t int_contacts_tab_current;
			if (lv_scr_act() == objects.join)
			{
				updateJoinRoller();

				// If this was our selected host, reassess ready state
				if (fromNodeId == targetHost)
				{
					// Force a roller change event to reassess ready state
					extern void joinRollerChanged(lv_event_t * e);
					joinRollerChanged(nullptr); // Pass nullptr since we don't need the event data
				}
			}
		}
	}

}

// JSON overload for addOrUpdateSession - handles TimedGameStart with player_ids array
void SessionManager::addOrUpdateSession(const JsonDocument& doc, uint32_t fromNodeId, ContactData* contact) {
	//LV_LOG_ERROR("[JSON_OVERLOAD] addOrUpdateSession JSON overload called from %u\n", fromNodeId);
	
	// Parse the session packet normally first
	SessionPacket packet = parseJSONSessionPacket(doc);
	//LV_LOG_ERROR("[JSON_OVERLOAD] Parsed packet: request=%d, sessionID=%u, currentSessionId=%u, targetHost=%u\n", 
	//             packet.request, packet.sessionID, currentSessionId, targetHost);
	
	// Check if this is a TimedGameStart packet that needs special handling
	if (packet.request == TimedGameStart && packet.sessionID == currentSessionId && fromNodeId == targetHost) {
		// Use the new JSON-aware processTimedGameStart function
		//LV_LOG_ERROR("[JSON_OVERLOAD] MATCH! Calling JSON processTimedGameStart for host %u\n", fromNodeId);
		processTimedGameStart(doc, fromNodeId);
		
		// Still update the session info normally for consistency
		uint32_t sessionId = packet.sessionID;
		SessionInfo& info = activeSessions[sessionId];
		info.packet = packet;
		info.lastSeen = millis();
		info.contactData = contact;
		
		return;
	}
	
	// For all other packets, use the normal addOrUpdateSession function
	addOrUpdateSession(packet, fromNodeId, contact);
}

void SessionManager::removeStaleHostingSessions(uint32_t maxAgeMs)
{
	uint32_t now = millis();
	auto it = activeSessions.begin();
	bool hostsRemoved = false;
	uint32_t removedTargetHost = 0;

	while (it != activeSessions.end())
	{
		uint32_t sessionId = it->first;
		uint32_t ageMs = now - it->second.lastSeen;

		// Never remove our current target host while we're actively in a session with them
		// Give extra grace period to avoid disconnecting players from their selected host
		bool isMyTargetHost = (sessionId == targetHost && (myStatus == InLobby || myStatus == Ready));
		uint32_t effectiveTimeout = isMyTargetHost ? (maxAgeMs * 2) : maxAgeMs; // Double timeout for target host

		if (ageMs > effectiveTimeout)
		{
			LV_LOG_INFO("[SESSION] Removing stale session: %u (last seen %u ms ago, timeout: %u ms, isTarget: %s)\n",
				sessionId, ageMs, effectiveTimeout, isMyTargetHost ? "YES" : "no");

			// Check if this was our target host
			if (sessionId == targetHost)
			{
				removedTargetHost = sessionId;
				LV_LOG_INFO("[SESSION] Our target host %u went stale, will need to un-ready\n", sessionId);
			}

			// Remove from hosting players list
			auto hostIt = std::find(hostingPlayers.begin(), hostingPlayers.end(), sessionId);
			if (hostIt != hostingPlayers.end())
			{
				hostingPlayers.erase(hostIt);
				hostsRemoved = true;
			}

			it = activeSessions.erase(it);
		}
		else
		{
			++it;
		}
	}

	// If hosts were removed and we're on the join screen, update the UI
	if (hostsRemoved && lv_scr_act() == objects.join)
	{
		extern void updateJoinRoller();
		updateJoinRoller();
		LV_LOG_INFO("[SESSION] Updated join roller due to stale host removal\n");

		// If our target host was removed, handle un-readying
		if (removedTargetHost != 0)
		{
			handleTargetHostDisconnected(removedTargetHost);
		}
	}
}

PsramVector<uint32_t> SessionManager::getActiveHosts()
{
	// Clean up hosts even less aggressively to prevent roller flicker
	// Only clean up every 15 seconds, and use much longer timeout if player is in a session
	static uint32_t lastCleanup = 0;
	uint32_t now = millis();
	if (now - lastCleanup > 15000)
	{
		// With 5-second unicast heartbeat, we can use shorter timeout for faster host disconnect detection
		uint32_t timeout = (myStatus == InLobby || myStatus == Ready) ? 30000 : 45000; // 30s if in session, 45s otherwise

		LV_LOG_INFO("[SESSION] Checking for stale hosts (timeout: %u ms, myStatus: %s)\n", timeout, playerStatusToString(myStatus));
		removeStaleHostingSessions(timeout);
		lastCleanup = now;
	}
	return hostingPlayers;
}

SessionInfo* SessionManager::getSessionInfo(uint32_t sessionId)
{
	auto it = activeSessions.find(sessionId);
	return (it != activeSessions.end()) ? &it->second : nullptr;
}

void SessionManager::startHosting(uint32_t sessionId)
{
	currentSessionId = sessionId;
	setMyStatus(Hosting);
	targetHost = 0;
	LV_LOG_INFO("[SESSION] Started hosting session: %u\n", sessionId);
	
	// Immediately broadcast ID packet when we start hosting so other boards can see our display name
	extern painlessMesh mesh;
	extern bool meshInitialized;
	if (meshInitialized) {
		String idPacketJson = createJSONStringIDPacket();
		if (!idPacketJson.isEmpty()) {
			bool sent = mesh.sendBroadcast(idPacketJson, false);
			LV_LOG_INFO("[SESSION] Host ID broadcast: %s - %s\n", sent ? "SUCCESS" : "FAILED", idPacketJson.c_str());
		}
	}
}

void SessionManager::stopHosting()
{
	if (myStatus == Hosting)
	{
		LV_LOG_INFO("[SESSION] Stopped hosting session: %u\n", currentSessionId);

		// Notify all lobby players that host is disconnecting
		notifyPlayersHostDisconnected();
	}

	// Clear timed start state
	timedStart.active = false;
	
	// Clear waiting message from UI
	extern objects_t objects;
	if (objects.lbl_host_random) {
		lv_label_set_text(objects.lbl_host_random, "");
	}

	// Clear lobby players
	lobbyPlayers.clear();
	currentSessionId = 0;
	setMyStatus(Idle);
	targetHost = 0;
}

void SessionManager::notifyPlayersHostDisconnected()
{
	if (myStatus != Hosting || lobbyPlayers.empty())
	{
		return;
	}

	extern painlessMesh mesh;

	// Send disconnection packet to all lobby players: (hostId, Idle, Reject)
	String disconnectPacket = createJSONSessionPacket(currentSessionId, Idle, Reject);

	for (const auto& player : lobbyPlayers)
	{
		bool sent = mesh.sendSingle(player.nodeId, disconnectPacket);
		if (sent)
		{
			LV_LOG_INFO("[HOST] Sent disconnect notification to player %u\n", player.nodeId);
		}
		else
		{
			LV_LOG_WARN("[HOST] Failed to send disconnect notification to player %u\n", player.nodeId);
		}
	}

	LV_LOG_INFO("[HOST] Notified %zu players of host disconnection\n", lobbyPlayers.size());
}

void SessionManager::joinSession(uint32_t hostSessionId)
{
	currentSessionId = hostSessionId;
	setMyStatus(InLobby);
	targetHost = hostSessionId; // hostSessionId is also the host's nodeId
	LV_LOG_INFO("[SESSION] Joining session: %u (host: %u)\n", hostSessionId, targetHost);

	// Send immediate join request instead of waiting for timer
	extern painlessMesh mesh;
	String joinPacket = createJSONSessionPacket(currentSessionId, myStatus, Request);
	if (!joinPacket.isEmpty())
	{
		bool sent = mesh.sendSingle(targetHost, joinPacket);
		LV_LOG_INFO("[SESSION] Sent immediate join request to %u: %s (JSON: %s)\n",
			targetHost, sent ? "SUCCESS" : "FAILED", joinPacket.c_str());
	}
}

void SessionManager::leaveSession()
{
	if (myStatus == InLobby || myStatus == Ready)
	{
		LV_LOG_INFO("[SESSION] Leaving session: %u (host: %u)\n", currentSessionId, targetHost);

		// Send farewell packet to the host we're leaving
		if (targetHost != 0)
		{
			extern painlessMesh mesh;
			String farewellPacket = createJSONSessionPacket(currentSessionId, Idle, Request);
			if (!farewellPacket.isEmpty())
			{
				bool sent = mesh.sendSingle(targetHost, farewellPacket);
				LV_LOG_INFO("[SESSION] Sent farewell packet to host %u: %s (JSON: %s)\n",
					targetHost, sent ? "SUCCESS" : "FAILED", farewellPacket.c_str());
			}
		}
	}
	currentSessionId = 0;
	setMyStatus(Idle);
	targetHost = 0;
}

void SessionManager::setMyStatus(PlayerStatus status)
{
	myStatus = status;
	gameState.myStatus = status;

	const char* statusNames[] = { "Idle", "Hosting", "InLobby", "Ready", "InGame" };
	const char* statusName = (status >= 0 && status <= 4) ? statusNames[status] : "Unknown";
	LV_LOG_INFO("[SESSION] Status synchronized: %s (sessionManager and gameState)\n", statusName);
}

void SessionManager::setReadyStatus(bool ready)
{
	if (myStatus == InLobby || myStatus == Ready)
	{
		PlayerStatus newStatus = ready ? Ready : InLobby;
		setMyStatus(newStatus); // Use the synchronized function
		LV_LOG_INFO("[SESSION] Ready status changed to: %s\n", ready ? "Ready" : "InLobby");
	}
}

void SessionManager::handleJoinRequest(const SessionPacket& packet, uint32_t fromNodeId, ContactData* contact)
{
	extern painlessMesh mesh;

	LV_LOG_INFO("[HOST] Processing join request from %u: sessionID=%u, status=%s, request=%d (mySessionId=%u)\n",
		fromNodeId, packet.sessionID, playerStatusToString(packet.status), packet.request, currentSessionId);

	// If lobby is full, send rejection
	if (isLobbyFull())
	{
		String rejectPacket = createJSONSessionPacket(currentSessionId, Hosting, Reject);
		mesh.sendSingle(fromNodeId, rejectPacket);
		LV_LOG_INFO("[HOST] Rejected join request from %u - lobby full\n", fromNodeId);
		return;
	}

	// Handle different request types
	if (packet.request == Request)
	{
		// Player wants to join or is updating their status
		if (packet.status == InLobby || packet.status == Ready)
		{
			// Find existing player or add new one
			auto it = std::find_if(lobbyPlayers.begin(), lobbyPlayers.end(),
				[fromNodeId](const LobbyPlayer& p) { return p.nodeId == fromNodeId; });

			if (it != lobbyPlayers.end())
			{
				// Update existing player
				PlayerStatus oldStatus = it->status;
				it->status = packet.status;
				it->lastSeen = millis();
				it->contactData = contact;
				LV_LOG_INFO("[HOST] Updated player %u status: %d -> %d\n", fromNodeId, oldStatus, packet.status);
			}
			else
			{
				// Add new player
				LobbyPlayer player = { fromNodeId, contact, packet.status, millis() };
				lobbyPlayers.push_back(player);
				LV_LOG_INFO("[HOST] Added player %u to lobby (status: %d)\n", fromNodeId, packet.status);
			}

			// Send acceptance
			String acceptPacket = createJSONSessionPacket(currentSessionId, Hosting, Accept);
			bool sent = mesh.sendSingle(fromNodeId, acceptPacket);
			LV_LOG_INFO("[HOST] Sent acceptance to %u: %s\n", fromNodeId, sent ? "SUCCESS" : "FAILED");

			// Update host UI
			updateHostPlayerList();
			checkHostStartButtonState();
		}
		else if (packet.status == Idle)
		{
			// Player is leaving
			removeLobbyPlayer(fromNodeId);
			LV_LOG_INFO("[HOST] Player %u left the lobby\n", fromNodeId);
		}
	}
}

void SessionManager::removeLobbyPlayer(uint32_t nodeId)
{
	auto it = std::find_if(lobbyPlayers.begin(), lobbyPlayers.end(),
		[nodeId](const LobbyPlayer& p) { return p.nodeId == nodeId; });

	if (it != lobbyPlayers.end())
	{
		lobbyPlayers.erase(it);
		LV_LOG_INFO("[HOST] Removed player %u from lobby\n", nodeId);

		// Update the UI to reflect the removal
		if (lv_scr_act() == objects.host)
		{
			updateHostPlayerList();
			// Also check if we need to disable the start button
			checkHostStartButtonState();
		}
	}
}

void SessionManager::removeStalePlayersSessions(uint32_t maxAgeMs)
{
	uint32_t now = millis();
	auto it = lobbyPlayers.begin();
	bool playersRemoved = false;

	while (it != lobbyPlayers.end())
	{
		if (now - it->lastSeen > maxAgeMs)
		{
			LV_LOG_INFO("[HOST] Removing stale player: %u\n", it->nodeId);
			it = lobbyPlayers.erase(it);
			playersRemoved = true;
		}
		else
		{
			++it;
		}
	}

	// Update UI if players were removed and we're on the host screen
	if (playersRemoved && lv_scr_act() == objects.host)
	{
		updateHostPlayerList();
		checkHostStartButtonState();
		LV_LOG_INFO("[HOST] Updated host UI after removing stale players\n");
	}

	// Also clean up hosting sessions
	removeStaleHostingSessions(maxAgeMs);
}

// Start time-synchronized game (for hosts)
void SessionManager::startTimedGame(int gameIndex, uint32_t delaySeconds) {
	LV_LOG_INFO("[HOST] NEW CODE: startTimedGame called with game %d, delay %u seconds\n", gameIndex, delaySeconds);
	
	if (!isHosting()) {
		LV_LOG_WARN("[HOST] Cannot start timed game - not hosting\n");
		return;
	}

	extern painlessMesh mesh;
	
	// Calculate start time: current mesh time + delay
	uint32_t currentMeshTime = mesh.getNodeTime();
	uint32_t gameStartTime = currentMeshTime + (delaySeconds * 1000000); // Convert seconds to microseconds
	
	// Initialize timed start tracking
	timedStart.gameIndex = gameIndex;
	timedStart.startTime = gameStartTime;
	timedStart.broadcastStart = millis();
	timedStart.active = true;
	
	LV_LOG_INFO("[HOST] Starting timed game %d in %u seconds (mesh time: %u -> %u)\n", 
	             gameIndex, delaySeconds, currentMeshTime, gameStartTime);

	// Send initial timed game start packets to all players
	String timedPacket = createJSONTimedGameStartPacket(currentSessionId, InGame, gameStartTime);
	
	if (lobbyPlayers.empty()) {
		// Solo mode - just schedule our own start
		LV_LOG_INFO("[HOST] Solo mode - only host will start at scheduled time\n");
	} else {
		// Multiplayer - send to all lobby players
		for (const auto& player : lobbyPlayers) {
			bool sent = mesh.sendSingle(player.nodeId, timedPacket);
			LV_LOG_INFO("[HOST] Sent timed game start to player %u: %s\n", 
			             player.nodeId, sent ? "SUCCESS" : "FAILED");
		}
	}
}

// Process timed game start packet (for clients)
void SessionManager::processTimedGameStart(const SessionPacket& packet, uint32_t fromNodeId) {
	LV_LOG_INFO("[CLIENT] processTimedGameStart: fromNodeId=%u, targetHost=%u, packet.sessionID=%u, currentSessionId=%u\n",
	             fromNodeId, targetHost, packet.sessionID, currentSessionId);
	
	// Only accept from our target host
	if (fromNodeId != targetHost || packet.sessionID != currentSessionId) {
		LV_LOG_INFO("[CLIENT] Ignoring timed game start from %u (expected host %u, expected session %u)\n", 
		             fromNodeId, targetHost, currentSessionId);
		return;
	}
	
	LV_LOG_INFO("[CLIENT] Received timed game start from host %u, start time: %u\n", 
	             fromNodeId, packet.startTime);
	
	// Set up our own timed start tracking
	extern painlessMesh mesh;
	uint32_t currentMeshTime = mesh.getNodeTime();
	
	// Get client's selected game from dropdown
	extern objects_t objects;
	int selectedGameIndex = 0;
	if (objects.ddl_join_games) {
		selectedGameIndex = lv_dropdown_get_selected(objects.ddl_join_games);
	}
	if (selectedGameIndex <= 0) selectedGameIndex = 1; // Default to game 1
	
	timedStart.gameIndex = selectedGameIndex;
	timedStart.startTime = packet.startTime;
	timedStart.broadcastStart = millis();
	timedStart.active = true;
	
	uint32_t delaySeconds = (packet.startTime - currentMeshTime) / 1000000; // Convert to seconds
	LV_LOG_INFO("[CLIENT] Game %d will start in %u seconds (mesh time %u -> %u)\n", 
	             selectedGameIndex, delaySeconds, currentMeshTime, packet.startTime);
	
	// Set our status to InGame immediately 
	setMyStatus(InGame);
	
	// CRITICAL: Set player count BEFORE starting briefing (which eventually calls startGameAfterBriefing)
	gameState.playerCount = packet.playerCount;
	LV_LOG_INFO("[CLIENT] Timed game start: playerCount = %d (BEFORE briefing)\n", packet.playerCount);
	
	// CRITICAL: Set the preserved player count for the startGameAfterBriefing function
	extern int preservedPlayerCount;
	preservedPlayerCount = packet.playerCount;
	
	// CRITICAL: Initialize playerIDs structure for clients
	// Clients will establish the complete structure when they receive score packets from all players
	//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Clearing playerIDs structure - will be built from score packets\n");
	memset(gameState.playerIDs, 0, sizeof(gameState.playerIDs));
	
	// Initialize with the host (sender of this packet) in slot 0
	extern Config config;
	gameState.playerIDs[0] = fromNodeId; // Host
	//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Set host (playerIDs[0]) = %u\n", fromNodeId);
	
	// Put ourselves in slot 1 as a starting point - this will be corrected when score packets are exchanged
	gameState.playerIDs[1] = config.user.nodeId; // Self
	//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Set self (playerIDs[1]) = %u\n", config.user.nodeId);
	
	// Other players will be added to the structure as we receive their score packets
	//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Basic structure established - other players will be added via score packets\n");

	// Show client briefing screen with countdown to game start
	LV_LOG_INFO("[CLIENT] Starting briefing with %u second countdown to game start\n", delaySeconds);
	extern void showPreGameBriefing(int gameNumber, int countdownSeconds);
	showPreGameBriefing(selectedGameIndex, delaySeconds);
}

// Process timed game start packet with full JSON (for clients) - extracts player_ids array
void SessionManager::processTimedGameStart(const JsonDocument& doc, uint32_t fromNodeId) {
	// First, parse the session packet normally
	SessionPacket packet = parseJSONSessionPacket(doc);
	
	LV_LOG_INFO("[CLIENT] processTimedGameStart (JSON overload): fromNodeId=%u, targetHost=%u, packet.sessionID=%u, currentSessionId=%u\n",
	             fromNodeId, targetHost, packet.sessionID, currentSessionId);
	
	// Only accept from our target host
	if (fromNodeId != targetHost || packet.sessionID != currentSessionId) {
		LV_LOG_INFO("[CLIENT] Ignoring timed game start from %u (expected host %u, expected session %u)\n", 
		             fromNodeId, targetHost, currentSessionId);
		return;
	}
	
	LV_LOG_INFO("[CLIENT] Received timed game start from host %u, start time: %u, playerCount: %d\n", 
	             fromNodeId, packet.startTime, packet.playerCount);
	
	// Set up our own timed start tracking
	extern painlessMesh mesh;
	uint32_t currentMeshTime = mesh.getNodeTime();
	
	// Get client's selected game from dropdown
	extern objects_t objects;
	int selectedGameIndex = 0;
	if (objects.ddl_join_games) {
		selectedGameIndex = lv_dropdown_get_selected(objects.ddl_join_games);
	}
	if (selectedGameIndex <= 0) selectedGameIndex = 1; // Default to game 1
	
	timedStart.gameIndex = selectedGameIndex;
	timedStart.startTime = packet.startTime;
	timedStart.broadcastStart = millis();
	timedStart.active = true;
	
	uint32_t delaySeconds = (packet.startTime - currentMeshTime) / 1000000; // Convert to seconds
	LV_LOG_INFO("[CLIENT] Game %d will start in %u seconds (mesh time %u -> %u)\n", 
	             selectedGameIndex, delaySeconds, currentMeshTime, packet.startTime);
	
	// Set our status to InGame immediately 
	setMyStatus(InGame);
	
	// CRITICAL: Set player count BEFORE starting briefing (which eventually calls startGameAfterBriefing)
	gameState.playerCount = packet.playerCount;
	LV_LOG_INFO("[CLIENT] Timed game start: playerCount = %d (BEFORE briefing)\n", packet.playerCount);
	
	// CRITICAL: Set the preserved player count for the startGameAfterBriefing function
	extern int preservedPlayerCount;
	preservedPlayerCount = packet.playerCount;
	
	// CRITICAL: Extract and use the complete player_ids array from the JSON
	//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Extracting complete player list from TimedGameStart packet\n");
	
	// Debug: Print the entire JSON packet that was received
	String debugJson;
	serializeJson(doc, debugJson);
	//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Received JSON: %s\n", debugJson.c_str());
	
	// Debug: Check what keys are in the document
	//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Checking JSON keys...\n");
	//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Has 'type': %s\n", doc.containsKey("type") ? "YES" : "NO");
	//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Has 'player_ids': %s\n", doc.containsKey("player_ids") ? "YES" : "NO");
	//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Has 'player_count': %s\n", doc.containsKey("player_count") ? "YES" : "NO");
	
	//if (doc.containsKey("player_ids")) {
	//	LV_LOG_ERROR("[CLIENT_PLAYERIDS] player_ids type: %s\n", doc["player_ids"].is<JsonArray>() ? "JsonArray" : "OTHER");
	//}
	
	memset(gameState.playerIDs, 0, sizeof(gameState.playerIDs));
	
	// Extract player_ids array from JSON
	extern Config config;
	if (doc.containsKey("player_ids")) {
		// Try to access as JsonArrayConst directly without type checking
		JsonArrayConst playerIds = doc["player_ids"];
		if (!playerIds.isNull()) {
			int arraySize = playerIds.size();
			
			//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Found player_ids array with %d players\n", arraySize);
			
			for (int i = 0; i < arraySize && i < 5; i++) {
				uint32_t playerId = playerIds[i].as<uint32_t>();
				gameState.playerIDs[i] = playerId;
				//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Set playerIDs[%d] = %u\n", i, playerId);
			}
			
			// Validate that we know about all other players now
			/*LV_LOG_ERROR("[CLIENT_PLAYERIDS] Complete structure: [0]=%u [1]=%u [2]=%u [3]=%u\n", 
			             gameState.playerIDs[0], gameState.playerIDs[1], gameState.playerIDs[2], gameState.playerIDs[3]);*/
			
			// Count non-zero entries to verify
			int nonZeroCount = 0;
			for (int i = 0; i < 5; i++) {
				if (gameState.playerIDs[i] != 0) nonZeroCount++;
			}
			//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Structure has %d non-zero entries (expected %d)\n", nonZeroCount, packet.playerCount);
			
		} else {
			//LV_LOG_ERROR("[CLIENT_PLAYERIDS] ERROR: player_ids exists but is null!\n");
			// Fallback to basic host+self structure
			gameState.playerIDs[0] = fromNodeId; // Host
			gameState.playerIDs[1] = config.user.nodeId; // Self
			//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Using fallback structure: host=%u, self=%u\n", fromNodeId, config.user.nodeId);
		}
	} else {
		//LV_LOG_ERROR("[CLIENT_PLAYERIDS] ERROR: No player_ids array found in TimedGameStart packet!\n");
		// Fallback to basic host+self structure
		gameState.playerIDs[0] = fromNodeId; // Host
		gameState.playerIDs[1] = config.user.nodeId; // Self
		//LV_LOG_ERROR("[CLIENT_PLAYERIDS] Using fallback structure: host=%u, self=%u\n", fromNodeId, config.user.nodeId);
	}

	// Show client briefing screen with countdown to game start
	LV_LOG_INFO("[CLIENT] Starting briefing with %u second countdown to game start\n", delaySeconds);
	extern void showPreGameBriefing(int gameNumber, int countdownSeconds);
	showPreGameBriefing(selectedGameIndex, delaySeconds);
}

// Check timed game status and start when time reached
void SessionManager::updateTimedGameStatus() {
	static uint32_t lastDebug = 0;
	if (millis() - lastDebug > 10000) { // Debug every 10 seconds
		LV_LOG_INFO("[DEBUG] updateTimedGameStatus: active=%s, isHosting=%s, myStatus=%s\n", 
		             timedStart.active ? "true" : "false", isHosting() ? "true" : "false", playerStatusToString(myStatus));
		lastDebug = millis();
	}
	
	if (!timedStart.active) return;

	extern painlessMesh mesh;
	uint32_t currentMeshTime = mesh.getNodeTime();
	uint32_t now = millis();
	
	// Calculate time until game start
	int32_t timeUntilStart = (int32_t)(timedStart.startTime - currentMeshTime) / 1000000; // Convert to seconds
	
	// Update UI with countdown status
	extern objects_t objects;
	if (timeUntilStart > 0) {
		// Show countdown message for both host and clients
		char statusText[64];
		snprintf(statusText, sizeof(statusText), "Transport in progress... %ds", timeUntilStart);
		
		// For hosts on host screen
		if (isHosting() && objects.lbl_host_random && lv_scr_act() == objects.host) {
			lv_label_set_text(objects.lbl_host_random, statusText);
		}
		
		// For clients, show status in console (could extend to show UI message)
		if (!isHosting()) {
			static uint32_t lastClientStatusPrint = 0;
			if (millis() - lastClientStatusPrint > 5000) { // Print every 5 seconds
				LV_LOG_INFO("[CLIENT] %s\n", statusText);
				lastClientStatusPrint = millis();
			}
		}
		
		// For hosts: resend timed start packets every 3 seconds to ensure delivery
		if (isHosting() && !lobbyPlayers.empty()) {
			static uint32_t lastResend = 0;
			if (now - lastResend > 3000) {
				String timedPacket = createJSONTimedGameStartPacket(currentSessionId, InGame, timedStart.startTime);
				
				for (const auto& player : lobbyPlayers) {
					bool sent = mesh.sendSingle(player.nodeId, timedPacket);
					LV_LOG_INFO("[HOST] Resent timed start to player %u: %s\n", 
					             player.nodeId, sent ? "SUCCESS" : "FAILED");
				}
				lastResend = now;
			}
		}
	} else {
		// Time reached - start the game!
		timedStart.active = false;
		
		LV_LOG_INFO("[SYNC] GAME START - isHost=%s, meshTime=%u, targetTime=%u, diff=%d microseconds\n", 
		             isHosting() ? "HOST" : "CLIENT", currentMeshTime, timedStart.startTime, 
		             (int32_t)(currentMeshTime - timedStart.startTime));
		
		LV_LOG_INFO("[GAME] Mesh time reached (%u), starting game %d\n", 
		             currentMeshTime, timedStart.gameIndex);
		
		// Clear countdown message
		if (isHosting() && objects.lbl_host_random && lv_scr_act() == objects.host) {
			lv_label_set_text(objects.lbl_host_random, "");
		}
		
		// Determine game index
		int gameIndex = timedStart.gameIndex;
		if (gameIndex <= 0) {
			// For clients, get game from dropdown
			if (objects.ddl_join_games) {
				gameIndex = lv_dropdown_get_selected(objects.ddl_join_games);
			}
			if (gameIndex <= 0) gameIndex = 1; // Default to game 1
		}
		
		// Start the game directly for both host and clients
		// Host will exit briefing screen and start game at synchronized time
		// "Bonus" start? commented out because convos.cpp/countdownTimerCallback() starts the games
		//LV_LOG_INFO("[SYNC] Starting synchronized game %d (host=%s)\n", gameIndex, isHosting() ? "true" : "false");
		//startGameAfterBriefing(gameIndex, isHosting());
	}
}


void SessionManager::cleanup()
{
	activeSessions.clear();
	hostingPlayers.clear();
	lobbyPlayers.clear();
	currentSessionId = 0;
	setMyStatus(Idle);
	targetHost = 0;
	timedStart.active = false;
	timedStart.gameIndex = 0;
	timedStart.startTime = 0;
	timedStart.broadcastStart = 0;
}

void SessionManager::handleTargetHostDisconnected(uint32_t hostNodeId)
{
	if (hostNodeId != targetHost)
	{
		return; // Not our target host
	}

	LV_LOG_INFO("[SESSION] Our target host %u disconnected, un-readying and selecting new host\n", hostNodeId);

	// Reset our session state
	leaveSession(); // This sets myStatus to Idle, currentSessionId = 0, targetHost = 0

	// Update ready state image to not ready
	extern objects_t objects;
	if (objects.img_join_readystate)
	{
		lv_image_set_src(objects.img_join_readystate, &img_ui___not_ready);
	}

	// Update the join roller to reflect current hosts
	extern void updateJoinRoller();
	updateJoinRoller();

	// Try to select a different host if available
	PsramVector<uint32_t> currentHosts = getActiveHosts();
	if (!currentHosts.empty() && objects.roller_join_games)
	{
		// Select the first available host
		lv_roller_set_selected(objects.roller_join_games, 0, LV_ANIM_OFF);

		// Trigger roller change event to update UI state
		extern void joinRollerChanged(lv_event_t * e);
		joinRollerChanged(nullptr);

		LV_LOG_INFO("[SESSION] Selected new host from %zu available hosts\n", currentHosts.size());
	}
	else
	{
		LV_LOG_INFO("[SESSION] No hosts available after disconnection\n");
	}
}

void SessionManager::printStatus()
{
	LV_LOG_INFO("[SESSION] Status: %d, SessionID: %u, TargetHost: %u\n",
		myStatus, currentSessionId, targetHost);
	LV_LOG_INFO("[SESSION] Active hosts: %zu\n", hostingPlayers.size());
	for (uint32_t hostId : hostingPlayers)
	{
		LV_LOG_INFO("  Host: %u\n", hostId);
	}
	LV_LOG_INFO("[SESSION] Lobby players: %zu\n", lobbyPlayers.size());
	for (const auto& player : lobbyPlayers)
	{
		LV_LOG_INFO("  Player: %u, status: %d\n", player.nodeId, player.status);
	}
}

// Session packet JSON functions - overloaded for different use cases
String createJSONSessionPacket(uint32_t sessionId, PlayerStatus status, JoinRequest request)
{
	JsonDocument doc;
	doc["type"] = "session";
	doc["sessionID"] = sessionId;
	doc["status"] = (int)status;
	doc["request"] = (int)request;
	doc["currentScore"] = 0;
	doc["startTime"] = 0;

	String result;
	serializeJson(doc, result);
	return result;
}

// Overload for in-game score updates 
String createJSONSessionPacket(uint32_t sessionId, PlayerStatus status, JoinRequest request, int32_t currentScore)
{
	JsonDocument doc;
	doc["type"] = "session";
	doc["sessionID"] = sessionId;
	doc["status"] = (int)status;
	doc["request"] = (int)request;
	doc["currentScore"] = currentScore;
	doc["startTime"] = 0;

	String result;
	serializeJson(doc, result);
	return result;
}

// Overload for timed game start packets
String createJSONTimedGameStartPacket(uint32_t sessionId, PlayerStatus status, int32_t startTime)
{
	JsonDocument doc;
	doc["type"] = "session";
	doc["sessionID"] = sessionId;
	doc["status"] = (int)status;
	doc["request"] = (int)TimedGameStart;
	doc["currentScore"] = 0;
	doc["startTime"] = startTime;
	doc["player_count"] = (int32_t)(sessionManager->lobbyPlayers.size() + 1); // +1 for host
	
	// Add playerIDs array so clients can establish correct structure
	JsonArray playerIDs = doc.createNestedArray("player_ids");
	extern Config config;
	playerIDs.add(config.user.nodeId); // Host is always first
	//LV_LOG_ERROR("[HOST_PACKET] Added host to player_ids: %u\n", config.user.nodeId);
	
	PsramVector<LobbyPlayer> players = sessionManager->lobbyPlayers;
	//LV_LOG_ERROR("[HOST_PACKET] Adding %d lobby players to player_ids array\n", players.size());
	for (size_t i = 0; i < players.size() && i < 5; i++) { // Max 4 players total (host + 3 clients)
		playerIDs.add(players[i].nodeId);
		//LV_LOG_ERROR("[HOST_PACKET] Added lobby player %d to player_ids: %u\n", i, players[i].nodeId);
	}
	
	// Debug: verify the array was created correctly
	//LV_LOG_ERROR("[HOST_PACKET] Final player_ids array size: %d\n", playerIDs.size());
	for (size_t i = 0; i < playerIDs.size(); i++) {
		//LV_LOG_ERROR("[HOST_PACKET] player_ids[%d] = %u\n", i, playerIDs[i].as<uint32_t>());
	}

	String result;
	size_t bytesWritten = serializeJson(doc, result);
	//LV_LOG_ERROR("[HOST_PACKET] Serialized TimedGameStart packet: %d bytes, result: %s\n", bytesWritten, result.c_str());
	return result;
}

// Deprecated - gameID no longer needed since players play individual games
/*
String createJSONGameStartPacket(uint32_t sessionId, PlayerStatus status, JoinRequest request, int32_t gameID) {
	JsonDocument doc;
	doc["type"] = "session";
	doc["sessionID"] = sessionId;
	doc["status"] = (int)status;
	doc["request"] = (int)request;
	doc["currentScore"] = 0;
	doc["gameID"] = gameID;

	String result;
	serializeJson(doc, result);
	return result;
}
*/

SessionPacket parseJSONSessionPacket(const JsonDocument& doc)
{
	SessionPacket packet;
	packet.sessionID = doc["sessionID"].as<uint32_t>();
	packet.status = (PlayerStatus)(doc["status"] | 0);
	packet.request = (JoinRequest)(doc["request"] | 0);
	packet.currentScore = doc["currentScore"] | 0;
	packet.startTime = doc["startTime"] | 0;
	packet.playerCount = doc["player_count"].as<uint32_t>() | 1;

	LV_LOG_INFO("[JSON] Parsed session packet: sessionID=%u, status=%s, request=%d, score=%d, startTime=%d, playerCount%d\n",
		packet.sessionID, playerStatusToString(packet.status), packet.request, packet.currentScore, packet.startTime, packet.playerCount);

	return packet;
}

// UI helper functions
const char* rollerStringFromSessions(const PsramVector<uint32_t>& hostIds, const char* defaultString)
{
	static String rollerOptions;
	rollerOptions.clear();

	if (hostIds.empty())
	{
		rollerOptions = defaultString;
		return rollerOptions.c_str();
	}

	// Filter hosts based on display name configuration
	DisplayOptions gameHostsOption = config.board.displayNameOptions.gameHosts;
	LV_LOG_INFO("[ROLLER_STRING] Display option gameHosts=%s, scanResults size=%zu\n", 
	              displayOptionsToString(gameHostsOption), scanResults ? scanResults->getContacts().size() : 0);

	for (uint32_t hostId : hostIds)
	{
		// Try to find contact info for this host
		ContactData* contact = config.contacts.findContact(hostId);
		if (!contact)
		{
			contact = scanResults->findContact(hostId);
		}
		
		LV_LOG_INFO("[ROLLER_STRING] Processing hostId=%u, contact=%s, displayName='%s'\n",
		              hostId, contact ? "FOUND" : "NULL", 
		              contact ? contact->displayName : "N/A");

		// Apply display name filtering
		bool shouldShow = false;
		switch (gameHostsOption)
		{
		case Everyone:
			shouldShow = true;
			break;
		case NotBlocked:
			shouldShow = !contact || !contact->isBlocked;
			break;
		case Crew:
			shouldShow = contact && contact->isCrew;
			break;
		case None:
			shouldShow = false; // Don't show names, but we might still show board IDs
			break;
		}

		if (shouldShow || gameHostsOption == None)
		{
			if (gameHostsOption == None || !contact)
			{
				// Show board ID instead of name
				rollerOptions += String(hostId);
			}
			else
			{
				// Show display name
				rollerOptions += getNameFromContact(contact);
			}
			rollerOptions += "\n";
		}
	}

	// Remove the last newline character
	if (!rollerOptions.isEmpty() && rollerOptions.endsWith("\n"))
	{
		rollerOptions.remove(rollerOptions.length() - 1);
	}

	// If no hosts passed the filter, show default message
	if (rollerOptions.isEmpty())
	{
		rollerOptions = defaultString;
	}

	return rollerOptions.c_str();
}

void updateJoinRoller()
{
	if (!sessionManager || !objects.roller_join_games) return;

	// Throttle updates to prevent UI flicker and state conflicts
	static uint32_t lastUpdateTime = 0;
	static String lastOptions = "";
	uint32_t now = millis();

	PsramVector<uint32_t> hosts = sessionManager->getActiveHosts();
	LV_LOG_INFO("[JOIN_ROLLER] Active hosts from sessionManager: %zu hosts\n", hosts.size());
	for (size_t i = 0; i < hosts.size(); i++) {
		LV_LOG_INFO("[JOIN_ROLLER]   Host[%zu]: nodeId=%u\n", i, hosts[i]);
	}
	const char* options = rollerStringFromSessions(hosts, "No Crews Found");

	// Only update if options actually changed or enough time has passed
	String currentOptions = String(options);
	if (currentOptions == lastOptions && (now - lastUpdateTime < 2000))
	{
		return; // Skip update to prevent UI flicker
	}

	lastOptions = currentOptions;
	lastUpdateTime = now;

	// Store current target host to try to preserve selection
	uint32_t currentTargetHost = sessionManager->getTargetHost();
	uint32_t currentSelection = lv_roller_get_selected(objects.roller_join_games);

	LV_LOG_INFO("[SESSION] Updating join roller: hosts=%zu, target=%u, selection=%u\n",
		hosts.size(), currentTargetHost, currentSelection);

	lv_roller_set_options(objects.roller_join_games, options, LV_ROLLER_MODE_NORMAL);

	// Try to find and restore selection based on target host (most reliable)
	bool selectionRestored = false;
	if (currentTargetHost != 0)
	{
		for (size_t i = 0; i < hosts.size(); i++)
		{
			if (hosts[i] == currentTargetHost)
			{
				lv_roller_set_selected(objects.roller_join_games, i, LV_ANIM_OFF);
				selectionRestored = true;
				LV_LOG_INFO("[SESSION] Restored roller selection to target host %u at index %zu\n",
					currentTargetHost, i);
				break;
			}
		}
	}

	// If no target host yet, but we have available hosts, auto-select the first one
	if (!selectionRestored && currentTargetHost == 0 && hosts.size() > 0)
	{
		lv_roller_set_selected(objects.roller_join_games, 0, LV_ANIM_OFF);
		LV_LOG_INFO("[SESSION] Auto-selected first available host %u (out of %zu hosts)\n", hosts[0], hosts.size());

		// Trigger roller change to join the session automatically
		extern void joinRollerChanged(lv_event_t * e);
		joinRollerChanged(nullptr); // Auto-join the first available host
		selectionRestored = true;
	}

	// If target host is gone, leave session and reset to "No Crews Found"
	// But be extra conservative - only do this if we're really sure the host is gone
	if (!selectionRestored && currentTargetHost != 0)
	{
		// Double-check by looking for the host in the active sessions (not just the hosting list)
		SessionInfo* hostInfo = sessionManager->getSessionInfo(currentTargetHost);
		bool hostReallyGone = (hostInfo == nullptr);

		if (hostReallyGone)
		{
			LV_LOG_INFO("[SESSION] Target host %u confirmed gone (no session info), leaving session\n", currentTargetHost);
			sessionManager->leaveSession();

			// Reset ready state image to not ready
			if (objects.img_join_readystate)
			{
				lv_image_set_src(objects.img_join_readystate, &img_ui___not_ready);
			}

			// Force selection to "No Crews Found" (index 0 if empty list)
			lv_roller_set_selected(objects.roller_join_games, 0, LV_ANIM_OFF);

			// Update button states
			extern void setMissionReadyState(lv_event_t * e);
			setMissionReadyState(nullptr);
		}
		else
		{
			LV_LOG_INFO("[SESSION] Target host %u not in host list but session info exists, keeping connection\n", currentTargetHost);
			// Don't reset the roller - the host might still be active, just not in the immediate host list
		}
	}

	LV_LOG_INFO("[SESSION] Join roller updated: %s\n", options);
}

void updateHostPlayerList()
{
	if (!sessionManager || !objects.list_host_players) return;

	// Reset manual selection tracking when list is updated
	extern lv_obj_t* selectedPlayerButton;
	extern uint32_t selectedPlayerNodeId;
	selectedPlayerButton = nullptr;
	selectedPlayerNodeId = 0;

	// Disable kick button since selection is cleared
	lv_obj_add_state(objects.btn_host_kick, LV_STATE_DISABLED);
	lv_obj_add_state(objects.cnt_host_kick, LV_STATE_DISABLED);
	lv_obj_set_user_data(objects.btn_host_kick, nullptr);

	// Clear existing list
	lv_obj_clean(objects.list_host_players);

	// Add each lobby player to the list
	PsramVector<LobbyPlayer> players = sessionManager->getLobbyPlayers();
	for (const auto& player : players)
	{
		// Get player name
		const char* playerName;
		if (player.contactData)
		{
			playerName = getNameFromContact(player.contactData);
		}
		else
		{
			static char nodeIdStr[16];
			snprintf(nodeIdStr, sizeof(nodeIdStr), "%u", player.nodeId);
			playerName = nodeIdStr;
		}

		// Choose icon based on ready status
		const lv_img_dsc_t* icon = (player.status == Ready) ? &img_ui___ready : &img_ui___not_ready;

		// Add button to list
		lv_obj_t* button = lv_list_add_button(objects.list_host_players, icon, playerName);
		lv_obj_set_style_text_font(button, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);
		lv_obj_set_style_bg_color(button, lv_color_make(158, 164, 186), LV_PART_MAIN | LV_STATE_CHECKED);

		// Aggressively ensure button starts in unchecked state
		lv_obj_remove_state(button, LV_STATE_CHECKED);
		lv_obj_clear_state(button, LV_STATE_CHECKED);

		// Double-check the initial state
		bool initialState = lv_obj_has_state(button, LV_STATE_CHECKED);
		if (initialState)
		{
			LV_LOG_WARN("[HOST] WARNING: Button for %s still checked after clearing! Forcing unchecked.\n", playerName);
			lv_obj_add_state(button, LV_STATE_DEFAULT); // Force to default state
			lv_obj_remove_state(button, LV_STATE_CHECKED);
		}

		// Store player nodeId in user data for kick functionality
		lv_obj_set_user_data(button, (void*)(uintptr_t)player.nodeId);
		lv_obj_add_event_cb(button, hostPlayerListClick, LV_EVENT_CLICKED, NULL);

		LV_LOG_INFO("[HOST] Added %s to player list (status: %d, nodeId: %u)\n", playerName, player.status, player.nodeId);
	}

	LV_LOG_INFO("[SESSION] Host player list updated with %zu players\n", players.size());
}

// Check if the start button should be enabled based on player ready states
void checkHostStartButtonState()
{
	if (!sessionManager || !objects.btn_host_start) return;

	// Get current lobby players
	PsramVector<LobbyPlayer> players = sessionManager->getLobbyPlayers();

	// Enable start button only if:
	// 1. A game is selected in the dropdown
	// 2. If there are players in the lobby, all must be Ready (empty lobby is OK for solo play)
	bool gameSelected = (lv_dropdown_get_selected(objects.ddl_host_games) > 0);
	bool allPlayersReady = true;

	// If there are players, check that all are ready
	for (const auto& player : players)
	{
		if (player.status != Ready)
		{
			allPlayersReady = false;
			break;
		}
	}

	bool shouldEnable = gameSelected && allPlayersReady;

	if (shouldEnable)
	{
		// Add NULL checks to prevent crashes during rapid screen transitions
		if (objects.btn_host_start) {
			lv_obj_clear_state(objects.btn_host_start, LV_STATE_DISABLED);
		}
		if (objects.cnt_host_start) {
			lv_obj_clear_state(objects.cnt_host_start, LV_STATE_DISABLED);
		}
		LV_LOG_INFO("[HOST] Start button enabled - game selected: %s, players: %zu, all ready: %s\n",
			gameSelected ? "yes" : "no", players.size(), allPlayersReady ? "yes" : "no");
	}
	else
	{
		// Add NULL checks to prevent crashes during rapid screen transitions
		if (objects.btn_host_start) {
			lv_obj_add_state(objects.btn_host_start, LV_STATE_DISABLED);
		}
		if (objects.cnt_host_start) {
			lv_obj_add_state(objects.cnt_host_start, LV_STATE_DISABLED);
		}
		LV_LOG_INFO("[HOST] Start button disabled - game selected: %s, players: %zu, all ready: %s\n",
			gameSelected ? "yes" : "no", players.size(), allPlayersReady ? "yes" : "no");
	}
}

void init_session_manager()
{
	if (sessionManager)
	{
		delete sessionManager;
	}
	sessionManager = new SessionManager();
	LV_LOG_INFO("[SESSION] SessionManager initialized\n");
}

// Relay a player's score to all other clients (host only)
void relayScoreToOtherClients(uint32_t originalSender, int32_t score) {
    extern Config config;
    extern painlessMesh mesh;
    extern SessionManager* sessionManager;
    
    // Only the host should relay scores
    if (!sessionManager || gameState.myStatus != InGame) {
        return;
    }
    
    // Don't relay if we're not actually the host (safety check)
    bool weAreHost = false;
    for (int i = 0; i < 5; i++) {
        if (gameState.playerIDs[i] == config.user.nodeId && i == 0) {
            weAreHost = true;
            break;
        }
    }
    
    if (!weAreHost) {
        return; // Only slot 0 (host) should relay
    }
    
    // Create a relay score packet with original sender info
    // We'll create a custom packet since we need to include originalSender info
    JsonDocument doc;
    doc["type"] = "session";
    doc["sessionID"] = gameState.sessionID;
    doc["status"] = (int)InGame;
    doc["request"] = (int)ScoreRelay;
    doc["currentScore"] = score;
    doc["originalSender"] = originalSender; // Critical: who actually sent this score
    
    String relayPacket;
    serializeJson(doc, relayPacket);
    if (relayPacket.isEmpty()) {
        LV_LOG_ERROR("[RELAY] Failed to create relay packet\n");
        return;
    }
    
    // Send to all clients except the original sender
    int relayCount = 0;
    for (int i = 0; i < 5; i++) {
        uint32_t targetNodeId = gameState.playerIDs[i];
        
        // Skip empty slots, ourselves, and the original sender
        if (targetNodeId == 0 || targetNodeId == config.user.nodeId || targetNodeId == originalSender) {
            continue;
        }
        
        bool sent = mesh.sendSingle(targetNodeId, relayPacket);
        if (sent) {
            relayCount++;
            //LV_LOG_ERROR("[RELAY] Relayed score %d%% from player %u to player %u: %s\n", 
            //             score, originalSender, targetNodeId, sent ? "SUCCESS" : "FAILED");
        }
    }
    
    LV_LOG_INFO("[RELAY] Relayed score %d%% from player %u to %d other clients\n", 
                 score, originalSender, relayCount);
}

// Process in-game score updates from other players
void processInGameScoreUpdate(const SessionPacket& packet, uint32_t fromNodeId) {
    if (gameState.myStatus != InGame) return;
    
    // Debug: Show current playerIDs structure
    static uint32_t lastStructureLog = 0;
    if (millis() - lastStructureLog > 5000) { // Every 5 seconds
        //LV_LOG_ERROR("[SCORE_STRUCTURE] Current playerIDs: [0]=%u [1]=%u [2]=%u [3]=%u\n", 
        //             gameState.playerIDs[0], gameState.playerIDs[1], gameState.playerIDs[2], gameState.playerIDs[3]);
        lastStructureLog = millis();
    }
    
    // Skip processing our own score updates - we don't store our own score in the scores[] array
    extern Config config;
    if (fromNodeId == config.user.nodeId) {
        // This is our own score update - ignore it since gameState.myScore is managed separately
        return;
    }
    
    // Find next available slot in scores[] array (which contains OTHER players only)
    // scores[0] = first other player, scores[1] = second other player, etc.
    // The scores[] array should map to overlay sliders sld_game_p1, sld_game_p2, etc.
    
    // First, find which slot this player occupies in playerIDs
    int playerSlot = -1;
    for (int i = 0; i < 5; i++) {
        if (gameState.playerIDs[i] == fromNodeId) {
            playerSlot = i;
            break;
        }
    }
    
    // If player not found in playerIDs, add them to first available slot 
    if (playerSlot == -1) {
        // Find the first available slot for this new player
        for (int i = 0; i < 5; i++) {
            if (gameState.playerIDs[i] == 0) {
                gameState.playerIDs[i] = fromNodeId;
                playerSlot = i;
                //LV_LOG_ERROR("[SCORE_STRUCTURE] Added new player %u to slot %d\n", fromNodeId, i);
                break;
            }
        }
        
        // Debug: Show updated structure
        //LV_LOG_ERROR("[SCORE_STRUCTURE] Updated playerIDs: [0]=%u [1]=%u [2]=%u [3]=%u\n", 
        //             gameState.playerIDs[0], gameState.playerIDs[1], gameState.playerIDs[2], gameState.playerIDs[3]);
    }
    
    if (playerSlot == -1) {
        //LV_LOG_ERROR("[SCORE] No available slot for player %u\n", fromNodeId);
        return;
    }
    
    // Now map playerSlot to scores[] array (excluding our own slot)
    // Find our own slot first
    int mySlot = -1;
    for (int i = 0; i < 5; i++) {
        if (gameState.playerIDs[i] == config.user.nodeId) {
            mySlot = i;
            break;
        }
    }
    
    if (mySlot == -1) {
        LV_LOG_ERROR("[SCORE] Cannot find my own slot in playerIDs\n");
        return;
    }
    
    // Calculate scores[] index by excluding our own slot
    int scoresIndex = playerSlot;
    if (playerSlot > mySlot) {
        scoresIndex = playerSlot - 1;  // Shift down if player is after us
    } else if (playerSlot == mySlot) {
        LV_LOG_ERROR("[SCORE] Received score update from myself? Ignoring.\n");
        return;
    }
    // If playerSlot < mySlot, no adjustment needed
    
    if (scoresIndex >= 0 && scoresIndex < 5) {
        gameState.scores[scoresIndex] = packet.currentScore;
        gameState.playerStatus[scoresIndex] = packet.status;
        gameState.timeSinceLastMessage[scoresIndex] = millis();
        
        //LV_LOG_ERROR("[SCORE_DEBUG] Updated player %u score to %d%% (playerSlot %d -> scoresIndex %d, mySlot %d)\n", 
        //             fromNodeId, packet.currentScore, playerSlot, scoresIndex, mySlot);
        
        // Note: No relay needed - full mesh means everyone sends to everyone
        
    } else {
        LV_LOG_ERROR("[SCORE] Invalid scoresIndex %d for player %u\n", scoresIndex, fromNodeId);
    }
    
    LV_LOG_INFO("[GAME] Received score update from player %u but no available slots\n", fromNodeId);
}