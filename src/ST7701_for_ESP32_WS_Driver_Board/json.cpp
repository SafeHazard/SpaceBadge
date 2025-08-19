#include "json.h"
#include "session.h"  // For SessionManager
#include <painlessMesh.h>
#include <WiFi.h>  // For MAC address access

extern painlessMesh mesh;

const char* DEFAULT_GAME_NAMES[] = {
	"Game1", "Game2", "Game3", "Game4", "Game5", "Game6"
};
const size_t NUM_DEFAULT_GAMES = sizeof(DEFAULT_GAME_NAMES) / sizeof(DEFAULT_GAME_NAMES[0]);

#if defined(_WIN32)
#include <chrono>
unsigned long millis() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}
#endif

// ContactManager implementation
ContactManager::ContactManager() : hashCache(nullptr) {}

ContactManager::~ContactManager()
{
    if (hashCache) free(hashCache);
}

// Hash function for node IDs
static inline size_t hashNodeId(uint32_t nodeId) {
    return (nodeId * 2654435761U) & (CACHE_SIZE - 1);  // Knuth's multiplicative hash
}

// Ensure the hash cache is allocated and initialized
void ContactManager::ensureCache()
{
    if (hashCache) return;
    hashCache = (CacheEntry*)ps_malloc(sizeof(CacheEntry) * CACHE_SIZE);
    if (!hashCache)
    {
        LV_LOG_ERROR("Failed to allocate PSRAM for hashCache!");
        return;  // Graceful fallback - cache will just be disabled
    }
    // Initialize all entries as invalid
    for (size_t i = 0; i < CACHE_SIZE; i++) {
        hashCache[i].valid = false;
    }
}


size_t ContactManager::findIndex(uint32_t nodeId) const
{
    const_cast<ContactManager*>(this)->ensureCache();
    if (!hashCache) {
        // Cache disabled - fallback to linear search
        auto it = std::find_if(contacts.begin(), contacts.end(),
            [nodeId](const ContactData& c) { return c.nodeId == nodeId; });
        return (it != contacts.end()) ? std::distance(contacts.begin(), it) : contacts.size();
    }

    // Check hash cache first
    size_t hashIndex = hashNodeId(nodeId);
    if (hashCache[hashIndex].valid && 
        hashCache[hashIndex].nodeId == nodeId &&
        hashCache[hashIndex].index < contacts.size() &&
        contacts[hashCache[hashIndex].index].nodeId == nodeId) {
        return hashCache[hashIndex].index;
    }

    // Cache miss - search and update cache
    auto it = std::find_if(contacts.begin(), contacts.end(),
        [nodeId](const ContactData& c) { return c.nodeId == nodeId; });
    
    if (it != contacts.end()) {
        size_t index = std::distance(contacts.begin(), it);
        // Update cache entry (may overwrite existing entry - that's OK)
        hashCache[hashIndex].nodeId = nodeId;
        hashCache[hashIndex].index = index;
        hashCache[hashIndex].valid = true;
        return index;
    }
    
    return contacts.size(); // not found
}

ContactData* ContactManager::findContact(uint32_t nodeId)
{
    size_t index = findIndex(nodeId);
    return index < contacts.size() ? &contacts[index] : nullptr;
}

const ContactData* ContactManager::findContact(uint32_t nodeId) const
{
    size_t index = findIndex(nodeId);
    return index < contacts.size() ? &contacts[index] : nullptr;
}

// Take a read-only ContactData object and add a copy of it to a ContactManager.
// Retain isCrew/isBlocked status on existing contacts.
// Also update the lastUpdateTime field.
void ContactManager::addOrUpdateContact(const ContactData& contact)
{
    LV_LOG_TRACE("[TRACE] addOrUpdateContact nodeId=%u\n", contact.nodeId);

    ensureCache();

    size_t index = findIndex(contact.nodeId);
    if (index < contacts.size())
    {
        bool isCrew = contacts[index].isCrew || contact.isCrew;
        bool isBlocked = contacts[index].isBlocked || contact.isBlocked;
        contacts[index] = contact;
        contacts[index].lastUpdateTime = millis();
        contacts[index].isCrew = isCrew;
        contacts[index].isBlocked = isBlocked;
    }
    else
    {
        // add the contact to contacts
        contacts.push_back(contact);
		ContactData& newContact = contacts.back(); //don't use FindIndex() since the vector may have been reallocated, making the pointer invalid
        newContact.lastUpdateTime = millis();

        // update the cache (hash-based cache handles this automatically in findIndex)
    }
}

