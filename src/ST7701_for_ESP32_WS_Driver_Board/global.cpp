#include "global.hpp"
#include "json.h"
#include <painlessMesh.h>

extern Config config;
extern GameState gameState;

String createJSONStringIDPacket()
{
	JsonDocument doc;
	String result = "";

	// Enhanced debug: Always log when creating ID packet for troubled boards
	LV_LOG_INFO("[JSON_CREATE] Creating ID packet - nodeId: %u, displayName: '%s', avatar: %d, status: %d, totalXP: %d\n", 
		config.user.nodeId, config.user.displayName, config.user.avatar, (int)gameState.myStatus, config.user.totalXP);

	doc["type"] = "id";
	doc["boardID"] = config.user.nodeId;
	doc["displayName"] = config.user.displayName;
	doc["avatarID"] = config.user.avatar;
	doc["status"] = (int)gameState.myStatus;
	doc["totalXP"] = config.user.totalXP;

	size_t bytesWritten = serializeJson(doc, result);
	LV_LOG_INFO("[JSON_CREATE] serializeJson returned %zu bytes, result length: %d, nodeId: %u\n", 
	             bytesWritten, result.length(), config.user.nodeId);
	
	if (bytesWritten == 0 || result.isEmpty())
	{
		LV_LOG_ERROR("[ERROR] Failed to serialize ID packet JSON, nodeId: %u\n", config.user.nodeId);
		doc.clear();
		return "";
	}

	LV_LOG_INFO("[JSON_CREATE] ID packet JSON created successfully: %s\n", result.c_str());
	doc.clear();
	return result;
}

// Implementation of game_parent::broadcastScore
#include "game_parent.h"
#include "session.h"

void game_parent::broadcastScore(int scorePercent) {
    // Only broadcast if we're in a multiplayer game session
    if (gameState.myStatus != InGame || gameState.sessionID == 0) {
        return;  // Not in game or solo mode - no need to broadcast
    }
    
    // Additional check: ensure SessionManager exists and we're in a session
    extern SessionManager* sessionManager;
    if (!sessionManager || sessionManager->getMySessionId() == 0) {
        return;  // No active session
    }
    
    // Throttle broadcasts to avoid spam (max once every 2 seconds)
    static uint32_t lastBroadcast = 0;
    uint32_t now = millis();
    if (now - lastBroadcast < 2000) {
        return;
    }
    lastBroadcast = now;
    
    // Update our own score in gameState
    gameState.myScore = scorePercent;
    
    // Create score update packet
    String scorePacket = createJSONSessionPacket(gameState.sessionID, InGame, Request, scorePercent);
    
    //LV_LOG_ERROR("[SCORE_MESH] Broadcasting score %d%% to ALL players in session %u\n", 
    //              scorePercent, gameState.sessionID);
    
    if (scorePacket.isEmpty()) {
        LV_LOG_ERROR("[SCORE] Failed to create score packet\n");
        return;
    }
    
    // FULL MESH: Send to ALL other players in the session (except ourselves)
    extern painlessMesh mesh;
    extern Config config;
    
    //LV_LOG_ERROR("[SCORE_MESH] Full mesh broadcast: sending to all %d players\n", gameState.playerCount);
    //for (int i = 0; i < gameState.playerCount && i < 5; i++) {
        //LV_LOG_ERROR("[SCORE_MESH]   playerIDs[%d] = %u\n", i, gameState.playerIDs[i]);
    //}
    
    int packetsSent = 0;
    for (int i = 0; i < 5; i++) {
        uint32_t targetNodeId = gameState.playerIDs[i];
        
        // Skip empty slots and ourselves
        if (targetNodeId == 0 || targetNodeId == config.user.nodeId) {
            continue;
        }
        
        bool sent = mesh.sendSingle(targetNodeId, scorePacket);
        if (sent) {
            packetsSent++;
        }
        
        //LV_LOG_ERROR("[SCORE_MESH] Sent score %d%% to player %u: %s\n", 
        //             scorePercent, targetNodeId, sent ? "SUCCESS" : "FAILED");
    }
    
    // Periodic summary logging
    static uint32_t lastSummaryLog = 0;
    if (now - lastSummaryLog > 30000) {  // Every 30 seconds
        LV_LOG_INFO("[SCORE] Score broadcast summary: %d%% sent to %d/%d players\n", 
                     scorePercent, packetsSent, gameState.playerCount - 1);
        lastSummaryLog = now;
    }
}
