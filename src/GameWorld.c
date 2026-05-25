/**
 * @file GameWorld.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do GameWorld.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "GameWorld.h"
#include "Jogador.h"
#include "Macros.h"
#include "Mapa.h"
#include "Obstaculo.h"
#include "Tipos.h"
#include "ResourceManager.h"

#include "raylib/raylib.h"
//#include "raylib/raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raylib/raygui.h"       // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

static void desenharFundo( GameWorld *gw );
static void atualizarCamera( GameWorld *gw );

static void inicializar( GameWorld *gw );
static void reiniciar( GameWorld *gw );

/**
 * @brief Cria uma instância alocada dinamicamente da struct GameWorld.
 */
GameWorld *createGameWorld( void ) {
    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
    inicializar( gw );
    return gw;
}

/**
 * @brief Destrói um objeto GameWorld e suas dependências.
 */
void destroyGameWorld( GameWorld *gw ) {
    if ( gw != NULL ) {
        destruirMapa( gw->mapa );
        destruirJogador( gw->jogador );
        free( gw );
    }
}

/**
 * @brief Lê a entrada do usuário e atualiza o estado do jogo.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    if ( !IsMusicStreamPlaying( rm.musicaFase01 ) ) {
        PlayMusicStream( rm.musicaFase01 );
    } else {
        UpdateMusicStream( rm.musicaFase01 );
    }

    if ( IsKeyPressed( KEY_R ) ) {
        reiniciar( gw );
        return;
    }

    Jogador *j = gw->jogador;
    gw->tempoDeJogo += delta;
    atualizarMapa( gw->mapa, gw, delta );
    entradaJogador( j, delta );
    atualizarJogador( j, gw, delta );
    atualizarCamera( gw );

}

/**
 * @brief Desenha o estado do jogo.
 */
static void desenharHudRec( Rectangle src, float x, float y, Color cor ) {
    Rectangle dest = { x, y, src.width * 2.0f, src.height * 2.0f };
    DrawTexturePro( rm.texturaHud, src, dest, (Vector2){ 0, 0 }, 0.0f, cor );
}

static void desenharNumeroHUD( int valor, int xRight, int y ) {
    if ( valor == 0 ) {
        Rectangle src = { 73 + 0 * 16, 433, 8, 11 };
        desenharHudRec( src, xRight - 16, y, WHITE );
        return;
    }
    
    int temp = valor;
    int curX = xRight;
    while ( temp > 0 ) {
        int dig = temp % 10;
        curX -= 16;
        Rectangle src = { 73 + dig * 16, 433, 8, 11 };
        desenharHudRec( src, curX, y, WHITE );
        temp /= 10;
    }
}

static void desenharTempoHUD( float tempo, int xRight, int y ) {
    int minutos = (int)(tempo / 60);
    int segundos = (int)(tempo) % 60;
    int centisegundos = (int)((tempo - (int)tempo) * 100);
    if ( centisegundos > 99 ) centisegundos = 99;
    if ( minutos > 9 ) minutos = 9;
    
    int curX = xRight;
    
    // Centisegundos
    int digit1 = centisegundos % 10;
    int digit2 = centisegundos / 10;
    
    curX -= 16;
    Rectangle srcDec = { 73 + digit1 * 16, 433, 8, 11 };
    desenharHudRec( srcDec, curX, y, WHITE );
    
    curX -= 16;
    Rectangle srcCent = { 73 + digit2 * 16, 433, 8, 11 };
    desenharHudRec( srcCent, curX, y, WHITE );
    
    // Dois pontos
    curX -= 10;
    Rectangle srcColon = { 90, 226, 5, 9 };
    desenharHudRec( srcColon, curX, y + 2, WHITE );
    
    // Segundos
    digit1 = segundos % 10;
    digit2 = segundos / 10;
    
    curX -= 16;
    Rectangle srcSec1 = { 73 + digit1 * 16, 433, 8, 11 };
    desenharHudRec( srcSec1, curX, y, WHITE );
    
    curX -= 16;
    Rectangle srcSec2 = { 73 + digit2 * 16, 433, 8, 11 };
    desenharHudRec( srcSec2, curX, y, WHITE );
    
    // Dois pontos
    curX -= 10;
    desenharHudRec( srcColon, curX, y + 2, WHITE );
    
    // Minutos
    curX -= 16;
    Rectangle srcMin = { 73 + minutos * 16, 433, 8, 11 };
    desenharHudRec( srcMin, curX, y, WHITE );
}

static void desenharVidasHUD( int vidas, int x, int y ) {
    Rectangle srcFace = { 40, 400, 16, 16 };
    desenharHudRec( srcFace, x, y, WHITE );
    
    Rectangle srcSonic = { 57, 400, 30, 8 };
    desenharHudRec( srcSonic, x + 36, y, WHITE );
    
    Rectangle srcCross = { 64, 409, 8, 6 };
    desenharHudRec( srcCross, x + 44, y + 18, WHITE );
    
    if ( vidas < 0 ) {
        // Draw custom minus sign with black shadow
        DrawRectangle( x + 66, y + 16, 8, 4, BLACK );
        DrawRectangle( x + 64, y + 14, 8, 4, WHITE );
        
        int count = -vidas;
        if ( count > 9 ) count = 9; // Clamp absolute value to 9
        Rectangle srcNum = { 73 + count * 16, 433, 8, 11 };
        desenharHudRec( srcNum, x + 76, y + 10, WHITE );
    } else {
        int count = (vidas > 9) ? 9 : vidas;
        Rectangle srcNum = { 73 + count * 16, 433, 8, 11 };
        desenharHudRec( srcNum, x + 64, y + 10, WHITE );
    }
}

