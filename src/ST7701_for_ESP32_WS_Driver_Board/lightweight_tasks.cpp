#include "lightweight_tasks.h"
#include "BAT_Driver.h"
#include "./src/ui/ui.h"
#include "./src/ui/images.h"
#include "badgeMode.h"
#include "mesh.h"
#include "session.h"
#include "json.h"
#include <painlessMesh.h>
#include <Arduino.h>

// Forward declaration for enum-to-string functions (defined in session.cpp)
extern const char* playerStatusToString(PlayerStatus status);

// Lightweight LVGL timers (minimal RAM usage compared to FreeRTOS tasks)
static lv_timer_t* battery_timer = nullptr;
static lv_timer_t* badgemode_timer = nullptr;
lv_timer_t* idpacket_timer = nullptr;  // Made non-static for debugging access
lv_timer_t* cleanup_timer = nullptr;   // Made non-static for debugging access
lv_timer_t* session_timer = nullptr;   // Made non-static for debugging access
static lv_timer_t* info_timer = nullptr;

extern painlessMesh mesh;
extern ContactManager* scanResults;
extern bool meshInitialized;
extern SessionManager* sessionManager;
extern Config config;
extern objects_t objects;

// Battery status update (every 10 seconds)
static void battery_task_cb(lv_timer_t* timer)
{
    switch (BAT_Get_Percentage(BAT_Get_Volts()))
    {
    case 100:
    case 90:
    case 80:
        lv_img_set_src(objects.img_main_battery, &img_ui_element___battery_100);
        break;
    case 70:
    case 60:
        lv_img_set_src(objects.img_main_battery, &img_ui_element___battery_075);
        break;
    case 50:
    case 40:
    case 30:
        lv_img_set_src(objects.img_main_battery, &img_ui_element___battery_050);
        break;
    case 20:
    case 10:
        lv_img_set_src(objects.img_main_battery, &img_ui_element___battery_025);
        break;
    case 0:
        lv_img_set_src(objects.img_main_battery, &img_ui_element___battery_000);
        break;
    default:
        lv_img_set_src(objects.img_main_battery, NULL);
        break;
    }
}

// Badge mode check (every 1 second)
static void badgemode_task_cb(lv_timer_t* timer)
{
    badgeModeCheck();
}

