#pragma once

#include "custom.h"

extern Config config;
extern ContactManager* scanResults;
extern int32_t int_contacts_tab_current;  // what contacts tab is being shown: -1 = none, 0 = scan, 1 = crew

extern uint32_t contactLastClicked;        // the ID of the last contact that was clicked
extern lv_obj_t* virtual_scan_list;
extern lv_obj_t* virtual_crew_list;

ContactData* idPacketToContactData(IDPacket* idPacket);
void contactListButtonClick(lv_event_t* e);
void populate_crew_list(lv_event_t* e);
void populate_scan_list(lv_event_t* e);
void tabContactsClicked(lv_event_t* e);
void checkContactsBlockClick(lv_event_t* e);
void checkContactsCrewClick(lv_event_t* e);
void crewRollerReleased(lv_event_t* e);
void scanRollerReleased(lv_event_t* e);
void refreshCurrentTabContactDisplay();

// Helper functions for contact management
const char* rollerStringFromContactManager(ContactManager* contactManager, String defaultString);
const char* rollerStringFromScanResults(ContactManager* scanManager, String defaultString);

using ContactPtrVector = std::vector<ContactData*, PsramAllocator<ContactData*>>;
void add_virtual_contact_list(lv_obj_t** targetList, const ContactPtrVector& source, lv_obj_t* parent);
void update_virtual_contact_rows(lv_event_t* e);
