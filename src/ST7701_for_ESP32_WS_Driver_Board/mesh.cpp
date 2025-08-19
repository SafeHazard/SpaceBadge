#include "mesh.h"
#include "json.h"  // For Config struct
#include "contacts.h"
#include "BAT_Driver.h"
#include "session.h"
#include "custom.h"  // For refreshRanksAfterXPChange()
#include "memory_manager.h"  // New hybrid memory management
#include <LittleFS.h>
#include <WiFi.h>  // For WiFi reset functionality
#include <MD5Builder.h>  // For password obfuscation

// Forward declaration for enum-to-string functions (defined in session.cpp)
extern const char* playerStatusToString(PlayerStatus status);

// Password deobfuscation for mesh security
// Takes stored obfuscated password and returns actual mesh password
static String deobfuscatePassword(const char* obfuscatedPassword) {
    if (!obfuscatedPassword || strlen(obfuscatedPassword) == 0) {
        return String(DEFAULT_MESH_PASSWORD);
    }
    
    // Step 1: Drop every 3rd character to get intermediate string
    String intermediate = "";
    size_t len = strlen(obfuscatedPassword);
    for (size_t i = 0; i < len; i++) {
        if ((i + 1) % 3 != 0) {  // Keep characters that aren't every 3rd
            intermediate += obfuscatedPassword[i];
        }
    }
    
    LV_LOG_INFO("[MESH_SECURITY] Deobfuscated %zu chars to %zu chars", len, intermediate.length());
    
    // Step 2: Calculate MD5 hash
    MD5Builder md5;
    md5.begin();
    md5.add(intermediate);
    md5.calculate();
    String hash = md5.toString();
    
    // Step 3: Use first 16 characters as mesh password (good mix of security and compatibility)
    String meshPassword = hash.substring(0, 16);
    
    LV_LOG_INFO("[MESH_SECURITY] Generated mesh password from obfuscated input");
    return meshPassword;
}

// Global instances
painlessMesh mesh;
Scheduler userScheduler; // to control your personal task
bool meshInitialized = false; // Track if mesh is initialized
uint32_t g_totalPacketsReceived = 0; // Track total packets received for health monitoring
uint32_t g_lastIDPacketTimerExecution = 0; // Track last ID packet timer execution for corruption detection
uint32_t g_idPacketTimerCallCount = 0; // Count timer callbacks to detect corruption

// Forward declarations
void handleIDPacket(const IDPacket& id, uint32_t from);
extern int32_t int_contacts_tab_current;  // ensure linkage to contacts.cpp
bool scanListDirty = false;

// Task to refresh the scan UI every 2 seconds (only if visible)
//Task taskScanUIRefresh(TASK_SECOND * 2, TASK_FOREVER, []()
//	{
//		if (int_contacts_tab_current == 0 && scanListDirty)
//		{
//			populate_scan_list(nullptr);
//			scanListDirty = false;
//		}
//	});

void listDirRecursive(fs::FS& fs, const char* dirname, int levels = 3)
{
	File root = fs.open(dirname);
	if (!root || !root.isDirectory())
	{
		Serial.printf("Failed to open dir: %s\n", dirname);
		return;
	}

	while (true)
	{
		File file = root.openNextFile();
		if (!file) break;

		String path = String(dirname);
		if (!path.endsWith("/")) path += "/";
		path += file.name();   // append relative name to full path

		if (file.isDirectory())
		{
			Serial.printf("DIR : %s/\n", path.c_str());
			if (levels > 0) listDirRecursive(fs, path.c_str(), levels - 1);
		}
		else
		{
			Serial.printf("FILE: %s (%u bytes)\n", path.c_str(), (unsigned)file.size());
		}
	}
}

// Simulation state
#define MAX_SIM_PEERS 500
IDPacket* simulatedPeers[MAX_SIM_PEERS] = { nullptr };

void simulateIncomingIDPacket(uint32_t fakeNodeID);
void simulateIncomingSessionPacket(uint32_t fakeNodeID);