// ID packet broadcast (dynamic interval based on network size and memory)
static void idpacket_task_cb(lv_timer_t* timer)
{
    static uint32_t current_interval = 10000; // Start with 10 seconds
    static uint32_t lastBroadcastTime = 0;
    static uint32_t totalBroadcasts = 0;
    static uint32_t timerCallbacks = 0;
    
    uint32_t now = millis();
    timerCallbacks++;
    
    // Update global callback counter for corruption detection
    extern uint32_t g_idPacketTimerCallCount;
    g_idPacketTimerCallCount++;
    
    // Log timer activity to detect if timer stops being called
    LV_LOG_INFO("[IDPacketTimer] Timer callback #%u called, nodeId=%u, interval=%u, now=%u\n", 
                 timerCallbacks, config.user.nodeId, current_interval, now);
    
    // Always broadcast if mesh is initialized, regardless of badge mode
    // Badge mode shouldn't prevent other boards from knowing we exist
    if (meshInitialized)
    {
        LV_LOG_INFO("[IDPacketTimer] ATTEMPTING broadcast - meshInit=%s, nodeId=%u\n", 
                     meshInitialized ? "true" : "false", config.user.nodeId);
        // Check memory before doing expensive operations
        uint32_t freeHeap = ESP.getFreeHeap();
        if (freeHeap < 10000) {  // Lowered threshold - 15k was too high
            LV_LOG_WARN("[IDPacketTimer] Skipping broadcast due to low memory: %u bytes\n", freeHeap);
            lv_timer_set_period(timer, 30000); // Slow down when low on memory
            return;
        }
        
        size_t nodeCount = mesh.getNodeList(false).size();
        static size_t lastNodeCount = 999; // Force initial print
        
        if (nodeCount != lastNodeCount) {
            LV_LOG_INFO("[IDPacketTimer] Mesh nodes changed: %zu, Free heap: %u\n", 
                         nodeCount, freeHeap);
            lastNodeCount = nodeCount;
        }
        
        // CRITICAL FIX: Always broadcast, even with 0 nodes
        // This helps other boards discover us and breaks the chicken-and-egg problem
        {
            String idPacketJson = createJSONStringIDPacket();
            LV_LOG_INFO("[IDPacketTimer] Created JSON packet, length=%d, nodeId=%u\n", 
                         idPacketJson.length(), config.user.nodeId);
            if (!idPacketJson.isEmpty())
            {
                bool sent = mesh.sendBroadcast(idPacketJson, false);
                totalBroadcasts++;
                uint32_t timeSinceLastBroadcast = (lastBroadcastTime > 0) ? (now - lastBroadcastTime) : 0;
                lastBroadcastTime = now;
                
                LV_LOG_INFO("[IDPacketTimer] Broadcast #%u to %zu nodes: %s (gap: %ums) nodeId=%u JSON: %s\n", 
                             totalBroadcasts, nodeCount, sent ? "SUCCESS" : "FAILED", 
                             timeSinceLastBroadcast, config.user.nodeId, idPacketJson.c_str());
                
                // If broadcast failed, try to diagnose why
                if (!sent) {
                    //LV_LOG_WARN("[IDPacketTimer] BROADCAST FAILED - meshInitialized=%s, nodes=%zu, json_len=%d, nodeId=%u\n",
                    //             meshInitialized ? "true" : "false", nodeCount, idPacketJson.length(), config.user.nodeId);
                    
                    // Additional diagnostics for broadcast failure
                    extern painlessMesh mesh;
                    LV_LOG_INFO("[IDPacketTimer] Mesh diagnostics - getNodeId=%u, getNodeList size=%zu\n", 
                                 mesh.getNodeId(), mesh.getNodeList(false).size());
                }

                // Adjust interval based on network size AND memory pressure
                uint32_t new_interval;
                if (freeHeap < 20000) {
                    // Memory pressure - slow down significantly
                    new_interval = 45000;  // Reduced from 60s - still need discovery
                    //LV_LOG_WARN("[IDPacketTimer] Memory pressure (%u bytes), using slow interval\n", freeHeap);
                } else if (nodeCount > 450)      new_interval = 30000;  // Further reduced for better discovery
                else if (nodeCount > 300) new_interval = 25000;  // Reduced
                else if (nodeCount > 200) new_interval = 20000;  // Reduced 
                else if (nodeCount > 100) new_interval = 15000;  // Reduced
                else if (nodeCount > 50)  new_interval = 10000;  // More aggressive
                else if (nodeCount > 10)  new_interval = 8000;   // Good for small groups
                else if (nodeCount > 3)   new_interval = 6000;   // New tier for very small groups
                else if (nodeCount > 0)   new_interval = 5000;   // Very fast when few nodes
                else                      new_interval = 7000;   // Lonely node - fast enough for discovery
                
                // Update timer period if needed (DISABLE jitter temporarily to fix corruption)
                if (new_interval != current_interval) {
                    current_interval = new_interval;
                    // JITTER DISABLED - was causing integer underflow and 42M ms intervals
                    // TODO: Fix jitter calculation later if needed for synchronization prevention
                    uint32_t jittered_interval = current_interval; // No jitter for now
                    
                    // Safety check: validate timer before modifying it (timers are not LVGL objects)
                    if (timer) {
                        lv_timer_set_period(timer, jittered_interval);
                        LV_LOG_INFO("[IDPacketTimer] Adjusted interval to %u ms (jittered: %u ms) for %zu nodes, %u bytes free\n", 
                                     current_interval, jittered_interval, nodeCount, freeHeap);
                    } else {
                        LV_LOG_ERROR("[IDPacketTimer] ERROR: Timer is NULL, cannot adjust interval!\n");
                    }
                }
            } else {
                //LV_LOG_WARN("[IDPacketTimer] EMPTY JSON PACKET - cannot broadcast, nodeId=%u\n", config.user.nodeId);
                
                // Diagnose why JSON packet is empty
                LV_LOG_INFO("[IDPacketTimer] Config check - nodeId=%u, displayName='%s', avatar=%d\n",
                             config.user.nodeId, config.user.displayName, config.user.avatar);
            }
        }
    }
    else
    {
        // Mesh not initialized - log why we're not broadcasting (but only occasionally)
        static uint32_t lastNoBroadcastLog = 0;
        if (now - lastNoBroadcastLog > 30000) {  // Log every 30 seconds
            //LV_LOG_WARN("[IDPacketTimer] Not broadcasting - meshInitialized=%s, airplaneMode=%s, nodeId=%u\n", 
            //             meshInitialized ? "true" : "false", 
            //             config.board.airplaneMode ? "true" : "false", config.user.nodeId);
            lastNoBroadcastLog = now;
        }
    }
    
    // HEALTH CHECK: Detect if timer is stuck
    static uint32_t lastTimerExecution = 0;
    if (lastTimerExecution > 0 && (now - lastTimerExecution) > (current_interval * 3)) {
        //LV_LOG_WARN("[IDPacketTimer] WARNING: Timer may be stuck! Expected ~%ums, actual gap: %ums\n", 
        //             current_interval, now - lastTimerExecution);
    }
    lastTimerExecution = now;
    
    // Store last successful execution time globally for corruption detection
    extern uint32_t g_lastIDPacketTimerExecution;
    g_lastIDPacketTimerExecution = now;
}

