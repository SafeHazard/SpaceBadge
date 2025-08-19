#include "Audio_PCM5101.h"
#include "json.h"

/////////////////
// Handles audio playback using PCM5101 DAC
// MP3s must be 44khz, stere or else they may not play correctly.
// Only plays one MP3 at a time. 
// If a second MP3 is requested while one is playing, it will stop the first and start the second.
/////////////////

// Audio components
audio_tools::I2SStream i2s;
audio_tools::VolumeStream volumeStream;
audio_tools::MP3DecoderHelix decoder;
PSRAMReader psramReader;
audio_tools::EncodedAudioStream decodedStream(&volumeStream, &decoder);
audio_tools::StreamCopy copier(decodedStream, psramReader);


bool isPlaying = false;
bool audioInitialized = false;

// ENHANCED SPAM PROTECTION - Multiple defensive layers against memory corruption
static uint32_t lastPlayTime = 0;
static uint32_t playCallCount = 0;
static const uint32_t MIN_PLAY_INTERVAL_MS = 75;   // Increased from 50ms to 75ms - more conservative
static const uint32_t MAX_CALLS_PER_SECOND = 6;    // Reduced from 10 to 6 - much more conservative  
static const uint32_t MAX_CALLS_PER_MINUTE = 120;  // NEW: Prevent sustained spam over longer periods
static uint32_t callsInLastSecond = 0;
static uint32_t callsInLastMinute = 0;
static uint32_t lastSecondStart = 0;
static uint32_t lastMinuteStart = 0;
static bool audioSystemHealthy = true;
static uint32_t consecutiveRejects = 0;             // NEW: Track rejected calls
static uint32_t totalPlayAttempts = 0;              // NEW: Track all attempts for diagnostics

// MEMORY PRESSURE DETECTION - Additional protection layer
static uint32_t lastMemoryCheck = 0;
static const uint32_t MEMORY_CHECK_INTERVAL = 1000; // Check every second
static const uint32_t MIN_FREE_MEMORY = 15000;      // Require 15KB free RAM
static bool memoryPressureMode = false;

static void AudioTask(void* pvParameters)
{
	while (true)
	{
		if (isPlaying)
		{
			if (psramReader.available() > 0)
			{
				copier.copyBytes(2048);  // copy up to 2048 bytes at a time
				//copier.copy();  // this calls psramReader -> decoder -> volume -> i2s
				// NOTE: this runs fast, so we need to throttle externally
				vTaskDelay(pdMS_TO_TICKS(10));  // 100 Hz
			}
			else
			{
				LV_LOG_INFO("Playback finished");
				isPlaying = false;
				decodedStream.end();
			}
		}
		else
		{
			// Health recovery check - restore audio system if it's been unhealthy for too long
			static uint32_t lastHealthCheck = 0;
			uint32_t now = millis();
			if (!audioSystemHealthy && (now - lastHealthCheck > 10000)) {  // Check every 10 seconds
				LV_LOG_INFO("Attempting to restore audio system health");
				audioSystemHealthy = true;  // Reset health flag
				callsInLastSecond = 0;      // Reset call counters
				playCallCount = 0;
				lastPlayTime = 0;
				lastHealthCheck = now;
			} else if (now - lastHealthCheck > 10000) {
				lastHealthCheck = now;  // Update check time even when healthy
			}
			
			vTaskDelay(pdMS_TO_TICKS(5));  // idle tick
		}
	}
}

void stopPlaying()
{
	if (isPlaying)
	{
		LV_LOG_INFO("Stopping current playback to play new track '%s'", track.name);
		try
		{
			isPlaying = false;
			decodedStream.end();
		}
		catch (const std::exception& e)
		{
			LV_LOG_ERROR("Error stopping playback: %s", e.what());
		}
	}
}

bool audio_init(float volume = 1.0f)
{
	// Check PSRAM
	if (psramFound()) 
	{
		LV_LOG_INFO("PSRAM found: %d bytes free\n", ESP.getFreePsram());
	}
	else 
	{
		LV_LOG_ERROR("PSRAM not found!\n");
		return false;
	}

	// Configure I2S output
	auto i2sConfig = i2s.defaultConfig(TX_MODE);
	i2sConfig.pin_bck = I2S_BCLK;
	i2sConfig.pin_ws = I2S_LRC;
	i2sConfig.pin_data = I2S_DOUT;
	i2sConfig.sample_rate = 44100;
	i2sConfig.bits_per_sample = 16;
	i2sConfig.channels = 2;
	i2sConfig.buffer_count = 8;
	i2sConfig.buffer_size = 512;

	// Initialize I2S output
	if (!i2s.begin(i2sConfig)) {
		LV_LOG_WARN("I2S initialization failed!");
		return false;
	}
	LV_LOG_INFO("I2S initialized");

	// Initialize VolumeStream
	volumeStream.setOutput(i2s);
	volumeStream.setVolume(volume);
	LV_LOG_INFO("Volume set to %f", volumeStream.volume);
	audioInitialized = true;

	// Initialize decoder
	decoder.begin();
	LV_LOG_INFO("MP3 decoder initialized");

	// Set up background task
	xTaskCreatePinnedToCore(
		AudioTask,         // Function to call
		"AudioTask",       // Name
		4096,              // Stack size
		NULL,              // Parameters
		1,                 // Priority (1 is fine)
		NULL,              // Task handle
		0                  // Core 0
	);
	return true;
}