bool ContactManager::removeContact(uint32_t nodeId)
{
    ensureCache();

    size_t index = findIndex(nodeId);
    if (index >= contacts.size()) return false;

    contacts.erase(contacts.begin() + index);

    // Invalidate hash cache - indices have changed
    if (hashCache) {
        for (size_t i = 0; i < CACHE_SIZE; i++) {
            hashCache[i].valid = false;
        }
    }

    return true;
}

// Remove contacts that haven't been seen for maxAgeMs milliseconds
size_t ContactManager::removeStaleContacts(uint32_t maxAgeMs)
{
    ensureCache();
    
    uint32_t now = millis();
    size_t removedCount = 0;
    
    // Use safer approach - collect indices to remove, then remove backwards
    std::vector<size_t, PsramAllocator<size_t>> indicesToRemove;
    
    for (size_t i = 0; i < contacts.size(); i++) {
        uint32_t age = now - contacts[i].lastUpdateTime;
        
        // Check if contact is stale (and don't remove crew/blocked contacts from scanResults)
        // Also protect contacts that are in active game sessions
        bool isInActiveSession = false;
        extern SessionManager* sessionManager;
        if (sessionManager) {
            // Check if this contact is in the current lobby
            PsramVector<LobbyPlayer> lobbyPlayers = sessionManager->getLobbyPlayers();
            for (const LobbyPlayer& player : lobbyPlayers) {
                if (player.nodeId == contacts[i].nodeId) {
                    isInActiveSession = true;
                    break;
                }
            }
            // Also check if this is our target host
            if (!isInActiveSession && sessionManager->getTargetHost() == contacts[i].nodeId) {
                isInActiveSession = true;
            }
        }
        
        if (age > maxAgeMs && !contacts[i].isCrew && !contacts[i].isBlocked && !isInActiveSession) {
            LV_LOG_INFO("[DEBUG] Marking stale contact %u (%s) for removal - age: %lu ms\n", 
                         contacts[i].nodeId, contacts[i].displayName, age);
            indicesToRemove.push_back(i);
        } else if (isInActiveSession) {
            LV_LOG_INFO("[DEBUG] Protecting contact %u (%s) from cleanup - in active session\n", 
                         contacts[i].nodeId, contacts[i].displayName);
        }
    }
    
    // Remove in reverse order to maintain valid indices
    for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
        contacts.erase(contacts.begin() + *it);
        removedCount++;
    }
    
    // Invalidate hash cache if any contacts were removed
    if (removedCount > 0 && hashCache) {
        for (size_t i = 0; i < CACHE_SIZE; i++) {
            hashCache[i].valid = false;
        }
    }
    
    if (removedCount > 0) {
        LV_LOG_INFO("[DEBUG] Removed %zu stale contacts\n", removedCount);
    }
    
    return removedCount;
}

// Update contact information from a JSON message (only Self Packets)
bool ContactManager::updateContactInfo(uint32_t nodeId, const String& msg, uint32_t meshNodeTime)
{
    ContactData* contact = findContact(nodeId);
    if (!contact)
    {
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, msg);
    if (error)
    {
		doc.clear();
        return false;
    }

    if (doc["DisplayName"].is<String>())
    {
        strlcpy(contact->displayName, doc["DisplayName"], sizeof(contact->displayName));
    }
    if (doc["TotalXP"].is<int>())
    {
        contact->totalXP = doc["TotalXP"];
    }

    contact->pendingSave = true;
    contact->lastUpdateTime = meshNodeTime;
	doc.clear();
    return true;
}

void ContactManager::fromJson(JsonObjectConst json)
{
    ensureCache();

    contacts.clear();
    // Clear hash cache
    if (hashCache) {
        for (size_t i = 0; i < CACHE_SIZE; i++) {
            hashCache[i].valid = false;
        }
    }

    for (const JsonPairConst kv : json)
    {
        ContactData contact;
        contact.nodeId = strtoul(kv.key().c_str(), nullptr, 10);

        JsonObjectConst data = kv.value().as<JsonObjectConst>();
        strlcpy(contact.displayName, data["DisplayName"] | "", sizeof(contact.displayName));
        contact.isCrew = data["isCrew"] | false;
        contact.isBlocked = data["isBlocked"] | false;
        contact.totalXP = data["TotalXP"] | 0;
        contact.avatar = data["Avatar"] | 0;

        addOrUpdateContact(contact);
    }
}

void ContactManager::toJson(JsonObject& json) const
{
    for (auto& contact : contacts)
    {
        if (contact.pendingSave)
        {
            char nodeIdStr[16];
            snprintf(nodeIdStr, sizeof(nodeIdStr), "%u", contact.nodeId);

            //JsonObject contactObj = json.createNestedObject(nodeIdStr);
            JsonObject contactObj = json[nodeIdStr];
            contactObj["DisplayName"] = contact.displayName;
            contactObj["isCrew"] = contact.isCrew;
            contactObj["isBlocked"] = contact.isBlocked;
            contactObj["TotalXP"] = contact.totalXP;
            contactObj["Avatar"] = contact.avatar;
        }
    }
}