// Cleanup task (every 30 seconds)
static void cleanup_task_cb(lv_timer_t* timer)
{
    const uint32_t SCAN_TIMEOUT_MS = 300000; // 5 minutes
    
    // Memory monitoring - more frequent when memory is low
    static uint32_t lastMemoryPrint = 0;
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t memoryCheckInterval = (freeHeap < 20000) ? 60000 : 300000; // 1 min if low, 5 min otherwise
    if (millis() - lastMemoryPrint > memoryCheckInterval) {
        uint32_t maxBlock = ESP.getMaxAllocHeap();
        uint32_t freePsram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
        LV_LOG_INFO("[MEMORY] Free heap: %u bytes, largest block: %u bytes, free PSRAM: %u bytes\n", 
                     freeHeap, maxBlock, freePsram);
        
        if (sessionManager) {
            sessionManager->printStatus();
        }
        
        // Multi-tier memory management
        if (freeHeap < 20000) {
            LV_LOG_INFO("[MEMORY] Low memory detected (%u bytes), initiating cleanup...\n", freeHeap);
            
            if (sessionManager) {
                sessionManager->removeStalePlayersSessions(120000); // 2 minutes instead of 5
            }
            
            if (scanResults) {
                scanResults->removeStaleContacts(120000); // 2 minutes instead of 5
            }
        }
        
        if (freeHeap < 15000) {
            LV_LOG_INFO("[MEMORY] Critical low memory detected (%u bytes), aggressive cleanup...\n", freeHeap);
            
            if (sessionManager) {
                sessionManager->removeStalePlayersSessions(30000); // 30 seconds
                // Don't call full cleanup() during normal operation as it breaks active sessions
                // sessionManager->cleanup(); // REMOVED - this was breaking active sessions
            }
            
            if (scanResults) {
                scanResults->removeStaleContacts(30000); // 30 seconds
            }
            
            // Force garbage collection
            heap_caps_malloc_extmem_enable(1);
        }
        
        //if (freeHeap < 10000) {
        //    Serial.printf("[MEMORY] EMERGENCY: Extremely low memory (%u bytes), reinitializing mesh...\n", freeHeap);
        //    
        //    // Emergency measures - reinitialize mesh to clear potential leaks
        //    extern bool meshInitialized;
        //    if (meshInitialized) {
        //        Serial.printf("[MEMORY] Emergency mesh reinit due to low memory\n");
        //        stop_mesh();
        //        delay(2000); // Give time for cleanup
        //        init_mesh();
        //    }
        //}
        
        lastMemoryPrint = millis();
    }
    
    // Clean up stale scan results
    if (scanResults) {
        size_t removed = scanResults->removeStaleContacts(SCAN_TIMEOUT_MS);
        
        // Update scan roller if contacts were removed and we're viewing scan tab
        extern int32_t int_contacts_tab_current;
        if (removed > 0 && int_contacts_tab_current == 0) {
            extern bool scanListDirty;
            scanListDirty = true;
        }
    }
    
    // Clean up stale session data with different timeouts for hosting vs general cleanup
    if (sessionManager) {
        // If we're hosting, check every 15 seconds and remove players not heard from in 30 seconds
        // With 5-second unicast heartbeat, we can use faster detection
        if (sessionManager->isHosting()) {
            static uint32_t lastHostingCleanup = 0;
            if (millis() - lastHostingCleanup > 15000) { // Check every 15 seconds when hosting
                sessionManager->removeStalePlayersSessions(30000); // 30 second timeout for lobby players
                lastHostingCleanup = millis();
                LV_LOG_INFO("[CLEANUP] Host cleanup - removed stale players (30s timeout)\n");
            }
        } else {
            // General cleanup with longer timeout
            sessionManager->removeStalePlayersSessions(SCAN_TIMEOUT_MS);
        }
        
        // Also check for stale hosts every cleanup cycle (30 seconds)
        // This ensures the join roller stays current even without network changes
        static uint32_t lastHostCleanup = 0;
        if (millis() - lastHostCleanup > 30000) {
            size_t hostsBefore = sessionManager->getActiveHosts().size();
            sessionManager->removeStaleHostingSessions(30000);
            size_t hostsAfter = sessionManager->getActiveHosts().size();
            
            if (hostsBefore != hostsAfter && lv_scr_act() == objects.join) {
                extern void updateJoinRoller();
                updateJoinRoller();
                LV_LOG_INFO("[CLEANUP] Updated join roller - hosts changed from %zu to %zu\n", hostsBefore, hostsAfter);
            }
            
            lastHostCleanup = millis();
        }
    }
}

