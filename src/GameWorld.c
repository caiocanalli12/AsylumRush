/**
 * @file GameWorld.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do GameWorld (Template Cru).
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

    if ( IsKeyPressed( KEY_R ) ) {
        reiniciar( gw );
        return;
    }

    Jogador *j = gw->jogador;
    gw->tempoDeJogo += delta;
    atualizarMapa( gw->mapa, gw, delta );
    entradaJogador( j, gw, delta );
    atualizarJogador( j, gw, delta );
    atualizarCamera( gw );

}

/**
 * @brief Desenha o estado do jogo.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( (Color) { 135, 206, 235, 255 } ); // Sky blue color

    BeginMode2D( gw->camera );
    desenharFundo( gw );
    desenharMapa( gw->mapa );
    // Cerca: fica ATRÁS do jogador quando ele está na rua, e À FRENTE quando está no mezanino
    bool noMezanino = gw->jogador->noMezanino;
    if ( !noMezanino ) {
        DrawTexture( rm.mezzanine_railing, 8300, 130, WHITE );
    }
    desenharJogador( gw->jogador );
    if ( noMezanino ) {
        DrawTexture( rm.mezzanine_railing, 8300, 130, WHITE );
    }
    EndMode2D();




    EndDrawing();

}

static void desenharFundo( GameWorld *gw ) {
    // Desenha o cenário suburbs excluindo a borda azul externa (10px esquerda, 30px topo)
    DrawTextureRec(
        rm.frozensuburbs,
        (Rectangle){ 10, 30, 10603, 283 },
        (Vector2){ 0, 0 },
        WHITE
    );
}

static void atualizarCamera( GameWorld *gw ) {

    Jogador *j = gw->jogador;
    Camera2D *c = &gw->camera;

    // Zoom dinâmico: mantém os 283px de altura do mundo preenchendo a tela (proporção correta)
    c->zoom = GetScreenHeight() / 283.0f;

    c->offset.x = GetScreenWidth() / 2.0f;
    c->offset.y = GetScreenHeight() / 2.0f;

    c->target.x = roundf( j->ret.x + j->ret.width / 2.0f );
    c->target.y = 283.0f / 2.0f; // Centraliza verticalmente a fase de altura 283 na tela

    int minX = (GetScreenWidth() / c->zoom) / 2;
    int maxX = calcularLarguraMapa( gw->mapa ) - (GetScreenWidth() / c->zoom) / 2;

    if ( c->target.x < minX ) {
        c->target.x = minX;
    } else if ( c->target.x > maxX ) {
        c->target.x = maxX;
    }

}


static void inicializar( GameWorld *gw ) {

    gw->mapa = carregarMapa( "resources/mapas/mapa_modelo.txt" );
    
    // Spawnar jogador em cima do novo chão (Y=220, altura do jogador é 30)
    float spawnX = 150.0f;
    float spawnY = 220.0f - 30.0f;
    gw->jogador = criarJogador( spawnX, spawnY, 20, 30 );

    gw->camera = (Camera2D) {
        .offset = { 0 },
        .target = { 0 },
        .rotation = 0.0f,
        .zoom = GetScreenHeight() / 283.0f  // Ajusta zoom para a altura do mundo (283px) preencher a tela
    };

    gw->gravidade = 900;
    gw->tempoDeJogo = 0.0f;

}

static void reiniciar( GameWorld *gw ) {
    destruirMapa( gw->mapa );
    destruirJogador( gw->jogador );
    inicializar( gw );
}