// Get crew
std::vector<ContactData*, PsramAllocator<ContactData*>> ContactManager::getCrew()
{
    std::vector<ContactData*, PsramAllocator<ContactData*>> result;
    for (auto& contact: contacts)
    {
        if (contact.isCrew)
        {
            result.push_back(&contact);
        }
    }
    return result;
}

bool ContactManager::hasPendingUpdates() const
{
    for (auto& contact : contacts)
    {
        if (contact.pendingSave) return true;
    }
    return false;
}

void ContactManager::clearPendingUpdates()
{
    for (auto& contact : contacts)
    {
        contact.pendingSave = false;
    }
}

// Get all contacts, including crew and blocked
std::vector<ContactData*, PsramAllocator<ContactData*>> ContactManager::getContacts()
{
    std::vector<ContactData*, PsramAllocator<ContactData*>> result;
    for (auto& contact : contacts)
    {
        //ContactData& contact = *it;
        //if (contact.isCrew)
        //{
            result.push_back(&contact);
        //}
    }
    return result;
}

std::vector<ContactData*, PsramAllocator<ContactData*>> ContactManager::getNonCrew()
{
    std::vector<ContactData*, PsramAllocator<ContactData*>> result;
    for (auto& contact : contacts)
    {
        if (!contact.isCrew)
        {
            result.push_back(&contact);
        }
    }
    return result;
}

ContactManager::iterator ContactManager::begin()
{
    return contacts.begin();
}

ContactManager::iterator ContactManager::end()
{
    return contacts.end();
}

ContactManager::const_iterator ContactManager::begin() const
{
    return contacts.begin();
}

ContactManager::const_iterator ContactManager::end() const
{
    return contacts.end();
}

// Function to serialize Config into JSON document
static void serializeConfig(const Config& config, JsonDocument& doc)
{
    //JsonObject root = doc.to<JsonObject>();

    // Serialize contacts
    //JsonObject contacts = root.createNestedObject("Contacts");
    JsonObject contacts = doc["Contacts"];
    config.contacts.toJson(contacts);

    // Serialize games
    //JsonObject games = root.createNestedObject("Games");
    JsonObject games = doc["Games"];
    for (const Game& game : config.games)
    {
        //JsonObject gameObj = games.createNestedObject(game.description);
        JsonObject gameObj = games[game.description];
        gameObj["XP"] = game.XP;
    }

    String temp;
    serializeJsonPretty(doc, temp); // Print the JSON document to Serial for debugging
    LV_LOG_INFO("JSON Output:\n%s\n", temp.c_str());
    // Serialize user
    //JsonObject user = root.createNestedObject("User");
    JsonObject user = doc["User"];
    user["DisplayName"] = config.user.displayName;
    user["TotalXP"] = config.user.totalXP;
    //JsonObject userAvatar = user.createNestedObject("Avatar");
    user["Avatar"] = config.user.avatar;

    // Serialize board
    //JsonObject board = root.createNestedObject("Board");
    JsonObject board = doc["Board"];
    board["AirplaneMode"] = config.board.airplaneMode;
    board["IntroWatched"] = config.board.introWatched;
    board["Volume"] = config.board.volume;
    board["Ssid"] = config.board.ssid;
    board["Password"] = config.board.password;
    board["Port"] = config.board.port;
    board["Channel"] = config.board.channel;
    board["Hidden"] = config.board.hidden;

    //JsonObject badgeMode = board.createNestedObject("BadgeMode");
    JsonObject badgeMode = board["BadgeMode"];
    badgeMode["Enabled"] = config.board.badgeMode.enabled;
    badgeMode["Delay"] = config.board.badgeMode.delay;

    //JsonObject displayNameOptions = board.createNestedObject("DisplayNameOptions");
    JsonObject displayNameOptions = board["DisplayNameOptions"];
    displayNameOptions["AwayMissions"] = config.board.displayNameOptions.gameHosts;
    displayNameOptions["ShowNamesFrom"] = config.board.displayNameOptions.showNamesFrom;

    //JsonObject ranks = board.createNestedObject("Ranks");
    JsonObject ranks = board["Ranks"];
    ranks["PO"] = config.board.ranks.po;
    ranks["Ens"] = config.board.ranks.ens;
    ranks["LTJG"] = config.board.ranks.ltjg;
    ranks["LT"] = config.board.ranks.lt;
    ranks["LCdr"] = config.board.ranks.lcdr;
    ranks["CDR"] = config.board.ranks.cdr;
    ranks["CAPT"] = config.board.ranks.capt;
}