// Session packet handling (dynamic interval)
static void session_task_cb(lv_timer_t* timer)
{
    if (!sessionManager) return;
    
    // Continue session maintenance during badge mode to prevent lobby desync
    // Badge mode shouldn't break multiplayer sessions
    
    PlayerStatus myStatus = sessionManager->getMyStatus();
    size_t nodeCount = mesh.getNodeList(false).size();
    
    // If hosting, broadcast session packets every 10 seconds
    if (myStatus == Hosting)
    {
        uint32_t sessionId = sessionManager->getMySessionId();
        String sessionPacketJson = createJSONSessionPacket(sessionId, Hosting, HostAdvertising);
        
        static uint32_t lastHostingPrint = 0;
        if (millis() - lastHostingPrint > 30000) { // Print every 30 seconds
            extern bool badgeMode_triggered;
            LV_LOG_INFO("[SESSION] Hosting session %u, mesh nodes: %zu, badgeMode: %s\n", 
                         sessionId, nodeCount, badgeMode_triggered ? "active" : "inactive");
            lastHostingPrint = millis();
        }
        
        // Throttle session broadcasts to prevent mesh overload
        static uint32_t lastSessionBroadcast = 0;
        uint32_t broadcastInterval = 10000; // 10 seconds base
        
        // Increase interval if network is large to reduce congestion
        if (nodeCount > 100) broadcastInterval = 15000;
        if (nodeCount > 200) broadcastInterval = 20000;
        if (nodeCount > 500) broadcastInterval = 30000;
        
        if (!sessionPacketJson.isEmpty() && nodeCount > 0 && 
            (millis() - lastSessionBroadcast >= broadcastInterval))
        {
            static uint32_t consecutiveBroadcastFailures = 0;
            try {
                bool sent = mesh.sendBroadcast(sessionPacketJson, false);
                if (sent) {
                    lastSessionBroadcast = millis();
                    consecutiveBroadcastFailures = 0; // Reset on success
                } else {
                    consecutiveBroadcastFailures++;
                    LV_LOG_ERROR("[SESSION] Failed to send hosting broadcast (failure #%u)\n", consecutiveBroadcastFailures);
                    
                    // Exponential backoff: double interval for each failure (max 2 minutes)
                    if (consecutiveBroadcastFailures > 1) {
                        uint32_t backoffInterval = min(120000U, broadcastInterval * (1 << (consecutiveBroadcastFailures - 1)));
                        //LV_LOG_WARN("[SESSION] Backing off to %u ms due to failures\n", backoffInterval);
                        lv_timer_set_period(timer, backoffInterval);
                    }
                }
            }
            catch (...) {
                consecutiveBroadcastFailures++;
                LV_LOG_ERROR("[SESSION] Exception during hosting broadcast (failure #%u)\n", consecutiveBroadcastFailures);
            }
        }
        
        // Set timer for hosting interval (10 seconds)
        lv_timer_set_period(timer, broadcastInterval);
    }
    // If in lobby or ready, send join requests to host more frequently for better responsiveness
    else if (myStatus == InLobby || myStatus == Ready)
    {
        uint32_t targetHost = sessionManager->getTargetHost();
        uint32_t sessionId = sessionManager->getMySessionId();
        
        if (targetHost != 0 && sessionId != 0)
        {
            JoinRequest request = Request;
            String sessionPacketJson = createJSONSessionPacket(sessionId, myStatus, request);
            
            if (!sessionPacketJson.isEmpty())
            {
                static uint32_t lastSuccessfulSend = 0;
                try {
                    bool sent = mesh.sendSingle(targetHost, sessionPacketJson);
                    if (sent) {
                        lastSuccessfulSend = millis();
                        static uint32_t lastJoinRequestPrint = 0;
                        if (millis() - lastJoinRequestPrint > 5000) { // Print every 5 seconds (more frequent)
                            LV_LOG_INFO("[JOIN] Sent join request to host %u (sessionId=%u, status=%s)\n", 
                                         targetHost, sessionId, playerStatusToString(myStatus));
                            lastJoinRequestPrint = millis();
                        }
                    } else {
                        LV_LOG_ERROR("[JOIN] Failed to send join request to host %u (sessionId=%u)\n", targetHost, sessionId);
                    }
                }
                catch (...) {
                    LV_LOG_ERROR("[JOIN] Exception sending join request to host %u\n", targetHost);
                }
                
                // Consistent 5-second heartbeat for unicast join requests
                // Unicast is reliable and targeted, so we can use a faster heartbeat
                uint32_t interval = 5000; // 5 seconds consistent heartbeat
                
                // Add small jitter to prevent synchronization  
                uint32_t jitter = random(500); // ±500ms jitter
                interval += (random(2) ? jitter : -jitter);
                
                lv_timer_set_period(timer, interval);
            } else {
                LV_LOG_ERROR("[JOIN] Empty sessionPacketJson for targetHost=%u, sessionId=%u\n", targetHost, sessionId);
                lv_timer_set_period(timer, 2000);
            }
        } else {
            static uint32_t lastNoTargetPrint = 0;
            if (millis() - lastNoTargetPrint > 10000) { // Print every 10 seconds
                //LV_LOG_WARN("[JOIN] Not sending join request: targetHost=%u, sessionId=%u\n", targetHost, sessionId);
                lastNoTargetPrint = millis();
            }
            lv_timer_set_period(timer, 2000);
        }
    }
    else
    {
        // Not in active session, check less frequently (30 seconds)
        lv_timer_set_period(timer, 30000);
    }
    
    // Update timed game status if active
    sessionManager->updateTimedGameStatus();
    
    // Use high-frequency timer during countdown (final 10 seconds) for precise synchronization
    if (sessionManager->hasActiveTimedStart()) {
        extern painlessMesh mesh;
        uint32_t currentMeshTime = mesh.getNodeTime();
        uint32_t gameStartTime = sessionManager->getGameStartTime();
        int32_t timeUntilStart = (int32_t)(gameStartTime - currentMeshTime) / 1000000; // Convert to seconds
        
        if (timeUntilStart <= 10 && timeUntilStart >= 0) {
            // Final 10 seconds: use 100ms precision for synchronized start
            lv_timer_set_period(timer, 100);
            return;
        }
    }
}