/**
 * @brief Desenha o estado do jogo.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( (Color) { 36, 0, 180, 255 } );

    BeginMode2D( gw->camera );
    desenharFundo( gw );
    desenharMapa( gw->mapa );
    desenharJogador( gw->jogador );
    EndMode2D();

    // Desenhar Rótulos do HUD (SCORE, TIME, RINGS)
    // SCORE label (X=25, Y=433, W=40, H=11 no spritesheet)
    desenharHudRec( (Rectangle){ 25, 433, 40, 11 }, 16, 16, WHITE );
    // TIME label (X=25, Y=457, W=32, H=11 no spritesheet)
    desenharHudRec( (Rectangle){ 25, 457, 32, 11 }, 16, 40, WHITE );
    // RINGS label (X=25 ou 73 piscando, Y=481, W=40, H=11 no spritesheet)
    bool ringsFlash = (gw->jogador->quantidadeAneis == 0) && (((int)(GetTime() * 4) % 2) == 0);
    Rectangle srcRings = ringsFlash ? (Rectangle){ 73, 481, 40, 11 } : (Rectangle){ 25, 481, 40, 11 };
    desenharHudRec( srcRings, 16, 64, WHITE );

    // Desenhar Valores do HUD (Alinhados à direita na coluna X=240)
    desenharNumeroHUD( gw->jogador->score, 240, 16 );
    desenharTempoHUD( gw->tempoDeJogo, 240, 40 );
    desenharNumeroHUD( gw->jogador->quantidadeAneis, 240, 64 );

    // Desenhar Vidas no Canto Inferior Esquerdo
    desenharVidasHUD( gw->jogador->quantidadeVidas, 16, GetScreenHeight() - 48 );

    // Textos de diagnóstico e FPS no canto superior direito
    DrawFPS( GetScreenWidth() - 100, 10 );
    if ( gw->jogador->invulneravel ) {
        DrawText( 
            TextFormat( "Invulnerável: (%.2fs/%.2fs)", gw->jogador->contadorTempoInvulnerabilidade, gw->jogador->tempoInvulnerabilidade ), 
            GetScreenWidth() - 320, 30, 20, ORANGE
        );
    }

    EndDrawing();

}

static void desenharFundo( GameWorld *gw ) {

    int larguraFundo = rm.texturaFundo.width;
    int larguraMapa = calcularLarguraMapa( gw->mapa );
    int alturaMapa = calcularAlturaMapa( gw->mapa );
    int repeticoes = larguraMapa / larguraFundo;

    int deslocamentoParallax = (int) ( ( gw->camera.target.x / (float) larguraMapa ) * 200 );

    for ( int i = 0; i <= repeticoes; i++ ) {
        DrawTexture( rm.texturaFundo, larguraFundo * i - deslocamentoParallax, alturaMapa - rm.texturaFundo.height, WHITE );
    }

}

static void atualizarCamera( GameWorld *gw ) {

    Jogador *j = gw->jogador;
    Camera2D *c = &gw->camera;

    c->offset.x = GetScreenWidth() / 2;
    c->offset.y = GetScreenHeight() / 2;

    // O target é arredondado para o inteiro mais próximo para garantir que a
    // translação da câmera ocorra sempre em posições inteiras de pixel. Sem esse
    // arredondamento, o valor float contínuo de ret.x faz os tiles serem
    // renderizados em posições subpixel, causando frestas visíveis entre eles.
    c->target.x = roundf( j->ret.x + j->ret.width / 2.0f );
    c->target.y = roundf( j->ret.y + j->ret.height / 2.0f );

    int minX = GetScreenWidth() / 2;
    int maxX = calcularLarguraMapa( gw->mapa ) - GetScreenWidth() / 2;
    int maxY = calcularAlturaMapa( gw->mapa ) - GetScreenHeight() / 2;

    if ( c->target.x < minX ) {
        c->target.x = minX;
    } else if ( c->target.x > maxX ) {
        c->target.x = maxX;
    }

    if ( c->target.y > maxY ) {
        c->target.y = maxY;
    }

}

static void inicializar( GameWorld *gw ) {

    //gw->mapa = carregarMapa( "resources/mapas/mapaTeste.txt" );
    gw->mapa = carregarMapa( "resources/mapas/mapa01.txt" );
    gw->jogador = criarJogador( GetScreenWidth() / 2 + 144, calcularAlturaMapa( gw->mapa ) - 196, 96, 96 );

    gw->camera = (Camera2D) {
        .offset = { 0 },    // deslocamento relativo da câmera em relação ao alvo
        .target = { 0 },    // o alvo da câmera, ou seja, a coordenada em que ela está centralizada
        .rotation = 0.0f,   // rotação da câmera em graus. o pivô é o alvo.
        .zoom = 1.0f        // zoom da câmera. 1.0f significa sem escala
    };

    gw->gravidade = 900;
    gw->tempoDeJogo = 0.0f;

}

static void reiniciar( GameWorld *gw ) {

    destruirMapa( gw->mapa );
    destruirJogador( gw->jogador );

    if ( IsMusicStreamPlaying( rm.musicaFase01 ) ) {
        StopMusicStream( rm.musicaFase01 );
    }

    inicializar( gw );

}
