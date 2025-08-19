#include "pngs.h"
#include <LittleFS.h>
#include <vector>

// Include compression libraries
#if LV_USE_LZ4_EXTERNAL
#include <lz4.h>
#endif

#if LV_USE_LZ4_INTERNAL
#include <lvgl.h>  // This should include the internal LZ4 through LVGL
// If that doesn't work, we'll need the direct path
extern "C" {
#include "..\libraries\lvgl\src\libs\lz4\lz4.h"
	//"../libraries/lvgl/src/libs/lz4/lz4.h"
}
#endif

#pragma pack(push, 1)
struct BinImgHeader {
	uint8_t magic;                   // Image magic number
	uint8_t cf;                      // Color format (LV_COLOR_FORMAT_RGB565, etc.)
	uint16_t flags;                  // Image flags (LV_IMAGE_FLAGS_COMPRESSED, etc.)
	uint16_t w;                      // Width
	uint16_t h;                      // Height
	uint16_t stride;                 // Bytes per row
	uint32_t compressed_size;        // Size of compressed data (0 if uncompressed)
	uint32_t decompressed_size;      // Size when decompressed (same as w*h*bpp if uncompressed)
	uint8_t compression_method;      // LV_IMAGE_COMPRESS_NONE, RLE, LZ4
	uint8_t reserved[3];             // Padding for alignment
};
#pragma pack(pop)

static_assert(sizeof(BinImgHeader) == 22, "Header must be exactly 22 bytes");

// Legacy header for backward compatibility
#pragma pack(push, 1)
struct LegacyBinImgHeader {
	uint8_t magic;
	uint8_t cf;
	uint16_t empty1;
	uint16_t w;
	uint16_t h;
	uint16_t stride;
	uint16_t empty2;
};
#pragma pack(pop)

static_assert(sizeof(LegacyBinImgHeader) == 12, "Legacy header must be exactly 12 bytes");

//static lv_img_dsc_t* transparent_img_dsc = nullptr;
constexpr size_t IMG_HEADER_SIZE = sizeof(lv_img_dsc_t);

// Helper function to parse image header from raw file data
bool parse_image_header(const uint8_t* file_data, size_t file_size, BinImgHeader& header, 
                       const uint8_t*& pixel_data, uint32_t& expected_data_size, const char* filename) {
    if (file_size < sizeof(LegacyBinImgHeader)) {
        LV_LOG_ERROR("[IMG] File too small for header: %s (%zu bytes)\n", filename, file_size);
        return false;
    }
    
    LegacyBinImgHeader* base_header = (LegacyBinImgHeader*)file_data;
    uint32_t bpp = (base_header->cf == LV_COLOR_FORMAT_RGB565A8) ? 3 : 2;
    
    // Check compression flag at offset 0x2 (base_header->empty1)
    bool is_compressed = (base_header->empty1 != 0);
    
    if (!is_compressed) {
        // Uncompressed format - compression flag at offset 0x2 is 0
        header.magic = base_header->magic;
        header.cf = base_header->cf;
        header.flags = 0;
        header.w = base_header->w;
        header.h = base_header->h;
        header.stride = base_header->stride;
        header.compressed_size = base_header->w * base_header->h * bpp;
        header.decompressed_size = header.compressed_size;
        header.compression_method = IMG_COMPRESS_NONE;
        pixel_data = file_data + sizeof(LegacyBinImgHeader);
        expected_data_size = header.compressed_size;
        
        //printf("[IMG] Detected uncompressed format: %s (%dx%d, %u bpp)\n", 
        //       filename, header.w, header.h, bpp);
        return true;
    } else {
        // Compressed format - compression flag at offset 0x2 is non-zero
        // Compression method is at offset 0xC (byte 12)
        // Compressed data starts at offset 0x10 (byte 16)
        if (file_size < 17) {
            LV_LOG_ERROR("[IMG] Compressed file too small for data: %s\n", filename);
            return false;
        }
        
        uint8_t compression_method = file_data[12];  // Offset 0xC
        
        // Skip RLE compression (method 1) - only support LZ4 (method 2)
        if (compression_method == 1) {
            LV_LOG_ERROR("[IMG] RLE compression not supported: %s\n", filename);
            return false;
        }
        
        if (compression_method != 2) {
            LV_LOG_ERROR("[IMG] Unknown compression method %d: %s\n", compression_method, filename);
            return false;
        }
        
        // Compressed size is file size minus 16 bytes (12-byte header + 4 bytes padding/info)
        uint32_t compressed_size = file_size - 16;
        uint32_t decompressed_size = base_header->w * base_header->h * bpp;
        
        if (file_size < 25) {
            LV_LOG_ERROR("[IMG] File too small for LZ4 format: %s\n", filename);
            return false;
        }
        
        uint32_t lz4_compressed_size = *(uint32_t*)(file_data + 16);    // Bytes 16-19
        uint32_t lz4_decompressed_size = *(uint32_t*)(file_data + 20);  // Bytes 20-23
        
        // Use the embedded sizes and data starts at offset 24
        pixel_data = file_data + 24;  // Skip 12-byte header + 1-byte method + 3 padding + 8-byte sizes
        expected_data_size = lz4_compressed_size;
        compressed_size = lz4_compressed_size;
        
        header.magic = base_header->magic;
        header.cf = base_header->cf;
        header.flags = 0;
        header.w = base_header->w;
        header.h = base_header->h;
        header.stride = base_header->stride;
        header.compressed_size = compressed_size;
        header.decompressed_size = decompressed_size;
        header.compression_method = compression_method;

        return true;
    }
}

