/**
 * @file Belial.c
 * @author thales
 * @brief Implementação do Jogador 2 (Belial - Elefante).
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Macros.h"
#include "Belial.h"
#include "Mapa.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void resolverColisaoBelialObstaculosMapaX( Belial *b, Mapa *mapa );
static bool belialNoChao( Belial *b, Mapa *mapa );
static bool belialNoChaoCustom( Belial *b, Mapa *mapa );
static float obterCurbRua( float x );

/**
 * @brief Cria uma instância alocada dinamicamente da struct Belial.
 */
Belial *criarBelial( float x, float y, float w, float h ) {

    Belial *novoBelial = (Belial*) malloc( sizeof( Belial ) );

    novoBelial->ret.x = x;
    novoBelial->ret.y = y;
    novoBelial->ret.width = w;
    novoBelial->ret.height = h;
    novoBelial->vel = (Vector2) { 0 };

    novoBelial->cor = ORANGE;

    novoBelial->velAndando = 300;
    novoBelial->velPulo = -500;
    novoBelial->velMaxQueda = 600;

    novoBelial->aceleracao = 1000;
    novoBelial->desaceleracao = 800;

    novoBelial->quantidadePulos = 0;
    novoBelial->quantidadeMaxPulos = 2;

    novoBelial->quantidadeVidas = 3;
    novoBelial->score = 0;

    novoBelial->estado = ESTADO_JOGADOR_PARADO;
    novoBelial->olhandoParaDireita = true;
    novoBelial->animTimer = 0.0f;
    novoBelial->animFrame = 0;
    novoBelial->puloY = 0.0f;
    novoBelial->puloVel = 0.0f;
    novoBelial->noPulo = false;
    novoBelial->noMezanino = false;

    novoBelial->socando = false;
    novoBelial->socandoFrame = 0;
    novoBelial->socandoTimer = 0.0f;
    novoBelial->socandoCooldown = 0.0f;

    novoBelial->socoAereo = false;
    novoBelial->socoAereoAterrissou = false;
    novoBelial->socoAereoCooldown = 0.0f;

    novoBelial->invencibilidade = 0.0f;

    // Sistema de knocked-out / respawn
    novoBelial->ativo = true;
    novoBelial->respawnTimer = 0.0f;

    return novoBelial;

}

/**
 * @brief Destrói um objeto Belial e libera seus recursos.
 */
void destruirBelial( Belial *b ) {
    if ( b != NULL ) {
        free( b );
    }
}

/**
 * @brief Verifica se o Belial está no chão base.
 */
static bool belialNoChao( Belial *b, Mapa *mapa ) {
    float cx = b->ret.x + b->ret.width / 2.0f;
    ElementoMapa *el = mapa->obstaculos;
    while ( el != NULL ) {
        Obstaculo *o = (Obstaculo*) el->objeto;
        if ( o->ret.y == 220.0f ) {
            float tolerancia = 12.0f;
            if ( cx >= o->ret.x - tolerancia && cx <= o->ret.x + o->ret.width + tolerancia ) {
                return true;
            }
        }
        el = el->proximo;
    }
    return false;
}

static float obterCurbRua( float x ) {
    if ( x < 8300.0f ) {
        return 180.0f;
    }
    if ( x > 8600.0f ) {
        return 252.0f;
    }
    return 180.0f + 0.24f * ( x - 8300.0f );
}

static bool belialNoChaoCustom( Belial *b, Mapa *mapa ) {
    float cx = b->ret.x + b->ret.width / 2.0f;
    if ( cx < 8300.0f ) {
        return belialNoChao( b, mapa );
    }
    if ( b->noMezanino ) {
        return true;
    } else {
        float feet_y = b->ret.y + b->ret.height;
        float curb = obterCurbRua( cx );
        if ( feet_y >= curb ) {
            return belialNoChao( b, mapa );
        }
    }
    return false;
}