// Helper function to convert string to display option
static DisplayOptions stringToDisplayOption(const char* str)
{
    if (strcmp(str, "Everyone") == 0) return Everyone;
    if (strcmp(str, "NotBlocked") == 0) return NotBlocked;
    if (strcmp(str, "Crew") == 0) return Crew;
    if (strcmp(str, "None") == 0) return None;
    return Everyone;  // default
}

// Read in a JSON document and return it (or an empty document)
JsonDocument readJson(const char* filename)
{
    // Create JsonDocument with larger capacity to avoid regular RAM consumption
    // Note: ArduinoJson's JsonDocument automatically uses heap allocation for large documents
    JsonDocument doc;  // Will automatically use heap for large documents
    LV_LOG_INFO("Starting read:");
    LV_LOG_INFO(filename);
    lv_fs_file_t f;
    lv_fs_res_t res = lv_fs_open(&f, filename, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK)
    {
        LV_LOG_ERROR("Failed to open JSON file: %d", (int)res);
        return doc;
    }
    LV_LOG_INFO("JSON open");
    // MEMORY OPTIMIZED: Stream-parse JSON directly from file instead of String concatenation
    lv_fs_close(&f);  // Close and reopen to reset file position
    res = lv_fs_open(&f, filename, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK) {
        LV_LOG_ERROR("Failed to reopen JSON file for parsing: %d", (int)res);
        return doc;
    }
    
    // Create a custom stream class for ArduinoJson to read directly from file
    class LVGLFileStream {
    private:
        lv_fs_file_t* file;
    public:
        LVGLFileStream(lv_fs_file_t* f) : file(f) {}
        
        int read() {
            uint8_t byte;
            uint32_t br;
            lv_fs_res_t res = lv_fs_read(file, &byte, 1, &br);
            if (res != LV_FS_RES_OK || br == 0) return -1;
            return (int)byte;
        }
        
        size_t readBytes(char* buffer, size_t length) {
            uint32_t br;
            lv_fs_res_t res = lv_fs_read(file, buffer, length, &br);
            if (res != LV_FS_RES_OK) return 0;
            return br;
        }
    };
    
    LVGLFileStream stream(&f);
    LV_LOG_INFO("Starting stream parsing of JSON");
    
    // Parse JSON directly from file stream - no String allocation!
    DeserializationError error = deserializeJson(doc, stream);
    
    // Close the file
    lv_fs_close(&f);
    LV_LOG_INFO("JSON file closed after stream parsing");

    if (error)
    {
        LV_LOG_ERROR("deserializeJson() failed: ");
        LV_LOG_ERROR(error.c_str());
        return doc;
    }

    if (doc.overflowed())
    {
        LV_LOG_ERROR("deserializeJson() failed: ");
        LV_LOG_ERROR("Not enough memory to store entire JSON document");
        return doc;
    }

    LV_LOG_INFO("JSON deserialized");
    return doc;
}

static bool writeJson(const char* fileName, const JsonDocument& doc)
{
    lv_fs_file_t f;
    lv_fs_res_t res = lv_fs_open(&f, fileName, LV_FS_MODE_WR);
    if (res != LV_FS_RES_OK)
    {
        LV_LOG_ERROR("Failed to open file for writing. Error: %d", (int)res);
        return false;
    }

    // First serialize to String to get the size
    String jsonString;
    serializeJson(doc, jsonString);

    // Write the string to file
    uint32_t bw;  // bytes written
    res = lv_fs_write(&f, jsonString.c_str(), jsonString.length(), &bw);
    if (res != LV_FS_RES_OK || bw != (uint32_t)jsonString.length())
    {
        LV_LOG_ERROR("Failed to write file. Error: %d", (int)res);
        return false;
    }

    if (&f)
    {
        res = lv_fs_close(&f);
    }

    if (res != LV_FS_RES_OK)
    {
        LV_LOG_ERROR("Failed to close file. Error: %d", (int)res);
        return false;
    }

    return true;
}

// Print a JSON document to the serial port
void serialPrintJson(JsonDocument doc)
{
    // Windows
#if defined(_WIN32) || defined(_WIN64)
    String jsonString;
    serializeJsonPretty(doc, jsonString);
    LV_LOG_INFO("%s\n", jsonString.c_str());
    // Arduino
#else
    Serial.begin(115200);
    serializeJsonPretty(doc, Serial);
#endif
}

// debug: converts a displayOption (int) to a string
static const char* displayOptionToString(int option)
{
    switch (option)
    {
    case Everyone: return "Everyone";
    case NotBlocked: return "NotBlocked";
    case Crew: return "Crew";
    case None: return "None";
    default: return "Unknown";
    }
}