// Decompress image data based on compression method
bool decompress_image_data(const uint8_t* compressed_data, size_t compressed_size,
                          uint8_t* decompressed_data, size_t decompressed_size,
                          uint8_t compression_method, uint32_t bpp) {
    switch (compression_method) {
        case IMG_COMPRESS_NONE:
            // No compression - direct copy
            if (compressed_size != decompressed_size) {
                LV_LOG_ERROR("[IMG] Size mismatch for uncompressed image: %zu != %zu\n", 
                       compressed_size, decompressed_size);
                return false;
            }
            memcpy(decompressed_data, compressed_data, decompressed_size);
            return true;
            
        case IMG_COMPRESS_RLE:
            // Use LVGL's RLE decompression
            #if LV_USE_RLE
            {
                lv_result_t result = lv_rle_decompress(compressed_data, compressed_size,
                                                      decompressed_data, decompressed_size, bpp);
                if (result != LV_RESULT_OK) {
                    LV_LOG_ERROR("[IMG] RLE decompression failed: %d\n", result);
                    return false;
                }
                return true;
            }
            #else
            LV_LOG_ERROR("[IMG] RLE compression not enabled in LVGL config\n");
            return false;
            #endif
            
        case IMG_COMPRESS_LZ4:
            // Use LVGL's LZ4 decompression
            #if LV_USE_LZ4_INTERNAL || LV_USE_LZ4_EXTERNAL
            {
                int result = LZ4_decompress_safe((const char*)compressed_data, (char*)decompressed_data,
                                                compressed_size, decompressed_size);
                if (result < 0) {
                    LV_LOG_ERROR("[IMG] LZ4 decompression failed: %d\n", result);
                    //LV_LOG_ERROR("[DEBUG] LZ4 error meanings: -1=generic, -9=source corruption/invalid data\n");
                    return false;
                }
                return true;
            }
            #else
            LV_LOG_ERROR("[IMG] LZ4 compression not enabled in LVGL config\n");
            return false;
            #endif
            
        default:
            LV_LOG_ERROR("[IMG] Unknown compression method: %d\n", compression_method);
            return false;
    }
}