void entradaBelial( Belial *b, GameWorld *gw, float delta ) {

    if ( !b->ativo ) return; // Belial fora de jogo, ignora entrada

    // Inputs exclusivos para o Belial: Setas direcionais do teclado
    bool direitaDown  = IsKeyDown( KEY_RIGHT );
    bool esquerdaDown = IsKeyDown( KEY_LEFT );
    bool cimaDown     = IsKeyDown( KEY_UP );
    bool baixoDown    = IsKeyDown( KEY_DOWN );
    bool puloPressed  = IsKeyPressed( KEY_L ); // Tecla L para pular

    bool emCooldownSoco = b->socando && b->socandoFrame == 3;
    bool emCooldownAereo = b->socoAereoAterrissou;
    bool bloqueado = emCooldownSoco || emCooldownAereo;

    if ( !bloqueado ) {
        if ( direitaDown ) {
            b->vel.x += b->aceleracao * delta;
            if ( b->vel.x > b->velAndando ) {
                b->vel.x = b->velAndando;
            }
            b->olhandoParaDireita = true;
        } else if ( esquerdaDown ) {
            b->vel.x -= b->aceleracao * delta;
            if ( b->vel.x < -b->velAndando ) {
                b->vel.x = -b->velAndando;
            }
            b->olhandoParaDireita = false;
        } else {
            if ( b->vel.x > 0 ) {
                b->vel.x -= b->desaceleracao * delta;
                if ( b->vel.x < 0 ) b->vel.x = 0;
            } else if ( b->vel.x < 0 ) {
                b->vel.x += b->desaceleracao * delta;
                if ( b->vel.x > 0 ) b->vel.x = 0;
            }
        }

        // Permite mover verticalmente na rua apenas na fase 0
        if ( gw->faseAtual == 0 && ( b->noPulo || belialNoChaoCustom( b, gw->mapa ) ) ) {
            if ( cimaDown ) {
                b->vel.y -= b->aceleracao * delta;
                if ( b->vel.y < -b->velAndando ) {
                    b->vel.y = -b->velAndando;
                }
            } else if ( baixoDown ) {
                b->vel.y += b->aceleracao * delta;
                if ( b->vel.y > b->velAndando ) {
                    b->vel.y = b->velAndando;
                }
            } else {
                if ( b->vel.y > 0 ) {
                    b->vel.y -= b->desaceleracao * delta;
                    if ( b->vel.y < 0 ) b->vel.y = 0;
                } else if ( b->vel.y < 0 ) {
                    b->vel.y += b->desaceleracao * delta;
                    if ( b->vel.y > 0 ) b->vel.y = 0;
                }
            }
        } else if ( gw->faseAtual != 0 ) {
            if ( b->vel.y > 0 ) {
                b->vel.y -= b->desaceleracao * delta;
                if ( b->vel.y < 0 ) b->vel.y = 0;
            } else if ( b->vel.y < 0 ) {
                b->vel.y += b->desaceleracao * delta;
                if ( b->vel.y > 0 ) b->vel.y = 0;
            }
        }

        // Pular com L
        if ( puloPressed && !b->socando && !b->socoAereo && !b->socoAereoAterrissou ) {
            if ( !b->noPulo && belialNoChaoCustom( b, gw->mapa ) ) {
                b->noPulo = true;
                b->puloVel = -350.0f;
                b->puloY = 0.0f;
                b->quantidadePulos = 1;
            } else if ( b->noPulo && b->quantidadePulos < b->quantidadeMaxPulos ) {
                b->puloVel = -350.0f;
                b->quantidadePulos++;
            }
        }
    } else {
        if ( b->vel.x > 0 ) {
            b->vel.x -= b->desaceleracao * delta;
            if ( b->vel.x < 0 ) b->vel.x = 0;
        } else if ( b->vel.x < 0 ) {
            b->vel.x += b->desaceleracao * delta;
            if ( b->vel.x > 0 ) b->vel.x = 0;
        }
        if ( b->vel.y > 0 ) {
            b->vel.y -= b->desaceleracao * delta;
            if ( b->vel.y < 0 ) b->vel.y = 0;
        } else if ( b->vel.y < 0 ) {
            b->vel.y += b->desaceleracao * delta;
            if ( b->vel.y > 0 ) b->vel.y = 0;
        }
    }

    // Ataque: Tecla Ç (KEY_SEMICOLON)
    if ( IsKeyPressed( KEY_SEMICOLON ) && !b->socando && !b->socoAereo && !b->socoAereoAterrissou ) {
        if ( belialNoChaoCustom( b, gw->mapa ) && !b->noPulo ) {
            b->socando = true;
            b->socandoFrame = 0;
            b->socandoTimer = 0.0f;
            b->socandoCooldown = 0.0f;
        } else if ( b->noPulo ) {
            b->socoAereo = true;
            b->socoAereoAterrissou = false;
            b->socoAereoCooldown = 0.0f;
        }
    }

    // Definição do estado
    if ( !b->noPulo && !belialNoChaoCustom( b, gw->mapa ) ) {
        b->estado = ESTADO_JOGADOR_PULANDO;
    } else if ( b->noPulo ) {
        b->estado = ESTADO_JOGADOR_PULANDO;
    } else if ( b->socoAereoAterrissou ) {
        b->estado = ESTADO_JOGADOR_SOCANDO;
    } else if ( b->socando ) {
        b->estado = ESTADO_JOGADOR_SOCANDO;
    } else if ( fabsf( b->vel.x ) > 1.0f || fabsf( b->vel.y ) > 1.0f ) {
        b->estado = ESTADO_JOGADOR_ANDANDO;
    } else {
        b->estado = ESTADO_JOGADOR_PARADO;
    }

}