// Test all json functions
static bool jsonTest(const char* inFile, const char* outFile)
{
    return true;
}

static void printContactData(const ContactData& contact, bool verbose = false, int indent = 0)
{
    String spaces(indent, ' ');
    printf("%sContact NodeID: %u\n", spaces.c_str(), contact.nodeId);
    printf("%s  DisplayName: %s\n", spaces.c_str(), contact.displayName);
    printf("%s  Roles: %s%s\n", spaces.c_str(),"x",
        contact.isCrew ? "Contact " : "");
    printf("%s  Status: %s%s\n", spaces.c_str(),
        contact.isBlocked ? "Blocked " : "",
        contact.pendingSave ? "PendingSave " : "");
    printf("%s  LastUpdate: %llu\n", spaces.c_str(), contact.lastUpdateTime);
    printf("%s  TotalXP: %d\n", spaces.c_str(), contact.totalXP);
    printf("%s  Avatar: %d\n", spaces.c_str(), contact.avatar);

}

static void printContactManager(const ContactManager& manager, bool verbose = false)
{
    printf("\n=== Contact Manager Debug Dump ===");

    // Print statistics
    size_t totalContacts = 0;
    size_t totalCrew = 0;
    size_t blockedContacts = 0;
    size_t pendingUpdates = 0;

    for (const auto& contact : manager)
    {
        if (contact.isCrew) totalContacts++;
        if (contact.isBlocked) blockedContacts++;
        if (contact.pendingSave) pendingUpdates++;
    }

    printf("\nStatistics:");
    printf("  Total Entries: %zu\n", manager.size());
    printf("  Contacts: %zu\n", totalContacts);
    printf("  Crew Members: %zu\n", totalCrew);
    printf("  Blocked: %zu\n", blockedContacts);
    printf("  Pending Updates: %zu\n", pendingUpdates);

    // Print all contacts
    printf("\nDetailed Contact List:\n");
    for (const auto& contact : manager)
    {
        printContactData(contact, verbose);
        printf("\n"); // Add spacing between contacts
    }

    // Print specific lists if requested
    if (verbose)
    {
        printf("\nBlocked Contacts:");
        for (const auto& contact : manager)
        {
            if (contact.isBlocked)
            {
                printContactData(contact, false, 2);
            }
        }

        printf("\nPending Updates:");
        for (const auto& contact : manager)
        {
            if (contact.pendingSave)
            {
                printContactData(contact, false, 2);
            }
        }
    }

    printf("\n=== End Contact Manager Dump ===\n");
}

void printConfig(const Config& config, bool verbose)
{
    printf("\n====== Configuration Debug Dump ======");

    // Print Contacts
    printf("\n--- Contact Manager ---\n");
    printContactManager(config.contacts, verbose);

    // Print Games
    printf("\n--- Games ---\n");
    for (const auto& game : config.games)
    {
        printf("  %s: %d XP\n", game.description, game.XP);
    }

    // Print User
    printf("\n--- User ---\n");
    printf("  DisplayName: %s\n", config.user.displayName);
    printf("  TotalXP: %d\n", config.user.totalXP);
    printf("  Avatar: %d\n", config.user.avatar);

    // Print Board Settings
    printf("\n--- Board Settings ---\n");
    printf("  AirplaneMode: %s\n", config.board.airplaneMode ? "true" : "false");
    printf("  IntroWatched: %s\n", config.board.introWatched ? "true" : "false");
    printf("  OutroWatched: %s\n", config.board.outroWatched ? "true" : "false");
    printf("  Avatars Unlocked: %d\n", config.board.avatars_unlocked);
    printf("  Volume: %d\n", config.board.volume);
    printf("  SSID: %s\n", config.board.ssid);
    printf("  Port: %d\n", config.board.port);
    printf("  Channel: %d\n", config.board.channel);
    printf("  Hidden: %s\n", config.board.hidden ? "true" : "false");

    printf("  Badge Mode:");
    printf("    Enabled: %s\n", config.board.badgeMode.enabled ? "true" : "false");
    printf("    Delay: %d\n", config.board.badgeMode.delay);

    printf("  Display Name Options:");
    printf("    Away Missions: %d\n", config.board.displayNameOptions.gameHosts);
    printf("    Show Names From: %d\n", config.board.displayNameOptions.showNamesFrom);

    printf("  Ranks:");
    printf("    CPO: %d\n", config.board.ranks.po);
    printf("    Ensign: %d\n", config.board.ranks.ens);
    printf("    LTJG: %d\n", config.board.ranks.ltjg);
    printf("    LT: %d\n", config.board.ranks.lt);
    printf("    LTC: %d\n", config.board.ranks.lcdr);
    printf("    CDR: %d\n", config.board.ranks.cdr);
    printf("    CAPT: %d\n", config.board.ranks.capt);

    printf("\n====== End Configuration Dump ======\n");
}

