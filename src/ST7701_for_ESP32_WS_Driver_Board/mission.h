#pragma once

#include "custom.h"

void setMissionReadyState(lv_event_t* e);
void hostPlayerListClick(lv_event_t* e);
void hostKickClicked(lv_event_t* e);
void hostStartClicked(lv_event_t* e);
void hostScreenLoaded(lv_event_t* e);
void joinScreenLoaded(lv_event_t* e);
void screenUnloaded(lv_event_t* e);
void joinRollerChanged(lv_event_t* e);
void joinReadyClicked(lv_event_t* e);
void joinGameChanged(lv_event_t* e);
void hostGameChanged(lv_event_t* e);