void atualizarBelial( Belial *b, GameWorld *gw, float delta ) {

    if ( !b->ativo ) return; // Belial fora de jogo, ignora fisica

    // Eixo X
    b->ret.x += b->vel.x * delta;
    resolverColisaoBelialObstaculosMapaX( b, gw->mapa );

    // Clamp horizontal
    float max_x = (float)calcularLarguraMapa( gw->mapa );
    if ( b->ret.x < 0.0f ) {
        b->ret.x = 0.0f;
        b->vel.x = 0.0f;
    } else if ( b->ret.x + b->ret.width > max_x ) {
        b->ret.x = max_x - b->ret.width;
        b->vel.x = 0.0f;
    }

    // Eixo Y
    float feet_y = b->ret.y + b->ret.height;
    float cx = b->ret.x + b->ret.width / 2.0f;

    if ( gw->faseAtual == 0 ) {
        if ( cx < 8300.0f ) {
            b->noMezanino = ( feet_y <= 180.0f );
        }
        if ( b->noMezanino && cx >= 9300.0f ) {
            b->noMezanino = false;
        }
    } else {
        b->noMezanino = false;
    }

    if ( b->noPulo || belialNoChaoCustom( b, gw->mapa ) ) {
        b->ret.y += b->vel.y * delta;
        feet_y = b->ret.y + b->ret.height;

        if ( gw->faseAtual == 0 ) {
            if ( cx < 8300.0f ) {
                if ( feet_y < 180.0f ) feet_y = 180.0f;
                if ( feet_y > 283.0f ) feet_y = 283.0f;
                b->ret.y = feet_y - b->ret.height;
            } else {
                if ( b->noMezanino ) {
                    if ( feet_y < 140.0f ) feet_y = 140.0f;
                    if ( feet_y > 180.0f ) feet_y = 180.0f;
                    b->ret.y = feet_y - b->ret.height;
                } else {
                    float curb = obterCurbRua( cx );
                    if ( feet_y < curb ) {
                        if ( b->vel.y < 0.0f ) {
                            feet_y = curb;
                        }
                    }
                    if ( feet_y > 283.0f ) feet_y = 283.0f;
                    b->ret.y = feet_y - b->ret.height;
                }
            }
        } else {
            float max_y = (float)rm.ifsp_highschool.height;
            if ( feet_y < 160.0f ) feet_y = 160.0f;
            if ( feet_y > max_y ) feet_y = max_y;
            b->ret.y = feet_y - b->ret.height;
        }
    } else {
        b->noPulo = false;
        b->puloY = 0.0f;
        b->quantidadePulos = 0;

        b->vel.y += gw->gravidade * delta;
        if ( b->vel.y > b->velMaxQueda ) {
            b->vel.y = b->velMaxQueda;
        }
        b->ret.y += b->vel.y * delta;

        if ( gw->faseAtual == 0 && cx >= 8300.0f ) {
            feet_y = b->ret.y + b->ret.height;
            float curb = obterCurbRua( cx );
            if ( feet_y >= curb ) {
                feet_y = curb;
                b->ret.y = feet_y - b->ret.height;
                b->vel.y = 0.0f;
            }
        }
    }

    // Pulo
    if ( b->noPulo ) {
        b->puloVel += gw->gravidade * delta;
        b->puloY += b->puloVel * delta;
        if ( b->puloY >= 0.0f ) {
            b->puloY = 0.0f;
            b->puloVel = 0.0f;
            b->noPulo = false;
            b->quantidadePulos = 0;
        }
    }

    // Queda do mapa: inicia knocked-out (respawn em 15 s)
    float limiteQueda = calcularAlturaMapa( gw->mapa );
    if ( b->ret.y > limiteQueda ) {
        b->quantidadeVidas--;
        b->ativo = false;
        if ( b->quantidadeVidas <= 0 ) {
            b->quantidadeVidas = 0;
            b->respawnTimer = 0.0f;
        } else {
            b->respawnTimer = 15.0f;
        }
        b->ret.x = -999.0f;
        b->ret.y = -999.0f;
        b->vel = (Vector2){ 0, 0 };
        b->quantidadePulos = 0;
        b->noPulo = false;
        b->puloY = 0.0f;
        b->estado = ESTADO_JOGADOR_PARADO;
    }

    // Atualização de Animação
    if ( b->socando ) {
        static const float socoDuracao[4] = { 0.08f, 0.08f, 0.12f, 0.5f };

        if ( b->socandoFrame < 3 ) {
            b->socandoTimer += delta;
            if ( b->socandoTimer >= socoDuracao[b->socandoFrame] ) {
                b->socandoTimer = 0.0f;
                b->socandoFrame++;
            }
        } else {
            b->socandoCooldown += delta;
            if ( b->socandoCooldown >= socoDuracao[3] ) {
                b->socando = false;
                b->socandoFrame = 0;
                b->socandoTimer = 0.0f;
                b->socandoCooldown = 0.0f;
            }
        }
    } else if ( b->estado == ESTADO_JOGADOR_PARADO ) {
        b->animTimer += delta;
        b->animFrame = 0;
    } else if ( b->estado == ESTADO_JOGADOR_ANDANDO ) {
        float currentSpeed = sqrtf( b->vel.x * b->vel.x + b->vel.y * b->vel.y );
        float animSpeed = ( currentSpeed / b->velAndando ) * 12.0f;
        if ( animSpeed < 4.0f ) animSpeed = 4.0f;
        b->animTimer += delta * animSpeed;
        b->animFrame = (int) b->animTimer;
    } else if ( b->estado == ESTADO_JOGADOR_PULANDO ) {
        b->animTimer += delta;
        b->animFrame = 0;
    }

    // Ataque aéreo
    if ( b->socoAereo && !b->socoAereoAterrissou ) {
        if ( !b->noPulo && belialNoChaoCustom( b, gw->mapa ) ) {
            b->socoAereoAterrissou = true;
            b->socoAereoCooldown = 0.0f;
        }
    }
    if ( b->socoAereoAterrissou ) {
        b->socoAereoCooldown += delta;
        if ( b->socoAereoCooldown >= 0.5f ) {
            b->socoAereo = false;
            b->socoAereoAterrissou = false;
            b->socoAereoCooldown = 0.0f;
        }
    }

    // Invencibilidade
    if ( b->invencibilidade > 0.0f ) {
        b->invencibilidade -= delta;
        if ( b->invencibilidade < 0.0f ) b->invencibilidade = 0.0f;
    }

}

