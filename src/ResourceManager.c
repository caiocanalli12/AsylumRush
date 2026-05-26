/**
 * @file ResourceManager.c
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager implementation (Template Cru).
 * 
 * @copyright Copyright (c) 2026
 */
#include "ResourceManager.h"

ResourceManager rm = { 0 };

void loadResourcesResourceManager( void ) {
    rm.frozensuburbs = LoadTexture( "resources/imagens/fundo/frozensuburbs.png" );
    rm.polarbear = LoadTexture( "resources/imagens/polarbear.png" );
    rm.mezzanine_railing = LoadTexture( "resources/imagens/fundo/mezzanine_railing.png" );
}

void unloadResourcesResourceManager( void ) {
    UnloadTexture( rm.frozensuburbs );
    UnloadTexture( rm.polarbear );
    UnloadTexture( rm.mezzanine_railing );
}