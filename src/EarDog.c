/**
 * @file EarDog.c
 * @brief Boss EarDog – Boss Fight (Fase 2).
 *
 * Sprite sheet mapeado: resources/imagens/EarDog.png (752x916)
 *
 * REGRAS DE PERSPECTIVA: O boss é sempre de PERFIL (lateral).
 * Nunca usa frames frontais/de câmera.
 *
 * Strips utilizados:
 *   Walk  (Y=29-57,  6fr) Strip 1  — caminhada lateral de perfil
 *   Bote  (Y=343-382,4fr) Strip 5 esq — corrida/bote LATERAL (Running, frames 0-3)
 *   Bark  (Y=651-701,3fr) Strip 13 dir — latido lateral
 *   Hit   (Y=428-468,4fr) Strip 7  dir — tomando golpe
 *   Dead  (Y=799-868,3fr) Strip 17     — caindo/morto
 *
 * STRIPS PROIBIDOS (frontal / voltado à câmera):
 *   Y=228-337 (Attacking frontal grande)
 *   Y=494-545 (Flies through the air — frontal)
 *   Y=343-382 frames 4-9 (poses frontais do ataque)
 */
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "raylib/raylib.h"

#include "EarDog.h"
#include "Wolf.h"
#include "Mapa.h"
#include "ResourceManager.h"
#include "Tipos.h"

/* ──────────────────────────────────────────────
   Sprite rectangles — SOMENTE perfil lateral
   ────────────────────────────────────────────── */

/* Strip 1: Caminhada lateral (Y=29-57, h=29) — 6 frames */
static const Rectangle WALK_FRAMES[6] = {
    { 16, 29, 44, 29 },
    { 72, 29, 42, 29 },
    { 127, 29, 45, 29 },
    { 186, 29, 44, 29 },
    { 241, 29, 44, 29 },
    { 300, 29, 45, 29 },
};

/* Strip 5 (lado esquerdo, Y=343-382): Running/Bote LATERAL — 4 frames
 * Esses são os únicos frames laterais de ataque. Os frames 4-9 da mesma
 * strip são frontais e portanto proibidos. */
static const Rectangle LUNGE_FRAMES[4] = {
    { 11, 343, 48, 40 },
    { 71, 343, 50, 40 },
    { 135, 343, 43, 40 },
    { 191, 343, 47, 40 },
};

/* Strip 5 (lado direito, Y=343-382): Dash de investida rápida (mesmo strip, loop) */
static const Rectangle DASH_FRAMES[4] = {
    { 11, 343, 48, 40 },
    { 71, 343, 50, 40 },
    { 135, 343, 43, 40 },
    { 191, 343, 47, 40 },
};

/* Strip 13 (dir, Y=651-701, h=51): Barking lateral — 3 frames */
static const Rectangle BARK_FRAMES[3] = {
    { 393, 651, 43, 51 },
    { 447, 651, 42, 51 },
    { 500, 651, 43, 51 },
};

/* Strip 7 (dir, Y=501, h=39): Gets a hit lateral — 3 frames */
static const Rectangle HIT_FRAMES[3] = {
    { 397, 501, 27, 39 },
    { 438, 502, 42, 38 },
    { 495, 501, 34, 39 },
};

/* Sitting, Sitting & Scratching, Sleeping/Exhausted — 4 frames */
static const Rectangle DEAD_FRAMES[4] = {
    { 396, 428, 39, 41 }, // Sitting
    { 478, 428, 38, 41 }, // Scratching 1
    { 528, 428, 38, 41 }, // Scratching 2
    { 626, 428, 42, 41 }, // Sleeping/Exhausted
};

/* Durações dos frames de hit */
static const float HIT_FRAME_DUR[4] = { 0.09f, 0.10f, 0.10f, 0.12f };

/* Escala de renderização */
#define EARDOG_SCALE 1.875f

/* Limites verticais da arena (Fase 2) */
#define ARENA_Y_MIN 200.0f
#define ARENA_Y_MAX 250.0f

/* ──────────────────────────────────────────────
   Forward declarations
   ────────────────────────────────────────────── */
static void resolverColisaoEarDogObstaculosMapaX( EarDog *ed, Mapa *mapa );
static void clamparArena( EarDog *ed, GameWorld *gw );
static void iniciarEstado( EarDog *ed, EstadoEarDog novoEstado, bool direcaoDireita );

