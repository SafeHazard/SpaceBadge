#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "Audio_PCM5101.h"

#ifdef __cplusplus
extern "C" {
#endif

	//extern CachedMP3 mp3_beep_cache[];
	//extern size_t mp3_beep_count;

	extern CachedMP3* mp3_beeps;

	void preload_all_beeps();
    bool preload_all_game_sfx();
	void free_all_beeps();
	void play_random_beep();
	bool init_audio(float volume);
	void setVolume(float vol);  // 0.0 to 1.0

#ifdef __cplusplus
}
#endif
