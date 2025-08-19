#pragma once
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Team bonus calculation result structure
struct TeamBonusResult {
    int32_t bonusXP;            // Additional XP earned from team performance
    int eligibleTeammates;      // Number of teammates who met criteria (99%+ score and connected)
    int totalTeammates;         // Total number of teammates (excluding self)
    int disconnectedCount;      // Number of teammates who met score criteria but were disconnected
    int lowScoreCount;          // Number of teammates who were connected but scored < 99%
};

void showIntroAtBoot();
void showIntro(lv_event_t* e);
void showOutro(lv_event_t* e);
void Skip(lv_event_t* e);
void NextLine(lv_event_t* e);
void showGameConvo(int gameId, int difficulty);
void showPreGameBriefing(int gameNumber, int countdownSeconds);
void showPostGameSummary(int gameNumber, int baseXP, int bonusXP, int previousAvatarCount, int newAvatarCount, const char* previousRank, const char* newRank, TeamBonusResult teamBonusDetails);
void startGameAfterBriefing(int gameNumber, bool hostMode);
void clearBriefingState();
void checkOutroUnlocks();

#ifdef __cplusplus
}
#endif