// Function to load configuration from a JSON file
bool loadConfig(Config& cfg, const char* filename) {
    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, filename, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK) {
        LV_LOG_ERROR("Failed to open file for reading, Error %d", (int)res);
        return false;
    }

    String jsonString;
    char buffer[256];
    uint32_t read = 0;

    while (lv_fs_read(&file, buffer, sizeof(buffer) - 1, &read) == LV_FS_RES_OK && read > 0) {
        buffer[read] = '\0';
        jsonString += buffer;
    }
    lv_fs_close(&file);

    JsonDocument doc;

    if (deserializeJson(doc, jsonString)) {
        LV_LOG_ERROR("Failed to parse JSON config. Loading defaults.");
        doc.clear(); // Clear the document & try again
    }

    JsonObject user = doc["User"];
    strlcpy(cfg.user.displayName, user["DisplayName"] | "", sizeof(cfg.user.displayName));
    if (cfg.user.displayName[0] == '\0') {
        // Generate default name using MAC address (available immediately, unlike nodeId)
        uint8_t mac[6];
        WiFi.macAddress(mac);
        
        // Use last 5 hex digits from MAC for unique identifier
        // Take bytes 3, 4, 5 and first nibble of byte 2 = 5 hex chars (rightmost part)
        uint32_t uniqueId = ((uint32_t)mac[3] << 12) | ((uint32_t)mac[4] << 4) | (mac[5] >> 4);
        
        snprintf(cfg.user.displayName, sizeof(cfg.user.displayName), "Queue Who %05X", uniqueId);
        LV_LOG_INFO("Generated default display name: %s (from MAC %02X:%02X:%02X:%02X:%02X:%02X)", 
                   cfg.user.displayName, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
    
    // Bounds check totalXP (should be non-negative and reasonable)
    int tempTotalXP = user["TotalXP"] | 0;
    cfg.user.totalXP = (tempTotalXP >= 0 && tempTotalXP <= 1000000) ? tempTotalXP : 0;
    
    // Bounds check avatar (should be non-negative)
    int tempAvatar = user["Avatar"] | 0;
    cfg.user.avatar = (tempAvatar >= 0 && tempAvatar <= 999) ? tempAvatar : 0;
    
    //LV_LOG_INFO("[DEBUG] Loaded config: avatar=%d, totalXP=%d, displayName='%s'\n", 
    //              cfg.user.avatar, cfg.user.totalXP, cfg.user.displayName); 

    JsonObject contacts = doc["Contacts"];
    for (JsonPair kv : contacts) {
        ContactData c;
        c.nodeId = strtoul(kv.key().c_str(), nullptr, 10);
        JsonObject entry = kv.value().as<JsonObject>();
        strlcpy(c.displayName, entry["DisplayName"] | "Queue Who", sizeof(c.displayName));
        c.isCrew = entry["isCrew"] | false;
        c.isBlocked = entry["isBlocked"] | false;
        
        // Bounds check contact totalXP
        int tempContactXP = entry["TotalXP"] | 0;
        c.totalXP = (tempContactXP >= 0 && tempContactXP <= 1000000) ? tempContactXP : 0;
        
        // Bounds check contact avatar
        int tempContactAvatar = entry["Avatar"] | 0;
        c.avatar = (tempContactAvatar >= 0 && tempContactAvatar <= 999) ? tempContactAvatar : 0;
        
        cfg.contacts.addOrUpdateContact(c);
    }

    cfg.games.clear();
    JsonObject games = doc["Games"];

    // Add all games that exist in JSON
	LV_LOG_INFO("loading games from JSON...\n");
    for (JsonPair kv : games) {
        Game g;
        strlcpy(g.description, kv.key().c_str(), sizeof(g.description));
        
        // Bounds check game XP - explicitly cast to JsonObject
        JsonObject gameObj = kv.value().as<JsonObject>();
        if (!gameObj.containsKey("XP")) {
            LV_LOG_ERROR("[ERROR] Game %s missing XP field!\n", g.description);
            g.XP = 0;
        } else {
            // Get the XP value directly as int
            int tempGameXP = gameObj["XP"].as<int>();
            LV_LOG_INFO("[DEBUG] Raw XP value for %s: %d\n", g.description, tempGameXP);
            g.XP = (tempGameXP >= -1 && tempGameXP <= 1000000) ? tempGameXP : 0;
        }
        
        LV_LOG_INFO("Loaded %s with %d XP\n", g.description, g.XP);
        cfg.games.push_back(g);
    }
    LV_LOG_INFO("Loaded %zu games from JSON\n", cfg.games.size());

    // Ensure all required game names exist
    for (size_t i = 0; i < NUM_DEFAULT_GAMES; ++i) {
        const char* name = DEFAULT_GAME_NAMES[i];
        auto it = std::find_if(cfg.games.begin(), cfg.games.end(), [name](const Game& g) {
            return strcmp(g.description, name) == 0;
            });
        if (it == cfg.games.end()) {
            Game g;
            strlcpy(g.description, name, sizeof(g.description));
            g.XP = 0;
            cfg.games.push_back(g);
        }
    }

    // Recalculate totalXP from sum of all game XP (excluding negative values)
    int calculatedTotalXP = 0;
    for (const auto& game : cfg.games) {
        if (game.XP != -1) {  
            calculatedTotalXP += game.XP;
        }
    }
    
    // Update totalXP if there's a mismatch
    if (cfg.user.totalXP != calculatedTotalXP) {
        LV_LOG_INFO("[DEBUG] TotalXP mismatch detected: stored=%d, calculated=%d. Updating to calculated value.\n", 
                   cfg.user.totalXP, calculatedTotalXP);
        cfg.user.totalXP = calculatedTotalXP;
    }

    JsonObject board = doc["Board"];
    cfg.board.airplaneMode = board["AirplaneMode"] | false;
    cfg.board.introWatched = board["IntroWatched"] | false;
    cfg.board.outroWatched = board["OutroWatched"] | false;
    
    // Load avatars_unlocked and outroWatched from Board section
    int tempAvatarsUnlocked = board["AvatarsUnlocked"] | 25;
    cfg.board.avatars_unlocked = (tempAvatarsUnlocked >= 0 && tempAvatarsUnlocked <= 999) ? tempAvatarsUnlocked : 25;

    cfg.board.volume = board["Volume"] | 50;
    if(cfg.board.volume < 0 || cfg.board.volume > 100) {
        cfg.board.volume = 100; // Ensure volume is within valid range
	}
    strlcpy(cfg.board.ssid, board["Ssid"] | "sheetmetalcon", sizeof(cfg.board.ssid));
    strlcpy(cfg.board.password, board["Password"] | "V9$Jqc8EmDPHVQ3kGf_qgAVmjdrqj@y", sizeof(cfg.board.password));
    // Bounds check port (valid range 1-65535)
    int tempPort = board["Port"] | 5555;
    cfg.board.port = (tempPort >= 1 && tempPort <= 65535) ? tempPort : 5555;
    
    // Bounds check WiFi channel (valid range 1-14)
    int tempChannel = board["Channel"] | 6;
    cfg.board.channel = (tempChannel >= 1 && tempChannel <= 14) ? tempChannel : 6;
    
    cfg.board.hidden = board["Hidden"] | false;

    JsonObject badge = board["BadgeMode"];
    cfg.board.badgeMode.enabled = badge["Enabled"] | false;
    
    // Bounds check badge mode delay (should be reasonable, 1-3600 seconds)
    int tempDelay = badge["Delay"] | 60;
    cfg.board.badgeMode.delay = (tempDelay >= 1 && tempDelay <= 3600) ? tempDelay : 60;

    JsonObject display = board["DisplayNameOptions"];
    
    // Bounds check DisplayOptions enums (valid values 0-3 per json.h enum)
    int tempGameHosts = display["AwayMissions"] | 0;
    cfg.board.displayNameOptions.gameHosts = (DisplayOptions)((tempGameHosts >= 0 && tempGameHosts <= 3) ? tempGameHosts : 0);
    
    int tempShowNames = display["ShowNamesFrom"] | 0;
    cfg.board.displayNameOptions.showNamesFrom = (DisplayOptions)((tempShowNames >= 0 && tempShowNames <= 3) ? tempShowNames : 0);

    JsonObject ranks = board["Ranks"];
    
    // Bounds check rank values (should be positive and reasonable)
    int tempPO = ranks["PO"] | RANK_PO;
    cfg.board.ranks.po = (tempPO >= 0 && tempPO <= 1000000) ? tempPO : RANK_PO;
    
    int tempEns = ranks["Ens"] | RANK_ENS;
    cfg.board.ranks.ens = (tempEns >= 0 && tempEns <= 1000000) ? tempEns : RANK_ENS;
    
    int tempLTJG = ranks["LTJG"] | RANK_LTJG;
    cfg.board.ranks.ltjg = (tempLTJG >= 0 && tempLTJG <= 1000000) ? tempLTJG : RANK_LTJG;
    
    int tempLT = ranks["LT"] | RANK_LT;
    cfg.board.ranks.lt = (tempLT >= 0 && tempLT <= 1000000) ? tempLT : RANK_LT;
    
    int tempLCDR = ranks["LCdr"] | RANK_LCDR;
    cfg.board.ranks.lcdr = (tempLCDR >= 0 && tempLCDR <= 1000000) ? tempLCDR : RANK_LCDR;
    
    int tempCDR = ranks["CDR"] | RANK_CDR;
    cfg.board.ranks.cdr = (tempCDR >= 0 && tempCDR <= 1000000) ? tempCDR : RANK_CDR;
    
    int tempCapt = ranks["CAPT"] | RANK_CAPT;
    cfg.board.ranks.capt = (tempCapt >= 0 && tempCapt <= 1000000) ? tempCapt : RANK_CAPT;
    
	doc.clear(); // Clear the document to free memory

    return true;
}

// Function to convert Config struct to JSON document
static void configToJson(const Config& config, JsonDocument& doc)
{
    JsonObject user = doc["User"].to<JsonObject>();
    user["DisplayName"] = config.user.displayName;
    user["TotalXP"] = config.user.totalXP;
    user["Avatar"] = config.user.avatar;
    
    LV_LOG_INFO("[DEBUG] Saving config: avatar=%d, totalXP=%d, displayName='%s'\n", 
                  config.user.avatar, config.user.totalXP, config.user.displayName);

    //user["Image"] = config.user.image;  // Simplified

    JsonObject contacts = doc["Contacts"].to<JsonObject>();
    for (const auto& c : config.contacts) {
        char id[16];
        snprintf(id, sizeof(id), "%u", c.nodeId);
        JsonObject entry = contacts[id].to<JsonObject>();
        entry["DisplayName"] = c.displayName;
        entry["isCrew"] = c.isCrew;
        entry["isBlocked"] = c.isBlocked;
        entry["TotalXP"] = c.totalXP;
        entry["Avatar"] = c.avatar; 
    }

    JsonObject board = doc["Board"].to<JsonObject>();
    board["AirplaneMode"] = config.board.airplaneMode;
    board["IntroWatched"] = config.board.introWatched;
    board["Volume"] = config.board.volume;
    board["Ssid"] = config.board.ssid;
    board["Password"] = config.board.password;
    board["Port"] = config.board.port;
    board["Channel"] = config.board.channel;
    board["Hidden"] = config.board.hidden;
    board["AvatarsUnlocked"] = config.board.avatars_unlocked;
    board["OutroWatched"] = config.board.outroWatched;

    JsonObject badge = board["BadgeMode"].to<JsonObject>();
    badge["Enabled"] = config.board.badgeMode.enabled;
    badge["Delay"] = config.board.badgeMode.delay;

    JsonObject display = board["DisplayNameOptions"].to<JsonObject>();
    display["AwayMissions"] = config.board.displayNameOptions.gameHosts;
    display["ShowNamesFrom"] = config.board.displayNameOptions.showNamesFrom;

    JsonObject ranks = board["Ranks"].to<JsonObject>();
    ranks["PO"] = config.board.ranks.po;
    ranks["Ens"] = config.board.ranks.ens;
    ranks["LTJG"] = config.board.ranks.ltjg;
    ranks["LT"] = config.board.ranks.lt;
    ranks["LCdr"] = config.board.ranks.lcdr;
    ranks["CDR"] = config.board.ranks.cdr;
    ranks["CAPT"] = config.board.ranks.capt;

    LV_LOG_INFO("Serializing games...\n");
    JsonObject games = doc["Games"].to<JsonObject>();
    for (const Game& g : config.games) {
        JsonObject entry = games[g.description].to<JsonObject>();
        entry["XP"] = g.XP;
        LV_LOG_INFO("[DEBUG] Saving game: %s = %d XP\n", g.description, g.XP);
    }
    LV_LOG_INFO("Serialization completed.\n");
}

// Function to save the board configuration to a file
bool saveBoardConfig(Config& config, const char* fileName)
{
    JsonDocument defaultJson;

    //convert config struct to JsonDocument
    configToJson(config, defaultJson);
    //serializeConfig(config, defaultJson);
    if (defaultJson.size() < 1)
    {
        LV_LOG_ERROR("Error converting config to JsonDocument");
		defaultJson.clear();
        return false;
    }

    //write JsonDocument to fileName
    if (!writeJson(fileName, defaultJson))
    {
        LV_LOG_ERROR("Error writing config to: %s", fileName);
        defaultJson.clear();
        return false;
    }

    LV_LOG_INFO("Board config successfully written.");
    defaultJson.clear();
    return true;
}