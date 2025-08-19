#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

// Function to populate the avatar roller with unlocked avatars
void populate_avatar_roller(void);

// Callback for when avatar roller selection changes
void avatarRollerChanged(lv_event_t* e);

// Callback for when avatar roller is released - saves config to disk
void avatarRollerReleased(lv_event_t* e);

// Screen loaded callback for avatar screen
void screenAvatarLoaded(lv_event_t* e);

// Calculate how many avatars should be unlocked based on game XP
int calculateUnlockedAvatars();

// Update the avatar unlock count based on current game XP and refresh UI
void updateAvatarUnlocks();

#ifdef __cplusplus
}
#endif