#pragma once
#include <painlessMesh.h>
#include "json.h"  // For Config struct

// Default values if config fails to load
#define   DEFAULT_MESH_PREFIX     "whateverYouLike"
#define   DEFAULT_MESH_PASSWORD   ""
#define   DEFAULT_MESH_PORT       5555

extern painlessMesh  mesh;
extern Scheduler userScheduler; // to control your personal task

void init_mesh();
void stop_mesh();
void getNeighbors(bool self);
void getJSONMap();
void checkMeshHealth();  // Monitor and recover from mesh issues

// Callback function prototypes
void receivedCallback(uint32_t from, String &msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void handleSimulatorCommand(String input);