/* ──────────────────────────────────────────────
   Criação / Destruição
   ────────────────────────────────────────────── */

EarDog *criarEarDog( float x, float y, float w, float h ) {
    EarDog *ed = (EarDog*) calloc( 1, sizeof( EarDog ) );
    ed->ret.x      = x;
    ed->ret.y      = y;
    ed->ret.width  = w;
    ed->ret.height = h;
    ed->vel.x = 0.0f;
    ed->vel.y = 0.0f;
    ed->olhandoParaDireita = false;
    ed->animTimer   = 0.0f;
    ed->animFrame   = 0;
    ed->puloY       = 0.0f;
    ed->puloVel     = 0.0f;
    ed->estado      = ESTADO_EARDOG_INATIVO;
    ed->stateTimer  = 0.0f;
    ed->decideTimer = 2.0f;

    /* Vidas */
    ed->quantidadeVidas = 7.0f;
    ed->tomandoGolpe    = false;
    ed->hitFrame        = 0;
    ed->hitTimer        = 0.0f;
    ed->invencibilidade = 0.0f;
    ed->hitFlashTimer   = 0.0f;

    /* Ataque */
    ed->attackCooldown = 0.0f;
    ed->hasHitPlayer   = false;

    /* Summon atrelado a HP: começa em 7, dispara a cada 2 HP perdidos */
    ed->lastSummonHP   = 7.0f;
    ed->summonPendente = false;

    return ed;
}

void destruirEarDog( EarDog *ed ) {
    if ( ed != NULL ) free( ed );
}

/* ──────────────────────────────────────────────
   Helpers internos
   ────────────────────────────────────────────── */

static void iniciarEstado( EarDog *ed, EstadoEarDog novoEstado, bool direcaoDireita ) {
    ed->estado             = novoEstado;
    ed->stateTimer         = 0.0f;
    ed->animTimer          = 0.0f;
    ed->animFrame          = 0;
    ed->olhandoParaDireita = direcaoDireita;
}

static void clamparArena( EarDog *ed, GameWorld *gw ) {
    float mapW = (float)gw->mapa->colunas;
    if ( ed->ret.x < 0 )                  ed->ret.x = 0;
    if ( ed->ret.x + ed->ret.width > mapW ) ed->ret.x = mapW - ed->ret.width;

    float feetY = ed->ret.y + ed->ret.height;
    if ( feetY < ARENA_Y_MIN ) ed->ret.y = ARENA_Y_MIN - ed->ret.height;
    if ( feetY > ARENA_Y_MAX ) ed->ret.y = ARENA_Y_MAX - ed->ret.height;
}

/* ──────────────────────────────────────────────
   Receber Dano — com trigger de summon a cada 2 HP
   ────────────────────────────────────────────── */

void earDogReceberDano( EarDog *ed ) {
    if ( ed == NULL ) return;
    if ( ed->invencibilidade > 0.0f ) return;
    if ( ed->estado == ESTADO_EARDOG_MORRENDO ) return;

    ed->quantidadeVidas--;
    if ( ed->quantidadeVidas < 0 ) ed->quantidadeVidas = 0;

    /* Verifica se perdeu 2 HP desde o último summon → invoca 1 minion */
    if ( ed->lastSummonHP - ed->quantidadeVidas >= 2 && ed->quantidadeVidas > 0 ) {
        ed->summonPendente = true;
        ed->lastSummonHP   = ed->quantidadeVidas;
    }

    ed->tomandoGolpe    = true;
    ed->hitFrame        = 0;
    ed->hitTimer        = 0.0f;
    ed->estado          = ESTADO_EARDOG_TOMANDO_GOLPE;
    ed->invencibilidade = 1.2f;
    ed->hitFlashTimer   = 0.4f;

    /* Para o movimento ao tomar hit */
    ed->vel.x = 0.0f;
    ed->vel.y = 0.0f;
}

