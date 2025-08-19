#include "contacts.h"
#include <unordered_map>
#include "./src/ui/images.h"

int32_t int_contacts_tab_current = -1;    // what contacts tab is being shown: -1 = none, 0 = scan, 1 = crew
uint32_t contactLastClicked;              // the ID of the last contact that was clicked

// Forward declarations
static ContactData* getSelectedContact(lv_obj_t* roller, ContactManager* manager);
static void updateContactDetails(ContactData* contact);

lv_obj_t* virtual_scan_list = nullptr;    // virtual list for scan results
lv_obj_t* virtual_crew_list = nullptr;

static constexpr int kRowHeight = 60;  // adjust to match your styling
static constexpr int kMaxVisibleRows = 10;

// Update the virtual contact rows based on the current scroll position
void update_virtual_contact_rows(lv_event_t* e)
{
	LV_LOG_INFO("update_virtual_contact_rows called\n");
    lv_obj_t* list = e ? (lv_obj_t*)lv_event_get_target(e) : virtual_scan_list;  // or virtual_crew_list
    
    //ContactPtrVector* contacts = e ? static_cast<ContactPtrVector*>(lv_event_get_user_data(e)) : nullptr;
    ContactPtrVector* contacts = nullptr;
    if (e)
    {
        contacts = static_cast<ContactPtrVector*>(lv_event_get_user_data(e));
    }
    else
    {
        lv_obj_t* list = virtual_scan_list;  // or virtual_crew_list depending on context
        if (list)
        {
            contacts = static_cast<ContactPtrVector*>(lv_obj_get_user_data(list));
        }
    }

    if (!list || !contacts)
    {
        LV_LOG_WARN("[WARN] update_virtual_contact_rows: list or contacts is null\n");
        return;
    }

    lv_coord_t scroll_y = lv_obj_get_scroll_y(list);
    int first_visible = scroll_y / kRowHeight;

    uint32_t row_count = lv_obj_get_child_count(list);
    for (uint32_t i = 0; i < row_count; i++)
    {
        lv_obj_t* row = lv_obj_get_child(list, i);
        size_t index = first_visible + i;

        if (index < contacts->size())
        {
            if (!(*contacts)[index])
            {
                LV_LOG_WARN("[WARN] Null contact at index %zu\n", index);
                lv_obj_add_flag(row, LV_OBJ_FLAG_HIDDEN);
                continue;
            }
            ContactData* contact = (*contacts)[index];
            lv_obj_set_y(row, index * kRowHeight);
            lv_label_set_text(lv_obj_get_child(row, 0), getNameFromContact(contact));
            lv_obj_set_user_data(row, reinterpret_cast<void*>(contact->nodeId));
            lv_obj_clear_flag(row, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(row, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

//  Add a virtual contact list to the targetList object
void add_virtual_contact_list(lv_obj_t** targetList, const ContactPtrVector& source, lv_obj_t* parent)
{
	LV_LOG_INFO("add_virtual_contact_list called\n");
    //static ContactPtrVector* staticSource = nullptr;

    if (!*targetList)
    {
        // First time: create and populate the list container
        *targetList = lv_obj_create(parent);
        lv_obj_set_size(*targetList, 137, 169);
		lv_obj_set_pos(*targetList, -14, -11);
        lv_obj_set_scroll_dir(*targetList, LV_DIR_VER);
        lv_obj_set_scroll_snap_y(*targetList, LV_SCROLL_SNAP_START);
        lv_obj_set_flex_flow(*targetList, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_gap(*targetList, 4, 0);

        int visible_rows = std::min((int)source.size(), kMaxVisibleRows);
        for (int i = 0; i < visible_rows + 1; i++)
        {
            lv_obj_t* row = lv_obj_create(*targetList);
            lv_obj_set_size(row, LV_PCT(100), kRowHeight);

            lv_obj_t* label = lv_label_create(row);
            lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
            lv_obj_set_width(label, LV_PCT(100));
            lv_obj_center(label);

            lv_obj_add_event_cb(row, contactListButtonClick, LV_EVENT_CLICKED, nullptr);
        }

        // Attach scroll event
        lv_obj_add_event_cb(*targetList, update_virtual_contact_rows, LV_EVENT_SCROLL, nullptr);
    }

    // Before assigning a new vector, free the old one *attached to the list*
    ContactPtrVector* old = static_cast<ContactPtrVector*>(lv_obj_get_user_data(*targetList));
    if (old)
    {
        delete old;
    }

    // Make a copy and attach to the list safely
    ContactPtrVector* dynamicCopy = new ContactPtrVector(source);
    lv_obj_set_user_data(*targetList, dynamicCopy);
    
    // Add cleanup callback for when the list is deleted
    lv_obj_add_event_cb(*targetList, [](lv_event_t* e)
        {
            void* data = lv_event_get_user_data(e);
            if (data)
            {
                LV_LOG_TRACE("[TRACE] Freeing ContactPtrVector user_data: %p\n", data);
                delete static_cast<ContactPtrVector*>(data);
                lv_obj_set_user_data((lv_obj_t*)lv_event_get_target(e), nullptr);
            }
        }, LV_EVENT_DELETE, nullptr); // Use nullptr instead of old pointer
    
    // simulate a scroll event manually to force UI update
    update_virtual_contact_rows(nullptr);
}

// Convert an IDPacket to a ContactData object
ContactData* idPacketToContactData(IDPacket* idPacket)
{
    if (nullptr == idPacket) return nullptr; // No packet found, exit
    ContactData* contact = new ContactData();
    contact->avatar = idPacket->avatarID;
    contact->isBlocked = false;
    strlcpy(contact->displayName, idPacket->displayName, sizeof(contact->displayName));
    contact->isCrew = false;
    contact->nodeId = idPacket->boardID;
    contact->totalXP = idPacket->totalXP;
    contact->lastUpdateTime = idPacket->timeArrived;
    return contact;
}

void contactListButtonClick(lv_event_t* e)
{
    // clear out the details
    lv_label_set_text(objects.lbl_contacts_name, "Select A Contact");
    lv_obj_remove_state(objects.check_contacts_block, LV_STATE_CHECKED);
    lv_obj_remove_state(objects.check_contacts_crew, LV_STATE_CHECKED);
    lv_label_set_text(objects.lbl_contacts_xp, "0");

    // extract the ContactData from the user data of the clicked list entry
    // save that info in a global so we can use it in other functions later
    contactLastClicked = (uint32_t)(lv_obj_get_user_data((lv_obj_t*)lv_event_get_target(e)));

    // try and get the contactData from config.contacts first
    ContactData* contact = config.contacts.findContact(contactLastClicked);
    if (nullptr == contact)
    {
        // if that fails, try and get it from scanResults
        contact = scanResults->findContact(contactLastClicked);
        if (nullptr == contact) return; // no contact found in either list; fail silently
    }

    // enable the checkboxes
    lv_obj_remove_state(objects.check_contacts_block, LV_STATE_DISABLED);
    lv_obj_remove_state(objects.check_contacts_crew, LV_STATE_DISABLED);

    //set display name in the label
    lv_label_set_text(objects.lbl_contacts_name, getNameFromContact(contact));

    //set the avatar image
    lv_image_set_src(objects.img_contacts_avatar, img_avatar_82[contact->avatar % int_avatar_82].img);

    //set the XP
    char str_xp[16];
    snprintf(str_xp, sizeof(str_xp), "%d", contact->totalXP);
    lv_label_set_text(objects.lbl_contacts_xp, str_xp);

    if (contact->isCrew)
    {
        //if the contact is a friend, set the crew checkbox
        lv_obj_add_state(objects.check_contacts_crew, LV_STATE_CHECKED);
    }

    if (contact->isBlocked)
    {
        //if the contact is blocked, set the blocked checkbox
        lv_obj_add_state(objects.check_contacts_block, LV_STATE_CHECKED);
    }
}

// Static string to avoid lifetime issues
static String rollerOptions;

const char* rollerStringFromContactManager(ContactManager* contactManager, String defaultString = "No Contacts")
{
    rollerOptions.clear();
    if (nullptr == contactManager) {
        rollerOptions = defaultString;
        return rollerOptions.c_str();
    }
    
    ContactPtrVector contacts = contactManager->getContacts();
    if (contacts.empty()) {
        rollerOptions = defaultString;
        return rollerOptions.c_str();
    }
    
    for (const auto& contact : contacts)
    {
        if (!contact) continue; // skip null contacts
        rollerOptions += getNameFromContact(contact);
        rollerOptions += "\n"; // add newline for each contact
    }
    
    // Remove the last newline character
    if (!rollerOptions.isEmpty())
        rollerOptions.remove(rollerOptions.length() - 1);
        
    return rollerOptions.c_str();
}

// Filter scan results to exclude crew members
const char* rollerStringFromScanResults(ContactManager* scanManager, String defaultString = "No Scan Results")
{
    rollerOptions.clear();
    if (nullptr == scanManager) {
        rollerOptions = defaultString;
        return rollerOptions.c_str();
    }
    
    ContactPtrVector scanContacts = scanManager->getContacts();
    if (scanContacts.empty()) {
        rollerOptions = defaultString;
        return rollerOptions.c_str();
    }
    
    bool hasResults = false;
    for (const auto& contact : scanContacts)
    {
        if (!contact) continue; // skip null contacts
        
        // Skip contacts that are already crew members
        ContactData* existingContact = config.contacts.findContact(contact->nodeId);
        if (existingContact && existingContact->isCrew) {
            LV_LOG_INFO("[DEBUG] Filtering out crew member from scan: %s (ID: %u)\n", 
                         contact->displayName, contact->nodeId);
            continue;
        }
        
        rollerOptions += getNameFromContact(contact);
        rollerOptions += "\n"; // add newline for each contact
        hasResults = true;
    }
    
    // If no non-crew contacts found, show default message
    if (!hasResults) {
        rollerOptions = defaultString;
        return rollerOptions.c_str();
    }
    
    // Remove the last newline character
    if (!rollerOptions.isEmpty())
        rollerOptions.remove(rollerOptions.length() - 1);
        
    return rollerOptions.c_str();
}

const char* rollerStringFromContactManager(ContactManager& contactManager, String defaultString = "No Contacts")
{
    return rollerStringFromContactManager(&contactManager, defaultString);
}

// populate the crew list with the current crew members
void populate_crew_list(lv_event_t* e)
{
	LV_LOG_INFO("populate_crew_list called\n");

    // Safety check for valid roller object
    if (!objects.roller_contacts_crew || !lv_obj_is_valid(objects.roller_contacts_crew)) {
        LV_LOG_ERROR("[ERROR] Invalid roller_contacts_crew object\n");
        return;
    }

    // Preserve current selection
    uint32_t currentSelection = lv_roller_get_selected(objects.roller_contacts_crew);
    uint32_t selectedNodeId = 0;
    
    // Get currently selected contact's node ID
    ContactPtrVector oldContacts = config.contacts.getContacts();
    if (currentSelection < oldContacts.size() && oldContacts[currentSelection]) {
        selectedNodeId = oldContacts[currentSelection]->nodeId;
    }

    // Set new options
    lv_roller_set_options(objects.roller_contacts_crew, rollerStringFromContactManager(config.contacts, "No Contacts"), LV_ROLLER_MODE_NORMAL);
    
    // Try to restore selection to same contact
    if (selectedNodeId != 0) {
        ContactPtrVector newContacts = config.contacts.getContacts();
        for (size_t i = 0; i < newContacts.size(); i++) {
            if (newContacts[i] && newContacts[i]->nodeId == selectedNodeId) {
                lv_roller_set_selected(objects.roller_contacts_crew, i, LV_ANIM_OFF);
                LV_LOG_INFO("[DEBUG] Restored crew roller selection to index %zu (nodeId: %u)\n", i, selectedNodeId);
                
                // Update UI details for restored selection
                updateContactDetails(newContacts[i]);
                contactLastClicked = selectedNodeId;
                break;
            }
        }
    }

    //static auto cached = config.contacts.getCrew();
    //add_virtual_contact_list(&virtual_crew_list, cached, objects.tab_contacts_crew);
    //static std::unordered_map<int32_t, lv_obj_t*> crew_list_buttons;

    //std::unordered_map<int32_t, bool> currentIDs;
    //for (const auto& [id, _] : crew_list_buttons)
    //{
    //    currentIDs[id] = false;
    //}

    //std::vector<ContactData*, PsramAllocator<ContactData*>> contacts = config.contacts.getContacts();

    //for (ContactData* contact : contacts)
    //{
    //    currentIDs[contact->nodeId] = true;

    //    if (crew_list_buttons.count(contact->nodeId) == 0)
    //    {
    //        lv_obj_t* entry = lv_list_add_button(objects.list_contacts_crew, &img_16x16, getNameFromContact(contact));
    //        lv_obj_set_user_data(entry, (void*)contact->nodeId); // Store the nodeID to look up later
    //        lv_obj_add_event_cb(entry, contactListButtonClick, LV_EVENT_CLICKED, NULL);
    //        crew_list_buttons[contact->nodeId] = entry;
    //    }
    //}

    //// Remove any stale entries
    //for (const auto& [id, seen] : currentIDs)
    //{
    //    if (!seen && crew_list_buttons.count(id))
    //    {
    //        lv_obj_del(crew_list_buttons[id]);
    //        crew_list_buttons.erase(id);
    //    }
    //}
}

// Get selected contact from roller index
static ContactData* getSelectedContact(lv_obj_t* roller, ContactManager* manager) {
    if (!roller || !manager) return nullptr;
    
    uint32_t selected = lv_roller_get_selected(roller);
    ContactPtrVector contacts = manager->getContacts();
    
    if (selected >= contacts.size()) return nullptr;
    return contacts[selected];
}

// Get selected contact from scan roller (with crew filtering)
static ContactData* getSelectedScanContact(lv_obj_t* roller, ContactManager* scanManager) {
    if (!roller || !scanManager) return nullptr;
    
    uint32_t selected = lv_roller_get_selected(roller);
    ContactPtrVector scanContacts = scanManager->getContacts();
    
    // Build filtered list to match roller options
    uint32_t currentIndex = 0;
    for (const auto& contact : scanContacts) {
        if (!contact) continue;
        
        // Skip contacts that are already crew members
        ContactData* existingContact = config.contacts.findContact(contact->nodeId);
        if (existingContact && existingContact->isCrew) {
            continue;
        }
        
        // If this is the selected index in our filtered list
        if (currentIndex == selected) {
            return contact;
        }
        currentIndex++;
    }
    
    return nullptr; // Index out of bounds
}

// Update contact details UI based on selected contact
static void updateContactDetails(ContactData* contact) {
    if (!contact) {
        // Clear/disable UI elements when no contact selected
        lv_label_set_text(objects.lbl_contacts_name, "Select A Contact");
        lv_label_set_text(objects.lbl_contacts_xp, "0");
        
        // Clear checkbox states and disable them
        lv_obj_clear_state(objects.check_contacts_crew, LV_STATE_CHECKED);
        lv_obj_clear_state(objects.check_contacts_block, LV_STATE_CHECKED);
        lv_obj_add_state(objects.check_contacts_crew, LV_STATE_DISABLED);
        lv_obj_add_state(objects.check_contacts_block, LV_STATE_DISABLED);
        
        // Clear avatar - set to default 16x16 image
        if (objects.img_contacts_avatar) {
            lv_image_set_src(objects.img_contacts_avatar, &img_16x16);
        }
        return;
    }
    
    // Update name
    lv_label_set_text(objects.lbl_contacts_name, getNameFromContact(contact));
    
    // Update XP
    char xpText[32];
    snprintf(xpText, sizeof(xpText), "%u XP", contact->totalXP);
    lv_label_set_text(objects.lbl_contacts_xp, xpText);
    
    // Enable checkboxes
    lv_obj_clear_state(objects.check_contacts_crew, LV_STATE_DISABLED);
    lv_obj_clear_state(objects.check_contacts_block, LV_STATE_DISABLED);
    
    // Set checkbox states
    if (contact->isCrew) {
        lv_obj_add_state(objects.check_contacts_crew, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(objects.check_contacts_crew, LV_STATE_CHECKED);
    }
    
    if (contact->isBlocked) {
        lv_obj_add_state(objects.check_contacts_block, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(objects.check_contacts_block, LV_STATE_CHECKED);
    }
    
    // Update avatar
    if (objects.img_contacts_avatar && img_avatar_82 && int_avatar_82 > 0) {
        bool avatar_set = false;
        
        // Validate avatar index
        if (contact->avatar >= 0 && contact->avatar < (int)int_avatar_82 && img_avatar_82[contact->avatar].name) {
            const lv_img_dsc_t* avatar_img = get_cached_image(img_avatar_82, int_avatar_82, img_avatar_82[contact->avatar].name);
            if (avatar_img) {
                lv_image_set_src(objects.img_contacts_avatar, avatar_img);
                avatar_set = true;
            }
        }
        
        // Fallback to first avatar if current is invalid
        if (!avatar_set && int_avatar_82 > 0) {
            const lv_img_dsc_t* fallback_img = &img_16x16; //get_cached_image(img_avatar_82, int_avatar_82, img_avatar_82[0].name);
            if (fallback_img) {
                lv_image_set_src(objects.img_contacts_avatar, fallback_img);
            }
        }
    }
}

// Crew roller selection callback
void crewRollerReleased(lv_event_t* e) {
    LV_LOG_TRACE("[TRACE] crewRollerReleased()\n");
    ContactData* contact = getSelectedContact(objects.roller_contacts_crew, &config.contacts);
    updateContactDetails(contact);
    
    // Update contactLastClicked for checkbox callbacks
    contactLastClicked = contact ? contact->nodeId : 0;
}

// Scan roller selection callback
void scanRollerReleased(lv_event_t* e) {
    LV_LOG_TRACE("[TRACE] scanRollerReleased()\n");
    ContactData* contact = getSelectedScanContact(objects.roller_contacts_scan, scanResults);
    updateContactDetails(contact);
    
    // Update contactLastClicked for checkbox callbacks
    contactLastClicked = contact ? contact->nodeId : 0;
}

// populates the results in the 'scan' list of contacts
void populate_scan_list(lv_event_t* e)
{
    LV_LOG_TRACE("[TRACE] populate_scan_list()\n");

    // Safety check for valid roller object
    if (!objects.roller_contacts_scan || !lv_obj_is_valid(objects.roller_contacts_scan)) {
        LV_LOG_ERROR("[ERROR] Invalid roller_contacts_scan object\n");
        return;
    }

    // Preserve current selection
    uint32_t currentSelection = lv_roller_get_selected(objects.roller_contacts_scan);
    uint32_t selectedNodeId = 0;
    
    // Get currently selected contact's node ID
    ContactPtrVector oldContacts = scanResults->getContacts();
    if (currentSelection < oldContacts.size() && oldContacts[currentSelection]) {
        selectedNodeId = oldContacts[currentSelection]->nodeId;
    }

    // Set new options (filtered to exclude crew members)
    lv_roller_set_options(objects.roller_contacts_scan, rollerStringFromScanResults(scanResults, "No Scan Results"), LV_ROLLER_MODE_NORMAL);
    
    // Try to restore selection to same contact (accounting for crew filtering)
    bool selectionRestored = false;
    if (selectedNodeId != 0) {
        ContactPtrVector newContacts = scanResults->getContacts();
        uint32_t filteredIndex = 0;
        
        for (size_t i = 0; i < newContacts.size(); i++) {
            if (!newContacts[i]) continue;
            
            // Skip contacts that are crew members (same filtering as roller)
            ContactData* existingContact = config.contacts.findContact(newContacts[i]->nodeId);
            if (existingContact && existingContact->isCrew) {
                continue;
            }
            
            // Check if this is our target contact
            if (newContacts[i]->nodeId == selectedNodeId) {
                lv_roller_set_selected(objects.roller_contacts_scan, filteredIndex, LV_ANIM_OFF);
                LV_LOG_INFO("[DEBUG] Restored scan roller selection to filtered index %u (nodeId: %u)\n", filteredIndex, selectedNodeId);
                
                // Update UI details for restored selection
                updateContactDetails(newContacts[i]);
                contactLastClicked = selectedNodeId;
                selectionRestored = true;
                break;
            }
            
            filteredIndex++; // Only increment for non-crew contacts
        }
    }
    
    // If no selection was restored, check if filtered scan results are empty
    if (!selectionRestored) {
        ContactPtrVector currentContacts = scanResults->getContacts();
        bool hasNonCrewContacts = false;
        
        // Check if there are any non-crew contacts
        for (const auto& contact : currentContacts) {
            if (!contact) continue;
            
            ContactData* existingContact = config.contacts.findContact(contact->nodeId);
            if (!existingContact || !existingContact->isCrew) {
                hasNonCrewContacts = true;
                break;
            }
        }
        
        if (!hasNonCrewContacts) {
            LV_LOG_INFO("[DEBUG] No non-crew contacts in scan list, clearing contact details UI\n");
            updateContactDetails(nullptr);
            contactLastClicked = 0;
        }
    }

    // return if the contacts screen is NOT active to save CPU
    //if (lv_obj_get_screen(objects.contacts) != lv_scr_act()) return;

    ////static auto cached = scanResults->getContacts();
    ////add_virtual_contact_list(&virtual_scan_list, cached, objects.tab_contacts_scan);

    //// declare unordered maps
    //static std::unordered_map<int32_t, lv_obj_t*> scan_list_buttons;    // the map is between nodeIDs and pointers to button objects
    //std::unordered_map<int32_t, bool> currentIDs;                       // map between nodeID and if that ID has been seen

    //// populate scan_list_buttons with all existing buttons in the list
    //for (uint32_t i = 0; i < lv_obj_get_child_count(objects.list_contacts_scan); i++)
    //{
    //    lv_obj_t* button = lv_obj_get_child(objects.list_contacts_scan, i);
    //    int32_t id = (uint32_t)lv_obj_get_user_data(button);
    //    scan_list_buttons[id] = button; // create an id entry for each button
    //    currentIDs[id] = false;  // create an entry for each id and set each to false (false = 'id does not exist in scanResults')
    //}

    //// go through each contact in scanResults
    ////   update existing buttons with usernames
    ////   create new buttons for new contacts
    ////   remove buttons for contacts that have aged off
    //for (size_t i = 0; i < scanResults->size(); i++)
    //{
    //    // get a pointer to the i-th contact
    //    ContactData* contact = scanResults->getContacts()[i];
    //    if (!contact) continue; // just in case - this should never happen
    //    currentIDs[contact->nodeId] = true; // mark contact as 'exists' (creates the currentIDs[] entry if it doesn't exist)

    //    // if the contact is NOT listed in the UI yet
    //    if (scan_list_buttons.count(contact->nodeId) == 0)
    //    {
    //        // create the button
    //        lv_obj_t* entry = lv_list_add_button(objects.list_contacts_scan, img_avatar_16[contact->avatar % int_avatar_16].img, getNameFromContact(contact));
    //        lv_obj_set_user_data(entry, (void*)contact->nodeId);
    //        lv_obj_add_event_cb(entry, contactListButtonClick, LV_EVENT_CLICKED, NULL);
    //        scan_list_buttons[contact->nodeId] = entry;
    //        continue;
    //    }

    //    // if the contact IS listed in the UI already
    //    // update their display name
    //    else
    //    {
    //        // make sure the contact still exists
    //        if (currentIDs[contact->nodeId])
    //        {
    //            lv_list_set_button_text(objects.list_contacts_scan, scan_list_buttons[contact->nodeId], getNameFromContact(contact));
    //            continue;
    //        }
    //    }
    //}

    //// for each contact no longer present in scanResults, remove the button
    //for (const auto& [id, seen] : currentIDs)
    //{
    //    if (!seen && scan_list_buttons.count(id))
    //    {
    //        lv_obj_delete(scan_list_buttons[id]); // remove the button
    //        scan_list_buttons.erase(id);
    //    }
    //}

}

// Fires when a tab header on the contacts page is clicked
void tabContactsClicked(lv_event_t* e)
{
    play_random_beep();
    int_contacts_tab_current = (int32_t)lv_event_get_user_data(e);
    
    LV_LOG_INFO("[DEBUG] Switched to tab %d\n", int_contacts_tab_current);
    
    // Check if there's a selected contact in the new tab and show its details
    ContactData* selectedContact = nullptr;
    
    if (int_contacts_tab_current == 0) { // Scan tab
        selectedContact = getSelectedScanContact(objects.roller_contacts_scan, scanResults);
    } else if (int_contacts_tab_current == 1) { // Crew tab
        selectedContact = getSelectedContact(objects.roller_contacts_crew, &config.contacts);
    }
    
    if (selectedContact) {
        // Show the selected contact's details
        updateContactDetails(selectedContact);
        contactLastClicked = selectedContact->nodeId;
        LV_LOG_INFO("[DEBUG] Showing details for selected contact: %s (ID: %u)\n", 
                     selectedContact->displayName, selectedContact->nodeId);
    } else {
        // No contact selected, clear UI
        updateContactDetails(nullptr);
        contactLastClicked = 0;
        LV_LOG_INFO("[DEBUG] No contact selected, cleared details\n");
    }
}

// Public function to refresh the contact display for the current tab
void refreshCurrentTabContactDisplay() {
    // Check if there's a selected contact in the current tab and show its details
    ContactData* selectedContact = nullptr;
    
    if (int_contacts_tab_current == 0) { // Scan tab
        selectedContact = getSelectedScanContact(objects.roller_contacts_scan, scanResults);
    } else if (int_contacts_tab_current == 1) { // Crew tab
        selectedContact = getSelectedContact(objects.roller_contacts_crew, &config.contacts);
    }
    
    if (selectedContact) {
        // Show the selected contact's details
        updateContactDetails(selectedContact);
        contactLastClicked = selectedContact->nodeId;
        LV_LOG_INFO("[DEBUG] Refreshed display for selected contact: %s (ID: %u)\n", 
                     selectedContact->displayName, selectedContact->nodeId);
    } else {
        // No contact selected, clear UI
        updateContactDetails(nullptr);
        contactLastClicked = 0;
        LV_LOG_INFO("[DEBUG] No contact selected, cleared details\n");
    }
}

// Fires when the user clicks 'block' on the contacts page
void checkContactsBlockClick(lv_event_t* e)
{
    if (0 == contactLastClicked) return; // no contact selected, exit

    play_random_beep();

    char display[64];
    ContactData* contact;

    //if isBlocked is now TRUE:
    if (lv_obj_get_state(objects.check_contacts_block) & LV_STATE_CHECKED)
    {
        contact = config.contacts.findContact(contactLastClicked);
        if (contact) // if they're in the config.contacts list
        {
            contact->isBlocked = true;
            snprintf(display, sizeof(display), "Crewmate '%s' blocked.", getNameFromContact(contact));
            lv_label_set_text(objects.lbl_contacts_name, display);
        }
        else // if not, add them to config.contacts, remove from scanResults
        {
            contact = scanResults->findContact(contactLastClicked); // pull the info from scanResults
            if (contact)
            {
                contact->isBlocked = true; // set isBlocked = TRUE before storing
                config.contacts.addOrUpdateContact(*contact);
                scanResults->removeContact(contactLastClicked); // and remove them from scanResults
                snprintf(display, sizeof(display), "'%s' blocked.", getNameFromContact(contact));
                lv_label_set_text(objects.lbl_contacts_name, display);
            }
            else // contact no longer exists for some reason
            {
                lv_label_set_text(objects.lbl_contacts_name, "Contact lost.");
            }
        }
    }
    // if isBlocked is now FALSE:
    else
    {
        // crew = TRUE, isBlocked = FALSE -> keep in config.contacts
        if (lv_obj_get_state(objects.check_contacts_crew) & LV_STATE_CHECKED)
        {
            contact = config.contacts.findContact(contactLastClicked);
            if (contact) // if they're in the config.contacts list
            {
                contact->isBlocked = false;
                snprintf(display, sizeof(display), "Crew '%s' allowed.", getNameFromContact(contact));
                lv_label_set_text(objects.lbl_contacts_name, display);
            }
            else // contact no longer exists for some reason
            {
                lv_label_set_text(objects.lbl_contacts_name, "Contact lost.");
            }
        }
        else // crew = FALSE, isBlocked = FALSE -> remove from config.contacts (scanning will pick them up again when in range)
        {
            contact = config.contacts.findContact(contactLastClicked); // pull the info from scanResults
            if (contact)
            {
                snprintf(display, sizeof(display), "Contact '%s' allowed.", getNameFromContact(contact));
                lv_label_set_text(objects.lbl_contacts_name, display);
                config.contacts.removeContact(contactLastClicked);
            }
            else
            {
                lv_label_set_text(objects.lbl_contacts_name, "Contact lost.");
            }
        }
    }

    // clear out the details (prevents double-clicking from causing issues)
    updateContactDetails(nullptr);
    contactLastClicked = 0;

    // Save config changes to disk (async to avoid lag)
    extern void saveConfigAsync();
    saveConfigAsync();

    populate_crew_list(NULL); // then update the crew list
    populate_scan_list(NULL); // and update the scan list
}

// fires when a contact's 'Crew' block is un/checked
void checkContactsCrewClick(lv_event_t* e)
{
    if (0 == contactLastClicked) return; // no contact selected, exit

    play_random_beep();

    char display[64];
    ContactData* contact;

    //if crew is now TRUE:
    if (lv_obj_get_state(objects.check_contacts_crew) & LV_STATE_CHECKED)
    {
        contact = config.contacts.findContact(contactLastClicked);
        if (contact) // if they're in the config.contacts list
        {
            contact->isCrew = true;
            snprintf(display, sizeof(display), "'%s' added to crew.", getNameFromContact(contact));
            lv_label_set_text(objects.lbl_contacts_name, display);
        }
        else // if not, add them to config.contacts, remove from scanResults
        {
            contact = scanResults->findContact(contactLastClicked); // pull the info from scanResults
            if (contact)
            {
                contact->isCrew = true; // set crew = TRUE before storing
                config.contacts.addOrUpdateContact(*contact);
                snprintf(display, sizeof(display), "'%s' added to crew.", getNameFromContact(contact));
                lv_label_set_text(objects.lbl_contacts_name, display);
                scanResults->removeContact(contactLastClicked); // and remove them from scanResults
            }
            else // should never happen, but just in case...
            {
                lv_label_set_text(objects.lbl_contacts_name, "Contact lost.");
            }
        }
    }
    // if crew is now FALSE:
    else
    {
        // crew = FALSE, isBlocked = TRUE -> keep in config.contacts
        if (lv_obj_get_state(objects.check_contacts_block) & LV_STATE_CHECKED)
        {
            contact = config.contacts.findContact(contactLastClicked);
            if (contact) // if they're in the config.contacts list
            {
                contact->isCrew = false;
                snprintf(display, sizeof(display), "'%s' removed from crew.", getNameFromContact(contact));
                lv_label_set_text(objects.lbl_contacts_name, display);
            }
            else
            {
                lv_label_set_text(objects.lbl_contacts_name, "Contact lost.");
            }
        }
        else // crew = FALSE, isBlocked = FALSE -> remove from config.contacts (scanning will pick them up again when in range)
        {
            contact = config.contacts.findContact(contactLastClicked);
            if (contact)
            {
                snprintf(display, sizeof(display), "'%s' removed from crew.", getNameFromContact(contact));
                lv_label_set_text(objects.lbl_contacts_name, display);
                config.contacts.removeContact(contactLastClicked);
            }
            else
            {
                lv_label_set_text(objects.lbl_contacts_name, "Contact lost.");
            }
        }
    }

    // clear out the details (prevents double-clicking from causing issues)
    updateContactDetails(nullptr);
    contactLastClicked = 0;

    // Save config changes to disk (async to avoid lag)
    extern void saveConfigAsync();
    saveConfigAsync();

    populate_crew_list(NULL); // then update the crew list
    populate_scan_list(NULL); // and update the scan list
}