// Load an entire directory of .bin images into RAM
bool preload_image_directory(const char* dirPath, CachedImage** outArray, size_t* outCount) {
	if (!dirPath || !outArray || !outCount) return false;

	File dir = LittleFS.open(dirPath);
	if (!dir || !dir.isDirectory()) {
		LV_LOG_ERROR("Invalid image directory: %s\n", dirPath);
		return false;
	}
	
	// For overlay directory, try memory cleanup first
	if (strcmp(dirPath, "/images/overlay") == 0) {
		LV_LOG_INFO("Preparing for overlay image loading - performing memory cleanup\n");
		// Force garbage collection
		ESP.getMinFreeHeap();
		delay(100); // Give some time for cleanup
		
		uint32_t freePSRAM = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
		uint32_t largestFree = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
		LV_LOG_INFO("Post-cleanup: Free PSRAM: %u, Largest block: %u\n", freePSRAM, largestFree);
		
		// If largest free block is less than 250KB, warn that overlay loading might fail
		if (largestFree < 250000) {
			LV_LOG_WARN("Largest free PSRAM block (%u) may be too small for overlay images\n", largestFree);
		}
	}

	std::vector<CachedImage, PsramAllocator<CachedImage>> imageList;

	while (true) {
		File entry = dir.openNextFile();
		if (!entry) break;

		if (!entry.isDirectory()) {
			//String name = entry.name();
			//if (name.endsWith(".bin")) {
			//	String fullPath = String(dirPath) + "/" + name;
			char name[64];
			strlcpy(name, entry.name(), sizeof(name));

			if (strstr(name, ".bin"))
			{
				char fullPath[128];
				snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, name);

				File file = LittleFS.open(fullPath, "r");
				if (!file)
				{
					LV_LOG_ERROR("Failed to open image file: %s\n", fullPath);
					continue;
				}

				size_t size = file.size();
				if (size < sizeof(BinImgHeader) + 4) {
					LV_LOG_ERROR("File too small: %s\n", fullPath);
					file.close();
					continue;
				}

				if (size > MAX_IMAGE_SIZE) {
					LV_LOG_WARN("Image too large (%zu bytes > %d): %s\n", size, MAX_IMAGE_SIZE, fullPath);
					file.close();
					continue;
				}
				
				LV_LOG_INFO("Loading overlay image: %s (%zu bytes)\n", name, size);

				// allocate pixel buffer
				uint8_t* pixels = (uint8_t*)heap_caps_malloc(size, IMAGE_ALLOC_FLAGS | MALLOC_CAP_8BIT);
				if (!pixels) {
					uint32_t freePSRAM = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
					LV_LOG_ERROR("Failed to allocate %zu bytes PSRAM for image: %s (free PSRAM: %u)\n", 
					            size, fullPath, freePSRAM);
					file.close();
					continue;
				}

				// read image data
				file.read(pixels, size);
				file.close();
				file = File();  // forces immediate wrapper destruction

				// Parse image header using shared helper function
				BinImgHeader hdr = {};
				const uint8_t* pixel_data = nullptr;
				uint32_t expected_data_size = 0;
				
				if (!parse_image_header(pixels, size, hdr, pixel_data, expected_data_size, name)) {
					heap_caps_free(pixels);
					continue;
				}
				
				uint8_t bpp = (hdr.cf == LV_COLOR_FORMAT_RGB565A8) ? 3 : 2;
				uint32_t data_size = hdr.decompressed_size;

				// Allocate new buffer to hold the real lv_img_dsc_t
				size_t alloc_size = IMG_HEADER_SIZE + data_size;
				uint8_t* full_img = nullptr;
				
				// Try different allocation strategies for large images
				if (alloc_size > 200000) { // For images >200KB, try more flexible allocation
					LV_LOG_INFO("Attempting large image allocation: %zu bytes\n", alloc_size);
					
					// Try with 32-byte alignment first
					full_img = (uint8_t*)heap_caps_aligned_alloc(32, alloc_size, MALLOC_CAP_SPIRAM);
					if (full_img) {
						LV_LOG_INFO("Large image allocated with 32-byte alignment\n");
					} else {
						LV_LOG_WARN("32-byte aligned allocation failed, trying standard PSRAM\n");
						
						// Try standard PSRAM allocation
						full_img = (uint8_t*)heap_caps_malloc(alloc_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
						if (full_img) {
							LV_LOG_INFO("Large image allocated with PSRAM | 8BIT\n");
						} else {
							LV_LOG_WARN("PSRAM | 8BIT allocation failed, trying PSRAM only\n");
							
							// Try PSRAM only
							full_img = (uint8_t*)heap_caps_malloc(alloc_size, MALLOC_CAP_SPIRAM);
							if (full_img) {
								LV_LOG_INFO("Large image allocated with PSRAM only\n");
							} else {
								LV_LOG_WARN("All PSRAM allocations failed, trying chunked allocation\n");
								
								// Try chunked allocation - allocate in smaller pieces
								const size_t CHUNK_SIZE = 32768; // 32KB chunks
								size_t num_chunks = (alloc_size + CHUNK_SIZE - 1) / CHUNK_SIZE;
								
								LV_LOG_INFO("Attempting chunked allocation: %zu bytes in %zu chunks of %zu bytes\n", 
								           alloc_size, num_chunks, CHUNK_SIZE);
								
								// Allocate array to hold chunk pointers
								uint8_t** chunks = (uint8_t**)malloc(num_chunks * sizeof(uint8_t*));
								if (!chunks) {
									LV_LOG_ERROR("Failed to allocate chunk pointer array\n");
								} else {
									bool chunks_success = true;
									
									// Allocate each chunk
									for (size_t i = 0; i < num_chunks; i++) {
										size_t chunk_size = (i == num_chunks - 1) ? 
										                   (alloc_size - i * CHUNK_SIZE) : CHUNK_SIZE;
										chunks[i] = (uint8_t*)heap_caps_malloc(chunk_size, MALLOC_CAP_SPIRAM);
										if (!chunks[i]) {
											LV_LOG_ERROR("Failed to allocate chunk %zu of %zu\n", i + 1, num_chunks);
											chunks_success = false;
											// Free previously allocated chunks
											for (size_t j = 0; j < i; j++) {
												heap_caps_free(chunks[j]);
											}
											break;
										}
									}
									
									if (chunks_success) {
										// Allocate final contiguous buffer and copy data
										full_img = (uint8_t*)malloc(alloc_size);
										if (full_img) {
											LV_LOG_INFO("Chunked allocation successful, consolidating...\n");
											// This is just a placeholder - we'd need to actually copy the data
											// For now, free the chunks and use regular approach
											for (size_t i = 0; i < num_chunks; i++) {
												heap_caps_free(chunks[i]);
											}
											free(full_img);
											full_img = nullptr;
											LV_LOG_WARN("Chunked approach needs implementation - skipping for now\n");
										} else {
											LV_LOG_ERROR("Failed to allocate consolidation buffer\n");
											for (size_t i = 0; i < num_chunks; i++) {
												heap_caps_free(chunks[i]);
											}
										}
									}
									
									free(chunks);
								}
								
								// If chunked allocation also failed, skip this image
								if (!full_img) {
									LV_LOG_ERROR("All allocation strategies failed, skipping: %s\n", name);
								}
							}
						}
					}
				} else {
					// For smaller images, use standard allocation
					full_img = (uint8_t*)heap_caps_malloc(alloc_size, IMAGE_ALLOC_FLAGS);
				}
				
				if (!full_img) {
					uint32_t freePSRAM = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
					uint32_t freeInternal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
					LV_LOG_ERROR("Failed to allocate %zu bytes for final image: %s (free PSRAM: %u, internal: %u)\n", 
					            alloc_size, fullPath, freePSRAM, freeInternal);
					heap_caps_free(pixels);
					continue;
				}

				// Recreate the lv_img_dsc struct
				lv_img_dsc_t* img = (lv_img_dsc_t*)full_img;
				memset((void*)img, 0, IMG_HEADER_SIZE);
				img->header.magic = hdr.magic;
				img->header.w = hdr.w;
				img->header.h = hdr.h;
				img->header.cf = hdr.cf;
				img->data_size = data_size;
				img->data = full_img + IMG_HEADER_SIZE;

				// Decompress or copy the pixels
				bool decompress_success = decompress_image_data(pixel_data, expected_data_size,
				                                               (uint8_t*)img->data, data_size,
				                                               hdr.compression_method, bpp);
				
				if (!decompress_success) {
					LV_LOG_ERROR("[IMG] Failed to decompress image: %s\n", name);
					heap_caps_free(full_img);
					heap_caps_free(pixels);
					continue;
				}
				
				heap_caps_free(pixels);

				// Check image alignment in RAM
				uintptr_t addr = (uintptr_t)img->data;
				if (addr % 4 != 0) {
					LV_LOG_WARN("WARNING: img->data for %s is not 4-byte aligned: 0x%08lx\n", name, addr);
				}

				// Fix bad data_size on fully transparent files
				if (img->data_size == 0 && (img->header.cf == LV_COLOR_FORMAT_RGB565 || img->header.cf == LV_COLOR_FORMAT_RGB565A8)) {
					memset((void*)img->data, 0, size - IMG_HEADER_SIZE);
					if (!img->data) {
						free(full_img);
						continue;
					}
				}

				imageList.push_back({ strdup(name),
				                      img });
				
				// Allow spinner animation during preloading
				lv_tick_inc(5);
				lv_timer_handler();
				delay(10); // Small delay to let spinner animate
			}
		}
		
		// Process LVGL events after each file (even failed ones) to keep spinner smooth
		lv_tick_inc(2);
		lv_timer_handler();
	}
	dir.close();

	if (imageList.empty()) {
		*outArray = nullptr;
		*outCount = 0;
		return false;
	}

	*outCount = imageList.size();
	*outArray = (CachedImage*)heap_caps_malloc(sizeof(CachedImage) * imageList.size(), MALLOC_CAP_SPIRAM);
	memcpy(*outArray, imageList.data(), sizeof(CachedImage) * imageList.size());
	
	imageList.clear();
	imageList.shrink_to_fit(); // force memory release
	return true;
}

