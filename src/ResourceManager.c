/**
 * @file ResourceManager.c
 * @author thale
 * @brief ResourceManager implementation (Template Cru).
 * 
 * @copyright Copyright (c) 2026
 */
#include "ResourceManager.h"

ResourceManager rm = { 0 };

void loadResourcesResourceManager( void ) {
    rm.frozensuburbs = LoadTexture( "resources/imagens/fundo/frozensuburbs.png" );
    rm.polarbear = LoadTexture( "resources/imagens/polarbear.png" );
    rm.fly_attack_polarbear = LoadTexture( "resources/imagens/fly_attack_polarbear.png" );
    rm.mezzanine_railing = LoadTexture( "resources/imagens/fundo/mezzanine_railing.png" );
}

void unloadResourcesResourceManager( void ) {
    UnloadTexture( rm.frozensuburbs );
    UnloadTexture( rm.polarbear );
    UnloadTexture( rm.fly_attack_polarbear );
    UnloadTexture( rm.mezzanine_railing );
}