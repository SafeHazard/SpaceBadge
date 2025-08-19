#include "mp3s.h"

CachedMP3* mp3_beeps = nullptr;
size_t mp3_beepCount = 0;

CachedMP3* game_sfx = nullptr;
size_t mp3_sfxCount = 0;

void preload_all_beeps() {
    preload_mp3_directory("/mp3/beeps", &mp3_beeps, &mp3_beepCount);
}

bool preload_all_game_sfx() {
    return preload_mp3_directory("/mp3/effects", &game_sfx, &mp3_sfxCount);
}

void free_all_beeps() {
    if (mp3_beeps) {
		free_cached_mp3s(mp3_beeps, mp3_beepCount);
    }
}

void play_random_beep() 
{
#ifndef _WIN32
    int32_t random_index = rand() % mp3_beepCount;
    Play_Cached_Mp3(mp3_beeps[random_index]);
#endif
}

// Set the volume for the audio player
void setVolume(float vol) {
#ifndef _WIN32
    if (vol >= 0.0f && vol <= 100.0f) {
        set_copier_volume(vol/100.0f);
    }
    else
    {
		set_copier_volume(0.0f); // Set to 0 if out of range
    }
#endif
}

bool init_audio(float volume = 100.0f) 
{
	// Check if the volume is within the valid range
    if(0.0f > volume || volume > 100.0f) {
        volume = 0.0f;
	}

    // Initialize the audio system
    if (!audio_init(volume/100.0f)) {
        Serial.println("Failed to initialize audio system");
        return false;
    }
    return true;
}