// Get a cached image by filename
const lv_img_dsc_t* get_cached_image(CachedImage* list, size_t count, const char* filename) {
	if (!list || !filename || count == 0) return nullptr;

	for (size_t i = 0; i < count; ++i) {
		if (strcmp(list[i].name, filename) == 0) {
			return list[i].img;
		}
	}
	LV_LOG_ERROR("Cached image not found: %s\n", filename);
	return nullptr;
}

// Free all cached images from the 'images' array (of length 'count')
// Be sure to set 'images' to nullptr and 'count' to 0 after calling this function
void free_cached_images(CachedImage* images, size_t count)
{
	if (!images) return;

	for (size_t i = 0; i < count; ++i)
	{
		if (images[i].img)
		{
			// Do NOT free img->data, it's part of the same allocation!
			free(images[i].img);
		}
		if (images[i].name)
		{
			free((void*)images[i].name);
		}
	}

	free(images);
}

// Unified loading function for all 3 image directories with combined progress
bool preload_all_image_directories_with_progress(lv_obj_t* progressBar, lv_obj_t* progressLabel, lv_obj_t* shuttleImg) {
	if (!progressBar || !progressLabel) return false;
	
	// Directory paths and their target variables
	struct ImageDirInfo {
		const char* path;
		CachedImage** outArray;
		size_t* outCount;
	};
	
	ImageDirInfo dirs[] = {
		{"/images/overlay", &overlay_images, &overlay_images_size}, // Load overlay images FIRST to avoid fragmentation
		{"/avatars/82", &img_avatar_82, &int_avatar_82},
		{"/images/queue", &queue_images, &queue_images_size},
		{"/images/exp", &exp_images, &exp_images_size}
	};
	
	constexpr int numDirs = sizeof(dirs) / sizeof(dirs[0]);
	
	// First pass: count total files across all directories
	size_t totalFiles = 0;
	for (int d = 0; d < numDirs; d++) {
		File dir = LittleFS.open(dirs[d].path);
		if (dir && dir.isDirectory()) {
			while (true) {
				File entry = dir.openNextFile();
				if (!entry) break;
				if (!entry.isDirectory() && strstr(entry.name(), ".bin")) {
					totalFiles++;
				}
				entry.close();
			}
			dir.close();
		}
	}
	
	if (totalFiles == 0) return false;
	
	// Helper function to update progress and shuttle position
	auto updateProgress = [&](size_t currentFile, size_t totalFiles) {
		int progress = (currentFile * 100) / totalFiles;
		lv_bar_set_value(progressBar, progress, LV_ANIM_OFF);
		
		char progressText[8];
		snprintf(progressText, sizeof(progressText), "%d%%", progress);
		lv_label_set_text(progressLabel, progressText);
		
		// Move shuttle image along the progress bar
		if (shuttleImg) {
			int shuttleX = (240 - 200) / 2 + (progress * 170 / 100); // Move shuttle across 170px of the 200px bar
			lv_obj_set_pos(shuttleImg, shuttleX, 195);
		}
		
		lv_tick_inc(2);
		lv_timer_handler();
	};
	
	// Second pass: load all directories with unified progress
	size_t currentFile = 0;
	bool overallSuccess = true;
	
	for (int d = 0; d < numDirs; d++) {
		File dir = LittleFS.open(dirs[d].path);
		if (!dir || !dir.isDirectory()) {
			LV_LOG_ERROR("Invalid image directory: %s\n", dirs[d].path);
			overallSuccess = false;
			continue;
		}
		
		std::vector<CachedImage, PsramAllocator<CachedImage>> imageList;
		
		while (true) {
			File entry = dir.openNextFile();
			if (!entry) break;
			
			if (!entry.isDirectory()) {
				char name[64];
				strlcpy(name, entry.name(), sizeof(name));
				
				if (strstr(name, ".bin")) {
					char fullPath[128];
					snprintf(fullPath, sizeof(fullPath), "%s/%s", dirs[d].path, name);
					
					File file = LittleFS.open(fullPath, "r");
					if (!file) {
						LV_LOG_ERROR("Failed to open image: %s\n", fullPath);
						entry.close();
						currentFile++;
						
						// Update progress even for failed files
						updateProgress(currentFile, totalFiles);
						continue;
					}
					
					size_t size = file.size();
					
					if (size > MAX_IMAGE_SIZE) {
						LV_LOG_WARN("Image too large (%zu bytes): %s\n", size, fullPath);
						file.close();
						entry.close();
						currentFile++;
						
						// Update progress for skipped files
						updateProgress(currentFile, totalFiles);
						continue;
					}
					
					// allocate pixel buffer
					uint8_t* pixels = (uint8_t*)heap_caps_malloc(size, IMAGE_ALLOC_FLAGS | MALLOC_CAP_8BIT);
					if (!pixels) {
						LV_LOG_ERROR("Failed to allocate RAM for image: %s\n", fullPath);
						file.close();
						entry.close();
						currentFile++;
						
						// Update progress for failed allocations
						updateProgress(currentFile, totalFiles);
						continue;
					}
					
					// read image data
					file.read(pixels, size);
					file.close();
					file = File();  // forces immediate wrapper destruction
					
					// Parse image header using shared helper function
					BinImgHeader hdr = {};
					const uint8_t* pixel_data = nullptr;
					uint32_t expected_data_size = 0;
					
					if (!parse_image_header(pixels, size, hdr, pixel_data, expected_data_size, name)) {
						heap_caps_free(pixels);
						currentFile++;
						updateProgress(currentFile, totalFiles);
						continue;
					}
					
					uint32_t bpp = (hdr.cf == LV_COLOR_FORMAT_RGB565A8) ? 3 : 2;
					uint32_t data_size = hdr.decompressed_size;
					
					// Allocate new buffer to hold the real lv_img_dsc_t
					size_t alloc_size = IMG_HEADER_SIZE + data_size;
					uint8_t* full_img = (uint8_t*)heap_caps_malloc(alloc_size, IMAGE_ALLOC_FLAGS);
					if (!full_img) {
						free(pixels);
						entry.close();
						currentFile++;
						
						// Update progress for failed allocations
						updateProgress(currentFile, totalFiles);
						continue;
					}
					
					// Recreate the lv_img_dsc struct
					lv_img_dsc_t* img = (lv_img_dsc_t*)full_img;
					memset((void*)img, 0, IMG_HEADER_SIZE);
					img->header.magic = hdr.magic;
					img->header.w = hdr.w;
					img->header.h = hdr.h;
					img->header.cf = (lv_color_format_t)hdr.cf;
					img->data_size = data_size;
					img->data = full_img + IMG_HEADER_SIZE;

					// Decompress or copy the pixels
					bool decompress_success = decompress_image_data(pixel_data, expected_data_size,
					                                               (uint8_t*)img->data, data_size,
					                                               hdr.compression_method, bpp);
					
					if (!decompress_success) {
						LV_LOG_ERROR("[IMG] Failed to decompress image: %s\n", name);
						heap_caps_free(full_img);
						heap_caps_free(pixels);
						currentFile++;
						updateProgress(currentFile, totalFiles);
						continue;
					}
					
					heap_caps_free(pixels);
					
					// Check image alignment in RAM
					uintptr_t addr = (uintptr_t)img->data;
					if (addr % 4 != 0) {
						LV_LOG_WARN("WARNING: img->data for %s is not 4-byte aligned: 0x%08lx\n", name, addr);
					}
					
					// Fix bad data_size on fully transparent files
					if (img->data_size == 0 && (img->header.cf == LV_COLOR_FORMAT_RGB565 || img->header.cf == LV_COLOR_FORMAT_RGB565A8)) {
						memset((void*)img->data, 0, size - IMG_HEADER_SIZE);
						if (!img->data) {
							free(full_img);
							entry.close();
							currentFile++;
							
							// Update progress for failed processing
							updateProgress(currentFile, totalFiles);
							continue;
						}
					}
					
					imageList.push_back({ strdup(name), img });
					
					currentFile++;
					
					// Update progress bar and shuttle position
					updateProgress(currentFile, totalFiles);
					delay(2); // Small delay to make progress visible
				}
			}
			
			// Process LVGL events after each file (even failed ones) to keep progress smooth
			lv_tick_inc(2);
			lv_timer_handler();
		}
		dir.close();
		
		// Store results for this directory
		if (!imageList.empty()) {
			*(dirs[d].outCount) = imageList.size();
			*(dirs[d].outArray) = (CachedImage*)heap_caps_malloc(sizeof(CachedImage) * imageList.size(), MALLOC_CAP_SPIRAM);
			memcpy(*(dirs[d].outArray), imageList.data(), sizeof(CachedImage) * imageList.size());
		} else {
			*(dirs[d].outArray) = nullptr;
			*(dirs[d].outCount) = 0;
		}
		
		imageList.clear();
		imageList.shrink_to_fit(); // force memory release
	}
	
	return overallSuccess;
}