void earDogReceberDanoEspecial( EarDog *ed, int dano ) {
    if ( ed == NULL ) return;
    if ( ed->invencibilidade > 0.0f ) return;
    if ( ed->estado == ESTADO_EARDOG_MORRENDO ) return;

    ed->quantidadeVidas -= dano;
    if ( ed->quantidadeVidas < 0 ) ed->quantidadeVidas = 0;

    /* Verifica se perdeu 2 HP desde o último summon → invoca 1 minion */
    if ( ed->lastSummonHP - ed->quantidadeVidas >= 2 && ed->quantidadeVidas > 0 ) {
        ed->summonPendente = true;
        ed->lastSummonHP   = ed->quantidadeVidas;
    }

    ed->tomandoGolpe    = true;
    ed->hitFrame        = 0;
    ed->hitTimer        = 0.0f;
    ed->estado          = ESTADO_EARDOG_TOMANDO_GOLPE;
    ed->invencibilidade = 1.2f;
    ed->hitFlashTimer   = 0.4f;

    /* Para o movimento ao tomar hit */
    ed->vel.x = 0.0f;
    ed->vel.y = 0.0f;
}

/* ──────────────────────────────────────────────
   Update — Máquina de Estados
   ────────────────────────────────────────────── */

void atualizarEarDog( EarDog *ed, GameWorld *gw, float delta ) {
    if ( ed == NULL || gw->jogador == NULL ) return;

    Jogador *j = gw->jogador;

    /* Timers globais */
    if ( ed->invencibilidade > 0.0f ) {
        ed->invencibilidade -= delta;
        if ( ed->invencibilidade < 0.0f ) ed->invencibilidade = 0.0f;
    }
    if ( ed->hitFlashTimer > 0.0f ) {
        ed->hitFlashTimer -= delta;
        if ( ed->hitFlashTimer < 0.0f ) ed->hitFlashTimer = 0.0f;
    }
    if ( ed->attackCooldown > 0.0f ) {
        ed->attackCooldown -= delta;
        if ( ed->attackCooldown < 0.0f ) ed->attackCooldown = 0.0f;
    }

    ed->stateTimer += delta;

    /* Posição do jogador */
    float cxJ    = j->ret.x + j->ret.width  / 2.0f;
    float cyJ    = j->ret.y + j->ret.height;
    float cxE    = ed->ret.x + ed->ret.width  / 2.0f;
    float cyE    = ed->ret.y + ed->ret.height;
    float distX  = cxJ - cxE;
    float distY  = cyJ - cyE;
    float distAbs = fabsf( distX );
    bool  alvoDireita = ( distX > 0.0f );

    /* ═══════════════ INTRO STATES ═══════════════ */
    if ( ed->estado == ESTADO_EARDOG_INATIVO ) {
        return; // Esperando o trigger em GameWorld
    }

    if ( ed->estado == ESTADO_EARDOG_INTRO_CORRENDO ) {
        ed->ret.x += ed->vel.x * delta;
        ed->ret.y += ed->vel.y * delta;
        
        ed->animTimer += delta;
        if ( ed->animTimer >= 0.13f ) { // Mesma velocidade de caminhada
            ed->animTimer = 0.0f;
            ed->animFrame = (ed->animFrame + 1) % 6;
        }

        // Corre até a posição 600.0f ou mais e começa a lutar
        if ( (ed->vel.x < 0 && ed->ret.x <= 600.0f) || (ed->vel.x > 0 && ed->ret.x >= 600.0f) ) {
            ed->ret.x = 600.0f;
            iniciarEstado( ed, ESTADO_EARDOG_ANDANDO, ed->olhandoParaDireita );
            ed->attackCooldown = 1.0f;
        }
        return;
    }

    /* ═══════════════ TOMANDO GOLPE ═══════════════ */
    if ( ed->tomandoGolpe ) {
        ed->hitTimer += delta;
        if ( ed->hitTimer >= HIT_FRAME_DUR[ed->hitFrame % 3] ) {
            ed->hitTimer = 0.0f;
            ed->hitFrame++;
            if ( ed->hitFrame >= 3 ) {
                ed->tomandoGolpe = false;
                ed->hitFrame     = 0;
                if ( ed->quantidadeVidas <= 0 ) {
                    iniciarEstado( ed, ESTADO_EARDOG_MORRENDO, ed->olhandoParaDireita );
                } else {
                    iniciarEstado( ed, ESTADO_EARDOG_ANDANDO, ed->olhandoParaDireita );
                    ed->attackCooldown = 1.5f;
                }
            }
        }
        return; /* Não move durante hit */
    }

    /* ═══════════════ MORRENDO ═══════════════ */
    if ( ed->estado == ESTADO_EARDOG_MORRENDO ) {
        ed->animTimer += delta;
        if ( ed->animTimer >= 0.40f ) {
            ed->animTimer = 0.0f;
            if ( ed->animFrame < 3 ) ed->animFrame++;
        }
        ed->vel.x *= 0.85f;
        ed->ret.x += ed->vel.x * delta;
        return;
    }

    /* ═══════════════ LATINDO (Barking / Summon) ═══════════════ */
    if ( ed->estado == ESTADO_EARDOG_LATINDO ) {
        ed->animTimer += delta;
        if ( ed->animTimer >= 0.22f ) {
            ed->animTimer = 0.0f;
            ed->animFrame = (ed->animFrame + 1) % 3;
        }
        /* Após 1.6s de latido, sinaliza o spawn e volta a andar */
        if ( ed->stateTimer >= 1.6f ) {
            ed->attackCooldown = 3.0f;
            iniciarEstado( ed, ESTADO_EARDOG_ANDANDO, ed->olhandoParaDireita );
        }
        return;
    }

    /* ═══════════════ ATACANDO (Bote Lateral) ═══════════════ */
    if ( ed->estado == ESTADO_EARDOG_ATACANDO ) {
        /* Move em direção ao jogador durante o bote */
        ed->ret.x += ed->vel.x * delta;
        ed->ret.y += ed->vel.y * delta;

        /* Anima os 4 frames de Running lateral em loop rápido */
        ed->animTimer += delta;
        if ( ed->animTimer >= 0.08f ) {
            ed->animTimer = 0.0f;
            ed->animFrame = (ed->animFrame + 1) % 4;
        }

        /* Bote dura 0.5s */
        if ( ed->stateTimer >= 0.5f ) {
            ed->attackCooldown = 2.0f;
            iniciarEstado( ed, ESTADO_EARDOG_ANDANDO, ed->olhandoParaDireita );
        }

        resolverColisaoEarDogObstaculosMapaX( ed, gw->mapa );
        clamparArena( ed, gw );
        return;
    }

    /* ═══════════════ INVESTINDO (Dash rápido) ═══════════════ */
    if ( ed->estado == ESTADO_EARDOG_INVESTINDO ) {
        ed->ret.x += ed->vel.x * delta;
        ed->ret.y += ed->vel.y * delta;

        ed->animTimer += delta;
        if ( ed->animTimer >= 0.06f ) {
            ed->animTimer = 0.0f;
            ed->animFrame = (ed->animFrame + 1) % 4;
        }

        /* Dash dura 0.50s */
        if ( ed->stateTimer >= 0.50f ) {
            ed->attackCooldown = 1.8f;
            iniciarEstado( ed, ESTADO_EARDOG_ANDANDO, ed->olhandoParaDireita );
        }

        resolverColisaoEarDogObstaculosMapaX( ed, gw->mapa );
        clamparArena( ed, gw );
        return;
    }

    /* ═══════════════ PARADO / ANDANDO — Decisão de IA ═══════════════ */
    ed->decideTimer -= delta;

    if ( ed->decideTimer <= 0.0f && ed->attackCooldown <= 0.0f ) {
        ed->decideTimer = 0.0f;

        /* Latido: disparado externamente pelo summonPendente (ver earDogReceberDano) */
        if ( ed->summonPendente && ed->estado != ESTADO_EARDOG_LATINDO ) {
            /* summonPendente é lido por GameWorld; EarDog latido para animar */
            iniciarEstado( ed, ESTADO_EARDOG_LATINDO, alvoDireita );
            /* NÃO limpa summonPendente aqui — GameWorld o consome após ler */
            ed->decideTimer = 3.0f;
            return;
        }

        /* Bote lateral: quando jogador está 160-300px de distância e alinhado no Y */
        if ( distAbs >= 160.0f && distAbs <= 300.0f && fabsf(distY) < 55.0f ) {
            iniciarEstado( ed, ESTADO_EARDOG_ATACANDO, alvoDireita );
            ed->vel.x = alvoDireita ? 320.0f : -320.0f;
            ed->vel.y = 0.0f;
            ed->hasHitPlayer = false;
            ed->decideTimer  = 3.0f;
            return;
        }

        /* Dash: quando muito próximo (< 140px) */
        if ( distAbs < 140.0f && fabsf(distY) < 55.0f ) {
            iniciarEstado( ed, ESTADO_EARDOG_INVESTINDO, alvoDireita );
            ed->vel.x = alvoDireita ? 400.0f : -400.0f;
            ed->vel.y = 0.0f;
            ed->hasHitPlayer = false;
            ed->decideTimer  = 2.5f;
            return;
        }

        ed->decideTimer = 0.7f;
    }

    /* ─── Walk / Perseguição 2.5D ─── */
    float speedX = 0.0f;
    float speedY = 0.0f;

    /* Mantém distância mínima de ~80px e persegue o jogador */
    if ( distAbs > 90.0f )     speedX = alvoDireita ?  130.0f : -130.0f;
    else if ( distAbs < 60.0f ) speedX = alvoDireita ? -80.0f  :  80.0f;

    /* Perseguição no eixo Y (profundidade) */
    if ( fabsf(distY) > 12.0f )
        speedY = ( distY > 0.0f ) ? 90.0f : -90.0f;

    ed->vel.x = speedX;
    ed->vel.y = speedY;

    if      ( speedX > 0.0f ) ed->olhandoParaDireita = true;
    else if ( speedX < 0.0f ) ed->olhandoParaDireita = false;

    ed->ret.x += ed->vel.x * delta;
    ed->ret.y += ed->vel.y * delta;

    resolverColisaoEarDogObstaculosMapaX( ed, gw->mapa );
    clamparArena( ed, gw );

    /* Animação de caminhada */
    bool movendo = ( fabsf(speedX) > 1.0f || fabsf(speedY) > 1.0f );
    if ( movendo ) {
        ed->estado = ESTADO_EARDOG_ANDANDO;
        ed->animTimer += delta;
        if ( ed->animTimer >= 0.13f ) {
            ed->animTimer = 0.0f;
            ed->animFrame = (ed->animFrame + 1) % 6;
        }
    } else {
        ed->estado = ESTADO_EARDOG_PARADO;
    }
}

