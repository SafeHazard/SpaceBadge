#pragma once
#include <ArduinoJson.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

	// Default ranking system for players
#define RANK_PO 0
#define RANK_ENS 7200
#define RANK_LTJG 14400
#define RANK_LT 28800
#define RANK_LCDR 57600
#define RANK_CDR 86400
#define RANK_CAPT 172800

	//default avatar unlocking scores (per game)
#define AVATAR_UNLOCK_0 0
#define AVATAR_UNLOCK_1 1200
#define AVATAR_UNLOCK_2 2400
#define AVATAR_UNLOCK_3 4800
#define AVATAR_UNLOCK_4 9600
#define AVATAR_UNLOCK_5 14400
#define AVATAR_UNLOCK_6 28800

	constexpr int32_t PLAYER_TIMEOUT = 30000; // 30 seconds;

	String createJSONStringIDPacket();

	/// <summary> Structure for a PSRAM-cached MP3 file </summary>
	struct CachedMP3 {
		const char* name;
		uint8_t* data;
		size_t size;
	};

	/// <summary> Structure for a PSRAM-cached Image (.bin) file </summary>
	struct CachedImage {
		const char* name;          // filename (copied)
		lv_img_dsc_t* img;         // pointer to image in PSRAM
	};

	/// <summary> Game background using CachedImage approach for proper LVGL compatibility </summary>
	struct GameBackground {
		CachedImage cached_img;    // Uses the same layout as other working images
		char current_filename[64]; // Currently loaded background filename
		bool is_loaded;            // Whether a background is currently loaded
	};

	/// <summary>
	/// Represents a line of Queue's text with associated image indices.
	/// </summary>
	struct Line
	{
		char* text;
		uint32_t imgIdx;
		uint32_t expImgIdx;
	};

	/// <summary>
	/// Represents a Queue chat structure containing lines of text, the number of chat entries, and a pointer to the next screen object.
	/// </summary>
	struct QChat
	{
		Line* lines; //queuesExtremelyLongTextBecauseItSoundsJustLikeHimToSayAsLittleAsPossibleWithTheMostAmountOfWordsPossible;
		uint32_t chatCount;
		uint32_t nextScreen;
	};

	/// <summary> Represents the possible states a player can be in. Part of GameState and SessionPacket. </summary>
	enum PlayerStatus
	{
		Idle = 0,       // Connected to the mesh, not in a game
		Hosting = 1,    // In lobby, hosting
		InLobby = 2,    // In lobby (not hosting)
		Ready = 3,      // In lobby, ready to play
		InGame = 4,     // Currently playing
		Offline = 5     // Disconnected; assigned on-client to players who we haven't seen packets from in PLAYER_TIMEOUT ms
	};

	/// <summary> Used to accept or reject a join request. Part of SessionPacket. </summary>
	enum JoinRequest
	{
		Request = 0,        // Requesting to join a game
		Accept = 1,         // Accept the join request
		Reject = 2,         // Reject the join request
		HostAdvertising = 3,// Hosting a game (advertisement)
		BriefingStart = 4,  // Host signals briefing start (requires ACK)
		BriefingAck = 5,    // Client acknowledges briefing start
		TimedGameStart = 6, // Host signals game start at specific mesh time
		ScoreRelay = 7      // Host relays score from one client to other clients
	};

	/// <summary> Contains information about current game we're in (if any). Local. </summary>
	struct GameState
	{
		int32_t sessionID;               // game session ID
		int32_t playerCount;             // number of players in the game
		int32_t playerIDs[5];            // IDs fo other players
		int32_t avatarIDs[5];            // avatar IDs of other players
		PlayerStatus playerStatus[5];    // status of other players
		PlayerStatus myStatus;           // my status
		int32_t scores[5];               // score for other players (percentage)
		int32_t myScore;                 // my score (percentage)
		int32_t timeSinceLastMessage[5]; // time since last message from each player
	};

	/// <summary> Used to advertise a game, admit/reject players from the lobby and show ready status. Unicast. </summary>
	struct SessionPacket
	{
		uint32_t sessionID;     // game session ID
		PlayerStatus status;    // player status
		JoinRequest request;    // join request
		int32_t currentScore;   // current game score (percentage, 0-100) - used during InGame status
		int32_t startTime;      // mesh time when game should start (for TimedGameStart requests)
		int32_t playerCount;    // number of players in the game
	};

	/// <summary> Packet broadcast from each client regularly. Broadcast. </summary>
	struct IDPacket
	{
		int32_t boardID;        // board ID
		int32_t avatarID;       // avatar ID
		char displayName[64];   // display name
		PlayerStatus status;    // player status
		int32_t timeArrived;    // time the packet arrived
		int32_t totalXP;        // total XP
	};

#ifdef __cplusplus
}
#endif