// Format uptime as days/hours/minutes/seconds
static void format_uptime(uint32_t millis_uptime, char* buffer, size_t buffer_size)
{
    uint32_t seconds = millis_uptime / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    uint32_t days = hours / 24;
    
    seconds %= 60;
    minutes %= 60;
    hours %= 24;
    
    if (days > 0) {
        snprintf(buffer, buffer_size, "%ud %uh %um %us", days, hours, minutes, seconds);
    } else if (hours > 0) {
        snprintf(buffer, buffer_size, "%uh %um %us", hours, minutes, seconds);
    } else if (minutes > 0) {
        snprintf(buffer, buffer_size, "%um %us", minutes, seconds);
    } else {
        snprintf(buffer, buffer_size, "%us", seconds);
    }
}

// Info screen updates (every 1 second when info screen is active)
static void info_task_cb(lv_timer_t* timer)
{
    // Only update if info screen is currently displayed
    if (lv_scr_act() != objects.info) {
        return;
    }
    
    char buffer[32];
    
    // Update node count and check mesh health
    if (objects.lbl_info_nodecount) {
        uint32_t nodeCount = 0;
        if (meshInitialized && !config.board.airplaneMode) {
            nodeCount = mesh.getNodeList(false).size();
            // Perform mesh health check while we're updating node count
            checkMeshHealth();
        }
        snprintf(buffer, sizeof(buffer), "%u", nodeCount);
        lv_label_set_text(objects.lbl_info_nodecount, buffer);
    }
    
    // Update free RAM
    if (objects.lbl_info_freeram) {
        uint32_t freeRam = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        if (freeRam >= 1024) {
            snprintf(buffer, sizeof(buffer), "%.1fK", freeRam / 1024.0f);
        } else {
            snprintf(buffer, sizeof(buffer), "%ub", freeRam);
        }
        lv_label_set_text(objects.lbl_info_freeram, buffer);
    }
    
    // Update free PSRAM
    if (objects.lbl_info_freepsram) {
        uint32_t freePsram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
        if (freePsram >= 1024 * 1024) {
            snprintf(buffer, sizeof(buffer), "%.1fM", freePsram / (1024.0f * 1024.0f));
        } else if (freePsram >= 1024) {
            snprintf(buffer, sizeof(buffer), "%.1fK", freePsram / 1024.0f);
        } else {
            snprintf(buffer, sizeof(buffer), "%ub", freePsram);
        }
        lv_label_set_text(objects.lbl_info_freepsram, buffer);
    }
    
    // Update uptime
    if (objects.lbl_info_uptime) {
        format_uptime(millis(), buffer, sizeof(buffer));
        lv_label_set_text(objects.lbl_info_uptime, buffer);
    }
}

