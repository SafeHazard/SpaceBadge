#pragma once
#include <AudioLogger.h>
#include <AudioTools/CoreAudio/AudioOutput.h>
#include <AudioTools/Disk/AudioSource.h>
#include <AudioTools/CoreAudio/VolumeControl.h>
#include <AudioTools/CoreAudio/I2SStream.h>
#include <AudioTools/AudioCodecs/CodecMP3Helix.h>
#include <AudioTools/AudioCodecs/AudioEncoded.h>
#include <AudioTools/CoreAudio/StreamCopy.h>
#include <AudioTools/CoreAudio/VolumeStream.h>
// #include <AudioTools.h> //removed since audio_tools::Task conflicts with painlessMesh::Task and we can't sort out how to disambiguate them
#include <vector>
#include <lvgl.h>
#include "global.hpp"
#include "LittleFS.h"

// Hardware pins for your Waveshare ESP32-S3
#define I2S_BCLK 48
#define I2S_LRC 38
#define I2S_DOUT 47

// PSRAM Reader class
class PSRAMReader : public AudioStream {
private:
    uint8_t* buffer;
    size_t totalSize;
    size_t currentPos;
    bool active;

public:
    PSRAMReader();

    void setBuffer(uint8_t* buf, size_t size);

    int available() override;

    size_t readBytes(uint8_t* data, size_t len) override;

    void restart();
};

/**
* Loads all `.mp3` files in the given directory into PSRAM.
*
* @param dirPath Path to the folder, e.g., "/mp3/beeps"
* @param outArray Pointer to receive dynamically allocated array of CachedMP3s
* @param outCount Pointer to receive number of files loaded
* @return true if any MP3s were loaded; false otherwise
*/
bool preload_mp3_directory(const char* dirPath, CachedMP3** outArray, size_t* outCount);
bool free_cached_mp3s(CachedMP3* list, size_t count);

bool Play_Cached_Mp3(const CachedMP3& track);
bool Play_Cached_Mp3(const CachedMP3* list, size_t count, const char* filename);
bool audio_init(float volume);
bool set_copier_volume(float vol);  // Volume range: 0.0�1.0

// Audio system diagnostics and control
void audio_print_status();       // Print current audio system status
void audio_reset_health();       // Manually reset audio system health
bool audio_is_healthy();         // Check if audio system is healthy

void stopPlaying();  // Stop any currently playing audio
