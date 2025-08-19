#pragma once

#if defined(_WIN32) || defined(_WIN64)
// ArduinoJson needs these Arduino-type stubs on Windows
#include <string>
#include <sstream>

// Fake "String" class for ArduinoJson compatibility
class String : public std::string {
public:
    using std::string::string;
    const char* c_str() const { return std::string::c_str(); }

    // Add write() for ArduinoJson compatibility
    size_t write(uint8_t c) {
        this->push_back(static_cast<char>(c));
        return 1;
    }

    size_t write(const uint8_t* buffer, size_t length) {
        this->append(reinterpret_cast<const char*>(buffer), length);
        return length;
    }
};

// Fake "Stream" class for ArduinoJson compatibility
class Stream {
public:
    virtual int available() { return 0; }
    virtual int read() { return -1; }
    virtual int peek() { return -1; }
    virtual void flush() {}
};

#include "./ArduinoJson/src/ArduinoJson.h"

// Provide strlcpy for Windows
inline void strlcpy(char* dst, const char* src, size_t size) {
    if (size == 0) return;
    std::strncpy(dst, src, size - 1);
    dst[size - 1] = '\0';
}

#else
#include "FreeRTOS.h" //needed?
#include "xtensa/xtruntime-frames.h" //needed?
#include <Arduino.h>
#include <ArduinoJson.h>
#include <esp32-hal-psram.h> // For PSRAM support
#include <memory>

// Define ps_malloc as PSRAM allocation macro
#define ps_malloc(size) heap_caps_malloc(size, MALLOC_CAP_SPIRAM)

#endif

#include <vector>
#include <lvgl.h>
#include "global.hpp"

enum DisplayOptions
{
    Everyone = 0,   // everyone
    NotBlocked = 1, // all except isBlocked
    Crew = 2,       // crew only
    None = 3        // no names
};

struct ContactData
{
    uint32_t nodeId = 0;
    char displayName[64] = "";
    bool isCrew = false;
    bool isBlocked = false;
    bool pendingSave = false;
    uint64_t lastUpdateTime = 0;
    int totalXP = 0;
    int avatar;
};

struct Ranks
{
    int po = 0;
    int ens = 7200;
    int ltjg = 14400;
    int lt = 28800;
    int lcdr = 57600;
    int cdr = 86400;
    int capt = 172800;
};

struct BadgeMode
{
    bool enabled = true;
    int32_t delay = 60;
};

struct DisplayNameOptions
{
    DisplayOptions gameHosts; // = Everyone;
    DisplayOptions showNamesFrom; // = Everyone;
};

struct Board
{
    bool airplaneMode = false;
    char ssid[64] = "sheetmetalcon";
    char password[64] = "V9$Jqc8EmDPHVQ3kGf_qgAVmjdrqj@y";
    int port = 5555;
    int channel = 6;
    bool hidden = 0;
    int avatars_unlocked = 25;  // Number of avatars unlocked (hex values 00-18 by default, 25 base + game unlocks)
    bool introWatched = false;
    bool outroWatched = false;  // Track if user has seen the captain outro	
    uint8_t volume = 50; //0-100 (will be converted to 0.0f-1.0f in mp3s.cpp)
    BadgeMode badgeMode;
    DisplayNameOptions displayNameOptions;
    Ranks ranks;

};

struct Game
{
    char description[128];
    int XP = 0;
};

template <typename T>
class PsramAllocator
{
public:
    using value_type = T;

    PsramAllocator() = default;

    template <typename U>
    constexpr PsramAllocator(const PsramAllocator<U>&) noexcept {}

    [[nodiscard]] T* allocate(std::size_t n)
    {
        void* p = ps_malloc(n * sizeof(T));
        if (!p) throw std::bad_alloc();
        return static_cast<T*>(p);
    }

    void deallocate(T* p, std::size_t) noexcept
    {
        free(p);
    }
};

template <class T, class U>
bool operator==(const PsramAllocator<T>&, const PsramAllocator<U>&) { return true; }
template <class T, class U>
bool operator!=(const PsramAllocator<T>&, const PsramAllocator<U>&) { return false; }

// Contact management class
#define CACHE_SIZE 128  // Hash table size (power of 2)

struct CacheEntry {
    uint32_t nodeId;
    size_t index;
    bool valid;
};

class ContactManager
{
private:
    std::vector<ContactData, PsramAllocator<ContactData>> contacts;
    CacheEntry* hashCache;  // Hash-based cache instead of direct indexing

    size_t findIndex(uint32_t nodeId) const;

public:
    ContactManager();
    ~ContactManager();
	void ensureCache();

    // Core operations
    ContactData* findContact(uint32_t nodeId);
    const ContactData* findContact(uint32_t nodeId) const;
    void addOrUpdateContact(const ContactData& contact);
    bool updateContactInfo(uint32_t nodeId, const String& msg, uint32_t meshNodeTime);
    bool removeContact(uint32_t nodeId);
    size_t removeStaleContacts(uint32_t maxAgeMs);

    // JSON operations
    void fromJson(JsonObjectConst json);
    void toJson(JsonObject& json) const;

    // Update tracking
    bool hasPendingUpdates() const;
    void clearPendingUpdates();

    // Container operations
    size_t size() const { return contacts.size(); }
    //std::vector<ContactData>::iterator begin() { return contacts.begin(); }
    //std::vector<ContactData>::iterator end() { return contacts.end(); }
    //std::vector<ContactData>::const_iterator begin() const { return contacts.begin(); }
    //std::vector<ContactData>::const_iterator end() const { return contacts.end(); }
    using ContactVector = std::vector<ContactData, PsramAllocator<ContactData>>;
    using iterator = ContactVector::iterator;
    using const_iterator = ContactVector::const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;


    // Filtering
    std::vector<ContactData*, PsramAllocator<ContactData*>> getNonCrew();
    std::vector<ContactData*, PsramAllocator<ContactData*>> getContacts();
    std::vector<ContactData*, PsramAllocator<ContactData*>> getCrew();
};

struct Config
{
    ContactManager contacts;
    std::vector<Game, PsramAllocator<Game>> games;
    ContactData user;
    Board board;
};

bool loadConfig(Config& cfg, const char* filename = "L:/default.json");
//bool loadBoardConfig(Config& config, const char* fileName = "L:/default.json");
bool saveBoardConfig(Config& config, const char* fileName = "L:/default.json");
void printConfig(const Config& config, bool verbose = false);
void serialPrintJson(JsonDocument doc);

unsigned long millis();

JsonDocument readJson(const char* filename);
