#pragma once
#include <esp_heap_caps.h>
#include <stdint.h>
#include <Arduino.h>
#include "global.hpp"

// Memory management constants
#define CRITICAL_MEM_THRESHOLD 8000    // Critical memory threshold (8KB)
#define LOW_MEM_THRESHOLD 15000        // Low memory threshold (15KB) 
#define RESERVED_BUFFER_SIZE 32768     // 32KB reserved PSRAM buffer
#define MULTIPLAYER_PROTECT_THRESHOLD 20000  // Extra protection in multiplayer (20KB)

// Packet priorities for memory-aware processing
enum PacketPriority {
    PRIORITY_CRITICAL = 0,  // Session/game packets - always process
    PRIORITY_HIGH = 1,      // ID packets from game participants
    PRIORITY_NORMAL = 2,    // Regular ID packets
    PRIORITY_LOW = 3        // Other packets
};

// Memory state for decision making
enum MemoryState {
    MEM_NORMAL,     // >20KB free, process all packets
    MEM_LOW,        // 15-20KB free, prioritize packets
    MEM_CRITICAL    // <15KB free, only critical packets
};

class MemoryManager {
private:
    static uint8_t* reservedBuffer;
    static bool bufferAllocated;
    static uint32_t lastCleanupTime;
    static const uint32_t CLEANUP_INTERVAL = 30000; // 30 seconds
    
public:
    // Initialize reserved PSRAM buffer
    static bool initReservedBuffer();
    
    // Release reserved buffer in emergency
    static void releaseReservedBuffer();
    
    // Get current memory state
    static MemoryState getMemoryState();
    
    // Check if packet should be processed based on priority and memory state
    static bool shouldProcessPacket(PacketPriority priority, PlayerStatus gameStatus);
    
    // Determine packet priority based on type and sender
    static PacketPriority getPacketPriority(const char* type, uint32_t from, PlayerStatus gameStatus);
    
    // Perform memory cleanup if needed
    static void performCleanup();
    
    // Get free heap size
    static uint32_t getFreeHeap() { return ESP.getFreeHeap(); }
    
    // Check if we're in multiplayer mode
    static bool inMultiplayerMode();
    
    // Log memory statistics
    static void logMemoryStats(const char* context = nullptr);
};

// Global memory manager instance
extern MemoryManager memoryManager;