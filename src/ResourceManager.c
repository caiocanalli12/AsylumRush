/**
 * @file ResourceManager.c
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager implementation.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "ResourceManager.h"
#include "Utils.h"

static void limparFundoBrancoCabecaSonic( Image *imagem ) {
    Color targetColor = GetImageColor( *imagem, 40, 400 );
    if ( targetColor.a == 0 ) return;

    int stackX[256];
    int stackY[256];
    int stackPtr = 0;

    stackX[stackPtr] = 40;
    stackY[stackPtr] = 400;
    stackPtr++;

    ImageDrawPixel( imagem, 40, 400, BLANK );

    while ( stackPtr > 0 ) {
        stackPtr--;
        int cx = stackX[stackPtr];
        int cy = stackY[stackPtr];

        int dx[] = { -1, 1, 0, 0 };
        int dy[] = { 0, 0, -1, 1 };

        for ( int i = 0; i < 4; i++ ) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if ( nx >= 40 && nx <= 55 && ny >= 400 && ny <= 415 ) {
                Color nc = GetImageColor( *imagem, nx, ny );
                if ( nc.r == targetColor.r && nc.g == targetColor.g && nc.b == targetColor.b && nc.a == targetColor.a ) {
                    ImageDrawPixel( imagem, nx, ny, BLANK );
                    if ( stackPtr < 256 ) {
                        stackX[stackPtr] = nx;
                        stackY[stackPtr] = ny;
                        stackPtr++;
                    }
                }
            }
        }
    }
}

ResourceManager rm = { 0 };

void loadResourcesResourceManager( void ) {

    rm.texturaJogador = carregarTexturaAlterandoCores( 
        "resources/imagens/sprites/sonic.png",
        (Color[]) {
            { 37, 102, 26, 255 },
            { 13, 72, 7, 255 },
        },
        (Color[]) {
            BLANK,
            BLANK,
        },
        2
    );

    rm.texturaBadniks = carregarTexturaAlterandoCores( 
        "resources/imagens/sprites/badniks.png",
        (Color[]) {
            { 13, 72, 7, 255 },
        },
        (Color[]) {
            BLANK,
        },
        1
    );

    rm.texturaItens = carregarTexturaAlterandoCores( 
        "resources/imagens/itens/itens.png",
        (Color[]) {
            { 16, 112, 132, 255 },
        },
        (Color[]) {
            BLANK,
        },
        1
    );

    rm.texturaTerreno = LoadTexture( "resources/imagens/tiles/terreno.png" );
    rm.texturaFundo = LoadTexture( "resources/imagens/fundo/fundo.png" );

    // Load HUD texture and make background and Sonic's head box background transparent
    Image imgHud = LoadImage( "resources/imagens/hud/hud.png" );
    ImageFormat( &imgHud, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 );
    ImageColorReplace( &imgHud, (Color){ 16, 112, 132, 255 }, BLANK );
    limparFundoBrancoCabecaSonic( &imgHud );
    rm.texturaHud = LoadTextureFromImage( imgHud );
    UnloadImage( imgHud );

    // Filtro de textura nearest-neighbor (ponto) para todas as texturas do jogo.
    // Evita interpolação bilinear nas bordas dos tiles e sprites, que causaria
    // bleeding de cor entre pixels adjacentes — efeito indesejável em pixel art.
    SetTextureFilter( rm.texturaJogador, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaBadniks, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaItens, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaTerreno, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaFundo, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaHud, TEXTURE_FILTER_POINT );

    rm.somAnel = LoadSound( "resources/sons/efeitos/anel.wav" );
    rm.somFrenagem = LoadSound( "resources/sons/efeitos/frenagem.wav" );
    rm.somHitComAnel = LoadSound( "resources/sons/efeitos/hit-com-anel.wav" );
    rm.somHitInimigo = LoadSound( "resources/sons/efeitos/hit-inimigo.wav" );
    rm.somMorte = LoadSound( "resources/sons/efeitos/morte.wav" );
    rm.somPulo = LoadSound( "resources/sons/efeitos/pulo.wav" );

    rm.musicaFase01 = LoadMusicStream( "resources/sons/musicas/green-hill-zone.mp3" );

}

void unloadResourcesResourceManager( void ) {

    UnloadTexture( rm.texturaJogador );
    UnloadTexture( rm.texturaBadniks );
    UnloadTexture( rm.texturaItens );
    UnloadTexture( rm.texturaTerreno );
    UnloadTexture( rm.texturaFundo );
    UnloadTexture( rm.texturaHud );

    UnloadSound( rm.somAnel );
    UnloadSound( rm.somFrenagem );
    UnloadSound( rm.somHitComAnel );
    UnloadSound( rm.somHitInimigo );
    UnloadSound( rm.somMorte );
    UnloadSound( rm.somPulo );

    UnloadMusicStream( rm.musicaFase01 );

}