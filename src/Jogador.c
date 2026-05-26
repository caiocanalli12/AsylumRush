/**
 * @file Jogador.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Jogador (Template Cru).
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Macros.h"
#include "Jogador.h"
#include "Mapa.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void resolverColisaoJogadorObstaculosMapaX( Jogador *j, Mapa *mapa );
static void resolverColisaoJogadorObstaculosMapaY( Jogador *j, Mapa *mapa );

/**
 * @brief Cria uma instância alocada dinamicamente da struct Jogador.
 */
Jogador *criarJogador( float x, float y, float w, float h ) {

    Jogador *novoJogador = (Jogador*) malloc( sizeof( Jogador ) );

    novoJogador->ret.x = x;
    novoJogador->ret.y = y;
    novoJogador->ret.width = w;
    novoJogador->ret.height = h;
    novoJogador->vel = (Vector2) { 0 };

    novoJogador->cor = BLUE;

    novoJogador->velAndando = 300;
    novoJogador->velPulo = -500;
    novoJogador->velMaxQueda = 600;

    novoJogador->aceleracao = 1000;
    novoJogador->desaceleracao = 800;

    novoJogador->quantidadePulos = 0;
    novoJogador->quantidadeMaxPulos = 1;

    novoJogador->quantidadeVidas = 3;
    novoJogador->score = 0;

    novoJogador->estado = ESTADO_JOGADOR_PARADO;
    novoJogador->olhandoParaDireita = true;
    novoJogador->animTimer = 0.0f;
    novoJogador->animFrame = 0;

    return novoJogador;

}

/**
 * @brief Destrói um objeto Jogador e libera seus recursos.
 */
void destruirJogador( Jogador *j ) {
    if ( j != NULL ) {
        free( j );
    }
}

/**
 * @brief Lê a entrada do usuário e atualiza as velocidades do jogador.
 */
void entradaJogador( Jogador *j, float delta ) {

    bool direitaDown  = IsKeyDown( KEY_RIGHT ) || IsKeyDown( KEY_D );
    bool esquerdaDown = IsKeyDown( KEY_LEFT )  || IsKeyDown( KEY_A );
    bool puloPressed  = IsKeyPressed( KEY_SPACE ) || IsKeyPressed( KEY_W ) || IsKeyPressed( KEY_UP );

    if ( direitaDown ) {
        j->vel.x += j->aceleracao * delta;
        if ( j->vel.x > j->velAndando ) {
            j->vel.x = j->velAndando;
        }
        j->olhandoParaDireita = true;
    } else if ( esquerdaDown ) {
        j->vel.x -= j->aceleracao * delta;
        if ( j->vel.x < -j->velAndando ) {
            j->vel.x = -j->velAndando;
        }
        j->olhandoParaDireita = false;
    } else {
        if ( j->vel.x > 0 ) {
            j->vel.x -= j->desaceleracao * delta;
            if ( j->vel.x < 0 ) j->vel.x = 0;
        } else if ( j->vel.x < 0 ) {
            j->vel.x += j->desaceleracao * delta;
            if ( j->vel.x > 0 ) j->vel.x = 0;
        }
    }

    if ( j->quantidadePulos > 0 ) {
        j->estado = ESTADO_JOGADOR_PULANDO;
    } else if ( fabsf( j->vel.x ) > 1.0f ) {
        j->estado = ESTADO_JOGADOR_ANDANDO;
    } else {
        j->estado = ESTADO_JOGADOR_PARADO;
    }

    if ( puloPressed && j->quantidadePulos < j->quantidadeMaxPulos ) {
        j->vel.y = j->velPulo;
        j->quantidadePulos++;
    }

}

/**
 * @brief Aplica física e resolve colisões do jogador com o mundo.
 */
