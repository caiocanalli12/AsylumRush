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
static bool jogadorNoChao( Jogador *j, Mapa *mapa );

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
    novoJogador->puloY = 0.0f;
    novoJogador->puloVel = 0.0f;
    novoJogador->noPulo = false;
    novoJogador->noMezanino = false;

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
static bool jogadorNoChao( Jogador *j, Mapa *mapa ) {
    float cx = j->ret.x + j->ret.width / 2.0f;
    ElementoMapa *el = mapa->obstaculos;
    while ( el != NULL ) {
        Obstaculo *o = (Obstaculo*) el->objeto;
        if ( o->ret.y == 220.0f ) {
            // Tolerancia de 12 pixels nas bordas para diminuir a hitbox de queda e permitir pulo perto das bordas
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

static bool jogadorNoChaoCustom( Jogador *j, Mapa *mapa ) {
    float cx = j->ret.x + j->ret.width / 2.0f;
    if ( cx < 8300.0f ) {
        return jogadorNoChao( j, mapa );
    }
    if ( j->noMezanino ) {
        return true; // Always on ground inside mezzanine Y range [140, 180]
    } else {
        float feet_y = j->ret.y + j->ret.height;
        float curb = obterCurbRua( cx );
        if ( feet_y >= curb ) {
            return jogadorNoChao( j, mapa );
        }
    }
    return false; // Falling between mezzanine and street
}



void entradaJogador( Jogador *j, GameWorld *gw, float delta ) {

    bool direitaDown  = IsKeyDown( KEY_RIGHT ) || IsKeyDown( KEY_D );
    bool esquerdaDown = IsKeyDown( KEY_LEFT )  || IsKeyDown( KEY_A );
    bool cimaDown     = IsKeyDown( KEY_UP )    || IsKeyDown( KEY_W );
    bool baixoDown    = IsKeyDown( KEY_DOWN )  || IsKeyDown( KEY_S );
    bool espacoPressed = IsKeyPressed( KEY_SPACE );

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

    // Permite mover verticalmente na rua durante o pulo ou no chao
    if ( j->noPulo || jogadorNoChaoCustom( j, gw->mapa ) ) {
        if ( cimaDown ) {
            j->vel.y -= j->aceleracao * delta;
            if ( j->vel.y < -j->velAndando ) {
                j->vel.y = -j->velAndando;
            }
        } else if ( baixoDown ) {
            j->vel.y += j->aceleracao * delta;
            if ( j->vel.y > j->velAndando ) {
                j->vel.y = j->velAndando;
            }
        } else {
            if ( j->vel.y > 0 ) {
                j->vel.y -= j->desaceleracao * delta;
                if ( j->vel.y < 0 ) j->vel.y = 0;
            } else if ( j->vel.y < 0 ) {
                j->vel.y += j->desaceleracao * delta;
                if ( j->vel.y > 0 ) j->vel.y = 0;
            }
        }
    }

    // Pular com espaço (apenas se nao estiver no pulo e estiver no chao)
    if ( espacoPressed && !j->noPulo && jogadorNoChaoCustom( j, gw->mapa ) ) {
        j->noPulo = true;
        j->puloVel = -350.0f; // velocidade inicial para cima
        j->puloY = 0.0f;
    }



    if ( !j->noPulo && !jogadorNoChaoCustom( j, gw->mapa ) ) {
        j->estado = ESTADO_JOGADOR_PULANDO; // Queda no buraco/mezanino
    } else if ( j->noPulo ) {
        j->estado = ESTADO_JOGADOR_PULANDO; // Pulo ativo
    } else if ( fabsf( j->vel.x ) > 1.0f || fabsf( j->vel.y ) > 1.0f ) {
        j->estado = ESTADO_JOGADOR_ANDANDO;
    } else {
        j->estado = ESTADO_JOGADOR_PARADO;
    }

}

/**
 * @brief Aplica física e resolve colisões do jogador com o mundo.
 */
void atualizarJogador( Jogador *j, GameWorld *gw, float delta ) {

    // Eixo X: Move horizontalmente e resolve colisões do mapa
    j->ret.x += j->vel.x * delta;
    resolverColisaoJogadorObstaculosMapaX( j, gw->mapa );

    // Eixo Y: Move verticalmente ou cai sob gravidade
    float feet_y = j->ret.y + j->ret.height;
    float cx = j->ret.x + j->ret.width / 2.0f;
    if ( cx < 8300.0f ) {
        j->noMezanino = ( feet_y <= 180.0f );
    }

    // Desce automaticamente do mezanino no final da passarela (monte de neve)
    if ( j->noMezanino && cx >= 9300.0f ) {
        j->noMezanino = false;
    }

    if ( j->noPulo || jogadorNoChaoCustom( j, gw->mapa ) ) {
        j->ret.y += j->vel.y * delta;
        feet_y = j->ret.y + j->ret.height;
        
        if ( cx < 8300.0f ) {
            // Região unificada: clamp entre o topo da rua (180) e o fundo (283)
            if ( feet_y < 180.0f ) feet_y = 180.0f;
            if ( feet_y > 283.0f ) feet_y = 283.0f;
            j->ret.y = feet_y - j->ret.height;
        } else {
            // Região com declive e mezanino (x >= 8300.0f)
            if ( j->noMezanino ) {
                if ( feet_y < 140.0f ) feet_y = 140.0f;
                if ( feet_y > 180.0f ) feet_y = 180.0f;
                j->ret.y = feet_y - j->ret.height;
                

            } else {
                float curb = obterCurbRua( cx );
                if ( feet_y < curb ) {
                    if ( j->vel.y < 0.0f ) {
                        feet_y = curb;
                    }
                }
                if ( feet_y > 283.0f ) feet_y = 283.0f;
                j->ret.y = feet_y - j->ret.height;
            }
        }
    } else {
        // Zera o estado do pulo normal e cai no buraco
        j->noPulo = false;
        j->puloY = 0.0f;
        
        j->vel.y += gw->gravidade * delta;
        if ( j->vel.y > j->velMaxQueda ) {
            j->vel.y = j->velMaxQueda;
        }
        j->ret.y += j->vel.y * delta;
        
        // Pousar na rua/curb apenas na região do mezanino (x >= 8300)
        // Na região plana (x < 8300) o jogador cai livremente no buraco até o respawn
        if ( cx >= 8300.0f ) {
            feet_y = j->ret.y + j->ret.height;
            float curb = obterCurbRua( cx );
            if ( feet_y >= curb ) {
                feet_y = curb;
                j->ret.y = feet_y - j->ret.height;
                j->vel.y = 0.0f;
            }
        }
    }

    // Processamento do pulo com gravidade interna
    if ( j->noPulo ) {
        j->puloVel += gw->gravidade * delta;
        j->puloY += j->puloVel * delta;
        if ( j->puloY >= 0.0f ) {
            j->puloY = 0.0f;
            j->puloVel = 0.0f;
            j->noPulo = false;
        }
    }

    // Se o jogador cair do mapa, perde uma vida e respawna
    float limiteQueda = calcularAlturaMapa( gw->mapa );
    if ( j->ret.y > limiteQueda ) {
        j->quantidadeVidas--;
        
        // Respawnar jogador na posição inicial
        j->ret.x = 150.0f;
        j->ret.y = 220.0f - j->ret.height;
        j->vel = (Vector2){ 0, 0 };
        j->quantidadePulos = 0;
        j->noPulo = false;
        j->puloY = 0.0f;
        j->estado = ESTADO_JOGADOR_PARADO;
    }

    // Atualiza a animação do urso polar
    if ( j->estado == ESTADO_JOGADOR_PARADO ) {
        j->animTimer = 0.0f;
        j->animFrame = 0;
    } else if ( j->estado == ESTADO_JOGADOR_ANDANDO ) {
        // Velocidade da animação proporcional à velocidade do jogador (fps base = 12.0)
        float currentSpeed = sqrtf( j->vel.x * j->vel.x + j->vel.y * j->vel.y );
        float animSpeed = ( currentSpeed / j->velAndando ) * 12.0f;
        if ( animSpeed < 4.0f ) animSpeed = 4.0f; // velocidade mínima para movimento sutil
        j->animTimer += delta * animSpeed;
        j->animFrame = (int) j->animTimer;
    } else if ( j->estado == ESTADO_JOGADOR_PULANDO ) {
        float vertical_vel = j->noPulo ? j->puloVel : j->vel.y;
        if ( vertical_vel < 0 ) {
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

    // Escalonamento suave por perspectiva de profundidade baseado na posição Y dos pés
    float feet_y = j->ret.y + j->ret.height;
    float t = ( feet_y - 220.0f ) / 103.0f; // varia de ~ -0.39 a ~ 0.61
    float scale = 1.0f + t * 0.25f; // escala varia suavemente de ~ 0.90 ate ~ 1.15

    float drawW = fabsf( src.width ) * scale;
    float drawH = src.height * scale;
    float drawX = ( j->ret.x + j->ret.width / 2.0f ) - drawW / 2.0f;
    // Aplicamos o offset vertical do pulo escalonado de acordo com a profundidade
    float drawY = ( feet_y + j->puloY * scale ) - drawH;

    Rectangle dest = { drawX, drawY, drawW, drawH };
    Vector2 origin = { 0, 0 };

    DrawTexturePro( rm.polarbear, src, dest, origin, 0.0f, WHITE );

}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo X.
 */
static void resolverColisaoJogadorObstaculosMapaX( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {
        Obstaculo *o = (Obstaculo*) el->objeto;

        // Ignora colisões horizontais com os blocos de chão em 2.5D
        if ( o->ret.y == 220.0f ) {
            el = el->proximo;
            continue;
        }

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
