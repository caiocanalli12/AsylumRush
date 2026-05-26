/**
 * @file ResourceManager.h
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager struct and function declarations (Template Cru).
 * 
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "raylib/raylib.h"

typedef struct ResourceManager {
    Texture2D frozensuburbs;
    Texture2D polarbear;
} ResourceManager;

extern ResourceManager rm;

void loadResourcesResourceManager( void );
void unloadResourcesResourceManager( void );