void init_lightweight_tasks()
{
    LV_LOG_INFO("[LIGHTWEIGHT] Initializing lightweight timer-based tasks...\n");
    
    // Create lightweight timers (much less RAM than FreeRTOS tasks)
    battery_timer = lv_timer_create(battery_task_cb, 10000, NULL);    // Every 10 seconds
    badgemode_timer = lv_timer_create(badgemode_task_cb, 1000, NULL); // Every 1 second
    idpacket_timer = lv_timer_create(idpacket_task_cb, 10000, NULL);  // Every 10 seconds (dynamic)
    cleanup_timer = lv_timer_create(cleanup_task_cb, 30000, NULL);    // Every 30 seconds
    session_timer = lv_timer_create(session_task_cb, 10000, NULL);    // Dynamic interval
    info_timer = lv_timer_create(info_task_cb, 1000, NULL);           // Every 1 second
    
    // Initially pause info timer (only active when info screen is displayed)
    if (info_timer) {
        lv_timer_pause(info_timer);
    }
    
    // Reset corruption detection
    extern uint32_t g_lastIDPacketTimerExecution;
    extern uint32_t g_idPacketTimerCallCount; 
    g_lastIDPacketTimerExecution = millis();
    g_idPacketTimerCallCount = 0; // Reset callback counter
    
    LV_LOG_INFO("[LIGHTWEIGHT] All timers created successfully\n");
    uint32_t freeHeap = ESP.getFreeHeap();
    LV_LOG_INFO("[LIGHTWEIGHT] Free heap after timer creation: %u bytes\n", freeHeap);
}

void cleanup_lightweight_tasks()
{
    if (battery_timer) { lv_timer_del(battery_timer); battery_timer = nullptr; }
    if (badgemode_timer) { lv_timer_del(badgemode_timer); badgemode_timer = nullptr; }
    if (idpacket_timer) { lv_timer_del(idpacket_timer); idpacket_timer = nullptr; }
    if (cleanup_timer) { lv_timer_del(cleanup_timer); cleanup_timer = nullptr; }
    if (session_timer) { lv_timer_del(session_timer); session_timer = nullptr; }
    if (info_timer) { lv_timer_del(info_timer); info_timer = nullptr; }
}

void start_info_screen_updates()
{
    if (info_timer) {
        lv_timer_resume(info_timer);
        LV_LOG_INFO("[INFO] Started info screen updates\n");
    }
}

void stop_info_screen_updates()
{
    if (info_timer) {
        lv_timer_pause(info_timer);
        LV_LOG_INFO("[INFO] Stopped info screen updates\n");
    }
}

// Keep compatibility with existing config save functionality
void saveConfigAsync()
{
    // Direct synchronous save since we don't have the heavy task queue anymore
    LV_LOG_INFO("[CONFIG] Saving config synchronously...\n");
    extern Config config;
    extern bool saveBoardConfig(Config& config, const char* fileName);
    
    if (saveBoardConfig(config, "L:/default.json")) {
        LV_LOG_INFO("[CONFIG] Config saved successfully\n");
    } else {
        LV_LOG_ERROR("[ERROR] Failed to save config\n");
    }
}