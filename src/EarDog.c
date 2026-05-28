/**
 * @file EarDog.c
 * @author thale
 * @brief Implementação do EarDog (NPC da Fase 2).
 *
 * @copyright Copyright (c) 2026
 */
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "EarDog.h"
#include "Mapa.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void resolverColisaoEarDogObstaculosMapaX( EarDog *ed, Mapa *mapa );

/**
 * @brief Cria uma instância alocada dinamicamente da struct EarDog.
 */
EarDog *criarEarDog( float x, float y, float w, float h ) {
    EarDog *ed = (EarDog*) malloc( sizeof( EarDog ) );
    ed->ret.x = x;
    ed->ret.y = y;
    ed->ret.width = w;
    ed->ret.height = h;
    ed->vel.x = 120.0f; // Inicia patrulhando para a direita
    ed->vel.y = 0;
    ed->olhandoParaDireita = false;
    ed->animTimer = 0.0f;
    ed->animFrame = 0;
    ed->movementTimer = 0.0f;
    ed->puloY = 0.0f;
    ed->puloVel = 0.0f;
    ed->estado = ESTADO_EARDOG_PARADO;
    return ed;
}

/**
 * @brief Destrói um objeto EarDog e libera seus recursos.
 */
void destruirEarDog( EarDog *ed ) {
    if ( ed != NULL ) {
        free( ed );
    }
}

/**
 * @brief Atualiza a física, movimentação e animação do EarDog.
 */
void atualizarEarDog( EarDog *ed, GameWorld *gw, float delta ) {
    if ( ed == NULL ) return;

    // Update movement timer and invert direction every 4 seconds
    ed->movementTimer += delta;
    if ( ed->movementTimer >= 4.0f ) {
        ed->vel.x = -ed->vel.x;
        ed->olhandoParaDireita = !(ed->vel.x > 0);
        ed->movementTimer = 0.0f;
    }

    // Apply horizontal movement
    ed->ret.x += ed->vel.x * delta;

    // Resolve horizontal collisions with map obstacles
    resolverColisaoEarDogObstaculosMapaX( ed, gw->mapa );

    // Simple walk animation cycle (optional)
    ed->animTimer += delta;
    if ( ed->animTimer >= 0.2f ) {
        ed->animTimer = 0.0f;
        ed->animFrame = ( ed->animFrame + 1 ) % 6; // Assuming 6 walk frames
    }
}



/**
 * @brief Desenha o EarDog na tela.
 */
void desenharEarDog( EarDog *ed ) {
    if ( ed == NULL ) return;

    // Retângulos contendo as coordenadas exatas dos sprites na linha 1 de EarDog.png
    static const Rectangle walk_frames[6] = {
        { 16, 31, 44, 27 },
        { 72, 33, 42, 25 },
        { 127, 34, 45, 24 },
        { 186, 32, 44, 26 },
        { 241, 33, 44, 25 },
        { 300, 33, 45, 25 }
    };

    Texture2D tex = rm.earDog;
    Rectangle src = walk_frames[ed->animFrame];

    // Inverte a imagem horizontalmente se o personagem estiver olhando para a esquerda
    if ( !ed->olhandoParaDireita ) {
        src.width = -src.width;
    }

    // Escalonamento visual (escala 2.0x - tamanho anteriormente selecionado)
    float scale = 2.0f;
    float drawW = fabsf( src.width ) * scale;
    float drawH = src.height * scale;

    // Alinhamento horizontal no retângulo físico de colisão, e vertical pelos pés com offset de pulo
    float feet_y = ed->ret.y + ed->ret.height;
    float drawX = ( ed->ret.x + ed->ret.width / 2.0f ) - drawW / 2.0f;
    float drawY = ( feet_y + ed->puloY ) - drawH;

    Rectangle dest = { drawX, drawY, drawW, drawH };
    Vector2 origin = { 0, 0 };

    DrawTexturePro( tex, src, dest, origin, 0.0f, WHITE );
}

static void resolverColisaoEarDogObstaculosMapaX( EarDog *ed, Mapa *mapa ) {
    ElementoMapa *el = mapa->obstaculos;
    while ( el != NULL ) {
        Obstaculo *o = (Obstaculo*) el->objeto;

        // Ignora colisões com blocos de chão que pertencem ao background/plataformas do nível 2.5D
        if ( o->ret.y == 220.0f ) {
            el = el->proximo;
            continue;
        }

        if ( CheckCollisionRecs( ed->ret, o->ret ) ) {
            // Empurra para fora do obstáculo
            if ( ed->ret.x + ed->ret.width / 2.0f < o->ret.x + o->ret.width / 2.0f ) {
                ed->ret.x = o->ret.x - ed->ret.width;
            } else {
                ed->ret.x = o->ret.x + o->ret.width;
            }
            // Inverte a direção imediatamente ao colidir
            ed->vel.x = -ed->vel.x;
            ed->olhandoParaDireita = !(ed->vel.x > 0);
            ed->movementTimer = 0.0f; // Reseta o timer de mudança de direção ao rebater
        }

        el = el->proximo;
    }
}