// PSRAM Reader Class Implementation
PSRAMReader::PSRAMReader()
	: buffer(nullptr), totalSize(0), currentPos(0), active(false) {
}

void PSRAMReader::setBuffer(uint8_t* buf, size_t size) {
	buffer = buf;
	totalSize = size;
	currentPos = 0;
	active = true;
}

int PSRAMReader::available() {
	if (!active || !buffer) return 0;
	return totalSize - currentPos;
}

size_t PSRAMReader::readBytes(uint8_t* data, size_t len) {
	if (!active || !buffer || currentPos >= totalSize) return 0;

	size_t remaining = totalSize - currentPos;
	size_t toRead = min(len, remaining);

	if (toRead > 0) {
		memcpy(data, buffer + currentPos, toRead);
		currentPos += toRead;
	}

	return toRead;
}

void PSRAMReader::restart() {
	currentPos = 0;
}

// Function to preload MP3 files from a directory in LittleFS into PSRAM
bool preload_mp3_directory(const char* dirPath, CachedMP3** outArray, size_t* outCount) {
	if (!dirPath || !outArray || !outCount) return false;

	File dir = LittleFS.open(dirPath);
	if (!dir || !dir.isDirectory()) {
		LV_LOG_ERROR("Invalid MP3 directory: %s", dirPath);
		return false;
	}

	std::vector<CachedMP3, PsramAllocator<CachedMP3>> cacheList;

	while (true) {
		File entry = dir.openNextFile();
		if (!entry) break;

		if (!entry.isDirectory()) {
			String name = entry.name();
			if (name.endsWith(".mp3")) {
				String fullPath = String(dirPath) + "/" + name;
				File mp3File = LittleFS.open(fullPath, "r");
				if (!mp3File) {
					LV_LOG_WARN("Failed to open MP3 file: %s", fullPath.c_str());
					continue;
				}

				size_t fileSize = mp3File.size();
				uint8_t* buf = (uint8_t*)ps_malloc(fileSize);
				if (!buf) {
					LV_LOG_ERROR("Out of PSRAM while loading %s", fullPath.c_str());
					mp3File.close();
					continue;
				}

				size_t bytesRead = mp3File.read(buf, fileSize);
				mp3File.close();

				if (bytesRead != fileSize) {
					LV_LOG_ERROR("Read error on %s: got %d/%d", fullPath.c_str(), bytesRead, fileSize);
					free(buf);
					continue;
				}

				cacheList.push_back({
					.name = strdup(name.c_str()),
					.data = buf,
					.size = fileSize
					});

				LV_LOG_INFO("Cached %s (%d bytes)", fullPath.c_str(), fileSize);
			}
		}
	}

	dir.close();

	if (cacheList.empty()) {
		*outArray = nullptr;
		*outCount = 0;
		return false;
	}

	*outCount = cacheList.size();
	*outArray = (CachedMP3*)heap_caps_malloc(sizeof(CachedMP3) * cacheList.size(), MALLOC_CAP_SPIRAM);
	if (!*outArray) {
		LV_LOG_ERROR("Failed to allocate memory for CachedMP3 array");
		*outCount = 0;
		return false;
	}
	memcpy(*outArray, cacheList.data(), sizeof(CachedMP3) * cacheList.size());
	return true;
}

bool free_cached_mp3s(CachedMP3* list, size_t count) {
	if (!list || count == 0) return false;
	for (size_t i = 0; i < count; ++i) {
		if (list[i].data) {
			free(list[i].data);
			list[i].data = nullptr;
		}
		if (list[i].name) {
			free((void*)list[i].name);
			list[i].name = nullptr;
		}
	}
	free(list);
	return true;
}