/* ──────────────────────────────────────────────
   Hitboxes
   ────────────────────────────────────────────── */

Rectangle earDogObterHitbox( EarDog *ed ) {
    float scale  = EARDOG_SCALE;
    float spriteW = 44.0f * scale;
    float spriteH = 29.0f * scale;

    float feet_y = ed->ret.y + ed->ret.height;
    float drawX  = ( ed->ret.x + ed->ret.width / 2.0f ) - spriteW / 2.0f;
    float drawY  = feet_y - spriteH;

    return (Rectangle){ drawX, drawY, spriteW, spriteH };
}

Rectangle earDogObterHitboxAtaque( EarDog *ed ) {
    /* Dano ativo durante: Bote (Atacando) e Dash (Investindo) */
    bool atacando  = ( ed->estado == ESTADO_EARDOG_ATACANDO );
    bool investindo = ( ed->estado == ESTADO_EARDOG_INVESTINDO );

    if ( !atacando && !investindo ) return (Rectangle){0,0,0,0};
    if ( ed->hasHitPlayer )         return (Rectangle){0,0,0,0};

    Rectangle corpo = earDogObterHitbox( ed );
    float hw = corpo.width  * 0.4f;
    float hh = corpo.height * 0.5f;

    /* Hitbox isolada apenas na cabeça frontal */
    float offsetX = ed->olhandoParaDireita ? ( corpo.width - hw ) : 0.0f;

    return (Rectangle){
        corpo.x + offsetX,
        corpo.y + corpo.height * 0.1f, // Mais para cima/perto da cabeça
        hw, hh
    };
}

