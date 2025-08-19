#pragma once
#include <stdint.h>
#include <lvgl.h>
#include "global.hpp"
#include "json.h"

#ifdef __cplusplus
extern "C" {
#endif

	bool preload_image_directory(const char* dirPath, CachedImage** outArray, size_t* outCount);
	//bool preload_image_directory_with_progress(const char* dirPath, CachedImage** outArray, size_t* outCount, 
	//                                         lv_obj_t* progressBar, lv_obj_t* progressLabel);
	bool preload_all_image_directories_with_progress(lv_obj_t* progressBar, lv_obj_t* progressLabel, lv_obj_t* shuttleImg = nullptr);
	const lv_img_dsc_t* get_cached_image(CachedImage* list, size_t count, const char* filename);
	void free_cached_images(CachedImage* images, size_t count);

	// Game background PSRAM reservation functions
	bool init_game_background_psram();
	bool load_game_background(int gameNumber, int difficultyLevel);
	const lv_img_dsc_t* get_current_game_background();
	void cleanup_game_background_psram();

	extern CachedImage* img_avatar_82;
	extern CachedImage* queue_images;
	extern CachedImage* exp_images;
	extern CachedImage* overlay_images;
	extern size_t queue_images_size;
	extern size_t exp_images_size;
	extern size_t overlay_images_size;
	extern size_t int_avatar_82;

	// Game background PSRAM reservation
	extern GameBackground* game_background;

#define MAX_IMAGE_SIZE 307200  // 300 KB per image cap (increased for overlay images)
//#define IMAGE_ALLOC_FLAGS MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
#define IMAGE_ALLOC_FLAGS MALLOC_CAP_SPIRAM

// Game background constants - 320x240@RGB565
#define GAME_BG_HEIGHT 320
#define GAME_BG_WIDTH 240
#define GAME_BG_PIXEL_SIZE 2  // RGB565 = 2 bytes per pixel
#define GAME_BG_DATA_SIZE (GAME_BG_WIDTH * GAME_BG_HEIGHT * GAME_BG_PIXEL_SIZE)  // 153,600 bytes

// Image compression constants
#define IMG_COMPRESS_NONE 0
#define IMG_COMPRESS_RLE 1
#define IMG_COMPRESS_LZ4 2

// Image format magic numbers
#define IMG_MAGIC_COMPRESSED 0xAB    // New compressed format
#define IMG_MAGIC_LEGACY 0xFF        // Legacy uncompressed format (or other existing magic)


#ifdef __cplusplus
}
#endif