// Function to cache an MP3 file from LittleFS into PSRAM
bool cacheMP3FromLittleFS(const char* filename)
{
	LV_LOG_INFO("Attempting to cache: %s\n", filename);

	// PSRAM buffer for cached MP3
	uint8_t* cachedMP3 = nullptr;
	size_t cachedSize = 0;

	// Open the file from LittleFS using Arduino's File API
	File file = LittleFS.open(filename, "r");
	if (!file) {
		LV_LOG_ERROR("Failed to open file: %s\n", filename);
		return false;
	}

	size_t fileSize = file.size();
	LV_LOG_INFO("File size: %d bytes\n", fileSize);

	// Allocate buffer in PSRAM
	cachedMP3 = (uint8_t*)ps_malloc(fileSize);
	if (!cachedMP3) {
		LV_LOG_ERROR("PSRAM allocation failed!\n");
		file.close();
		return false;
	}

	// Read entire file to PSRAM
	size_t bytesRead = file.readBytes((char*)cachedMP3, fileSize);
	file.close();

	if (bytesRead != fileSize) {
		LV_LOG_ERROR("Read mismatch: expected %d, got %d\n", fileSize, bytesRead);
		free(cachedMP3);
		cachedMP3 = nullptr;
		return false;
	}

	cachedSize = fileSize;
	LV_LOG_INFO("Successfully cached %d bytes to PSRAM\n", cachedSize);
	LV_LOG_INFO("Free PSRAM after caching: %d bytes\n", ESP.getFreePsram());

	return true;
}

/// <summary>
/// Plays a cached MP3 track from PSRAM with spam protection.
/// MP3 must be 44khz, stereo or else it may not play correctly.
/// Implements rate limiting to prevent crashes from repeated calls.
/// </summary>
/// <param name="track">A CachedMP3 object</param>
/// <returns>true if playback starts, false if blocked or track data is missing</returns>
bool Play_Cached_Mp3(const CachedMP3& track)
{
	if (!track.data || track.size == 0)
	{
		LV_LOG_ERROR("Invalid track data.");
		return false;
	}

	// ENHANCED SPAM PROTECTION: Multi-layer defense against memory corruption
	uint32_t now = millis();
	totalPlayAttempts++;  // Track all attempts for diagnostics
	
	// Check if audio system is healthy
	if (!audioSystemHealthy) {
		static uint32_t lastHealthWarning = 0;
		if (now - lastHealthWarning > 5000) {  // Warn every 5 seconds
			LV_LOG_WARN("Audio system unhealthy - rejecting play request (attempt #%u)", totalPlayAttempts);
			lastHealthWarning = now;
		}
		consecutiveRejects++;
		return false;
	}
	
	// MEMORY PRESSURE DETECTION - Prevent calls when RAM is critically low
	if (now - lastMemoryCheck >= MEMORY_CHECK_INTERVAL) {
		uint32_t freeRam = ESP.getFreeHeap();
		memoryPressureMode = (freeRam < MIN_FREE_MEMORY);
		lastMemoryCheck = now;
		
		if (memoryPressureMode) {
			LV_LOG_WARN("Memory pressure detected - %u bytes free (need %u+)", 
						freeRam, MIN_FREE_MEMORY);
		}
	}
	
	if (memoryPressureMode) {
		LV_LOG_INFO("Audio call blocked due to memory pressure");
		consecutiveRejects++;
		return false;
	}
	
	// Reset call counters at time boundaries
	if (now - lastSecondStart >= 1000) {
		callsInLastSecond = 0;
		lastSecondStart = now;
	}
	
	if (now - lastMinuteStart >= 60000) {
		callsInLastMinute = 0;
		lastMinuteStart = now;
	}
	
	// Check minimum interval between calls (75ms minimum)
	if (now - lastPlayTime < MIN_PLAY_INTERVAL_MS) {
		LV_LOG_INFO("Audio call blocked - too soon after last call (%ums < %ums)", 
					 now - lastPlayTime, MIN_PLAY_INTERVAL_MS);
		consecutiveRejects++;
		return false;
	}
	
	// Check calls per second limit (6 calls/sec max)
	if (callsInLastSecond >= MAX_CALLS_PER_SECOND) {
		static uint32_t lastSpamWarning = 0;
		if (now - lastSpamWarning > 1000) {  // Warn once per second
			LV_LOG_WARN("Audio spam detected - rejecting play request (call #%u this second)", 
						callsInLastSecond);
			lastSpamWarning = now;
			
			// If spam continues, mark audio system as unhealthy
			if (callsInLastSecond > MAX_CALLS_PER_SECOND * 2) {
				LV_LOG_ERROR("Severe audio spam detected - marking audio system unhealthy");
				audioSystemHealthy = false;
				// System will recover after 10 seconds
				// (see audio health check in AudioTask)
			}
		}
		consecutiveRejects++;
		return false;
	}
	
	// NEW: Check calls per minute limit (120 calls/min max)
	if (callsInLastMinute >= MAX_CALLS_PER_MINUTE) {
		static uint32_t lastMinuteWarning = 0;
		if (now - lastMinuteWarning > 10000) {  // Warn every 10 seconds
			LV_LOG_WARN("Sustained audio spam detected - rejecting play request (call #%u this minute)", 
						callsInLastMinute);
			lastMinuteWarning = now;
		}
		consecutiveRejects++;
		return false;
	}
	
	// Update all counters - call accepted
	lastPlayTime = now;
	playCallCount++;
	callsInLastSecond++;
	callsInLastMinute++;
	consecutiveRejects = 0;  // Reset consecutive rejects on successful call
	
	// If already playing, stop it before playing new one
	if (isPlaying)
	{
		LV_LOG_INFO("Stopping current playback to play new track '%s'", track.name);
		try {
			isPlaying = false;
			decodedStream.end();
		} catch (...) {
			LV_LOG_ERROR("Exception during audio cleanup - marking system unhealthy");
			audioSystemHealthy = false;
			return false;
		}
		vTaskDelay(pdMS_TO_TICKS(25));  // Increased delay for better cleanup
	}

	try {
		psramReader.setBuffer(track.data, track.size);
		decodedStream.begin();  // Do NOT call if already playing
		
		AudioInfo info = decoder.audioInfo();
		LV_LOG_INFO("MP3 '%s' Decoder reports: %d Hz, %d ch, %d bits (attempt #%u)",
			track.name, info.sample_rate, info.channels, info.bits_per_sample, totalPlayAttempts);
		isPlaying = true;
		return true;
	} catch (...) {
		LV_LOG_ERROR("Exception during audio initialization for '%s' - marking system unhealthy", track.name);
		audioSystemHealthy = false;
		return false;
	}
}