/* ──────────────────────────────────────────────
   Desenho — sempre de perfil, nunca frontal
   ────────────────────────────────────────────── */

void desenharEarDog( EarDog *ed ) {
    if ( ed == NULL ) return;

    Texture2D tex = rm.earDog;
    Rectangle src;

    /* Seleciona strip conforme estado */
    switch ( ed->estado ) {

        case ESTADO_EARDOG_INATIVO:
            return; // Nao desenha

        case ESTADO_EARDOG_MORRENDO: {
            int fi = ed->animFrame < 4 ? ed->animFrame : 3;
            src = DEAD_FRAMES[fi];
            break;
        }

        case ESTADO_EARDOG_TOMANDO_GOLPE: {
            int fi = ed->hitFrame < 3 ? ed->hitFrame : 2;
            src = HIT_FRAMES[fi];
            break;
        }

        case ESTADO_EARDOG_LATINDO: {
            src = BARK_FRAMES[ed->animFrame % 3];
            break;
        }

        case ESTADO_EARDOG_ATACANDO: {
            /* Bote lateral — frames de Running (somente lado esquerdo, de perfil) */
            src = LUNGE_FRAMES[ed->animFrame % 4];
            break;
        }

        case ESTADO_EARDOG_INVESTINDO: {
            /* Dash lateral — mesmos frames de Running de perfil */
            src = DASH_FRAMES[ed->animFrame % 4];
            break;
        }

        /* PARADO, ANDANDO — caminhada de perfil */
        default: {
            src = WALK_FRAMES[ed->animFrame % 6];
            break;
        }
    }

    /* O sprite original do EarDog olha para a ESQUERDA. 
       Portanto, devemos flipar se ele estiver olhando para a DIREITA. */
    bool flipH = ed->olhandoParaDireita;
    float srcW = flipH ? -fabsf(src.width) : fabsf(src.width);

    float scale = EARDOG_SCALE;
    float drawW = fabsf(src.width)  * scale;
    float drawH = src.height * scale;

    float feet_y = ed->ret.y + ed->ret.height;
    float drawX  = ( ed->ret.x + ed->ret.width / 2.0f ) - drawW / 2.0f;
    float drawY  = feet_y - drawH;

    Rectangle srcRect  = { src.x, src.y, srcW, src.height };
    Rectangle destRect = { drawX, drawY, drawW, drawH };
    Vector2   origin   = { 0.0f, 0.0f };

    /* Tint: flash branco pulsante ao tomar hit, pisca durante invencibilidade */
    Color tint = WHITE;

    if ( ed->hitFlashTimer > 0.0f ) {
        int flashPhase = (int)( ed->hitFlashTimer / 0.06f ) % 2;
        tint = ( flashPhase == 0 ) ?
            (Color){ 255, 255, 255, 255 } :
            (Color){ 255, 200, 200, 255 };
    } else if ( ed->invencibilidade > 0.0f ) {
        int invPhase = (int)( ed->invencibilidade / 0.10f ) % 2;
        tint = ( invPhase == 0 ) ?
            (Color){ 255, 255, 255, 255 } :
            (Color){ 255, 255, 255, 120 };
    }

    /* Outline escuro para destacar do fundo */
    if ( ed->quantidadeVidas > 0 ) {
        Color outline = { 20, 10, 30, 180 };
        float ox = 1.5f;
        DrawTexturePro( tex, srcRect, (Rectangle){ drawX-ox, drawY,    drawW, drawH }, origin, 0.0f, outline );
        DrawTexturePro( tex, srcRect, (Rectangle){ drawX+ox, drawY,    drawW, drawH }, origin, 0.0f, outline );
        DrawTexturePro( tex, srcRect, (Rectangle){ drawX,    drawY-ox, drawW, drawH }, origin, 0.0f, outline );
        DrawTexturePro( tex, srcRect, (Rectangle){ drawX,    drawY+ox, drawW, drawH }, origin, 0.0f, outline );
    }

    DrawTexturePro( tex, srcRect, destRect, origin, 0.0f, tint );
}

/* ──────────────────────────────────────────────
   Colisão com obstáculos (eixo X)
   ────────────────────────────────────────────── */

static void resolverColisaoEarDogObstaculosMapaX( EarDog *ed, Mapa *mapa ) {
    ElementoMapa *el = mapa->obstaculos;
    while ( el != NULL ) {
        Obstaculo *o = (Obstaculo*) el->objeto;

        /* Ignora blocos de chão */
        if ( o->ret.y >= 190.0f ) {
            el = el->proximo;
            continue;
        }

        if ( CheckCollisionRecs( ed->ret, o->ret ) ) {
            if ( ed->ret.x + ed->ret.width / 2.0f < o->ret.x + o->ret.width / 2.0f ) {
                ed->ret.x = o->ret.x - ed->ret.width;
            } else {
                ed->ret.x = o->ret.x + o->ret.width;
            }
            ed->vel.x *= -0.3f;
        }

        el = el->proximo;
    }
}
