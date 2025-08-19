#pragma once
#include "global.hpp"
#include "json.h"
#include <vector>
#include <unordered_map>

// PSRAM allocators for session management containers
template<typename T>
using PsramVector = std::vector<T, PsramAllocator<T>>;

template<typename K, typename V>
using PsramUnorderedMap = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, 
    PsramAllocator<std::pair<const K, V>>>;

// Structure to store session info with timing
struct SessionInfo {
    SessionPacket packet;
    uint32_t lastSeen;
    ContactData* contactData; // Pointer to contact data for display name
};

// Player info for lobby management
struct LobbyPlayer {
    uint32_t nodeId;
    ContactData* contactData;
    PlayerStatus status;
    uint32_t lastSeen;
};

// Time-synchronized game start tracking
struct TimedGameStartInfo {
    int gameIndex;
    uint32_t startTime;         // mesh time when game starts
    uint32_t broadcastStart;    // when we started broadcasting
    bool active;
    
    // Constructor for initialization
    TimedGameStartInfo() : gameIndex(0), startTime(0), broadcastStart(0), active(false) {}
};

// Manages incoming session packets and active game sessions
class SessionManager {
private:
    PsramUnorderedMap<uint32_t, SessionInfo> activeSessions; // sessionID -> SessionInfo
    PsramVector<uint32_t> hostingPlayers; // List of players currently hosting
    uint32_t currentSessionId = 0; // Our current session (0 = not in session)
    PlayerStatus myStatus = Idle;
    uint32_t targetHost = 0; // Host we're trying to join (0 = none)
    
    // Time-synchronized game start (for hosts and clients)
    TimedGameStartInfo timedStart; // Current timed game start info
    
public:
    SessionManager();
    
    // Session packet management
    void addOrUpdateSession(const SessionPacket& packet, uint32_t fromNodeId, ContactData* contact = nullptr);
    void addOrUpdateSession(const JsonDocument& doc, uint32_t fromNodeId, ContactData* contact = nullptr); // For TimedGameStart with player_ids
    void removeStaleHostingSessions(uint32_t maxAgeMs = 45000); // Default 45 seconds with 5s heartbeat
    PsramVector<uint32_t> getActiveHosts();
    SessionInfo* getSessionInfo(uint32_t sessionId);
    
    // Host management
    void startHosting(uint32_t sessionId);
    void stopHosting();
    void notifyPlayersHostDisconnected(); // Broadcast disconnection to all lobby players
    bool isHosting() const { return myStatus == Hosting; }
    uint32_t getMySessionId() const { return currentSessionId; }
    
    // Join management
    void joinSession(uint32_t hostSessionId);
    void leaveSession();
    void setReadyStatus(bool ready);
    bool isInLobby() const { return myStatus == InLobby || myStatus == Ready; }
    bool isReady() const { return myStatus == Ready; }
    uint32_t getTargetHost() const { return targetHost; }
    
    // Status management
    PlayerStatus getMyStatus() const { return myStatus; }
    void setMyStatus(PlayerStatus status);
    
    // Lobby management (for hosts)
    void handleJoinRequest(const SessionPacket& packet, uint32_t fromNodeId, ContactData* contact);
    void removeLobbyPlayer(uint32_t nodeId);
    void removeStalePlayersSessions(uint32_t maxAgeMs = 30000); // Default 30 seconds with 5s heartbeat
    PsramVector<LobbyPlayer> getLobbyPlayers() const { return lobbyPlayers; }
    size_t getLobbyPlayerCount() const { return lobbyPlayers.size(); }
    bool isLobbyFull() const { return lobbyPlayers.size() >= 5; }
    
    // Network disconnection handling
    void handleTargetHostDisconnected(uint32_t hostNodeId);
    
    // Time-synchronized game start management
    void startTimedGame(int gameIndex, uint32_t delaySeconds = 15); // Host: schedule game start
    void processTimedGameStart(const SessionPacket& packet, uint32_t fromNodeId); // Client: handle timed start
    void processTimedGameStart(const JsonDocument& doc, uint32_t fromNodeId); // Client: handle timed start with full JSON
    void updateTimedGameStatus(); // Check if it's time to start game
    bool hasActiveTimedStart() const { return timedStart.active; }
    uint32_t getGameStartTime() const { return timedStart.startTime; }
    
    // Cleanup
    void cleanup();
    
    // Debug
    void printStatus();

    PsramVector<LobbyPlayer> lobbyPlayers; // Players in our hosted lobby
};

// Global session manager instance
extern SessionManager* sessionManager;

// Session packet creation functions
String createJSONSessionPacket(uint32_t sessionId, PlayerStatus status, JoinRequest request);
String createJSONSessionPacket(uint32_t sessionId, PlayerStatus status, JoinRequest request, int32_t currentScore);
String createJSONTimedGameStartPacket(uint32_t sessionId, PlayerStatus status, int32_t startTime);
SessionPacket parseJSONSessionPacket(const JsonDocument& doc);

// UI helper functions
const char* rollerStringFromSessions(const PsramVector<uint32_t>& hostIds, const char* defaultString = "No Crews Found");
void updateJoinRoller();
void updateHostPlayerList();
void checkHostStartButtonState();

// Initialize session management
void init_session_manager();