void handleSimulatorCommand(String input)
{
	Serial.printf("[SIM CMD] input='%s'\n", input.c_str());
	input.trim();
	if (input.startsWith("sim.id "))
	{
		uint32_t id = strtoul(input.c_str() + 7, nullptr, 10);
		simulateIncomingIDPacket(id);
	}
	else if (input == "sim.all")
	{
		for (uint32_t i = 1; i <= MAX_SIM_PEERS; i++)
		{
			simulateIncomingIDPacket(i);
		}
	}
	else if (input == "ram")
	{
		Serial.printf("RAM: %u bytes, PSRAM: %u bytes\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
		//heap_caps_print_heap_info(MALLOC_CAP_INTERNAL);
	}
	else if (input == "map")
	{
		getJSONMap();
		getNeighbors(false);
	}
	else if (input == "batt")
	{
		printf("Battery: %d%%\n", BAT_Get_Percentage(BAT_Get_Volts()));
	}
	else if (input == "dir")
	{
		listDirRecursive(LittleFS, "/", 5); // List 5 levels deep
	}
	else if (input.startsWith("sim.add "))
	{
		// Format: sim.add <count>
		uint32_t count = strtoul(input.c_str() + 8, nullptr, 10);
		static uint32_t nextSimID = 1000;
		for (uint32_t i = 0; i < count && nextSimID < MAX_SIM_PEERS; ++i)
		{
			simulateIncomingIDPacket(nextSimID++);
		}
	}
	else if (input.startsWith("sim.session "))
	{
		uint32_t id = strtoul(input.c_str() + 12, nullptr, 10);
		simulateIncomingSessionPacket(id);
	}
	else if (input == "sim.clear")
	{
		for (uint32_t i = 0; i < MAX_SIM_PEERS; i++)
		{
			if (simulatedPeers[i])
			{
				free(simulatedPeers[i]); // Free the IDPacket memory (allocated with ps_malloc)
				simulatedPeers[i] = nullptr;
			}
		}
		LV_LOG_INFO("Simulated peers cleared");
	}
	else if (input.startsWith("setxp "))
	{
		// Format: setxp <game_number> <xp_value>
		// Example: setxp 2 500 (sets game 2 XP to 500, which is config.games[1])
		extern Config config;
		
		char* rest = const_cast<char*>(input.c_str() + 6); // Skip "setxp "
		char* gameNumStr = strtok(rest, " ");
		char* xpValueStr = strtok(nullptr, " ");
		
		if (gameNumStr && xpValueStr) {
			int gameNum = atoi(gameNumStr);
			int xpValue = atoi(xpValueStr);
			
			// Convert 1-based game number to 0-based array index
			int gameIndex = gameNum - 1;
			
			if (gameIndex >= 0 && gameIndex < (int)config.games.size()) {
				config.games[gameIndex].XP = xpValue;
				Serial.printf("[SETXP] Set game %d (%s) XP to %d\n", 
					gameNum, config.games[gameIndex].description, xpValue);
				
				// Recalculate total XP as sum of all game XPs (excluding negative values)
				int newTotalXP = 0;
				for (const auto& game : config.games) {
					if (game.XP != -1) {  
						newTotalXP += game.XP;
					}
				}
				config.user.totalXP = newTotalXP;
				Serial.printf("[SETXP] Updated totalXP to %d (sum of all games)\n", newTotalXP);
				
				// Update pip ranks based on new totalXP
				refreshRanksAfterXPChange();
				
				// Save the updated config
				if (saveBoardConfig(config)) {
					Serial.printf("[SETXP] Config saved successfully\n");
				} else {
					LV_LOG_WARN("[SETXP] Warning: Failed to save config\n");
				}
			} else {
				LV_LOG_ERROR("[SETXP] Error: Game number %d out of range (1-%zu)\n", 
					gameNum, config.games.size());
			}
		} else {
			Serial.printf("[SETXP] Usage: setxp <game_number> <xp_value>\n");
			Serial.printf("[SETXP] Example: setxp 2 500\n");
		}
	}
	else if (input == "config")
	{
		// Pretty-print the current configuration
		extern Config config;
		printConfig(config, false); // false = non-verbose mode
	}
	else if (input == "mesh.reset")
	{
		// Manual mesh reset command for debugging
		Serial.printf("[MESH_RESET] Manual mesh reset triggered\n");
		stop_mesh();
		delay(2000);
		
		// Also restart timers to fix callback corruption
		extern void cleanup_lightweight_tasks();
		extern void init_lightweight_tasks();
		Serial.printf("[MESH_RESET] Restarting timers to fix potential callback corruption\n");
		cleanup_lightweight_tasks();
		delay(1000);
		init_lightweight_tasks();
		
		init_mesh();
		Serial.printf("[MESH_RESET] Manual mesh reset completed with timer restart\n");
	}
	else if (input == "mesh.health")
	{
		// Manual mesh health check
		checkMeshHealth();
	}
	else if (input == "timers.check")
	{
		// Check LVGL timer states with detailed diagnostics
		extern lv_timer_t* idpacket_timer;
		extern lv_timer_t* session_timer;
		extern lv_timer_t* cleanup_timer;
		extern uint32_t g_idPacketTimerCallCount;
		
		Serial.printf("[TIMERS] ID packet timer: %s (callback count: %u)\n", 
		              idpacket_timer ? (lv_timer_get_paused(idpacket_timer) ? "PAUSED" : "RUNNING") : "NULL",
		              g_idPacketTimerCallCount);
		Serial.printf("[TIMERS] Session timer: %s\n", 
		              session_timer ? (lv_timer_get_paused(session_timer) ? "PAUSED" : "RUNNING") : "NULL");
		Serial.printf("[TIMERS] Cleanup timer: %s\n", 
		              cleanup_timer ? (lv_timer_get_paused(cleanup_timer) ? "PAUSED" : "RUNNING") : "NULL");
		Serial.printf("[TIMERS] System next timer in: %u ms\n", lv_timer_get_time_until_next());
		Serial.printf("[TIMERS] LVGL timer handler running: %s\n", "checking...");
		
		// Test if LVGL timer system is actually working
		uint32_t beforeTime = lv_timer_get_time_until_next();
		delay(100);
		uint32_t afterTime = lv_timer_get_time_until_next();
		Serial.printf("[TIMERS] Timer system test - before: %u ms, after: %u ms (should be ~100ms less)\n", beforeTime, afterTime);
	}
	else if (input == "timers.restart")
	{
		// Restart lightweight timers
		Serial.printf("[TIMERS] Restarting lightweight timers...\n");
		extern void cleanup_lightweight_tasks();
		extern void init_lightweight_tasks();
		cleanup_lightweight_tasks();
		delay(1000);
		init_lightweight_tasks();
		Serial.printf("[TIMERS] Timers restarted\n");
	}
	else if (input == "id.force")
	{
		// Force immediate ID packet broadcast for testing
		Serial.printf("[ID_FORCE] Forcing immediate ID packet broadcast\n");
		if (meshInitialized) {
			String idPacketJson = createJSONStringIDPacket();
			if (!idPacketJson.isEmpty()) {
				bool sent = mesh.sendBroadcast(idPacketJson, false);
				Serial.printf("[ID_FORCE] Broadcast result: %s - %s\n", sent ? "SUCCESS" : "FAILED", idPacketJson.c_str());
			} else {
				Serial.printf("[ID_FORCE] Failed to create ID packet JSON\n");
			}
		} else {
			Serial.printf("[ID_FORCE] Mesh not initialized\n");
		}
	}
	else if (input == "timers.test")
	{
		// Create a simple test timer to verify LVGL timer system works
		Serial.printf("[TIMER_TEST] Creating 5-second test timer...\n");
		static lv_timer_t* test_timer = nullptr;
		
		if (test_timer) {
			lv_timer_del(test_timer);
			test_timer = nullptr;
		}
		
		test_timer = lv_timer_create([](lv_timer_t* timer) {
			Serial.printf("[TIMER_TEST] SUCCESS: Test timer callback executed at %u ms!\n", millis());
			lv_timer_del(timer); // One-shot
		}, 5000, nullptr);
		
		if (test_timer) {
			Serial.printf("[TIMER_TEST] Test timer created, should fire in 5 seconds\n");
		} else {
			Serial.printf("[TIMER_TEST] FAILED to create test timer\n");
		}
	}
	else if (input == "audio.status")
	{
		// Show audio system status and spam protection state
		extern void audio_print_status();
		audio_print_status();
	}
	else if (input == "audio.reset")
	{
		// Manually reset audio system health
		extern void audio_reset_health();
		audio_reset_health();
	}
	else if (input == "audio.test")
	{
		// Test audio spam protection by calling Play_Cached_Mp3 rapidly
		Serial.printf("[AUDIO_TEST] Testing spam protection with rapid beep calls...\n");
		extern void play_random_beep();
		for (int i = 0; i < 20; i++) {
			Serial.printf("[AUDIO_TEST] Call #%d\n", i + 1);
			play_random_beep();
			delay(10); // Very rapid calls to trigger protection
		}
		Serial.printf("[AUDIO_TEST] Test complete - check audio system status\n");
	}
}

void init_mesh()
{
	LV_LOG_INFO("Initializing mesh...\n");
	extern Config config;
	
	// Safety check - don't initialize if in airplane mode
	if (config.board.airplaneMode) {
		LV_LOG_INFO("Skipping mesh init - airplane mode enabled\n");
		return;
	}
	
	// Throttle mesh initialization to prevent rapid reinit cycles
	static uint32_t lastMeshInit = 0;
	uint32_t now = millis();
	if (now - lastMeshInit < 10000) {  // Increased from 5s to 10s for stability
		LV_LOG_INFO("Throttling mesh init, too soon since last attempt (need 10s gap)\n");
		return;
	}
	lastMeshInit = now;
	
	// Gentle WiFi setup to avoid corrupting LVGL timers
	// Process any pending LVGL operations before WiFi changes
	lv_timer_handler();
	
	// Skip WiFi.mode(WIFI_OFF) as it can disrupt timer interrupts
	if (WiFi.getMode() != WIFI_MODE_APSTA) {
		WiFi.mode(WIFI_MODE_APSTA);  // Required for mesh operation
		delay(300);  // Reduced delay, only when mode actually changes
		lv_timer_handler();  // Process any timer events after WiFi mode change
	}
	
	String meshSSID = strlen(config.board.ssid) > 0 ? String(config.board.ssid) : String(DEFAULT_MESH_PREFIX);
	String meshPassword = deobfuscatePassword(config.board.password);  // Use deobfuscated password
	uint16_t meshPort = config.board.port > 0 ? config.board.port : DEFAULT_MESH_PORT;
	uint8_t meshChannel = config.board.channel > 0 ? config.board.channel : 6;

	LV_LOG_INFO("Initializing mesh: SSID='%s', Port=%d\n", meshSSID.c_str(), meshPort);

	if(meshInitialized) {
		LV_LOG_INFO("Stopping previous mesh instance...\n");
		mesh.stop();  // Ensure any previous mesh instance is stopped
		delay(500);  // Give time for cleanup
		lv_timer_handler();  // Process any cleanup events
	}

	// Initialize mesh with error handling
	try {
		mesh.init(meshSSID, meshPassword, &userScheduler, meshPort, WIFI_MODE_APSTA, meshChannel);
		LV_LOG_INFO("Mesh initialization successful\n");
	} catch (const std::exception& e) {
		LV_LOG_ERROR("Mesh initialization failed: %s\n", e.what());
		meshInitialized = false;
		return;
	}
	mesh.setContainsRoot(false);  // Reduce network overhead
	mesh.onReceive(&receivedCallback);
	mesh.onNewConnection(&newConnectionCallback);
	mesh.onChangedConnections(&changedConnectionCallback);
	mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

	// Set the Node ID in config now that mesh is initialized
	config.user.nodeId = mesh.getNodeId();
	LV_LOG_INFO("[INFO] Assigned node ID from mesh: %u\n", config.user.nodeId);

	meshInitialized = true;  // Set the flag to true

	// Initialize memory management system
	if (!memoryManager.initReservedBuffer()) {
		LV_LOG_WARN("[WARNING] Failed to allocate reserved memory buffer\n");
	}

	// Check if timers are still valid after mesh init (should not be needed with gentler WiFi ops)
	extern lv_timer_t* idpacket_timer;
	if (!idpacket_timer) {
		//LV_LOG_WARN("[MESH_INIT] WARNING: ID packet timer is NULL after mesh init - this indicates a deeper issue\n");
	}

	//userScheduler.addTask(taskScanUIRefresh);
	//taskScanUIRefresh.enable();

	LV_LOG_INFO("Mesh initialized with NodeID: %u\n", mesh.getNodeId());
}

void stop_mesh()
{
	LV_LOG_INFO("Stopping mesh...");
	if (!meshInitialized)
	{
		LV_LOG_INFO("Mesh is not initialized, nothing to stop.");
		return;
	}
	
	// Stop mesh and clear all callbacks to prevent dangling references
	mesh.stop();
	
	// Gentle WiFi reset without disrupting LVGL timers
	// Avoid WiFi.mode(WIFI_OFF) as it can corrupt LVGL timer interrupts
	WiFi.disconnect(false);  // false = don't erase credentials, less disruptive
	
	// Clear session state to prevent stale connections
	if (sessionManager) {
		sessionManager->stopHosting();
		sessionManager->leaveSession();
	}
	
	// Clear scan results to remove stale contact data
	if (scanResults) {
		scanResults->removeStaleContacts(0);  // Remove all contacts immediately
	}
	
	// Shorter delay since we're using gentler WiFi operations
	delay(500);
	
	meshInitialized = false;  // Reset the flag
	LV_LOG_INFO("Mesh stopped with full WiFi reset.");
}

void handleIDPacket(const IDPacket& id, uint32_t from)
{
	LV_LOG_INFO("[ID_PACKET] Received from nodeId=%u: boardID=%u, name='%s', avatar=%u, XP=%u\n", 
	              from, id.boardID, id.displayName, id.avatarID, id.totalXP);

	if (id.boardID == 0) {
		//LV_LOG_ERROR("[ID_PACKET] ERROR: Ignoring packet - boardID is 0\n");
		return;
	}
	
	if (id.boardID == config.user.nodeId) {
		LV_LOG_INFO("[ID_PACKET] DEBUG: Ignoring packet - boardID matches our nodeId (%u)\n", config.user.nodeId);
		return;
	}
	
	// Safety check for scanResults pointer
	if (!scanResults) {
		//LV_LOG_ERROR("[ID_PACKET] CRITICAL ERROR: scanResults is null in handleIDPacket, myNodeId=%u\n", config.user.nodeId);
		return;
	}
	
	// Additional safety check - verify scanResults is functional
	size_t currentSize = scanResults->getContacts().size();
	LV_LOG_INFO("[ID_PACKET] scanResults check passed, current size=%zu, myNodeId=%u\n", currentSize, config.user.nodeId);

	ContactData contact;
	contact.nodeId = id.boardID;
	strlcpy(contact.displayName, id.displayName, sizeof(contact.displayName));
	contact.avatar = id.avatarID;
	contact.totalXP = id.totalXP;
	contact.lastUpdateTime = id.timeArrived;

	// Force contact data to be added/updated
	LV_LOG_INFO("[ID_PACKET] Adding contact to scanResults: nodeId=%u, name='%s', myNodeId=%u\n",
	              contact.nodeId, contact.displayName, config.user.nodeId);
	scanResults->addOrUpdateContact(contact);
	scanListDirty = true;
	
	// Also update display name in crew contacts if this contact is already crew
	ContactData* crewContact = config.contacts.findContact(contact.nodeId);
	if (crewContact) {
		// Update display name and other fields for crew member
		bool nameChanged = strcmp(crewContact->displayName, contact.displayName) != 0;
		strlcpy(crewContact->displayName, contact.displayName, sizeof(crewContact->displayName));
		crewContact->avatar = contact.avatar;
		crewContact->totalXP = contact.totalXP;
		crewContact->lastUpdateTime = contact.lastUpdateTime;
		
		if (nameChanged) {
			LV_LOG_INFO("[ID_PACKET] Updated crew member display name: nodeId=%u, new name='%s'\n",
			              contact.nodeId, contact.displayName);
			
			// Mark crew list as needing update if we're viewing the crew tab
			if (int_contacts_tab_current == 1) {
				populate_crew_list(nullptr);
			}
		}
	}
	
	LV_LOG_INFO("[ID_PACKET] Contact processed in scanResults: nodeId=%u, name='%s', scanResults size=%zu\n",
	              contact.nodeId, contact.displayName, scanResults->getContacts().size());
	
	// Immediately update scan roller if we're viewing the scan tab
	if (int_contacts_tab_current == 0) {
		populate_scan_list(nullptr);
		scanListDirty = false;
		LV_LOG_INFO("[ID_PACKET] Updated scan list UI for contacts tab\n");
	}
}

void receivedCallback(uint32_t from, String& msg)
{
	// Perform periodic memory cleanup
	memoryManager.performCleanup();
	
	// Add debug logging to track all packet reception  
	static uint32_t totalPacketsReceived = 0;
	totalPacketsReceived++;
	LV_LOG_INFO("[MESH_RX] Packet #%u from nodeId=%u, length=%d, nodeId=%u\n", 
	              totalPacketsReceived, from, msg.length(), config.user.nodeId);
	
	// Make packet count available to mesh health check
	extern uint32_t g_totalPacketsReceived;
	g_totalPacketsReceived = totalPacketsReceived;
	
	// Parse packet type first to determine priority
	JsonDocument doc;
	auto err = deserializeJson(doc, msg);
	if (err) {
		//LV_LOG_ERROR("[MESH_RX] JSON parse error from nodeId=%u: %s\n", from, err.c_str());
		doc.clear();
		return;
	}

	const char* type = doc["type"];
	if (!type) {
		doc.clear();
		return;
	}
	
	// Get current game status and determine packet priority
	extern GameState gameState;
	PacketPriority priority = memoryManager.getPacketPriority(type, from, gameState.myStatus);
	
	// Check if packet should be processed based on memory state and priority
	if (!memoryManager.shouldProcessPacket(priority, gameState.myStatus)) {
		uint32_t freeHeap = memoryManager.getFreeHeap();
		//LV_LOG_WARN("[MESH] Dropping %s packet from %u (priority=%d, mem=%u)\n", 
		//             type, from, priority, freeHeap);
		doc.clear();
		return;
	}
	
	// Log based on memory state  
	MemoryState memState = memoryManager.getMemoryState();
	if (memState == MEM_NORMAL) {
		LV_LOG_INFO("Received from %u msg=%s\n", from, msg.c_str());
	} else {
		LV_LOG_INFO("Received from %u type=%s (mem=%s)\n", from, type, 
		             (memState == MEM_LOW) ? "LOW" : "CRITICAL");
	}

	// if it's an ID packet, handle it
	if (strcmp(type, "id") == 0)
	{
		IDPacket id;
		id.boardID = doc["boardID"].as<uint32_t>();
		strlcpy(id.displayName, doc["displayName"] | "", sizeof(id.displayName));
		id.avatarID = doc["avatarID"] | 0;
		id.totalXP = doc["totalXP"] | 0;
		id.timeArrived = millis();
		LV_LOG_INFO("[ID_RX] Received ID packet from nodeId=%u: boardID=%u, displayName='%s', myNodeId=%u\n", 
		              from, id.boardID, id.displayName, config.user.nodeId);
		handleIDPacket(id, from);
	}
	// Handle session packets
	else if (strcmp(type, "session") == 0)
	{
		if (sessionManager) {
			SessionPacket sessionPacket = parseJSONSessionPacket(doc);
			
			// Find contact data for the sender
			ContactData* contact = config.contacts.findContact(from);
			if (!contact) {
				contact = scanResults->findContact(from);
			}
			
			LV_LOG_INFO("[SESSION] Received session packet from %u: sessionID=%u, status=%s, request=%d, contact=%s\n", 
			              from, sessionPacket.sessionID, playerStatusToString(sessionPacket.status), sessionPacket.request,
			              contact ? (strlen(contact->displayName) ? contact->displayName : "no_name") : "null");
			
			// For TimedGameStart packets, use the JSON overload to preserve player_ids array
			if (sessionPacket.request == TimedGameStart) {
				//LV_LOG_ERROR("[MESH_ROUTING] DETECTED TimedGameStart packet! Using JSON overload with player_ids\n");
				//LV_LOG_ERROR("[MESH_ROUTING] TimedGameStart enum value: %d, packet.request: %d\n", (int)TimedGameStart, sessionPacket.request);
				sessionManager->addOrUpdateSession(doc, from, contact);
			} else {
				//LV_LOG_ERROR("[MESH_ROUTING] Non-TimedGameStart packet (request=%d), using normal overload\n", sessionPacket.request);
				// For all other packets, use the normal overload
				sessionManager->addOrUpdateSession(sessionPacket, from, contact);
			}
			
			// Update UI if needed, but throttle to prevent excessive updates
			static uint32_t lastJoinRollerUpdate = 0;
			extern int32_t int_contacts_tab_current;
			if (lv_scr_act() == objects.join && (millis() - lastJoinRollerUpdate > 3000)) {
				updateJoinRoller();
				lastJoinRollerUpdate = millis();
			}

			// Only update player count from timed game start packets, not regular session packets
			if (sessionPacket.request == TimedGameStart && sessionPacket.playerCount > 0) {
				gameState.playerCount = sessionPacket.playerCount;
				LV_LOG_INFO("[MESH] Updated gameState.playerCount = %d from timed game start packet\n", sessionPacket.playerCount);
			}
		}
	}
	else {
		LV_LOG_INFO("[DEBUG] Received unknown packet type: '%s'\n", type);
	}

	doc.clear();
}

void newConnectionCallback(uint32_t nodeId)
{
	LV_LOG_INFO("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
	LV_LOG_INFO("Changed connections.\n");
	
	// Get current node list to see who's still connected
	std::list<uint32_t> currentNodes = mesh.getNodeList(false);
	
	if (sessionManager) {
		// Check if any of our active hosts are no longer in the mesh
		PsramVector<uint32_t> activeHosts = sessionManager->getActiveHosts();
		bool hostDisconnected = false;
		uint32_t disconnectedTargetHost = 0;
		
		for (uint32_t hostId : activeHosts) {
			// Check if this host is still in the current node list
			bool hostStillConnected = std::find(currentNodes.begin(), currentNodes.end(), hostId) != currentNodes.end();
			
			if (!hostStillConnected) {
				LV_LOG_INFO("[MESH] Host %u is no longer connected, removing from sessions\n", hostId);
				
				// Check if this was our target host
				if (hostId == sessionManager->getTargetHost()) {
					disconnectedTargetHost = hostId;
				}
				
				// Remove the host from our active sessions
				// This will be handled by the next call to removeStaleHostingSessions
				hostDisconnected = true;
			}
		}
		
		// If we detected disconnected hosts, force immediate cleanup and UI update
		if (hostDisconnected) {
			sessionManager->removeStaleHostingSessions(0); // Force immediate cleanup
			
			// If we're on the join screen, update the roller
			extern objects_t objects;
			if (lv_scr_act() == objects.join) {
				extern void updateJoinRoller();
				updateJoinRoller();
				LV_LOG_INFO("[MESH] Updated join roller due to network changes\n");
				
				// If our target host disconnected, handle un-readying
				if (disconnectedTargetHost != 0) {
					sessionManager->handleTargetHostDisconnected(disconnectedTargetHost);
				}
			}
		}
	}
}

void nodeTimeAdjustedCallback(int32_t offset)
{
	//LV_LOG_INFO("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void simulateIncomingIDPacket(uint32_t fakeNodeID)
{
	LV_LOG_INFO("[SIM] simulateIncomingIDPacket(%u)\n", fakeNodeID);

	if (fakeNodeID == 0 || fakeNodeID >= MAX_SIM_PEERS) return;

	if (!simulatedPeers[fakeNodeID])
	{
		LV_LOG_INFO("[SIM] Creating new IDPacket for node %u\n", fakeNodeID);

		IDPacket* p = (IDPacket*)ps_malloc(sizeof(IDPacket));
		if (!p) {
			//LV_LOG_ERROR("[ERROR] Failed to allocate memory for simulated IDPacket %u\n", fakeNodeID);
			return;
		}
		p->boardID = fakeNodeID;
		snprintf(p->displayName, sizeof(p->displayName), "SimBot %u", fakeNodeID);
		p->avatarID = fakeNodeID % 16;
		p->totalXP = random(1000, 5000);
		p->timeArrived = millis();
		simulatedPeers[fakeNodeID] = p;
	}
	handleIDPacket(*simulatedPeers[fakeNodeID], fakeNodeID);
}

void getNeighbors(bool self = false)
{
	std::list<uint32_t> nodes = mesh.getNodeList(self);
	LV_LOG_INFO("Neighbors(%u): %zu nodes\n", self, nodes.size());
	for (const auto& nodeId : nodes)
	{
		LV_LOG_INFO("Node ID: %u\n", nodeId);
	}
}

void getJSONMap()
{
	TSTRING mapJson = mesh.subConnectionJson(true);
	Serial.printf("Mesh JSON Map: %s\n", mapJson.c_str());
}

void simulateIncomingSessionPacket(uint32_t fakeNodeID)
{
	LV_LOG_INFO("(TODO) Simulated session packet from %u\n", fakeNodeID);
	// Placeholder for future session packet simulation
}

void checkMeshHealth()
{
	extern Config config;
	
	// Skip health check if in airplane mode
	if (config.board.airplaneMode || !meshInitialized) {
		return;
	}
	
	static uint32_t lastHealthCheck = 0;
	static uint32_t consecutiveFailures = 0;
	static uint32_t lastNodeCount = 0;
	
	uint32_t now = millis();
	
	// Check mesh health every 30 seconds
	if (now - lastHealthCheck < 30000) {
		return;
	}
	lastHealthCheck = now;
	
	// Get current node count
	std::list<uint32_t> nodes = mesh.getNodeList(false);
	uint32_t currentNodeCount = nodes.size();
	
	// Check WiFi connection status
	bool wifiConnected = WiFi.status() == WL_CONNECTED;
	uint32_t freeHeap = ESP.getFreeHeap();
	
	// Check for timer corruption (ID packet timer not executing)
	static uint32_t lastTimerCallCount = 0;
	static uint32_t lastCorruptionCheck = 0;
	bool timerCorrupted = false;
	
	// Check every 30 seconds if timer callback count has increased
	if ((now - lastCorruptionCheck) > 30000) {
		extern uint32_t g_idPacketTimerCallCount;
		if (g_idPacketTimerCallCount == lastTimerCallCount && lastTimerCallCount > 0) {
			// Timer callback count hasn't increased in 30 seconds - corruption detected
			timerCorrupted = true;
			LV_LOG_WARN("[MESH_HEALTH] Timer corruption: callback count stuck at %u for 30+ seconds\n", g_idPacketTimerCallCount);
		}
		lastTimerCallCount = g_idPacketTimerCallCount;
		lastCorruptionCheck = now;
	}
	
	LV_LOG_INFO("[MESH_HEALTH] Nodes: %u, WiFi: %s, Heap: %u, Failures: %u, TotalRX: %u, TimerOK: %s\n",
	              currentNodeCount, wifiConnected ? "OK" : "FAIL", freeHeap, consecutiveFailures, g_totalPacketsReceived,
	              timerCorrupted ? "CORRUPT" : "OK");
	
	// Handle timer corruption FIRST - independent of other conditions
	if (timerCorrupted) {
		LV_LOG_WARN("[MESH_HEALTH] DETECTED: Timer corruption, auto-restarting timer system\n");
		extern void cleanup_lightweight_tasks();
		extern void init_lightweight_tasks();
		cleanup_lightweight_tasks();
		delay(1000);
		init_lightweight_tasks();
		LV_LOG_INFO("[MESH_HEALTH] Timer system restarted automatically\n");
		return; // Exit early after timer restart
	}
	
	// Detect potential mesh death conditions
	bool meshMayBeDead = false;
	static uint32_t lastPacketReceived = 0;
	
	// Track when we last received a packet to detect mesh communication failure
	static uint32_t lastTotalPackets = 0;
	bool packetsStalled = (g_totalPacketsReceived == lastTotalPackets);
	lastTotalPackets = g_totalPacketsReceived;
	
	// Condition 1: No nodes for extended period and WiFi shows connected
	if (currentNodeCount == 0 && wifiConnected) {
		consecutiveFailures++;
		meshMayBeDead = (consecutiveFailures >= 3); // 3 consecutive failures = 90 seconds
	}
	// Condition 2: Very low memory could cause mesh instability
	else if (freeHeap < 10000) {
		consecutiveFailures++;
		meshMayBeDead = (consecutiveFailures >= 2); // Faster recovery on low memory
	}
	// Condition 3: WiFi disconnected entirely
	else if (!wifiConnected) {
		consecutiveFailures++;
		meshMayBeDead = (consecutiveFailures >= 2);
	}
	// Condition 4: NEW - Mesh shows nodes but no packet traffic (mesh communication dead)
	else if (currentNodeCount > 0 && packetsStalled) {
		consecutiveFailures++;
		meshMayBeDead = (consecutiveFailures >= 2); // 60 seconds of no packets despite nodes - be more aggressive
		LV_LOG_WARN("[MESH_HEALTH] DETECTED: Mesh shows %u nodes but no packet traffic for %u cycles (packets stalled)\n", 
		              currentNodeCount, consecutiveFailures);
	}
	else {
		consecutiveFailures = 0; // Reset failure count
	}
	
	// Attempt mesh recovery if dead state detected
	if (meshMayBeDead) {
		LV_LOG_WARN("[MESH_HEALTH] CRITICAL: Mesh appears dead, attempting recovery...\n");
		
		// Force mesh restart
		stop_mesh();
		delay(3000);  // Longer delay for complete reset
		init_mesh();
		
		consecutiveFailures = 0; // Reset after recovery attempt
	}
	
	lastNodeCount = currentNodeCount;
}