// Play Cached MP3 by filename
bool Play_Cached_Mp3(const CachedMP3* list, size_t count, const char* filename) {
	if (!list || !filename || count == 0) return false;

	for (size_t i = 0; i < count; ++i) {
		if (strcmp(list[i].name, filename) == 0) {
			return Play_Cached_Mp3(list[i]);
		}
	}

	LV_LOG_WARN("MP3 not found: %s", filename);
	return false;
}

bool set_copier_volume(float vol) {
	if (!audioInitialized) {
		LV_LOG_WARN("Audio not initialized, cannot set volume");
		return false;
	}
	return volumeStream.setVolume(vol);
}

// Audio system diagnostics and control functions
void audio_print_status() {
	uint32_t now = millis();
	uint32_t freeRam = ESP.getFreeHeap();
	LV_LOG_INFO("=== Enhanced Audio System Status ===");
	LV_LOG_INFO("System healthy: %s", audioSystemHealthy ? "true" : "false");
	LV_LOG_INFO("Audio initialized: %s", audioInitialized ? "true" : "false");
	LV_LOG_INFO("Currently playing: %s", isPlaying ? "true" : "false");
	LV_LOG_INFO("Memory pressure mode: %s (%u bytes free, need %u+)", 
				memoryPressureMode ? "ACTIVE" : "normal", freeRam, MIN_FREE_MEMORY);
	LV_LOG_INFO("Total play attempts: %u (successful: %u)", totalPlayAttempts, playCallCount);
	LV_LOG_INFO("Consecutive rejects: %u", consecutiveRejects);
	LV_LOG_INFO("Calls this second: %u/%u", callsInLastSecond, MAX_CALLS_PER_SECOND);
	LV_LOG_INFO("Calls this minute: %u/%u", callsInLastMinute, MAX_CALLS_PER_MINUTE);
	LV_LOG_INFO("Time since last play: %u ms", now - lastPlayTime);
	LV_LOG_INFO("Rate limits: %u ms minimum, %u calls/sec, %u calls/min", 
				MIN_PLAY_INTERVAL_MS, MAX_CALLS_PER_SECOND, MAX_CALLS_PER_MINUTE);
	LV_LOG_INFO("====================================");
}

void audio_reset_health() {
	LV_LOG_INFO("Manually resetting enhanced audio system health");
	audioSystemHealthy = true;
	memoryPressureMode = false;
	callsInLastSecond = 0;
	callsInLastMinute = 0;
	playCallCount = 0;
	consecutiveRejects = 0;
	totalPlayAttempts = 0;
	lastPlayTime = 0;
	uint32_t now = millis();
	lastSecondStart = now;
	lastMinuteStart = now;
	lastMemoryCheck = now;
	LV_LOG_INFO("All audio protection counters reset");
}

bool audio_is_healthy() {
	return audioSystemHealthy && audioInitialized;
}