void desenharBelial( Belial *b ) {

    // Idle frames (Row 1): 5 frames
    static const Rectangle idle_frames[5] = {
        { 1, 1, 46, 43 },
        { 51, 1, 46, 43 },
        { 100, 1, 46, 43 },
        { 149, 1, 48, 43 },
        { 199, 1, 49, 43 }
    };

    // Walk frames (Row 2): 6 frames
    static const Rectangle walk_frames[6] = {
        { 1, 47, 49, 43 },
        { 52, 47, 48, 43 },
        { 103, 49, 46, 43 },
        { 153, 48, 46, 43 },
        { 202, 48, 46, 43 },
        { 250, 49, 46, 43 }
    };

    // Jump frames (bater trombas como asas - Row 3): 6 frames
    static const Rectangle jump_frames[6] = {
        { 170, 93, 47, 43 }, // narrowest (tromba recolhida)
        { 115, 94, 49, 46 }, 
        { 59, 93, 51, 46 },  
        { 344, 101, 51, 44 }, 
        { 282, 97, 56, 48 }, 
        { 221, 95, 56, 48 }  // widest (tromba aberta)
    };

    // Attack / soco frames (Row 4): 4 frames
    static const Rectangle punch_frames[4] = {
        { 134, 144, 43, 43 },  // Frame 0: preparação
        { 184, 144, 37, 43 },  // Frame 1: recuo
        { 2, 143, 59, 42 },    // Frame 2: ataque ativo (tromba estendida)
        { 65, 143, 59, 44 }    // Frame 3: cooldown (recuperação)
    };

    // Aerial attack frames (Row 5): 2 frames
    static const Rectangle fly_frames[2] = {
        { 61, 195, 56, 52 },   // ascendente/ativo
        { 2, 194, 53, 49 }     // descendente/cooldown
    };

    Texture2D tex = rm.belial;
    Rectangle src;

    if ( b->socoAereo || b->socoAereoAterrissou ) {
        src = b->socoAereoAterrissou ? fly_frames[1] : fly_frames[0];
        // Os frames de ataque aéreo na spritesheet estão desenhados virados para a esquerda,
        // ao contrário do restante da spritesheet que olha para a direita. Invertemos a base:
        src.width = -src.width;
    } else if ( b->estado == ESTADO_JOGADOR_SOCANDO ) {
        src = punch_frames[b->socandoFrame];
    } else if ( b->estado == ESTADO_JOGADOR_PARADO ) {
        src = idle_frames[(int)(b->animTimer * 2.0f) % 5];
    } else if ( b->estado == ESTADO_JOGADOR_ANDANDO ) {
        src = walk_frames[b->animFrame % 6];
    } else { // ESTADO_JOGADOR_PULANDO
        // Usa as animações de "bater asa"
        src = jump_frames[(int)(b->animTimer * 10.0f) % 6];
    }

    // A spritesheet do Belial originalmente está desenhada voltada para a DIREITA (igual o urso)
    // Então, se ele estiver olhando para a esquerda, espelhamos a imagem invertendo a largura:
    if ( !b->olhandoParaDireita ) {
        src.width = -src.width;
    }

    float feet_y = b->ret.y + b->ret.height;
    float t = ( feet_y - 220.0f ) / 103.0f;
    float scale = 1.0f + t * 0.25f;

    float drawW = fabsf( src.width ) * scale;
    float drawH = src.height * scale;

    float drawX = ( b->ret.x + b->ret.width / 2.0f ) - drawW / 2.0f;
    float drawY = ( feet_y + b->puloY * scale ) - drawH;

    Rectangle dest = { drawX, drawY, drawW, drawH };
    Vector2 origin = { 0, 0 };

    Color tint = WHITE;
    if ( b->invencibilidade > 0.0f ) {
        int phase = (int)( b->invencibilidade / 0.15f ) % 2;
        if ( phase == 0 ) tint = (Color){ 255, 120, 120, 200 };
    }

    DrawTexturePro( tex, src, dest, origin, 0.0f, tint );

}

static void resolverColisaoBelialObstaculosMapaX( Belial *b, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {
        Obstaculo *o = (Obstaculo*) el->objeto;

        if ( o->ret.y == 220.0f ) {
            el = el->proximo;
            continue;
        }

        if ( CheckCollisionRecs( b->ret, o->ret ) ) {
            if ( b->ret.x + b->ret.width / 2.0f < o->ret.x + o->ret.width / 2.0f ) {
                b->ret.x = o->ret.x - b->ret.width;
            } else {
                b->ret.x = o->ret.x + o->ret.width;
            }
            b->vel.x = 0;
        }

        el = el->proximo;
    }

}