void atualizarJogador( Jogador *j, GameWorld *gw, float delta ) {

    // Eixo X: Move horizontalmente e resolve colisões
    j->ret.x += j->vel.x * delta;
    resolverColisaoJogadorObstaculosMapaX( j, gw->mapa );

    // Eixo Y: Aplica gravidade, move verticalmente e resolve colisões
    j->vel.y += gw->gravidade * delta;
    if ( j->vel.y > j->velMaxQueda ) {
        j->vel.y = j->velMaxQueda;
    }
    j->ret.y += j->vel.y * delta;
    resolverColisaoJogadorObstaculosMapaY( j, gw->mapa );

    // Se o jogador cair do mapa, perde uma vida e respawna
    float limiteQueda = calcularAlturaMapa( gw->mapa );
    if ( j->ret.y > limiteQueda ) {
        j->quantidadeVidas--;
        
        // Respawnar jogador na posição inicial
        j->ret.x = 150.0f;
        j->ret.y = 220.0f - j->ret.height;
        j->vel = (Vector2){ 0, 0 };
        j->quantidadePulos = 0;
        j->estado = ESTADO_JOGADOR_PARADO;
    }

    // Atualiza a animação do urso polar
    if ( j->estado == ESTADO_JOGADOR_PARADO ) {
        j->animTimer = 0.0f;
        j->animFrame = 0;
    } else if ( j->estado == ESTADO_JOGADOR_ANDANDO ) {
        // Velocidade da animação proporcional à velocidade do jogador (fps base = 12.0)
        float animSpeed = ( fabsf( j->vel.x ) / j->velAndando ) * 12.0f;
        if ( animSpeed < 4.0f ) animSpeed = 4.0f; // velocidade mínima para movimento sutil
        j->animTimer += delta * animSpeed;
        j->animFrame = (int) j->animTimer;
    } else if ( j->estado == ESTADO_JOGADOR_PULANDO ) {
        if ( j->vel.y < 0 ) {
            j->animFrame = 3; // Subindo (linha 1 col 4 / walk_frames[3])
        } else {
            j->animFrame = 5; // Caindo (linha 1 col 6 / walk_frames[5])
        }
        j->animTimer = 0.0f;
    }

}

/**
 * @brief Desenha o jogador.
 */
void desenharJogador( Jogador *j ) {

    // Retângulos dos frames da animação de andar
    static const Rectangle walk_frames[6] = {
        { 8, 119, 51, 65 },
        { 72, 123, 49, 61 },
        { 128, 122, 48, 62 },
        { 192, 116, 48, 68 },
        { 248, 126, 52, 58 },
        { 312, 125, 49, 59 }
    };



    Rectangle src;
    if ( j->estado == ESTADO_JOGADOR_PARADO ) {
        src = walk_frames[0];
    } else if ( j->estado == ESTADO_JOGADOR_ANDANDO ) {
        src = walk_frames[j->animFrame % 6];
    } else { // ESTADO_JOGADOR_PULANDO
        src = walk_frames[j->animFrame % 6];
    }

    // Inverte a imagem horizontalmente se o jogador estiver olhando para a esquerda
    if ( !j->olhandoParaDireita ) {
        src.width = -src.width;
    }

    // Centraliza horizontalmente o urso em relação ao retângulo físico (width=20)
    // E alinha a base (pés) perfeitamente ao chão físico (y + height)
    float drawW = fabsf( src.width );
    float drawH = src.height;
    float drawX = ( j->ret.x + j->ret.width / 2.0f ) - drawW / 2.0f;
    float drawY = ( j->ret.y + j->ret.height ) - drawH;

    DrawTextureRec( rm.polarbear, src, (Vector2){ drawX, drawY }, WHITE );

}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo X.
 */
static void resolverColisaoJogadorObstaculosMapaX( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {
        Obstaculo *o = (Obstaculo*) el->objeto;

        if ( CheckCollisionRecs( j->ret, o->ret ) ) {
            if ( j->ret.x + j->ret.width / 2.0f < o->ret.x + o->ret.width / 2.0f ) {
                j->ret.x = o->ret.x - j->ret.width;
            } else {
                j->ret.x = o->ret.x + o->ret.width;
            }
            j->vel.x = 0;
        }

        el = el->proximo;
    }

}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo Y.
 */
static void resolverColisaoJogadorObstaculosMapaY( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {
        Obstaculo *o = (Obstaculo*) el->objeto;

        if ( CheckCollisionRecs( j->ret, o->ret ) ) {
            if ( j->ret.y + j->ret.height / 2.0f < o->ret.y + o->ret.height / 2.0f ) {
                j->ret.y = o->ret.y - j->ret.height;
                j->quantidadePulos = 0;
            } else {
                j->ret.y = o->ret.y + o->ret.height;
            }
            j->vel.y = 0;
        }

        el = el->proximo;
    }

}