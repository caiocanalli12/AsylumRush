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
    Texture2D ifsp_highschool;
    Texture2D polarbear;
    Texture2D fly_attack_polarbear;
    Texture2D mezzanine_railing;
    Texture2D menu;
    Texture2D frozensuburbs_blurred;
    Texture2D earDog;
    Texture2D wolfDummy;
    Texture2D iceShard;
} ResourceManager;

extern ResourceManager rm;

void loadResourcesResourceManager( void );
void unloadResourcesResourceManager( void );