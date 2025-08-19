#include "memory_manager.h"
#include "session.h"

// Static member definitions
uint8_t* MemoryManager::reservedBuffer = nullptr;
bool MemoryManager::bufferAllocated = false;
uint32_t MemoryManager::lastCleanupTime = 0;

// Global instance
MemoryManager memoryManager;

// External references
extern GameState gameState;
extern SessionManager* sessionManager;

bool MemoryManager::initReservedBuffer() {
    if (bufferAllocated) return true;
    
    reservedBuffer = (uint8_t*)heap_caps_malloc(RESERVED_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (reservedBuffer) {
        bufferAllocated = true;
        LV_LOG_INFO("[MEM] Reserved %d bytes PSRAM buffer allocated\n", RESERVED_BUFFER_SIZE);
        return true;
    }
    
    LV_LOG_INFO("[MEM] Failed to allocate reserved PSRAM buffer\n");
    return false;
}

void MemoryManager::releaseReservedBuffer() {
    if (bufferAllocated && reservedBuffer) {
        heap_caps_free(reservedBuffer);
        reservedBuffer = nullptr;
        bufferAllocated = false;
        LV_LOG_INFO("[MEM] Emergency: Released reserved buffer (%d bytes)\n", RESERVED_BUFFER_SIZE);
    }
}

MemoryState MemoryManager::getMemoryState() {
    uint32_t freeHeap = getFreeHeap();
    
    // In multiplayer, require higher memory threshold
    uint32_t lowThreshold = inMultiplayerMode() ? MULTIPLAYER_PROTECT_THRESHOLD : LOW_MEM_THRESHOLD;
    
    if (freeHeap < CRITICAL_MEM_THRESHOLD) {
        return MEM_CRITICAL;
    } else if (freeHeap < lowThreshold) {
        return MEM_LOW;
    } else {
        return MEM_NORMAL;
    }
}

bool MemoryManager::shouldProcessPacket(PacketPriority priority, PlayerStatus gameStatus) {
    MemoryState memState = getMemoryState();
    
    // Always process critical packets
    if (priority == PRIORITY_CRITICAL) {
        return true;
    }
    
    // Memory state based filtering
    switch (memState) {
        case MEM_NORMAL:
            return true; // Process all packets
            
        case MEM_LOW:
            // In low memory, only process high priority and above
            return (priority <= PRIORITY_HIGH);
            
        case MEM_CRITICAL:
            // In critical memory, only process critical packets
            // Exception: if reserved buffer available, release it and allow one high priority
            if (priority == PRIORITY_HIGH && bufferAllocated) {
                releaseReservedBuffer();
                return true;
            }
            return false;
            
        default:
            return false;
    }
}

PacketPriority MemoryManager::getPacketPriority(const char* type, uint32_t from, PlayerStatus gameStatus) {
    if (!type) return PRIORITY_LOW;
    
    // Session packets are always critical when in game
    if (strcmp(type, "session") == 0) {
        if (gameStatus == InGame || gameStatus == InLobby || gameStatus == Hosting) {
            return PRIORITY_CRITICAL;
        }
        return PRIORITY_HIGH;
    }
    
    // ID packets priority depends on game status and sender
    if (strcmp(type, "id") == 0) {
        // If in game, check if sender is a game participant
        if (gameStatus == InGame) {
            for (int i = 0; i < 4; i++) {
                if (gameState.playerIDs[i] == from) {
                    return PRIORITY_HIGH; // Game participant
                }
            }
            return PRIORITY_LOW; // Not a game participant
        }
        
        // If in lobby, ID packets are high priority
        if (gameStatus == InLobby || gameStatus == Hosting) {
            return PRIORITY_HIGH;
        }
        
        return PRIORITY_NORMAL; // Regular ID packet when idle
    }
    
    return PRIORITY_LOW; // Unknown packet type
}

void MemoryManager::performCleanup() {
    uint32_t now = millis();
    if (now - lastCleanupTime < CLEANUP_INTERVAL) {
        return; // Too soon for cleanup
    }
    
    lastCleanupTime = now;
    
    MemoryState memState = getMemoryState();
    uint32_t freeBefore = getFreeHeap();
    
    if (memState >= MEM_LOW) {
        // Force garbage collection
        ESP.getMinFreeHeap(); // This can trigger GC
        
        uint32_t freeAfter = getFreeHeap();
        LV_LOG_INFO("[MEM] Cleanup: %u -> %u bytes (+%d)\n", 
                     freeBefore, freeAfter, (int)(freeAfter - freeBefore));
    }
}

bool MemoryManager::inMultiplayerMode() {
    return (gameState.myStatus == InGame || gameState.myStatus == InLobby || gameState.myStatus == Hosting) 
           && gameState.playerCount > 1;
}

void MemoryManager::logMemoryStats(const char* context) {
    uint32_t freeHeap = getFreeHeap();
    uint32_t freePsram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    MemoryState state = getMemoryState();
    
    const char* stateStr = (state == MEM_NORMAL) ? "NORMAL" : 
                          (state == MEM_LOW) ? "LOW" : "CRITICAL";
    
    if (context) {
        LV_LOG_INFO("[MEM] %s: Heap=%u, PSRAM=%u, State=%s, Reserved=%s\n", 
                     context, freeHeap, freePsram, stateStr, 
                     bufferAllocated ? "YES" : "NO");
    } else {
        LV_LOG_INFO("[MEM] Heap=%u, PSRAM=%u, State=%s, Reserved=%s\n", 
                     freeHeap, freePsram, stateStr, 
                     bufferAllocated ? "YES" : "NO");
    }
}