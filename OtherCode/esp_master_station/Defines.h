#ifndef _DEFINES_H
#define _DEFINES_H

#include <painlessMesh.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#define   MESH_SSID       "mblocks3d"
#define   MESH_PASSWORD   "mblocks3d"
#define   MESH_PORT       5555

typedef struct state
{
  painlessMesh* mesh;
} state_t;

#endif