CachedImage* img_avatar_82 = nullptr;  
CachedImage* queue_images = nullptr;
CachedImage* exp_images = nullptr;
CachedImage* overlay_images = nullptr;
size_t queue_images_size;
size_t exp_images_size;
size_t overlay_images_size;
size_t int_avatar_82;

// Game background PSRAM reservation
GameBackground* game_background = nullptr;

// Initialize PSRAM reservation for game backgrounds
bool init_game_background_psram() {
    LV_LOG_INFO("[GAME_BG] Initializing PSRAM reservation for game backgrounds...\n");
    
    // Check available PSRAM
    uint32_t freePSRAM = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    uint32_t largestBlock = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
    
    LV_LOG_INFO("[GAME_BG] Available PSRAM: %u bytes, Largest block: %u bytes\n", freePSRAM, largestBlock);
    
    // We need space for GameBackground struct only (image will be allocated dynamically)
    size_t totalNeeded = sizeof(GameBackground);
    
    if (largestBlock < totalNeeded) {
        LV_LOG_ERROR("[ERROR] Insufficient PSRAM for game background reservation. Need %zu bytes, have %u\n", 
               totalNeeded, largestBlock);
        return false;
    }
    
    // Allocate GameBackground structure in PSRAM
    game_background = (GameBackground*)heap_caps_malloc(sizeof(GameBackground), MALLOC_CAP_SPIRAM);
    if (!game_background) {
        LV_LOG_ERROR("[ERROR] Failed to allocate GameBackground structure in PSRAM\n");
        return false;
    }
    
    // Initialize state
    game_background->cached_img.name = nullptr;
    game_background->cached_img.img = nullptr;
    game_background->current_filename[0] = '\0';
    game_background->is_loaded = false;
    
    LV_LOG_INFO("[GAME_BG] Game background PSRAM reservation successful! Reserved %.1fKB\n", totalNeeded / 1024.0f);
    
    // Final memory check
    uint32_t finalFreePSRAM = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    LV_LOG_INFO("[GAME_BG] PSRAM after reservation: %u bytes (%.1fKB used)\n", 
           finalFreePSRAM, (freePSRAM - finalFreePSRAM) / 1024.0f);
    
    return true;
}

