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

    // Campos de combate
    ed->quantidadeVidas = 3;
    ed->tomandoGolpe = false;
    ed->hitFrame = 0;
    ed->hitTimer = 0.0f;
    ed->invencibilidade = 0.0f;

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
 *        Na Fase 2 não há movimentação em profundidade (eixo Y).
 */
void atualizarEarDog( EarDog *ed, GameWorld *gw, float delta ) {
    if ( ed == NULL ) return;

    // Reduz cooldown de invencibilidade
    if ( ed->invencibilidade > 0.0f ) {
        ed->invencibilidade -= delta;
        if ( ed->invencibilidade < 0.0f ) ed->invencibilidade = 0.0f;
    }

    // Animação de tomar golpe: 3 frames de ~0.12s cada
    if ( ed->tomandoGolpe ) {
        static const float hitDuracao[3] = { 0.12f, 0.12f, 0.14f };
        ed->hitTimer += delta;
        if ( ed->hitTimer >= hitDuracao[ed->hitFrame] ) {
            ed->hitTimer = 0.0f;
            ed->hitFrame++;
            if ( ed->hitFrame >= 3 ) {
                ed->tomandoGolpe = false;
                ed->hitFrame = 0;
                ed->estado = ESTADO_EARDOG_PARADO;
            }
        }
        // Enquanto toma golpe, não mover
        return;
    }

    // Update movement timer and invert direction every 4 seconds
    ed->movementTimer += delta;
    if ( ed->movementTimer >= 4.0f ) {
        ed->vel.x = -ed->vel.x;
        ed->olhandoParaDireita = !(ed->vel.x > 0);
        ed->movementTimer = 0.0f;
    }

    // Apply horizontal movement only (sem movimento em profundidade na fase 2)
    ed->ret.x += ed->vel.x * delta;

    // Resolve horizontal collisions with map obstacles
    resolverColisaoEarDogObstaculosMapaX( ed, gw->mapa );

    // Simple walk animation cycle
    ed->animTimer += delta;
    if ( ed->animTimer >= 0.2f ) {
        ed->animTimer = 0.0f;
        ed->animFrame = ( ed->animFrame + 1 ) % 6; // 6 walk frames
    }

    ed->estado = ESTADO_EARDOG_PARADO;
}

/**
 * @brief Aplica dano ao EarDog (chamado pelo GameWorld quando a hitbox de ataque colide).
 */
void earDogReceberDano( EarDog *ed ) {
    if ( ed == NULL ) return;
    if ( ed->invencibilidade > 0.0f ) return; // ainda invencível

    ed->quantidadeVidas--;
    if ( ed->quantidadeVidas < 0 ) ed->quantidadeVidas = 0;

    // Inicia animação de hit
    ed->tomandoGolpe = true;
    ed->hitFrame = 0;
    ed->hitTimer = 0.0f;
    ed->estado = ESTADO_EARDOG_TOMANDO_GOLPE;

    // Cooldown de invencibilidade: 1.5 segundos
    ed->invencibilidade = 1.5f;
}

/**
 * @brief Retorna o retângulo de hitbox do corpo do EarDog (para colisões de corpo a corpo).
 *        A hitbox é proporcional ao sprite desenhado na escala 2.0x.
 */
Rectangle earDogObterHitbox( EarDog *ed ) {
    // Frames de caminhada — usa o frame atual ou frame 0 para calcular o tamanho representativo
    static const Rectangle walk_frames[6] = {
        { 16, 31, 44, 27 },
        { 72, 33, 42, 25 },
        { 127, 34, 45, 24 },
        { 186, 32, 44, 26 },
        { 241, 33, 44, 25 },
        { 300, 33, 45, 25 }
    };
    // Frames de hit para dimensão (usa frame 0 do hit como referência)
    static const Rectangle hit_frames[3] = {
        { 406, 476, 47, 28 },   // "Gets a hit" frame 1
        { 464, 472, 52, 32 },   // "Gets a hit" frame 2
        { 530, 476, 46, 28 }    // "Gets a hit" frame 3
    };

    float scale = 2.0f;

    Rectangle src;
    if ( ed->tomandoGolpe ) {
        src = hit_frames[ed->hitFrame < 3 ? ed->hitFrame : 2];
    } else {
        src = walk_frames[ed->animFrame < 6 ? ed->animFrame : 0];
    }

    float spriteW = fabsf( src.width ) * scale;
    float spriteH = src.height * scale;

    float feet_y = ed->ret.y + ed->ret.height;
    float drawX = ( ed->ret.x + ed->ret.width / 2.0f ) - spriteW / 2.0f;
    float drawY = feet_y - spriteH;

    return (Rectangle){ drawX, drawY, spriteW, spriteH };
}

/**
 * @brief Desenha o EarDog na tela.
 */
void desenharEarDog( EarDog *ed ) {
    if ( ed == NULL ) return;

    // Retângulos contendo as coordenadas exatas dos sprites na linha 1 de EarDog.png (Walking)
    static const Rectangle walk_frames[6] = {
        { 16, 31, 44, 27 },
        { 72, 33, 42, 25 },
        { 127, 34, 45, 24 },
        { 186, 32, 44, 26 },
        { 241, 33, 44, 25 },
        { 300, 33, 45, 25 }
    };

    // Frames de "Gets a hit" do EarDog.png
    // Seção "Gets a hit:" localizada na área inferior-direita da spritesheet
    static const Rectangle hit_frames[3] = {
        { 406, 476, 47, 28 },   // hit frame 1 — recuo
        { 464, 472, 52, 32 },   // hit frame 2 — tremida
        { 530, 476, 46, 28 }    // hit frame 3 — volta
    };

    Texture2D tex = rm.earDog;
    Rectangle src;

    if ( ed->tomandoGolpe ) {
        int fi = ed->hitFrame;
        if ( fi < 0 ) fi = 0;
        if ( fi > 2 ) fi = 2;
        src = hit_frames[fi];
    } else {
        src = walk_frames[ed->animFrame];
    }

    // Inverte a imagem horizontalmente se o personagem estiver olhando para a esquerda
    if ( !ed->olhandoParaDireita ) {
        src.width = -src.width;
    }

    // Escalonamento visual (escala 2.0x)
    float scale = 2.0f;
    float drawW = fabsf( src.width ) * scale;
    float drawH = src.height * scale;

    // Alinhamento: centro horizontal no retângulo físico, base pelos pés
    float feet_y = ed->ret.y + ed->ret.height;
    float drawX = ( ed->ret.x + ed->ret.width / 2.0f ) - drawW / 2.0f;
    float drawY = feet_y - drawH;

    Rectangle dest = { drawX, drawY, drawW, drawH };
    Vector2 origin = { 0, 0 };

    // Pisca vermelho quando invencível (a cada 0.15s)
    Color tint = WHITE;
    if ( ed->invencibilidade > 0.0f ) {
        // Alterna entre branco e vermelho claro para indicar dano
        int phase = (int)( ed->invencibilidade / 0.15f ) % 2;
        if ( phase == 0 ) tint = (Color){ 255, 120, 120, 200 };
    }

    DrawTexturePro( tex, src, dest, origin, 0.0f, tint );
}

static void resolverColisaoEarDogObstaculosMapaX( EarDog *ed, Mapa *mapa ) {
    ElementoMapa *el = mapa->obstaculos;
    while ( el != NULL ) {
        Obstaculo *o = (Obstaculo*) el->objeto;

        // Ignora colisões com blocos de chão
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
            ed->movementTimer = 0.0f;
        }

        el = el->proximo;
    }
}
