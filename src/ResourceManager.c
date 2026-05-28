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
    rm.ifsp_highschool = LoadTexture( "resources/imagens/fundo/ifsp_highschool.png" );
    rm.polarbear = LoadTexture( "resources/imagens/polarbear.png" );
    rm.fly_attack_polarbear = LoadTexture( "resources/imagens/fly_attack_polarbear.png" );
    rm.mezzanine_railing = LoadTexture( "resources/imagens/fundo/mezzanine_railing.png" );
    rm.menu = LoadTexture( "resources/imagens/fundo/menu.png" );
    rm.frozensuburbs_blurred = LoadTexture( "resources/imagens/fundo/frozensuburbs_blurred.png" );
    rm.earDog = LoadTexture( "resources/imagens/EarDog.png" );
    
    // Set bilinear filter to eliminate scaling aliasing/serrilhamento
    SetTextureFilter( rm.menu, TEXTURE_FILTER_BILINEAR );
    SetTextureFilter( rm.frozensuburbs_blurred, TEXTURE_FILTER_BILINEAR );
}

void unloadResourcesResourceManager( void ) {
    UnloadTexture( rm.frozensuburbs );
    UnloadTexture( rm.ifsp_highschool );
    UnloadTexture( rm.polarbear );
    UnloadTexture( rm.fly_attack_polarbear );
    UnloadTexture( rm.mezzanine_railing );
    UnloadTexture( rm.menu );
    UnloadTexture( rm.frozensuburbs_blurred );
    UnloadTexture( rm.earDog );
}