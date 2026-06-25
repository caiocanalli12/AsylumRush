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
    rm.wolfDummy = LoadTexture( "resources/imagens/wolf_dummy.png" );
    rm.iceShard = LoadTexture( "resources/imagens/ice_shard.png" );
    rm.belial = LoadTexture( "resources/imagens/belial.png" );
    rm.gameoverBg = LoadTexture( "resources/imagens/gameover.png" );
    
    // Audios
    rm.bgmFrozen = LoadMusicStream( "resources/audios/frozensuburbs.mp3" );
    rm.bgmBoss = LoadMusicStream( "resources/audios/bossfight.wav" );
    rm.sfxHit = LoadSound( "resources/audios/hit.wav" );
    rm.sfxEspecial = LoadSound( "resources/audios/especial_polarbear.wav" );
    rm.sfxIceShardDeath = LoadSound( "resources/audios/iceshard_death.wav" );
    rm.sfxIceShardHit = LoadSound( "resources/audios/iceshard_hit.wav" );
    rm.sfxEarDogSpawn = LoadSound( "resources/audios/eardog.wav" );
    rm.sfxEarDogAttack = LoadSound( "resources/audios/eardog_atack.wav" );
    rm.sfxWolfAttack = LoadSound( "resources/audios/wolf_atack.wav" );
    rm.sfxLatido = LoadSound( "resources/audios/latido.wav" );
    rm.sfxBelialJump = LoadSound( "resources/audios/belial_jump.wav" );
    
    // Set bilinear filter to eliminate scaling aliasing/serrilhamento
    SetTextureFilter( rm.menu, TEXTURE_FILTER_BILINEAR );
    SetTextureFilter( rm.frozensuburbs_blurred, TEXTURE_FILTER_BILINEAR );
    SetTextureFilter( rm.wolfDummy, TEXTURE_FILTER_BILINEAR );
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
    UnloadTexture( rm.wolfDummy );
    UnloadTexture( rm.iceShard );
    UnloadTexture( rm.belial );
    UnloadTexture( rm.gameoverBg );
    
    // Unload Audios
    UnloadMusicStream( rm.bgmFrozen );
    UnloadMusicStream( rm.bgmBoss );
    UnloadSound( rm.sfxHit );
    UnloadSound( rm.sfxEspecial );
    UnloadSound( rm.sfxIceShardDeath );
    UnloadSound( rm.sfxIceShardHit );
    UnloadSound( rm.sfxEarDogSpawn );
    UnloadSound( rm.sfxEarDogAttack );
    UnloadSound( rm.sfxWolfAttack );
    UnloadSound( rm.sfxLatido );
}