// Load a specific game background using CachedImage approach
bool load_game_background(int gameNumber, int difficultyLevel) {
    if (!game_background) {
        LV_LOG_ERROR("[ERROR] Game background PSRAM not initialized\n");
        return false;
    }
    
    // Build the filename: /backgrounds/gameX/levelY.bin
    char filepath[64];
	if (gameNumber != 2)
	{
		snprintf(filepath, sizeof(filepath), "/backgrounds/game%d/level%d.bin", gameNumber, difficultyLevel);
	}
	else
	{
		snprintf(filepath, sizeof(filepath), "/backgrounds/game2/level1.bin");
	}
    
    // Check if this background is already loaded
    if (game_background->is_loaded && strcmp(game_background->current_filename, filepath) == 0) {
        LV_LOG_INFO("[GAME_BG] Background %s already loaded, skipping\n", filepath);
        return true;
    }
    
    LV_LOG_INFO("[GAME_BG] Loading background: %s\n", filepath);
    
    // Clean up previous image if it exists
    if (game_background->cached_img.img) {
        heap_caps_free(game_background->cached_img.img);
        game_background->cached_img.img = nullptr;
    }
    if (game_background->cached_img.name) {
        free((void*)game_background->cached_img.name);
        game_background->cached_img.name = nullptr;
    }
    
    // Open the background file
    File file = LittleFS.open(filepath, "r");
    if (!file) {
        LV_LOG_ERROR("[ERROR] Failed to open game background file: %s\n", filepath);
        return false;
    }
    
    // Check file size
    size_t fileSize = file.size();
    if (fileSize < sizeof(BinImgHeader)) {
        LV_LOG_ERROR("[ERROR] Game background file too small: %s (%zu bytes)\n", filepath, fileSize);
        file.close();
        return false;
    }
    
    LV_LOG_INFO("[GAME_BG] File size: %zu bytes\n", fileSize);
    
    // Read entire file to memory (same as CachedImage approach)
    uint8_t* pixels = (uint8_t*)heap_caps_malloc(fileSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!pixels) {
        LV_LOG_ERROR("[ERROR] Failed to allocate temporary buffer for background file\n");
        file.close();
        return false;
    }
    
    if (file.read(pixels, fileSize) != fileSize) {
        LV_LOG_ERROR("[ERROR] Failed to read background file: %s\n", filepath);
        heap_caps_free(pixels);
        file.close();
        return false;
    }
    file.close();
    
    // Parse image header using the same helper function as CachedImage
    BinImgHeader hdr = {};
    const uint8_t* pixel_data = nullptr;
    uint32_t expected_data_size = 0;
    
    const char* filename = strrchr(filepath, '/');
    filename = filename ? filename + 1 : filepath;
    
    if (!parse_image_header(pixels, fileSize, hdr, pixel_data, expected_data_size, filename)) {
        heap_caps_free(pixels);
        return false;
    }
    
    uint32_t bpp = (hdr.cf == LV_COLOR_FORMAT_RGB565A8) ? 3 : 2;
    uint32_t data_size = hdr.decompressed_size;
    
    // Allocate contiguous buffer for header + data (same as CachedImage approach)
    size_t alloc_size = IMG_HEADER_SIZE + data_size;
    uint8_t* full_img = (uint8_t*)heap_caps_malloc(alloc_size, MALLOC_CAP_SPIRAM);
    if (!full_img) {
        LV_LOG_ERROR("[ERROR] Failed to allocate image buffer for background (%.1fKB)\n", alloc_size / 1024.0f);
        heap_caps_free(pixels);
        return false;
    }
    
    // Create the lv_img_dsc struct at the beginning of the buffer
    lv_img_dsc_t* img = (lv_img_dsc_t*)full_img;
    memset((void*)img, 0, IMG_HEADER_SIZE);
    img->header.magic = hdr.magic;
    img->header.w = hdr.w;
    img->header.h = hdr.h;
    img->header.cf = (lv_color_format_t)hdr.cf;
    img->data_size = data_size;
    img->data = full_img + IMG_HEADER_SIZE;  // Data immediately follows header
    
    // Decompress or copy the pixels
    bool decompress_success = decompress_image_data(pixel_data, expected_data_size,
                                                   (uint8_t*)img->data, data_size,
                                                   hdr.compression_method, bpp);
    
    heap_caps_free(pixels);
    
    if (!decompress_success) {
        LV_LOG_ERROR("[ERROR] Failed to decompress background: %s\n", filepath);
        heap_caps_free(full_img);
        return false;
    }
    
    // Store in CachedImage structure
    game_background->cached_img.name = strdup(filename);
    game_background->cached_img.img = img;
    
    // Update state
    strncpy(game_background->current_filename, filepath, sizeof(game_background->current_filename) - 1);
    game_background->current_filename[sizeof(game_background->current_filename) - 1] = '\0';
    game_background->is_loaded = true;
    
    LV_LOG_INFO("[GAME_BG] Successfully loaded background: %s (%.1fKB)\n", filepath, data_size / 1024.0f);
    return true;
}

// Get the currently loaded game background image descriptor
const lv_img_dsc_t* get_current_game_background() {
    if (!game_background || !game_background->is_loaded || !game_background->cached_img.img) {
        return nullptr;
    }
    
    // Additional validation of the image descriptor
    if (game_background->cached_img.img->data == nullptr) {
        LV_LOG_ERROR("[ERROR] Game background cached_img has null data pointer\n");
        return nullptr;
    }
    
    return game_background->cached_img.img;
}

// Cleanup game background PSRAM reservation
void cleanup_game_background_psram() {
    if (!game_background) {
        return;
    }
    
    LV_LOG_INFO("[GAME_BG] Cleaning up game background PSRAM reservation...\n");
    
    // Clean up CachedImage data
    if (game_background->cached_img.img) {
        heap_caps_free(game_background->cached_img.img);
        game_background->cached_img.img = nullptr;
    }
    
    if (game_background->cached_img.name) {
        free((void*)game_background->cached_img.name);
        game_background->cached_img.name = nullptr;
    }
    
    heap_caps_free(game_background);
    game_background = nullptr;
    
    LV_LOG_INFO("[GAME_BG] Game background PSRAM cleanup complete\n");
}  
