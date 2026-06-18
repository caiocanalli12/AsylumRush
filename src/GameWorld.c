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
#include <string.h>

#include "GameWorld.h"
#include "Jogador.h"
#include "Wolf.h"
#include "IceShard.h"
#include "EarDog.h"
#include "Belial.h"
#include "Macros.h"
#include "Mapa.h"
#include "Obstaculo.h"
#include "Tipos.h"
#include "ResourceManager.h"
#include "raylib/raylib.h"

static void desenharFundo( GameWorld *gw );
static void atualizarCamera( GameWorld *gw );
static void inicializar( GameWorld *gw );
void ResetBossScene( GameWorld *gw );
static void reiniciar( GameWorld *gw );
static void trocarFase( GameWorld *gw, int novaFase );
static void resolverColisoesFase2( GameWorld *gw );
static Rectangle obterHitboxSocoPolarBear( Jogador *j );
static Rectangle obterHitboxVisualJogador( Jogador *j );

// --- Helpers de UI ---

// Verifica se o mouse está sobre um retângulo (em coordenadas de tela)
static bool mouseSobreRect( Rectangle ret ) {
    Vector2 mouse = GetMousePosition();
    return CheckCollisionPointRec( mouse, ret );
}

// Desenha um botão com hover. Retorna true se foi clicado.
static bool desenharBotao( Rectangle ret, const char *texto, int fontSize, Color corNormal, Color corHover, Color corTexto ) {
    bool hover = mouseSobreRect( ret );
    Color corAtual = hover ? corHover : corNormal;

    // Fundo do botão com cantos arredondados
    DrawRectangleRounded( ret, 0.3f, 8, corAtual );
    DrawRectangleRoundedLines( ret, 0.3f, 8, (Color){ 255, 255, 255, 80 } );

    // Texto centralizado
    int textW = MeasureText( texto, fontSize );
    int textX = (int)( ret.x + ( ret.width - textW ) / 2.0f );
    int textY = (int)( ret.y + ( ret.height - fontSize ) / 2.0f );
    DrawText( texto, textX, textY, fontSize, corTexto );

    return hover && IsMouseButtonPressed( MOUSE_BUTTON_LEFT );
}

// --- Criação / Destruição ---

/**
 * @brief Cria uma instância alocada dinamicamente da struct GameWorld.
 */
GameWorld *createGameWorld( void ) {
    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
    gw->mapa = NULL;
    gw->jogador = NULL;
    gw->earDog = NULL;
    gw->numWolves = 0;
    gw->numIceShards = 0;
    for ( int i = 0; i < MAX_WOLVES; i++ ) gw->wolves[i] = NULL;
    for ( int i = 0; i < MAX_ICE_SHARDS; i++ ) gw->iceShards[i] = NULL;
    for ( int i = 0; i < MAX_PROJETEIS; i++ ) gw->projeteis[i].ativo = false;
    gw->estadoTela = TELA_MENU;
    gw->faseAtual = 0;
    gw->gravidade = 900;
    gw->modo2Jogadores = false;
    gw->belial = NULL;
    gw->tempoDeJogo = 0.0f;
    gw->camera = (Camera2D) {
        .offset = { 0 },
        .target = { 0 },
        .rotation = 0.0f,
        .zoom = 1.0f
    };
    gw->menuBgOffset = 0.0f;
    gw->deveSair = false;
    return gw;
}

/**
 * @brief Destrói um objeto GameWorld e suas dependências.
 */
void destroyGameWorld( GameWorld *gw ) {
    if ( gw != NULL ) {
        if ( gw->mapa != NULL ) { destruirMapa( gw->mapa ); gw->mapa = NULL; }
        if ( gw->jogador != NULL ) { destruirJogador( gw->jogador ); gw->jogador = NULL; }
        if ( gw->belial != NULL ) { destruirBelial( gw->belial ); gw->belial = NULL; }
        if ( gw->earDog != NULL ) { destruirEarDog( gw->earDog ); gw->earDog = NULL; }
        for ( int i = 0; i < gw->numWolves; i++ ) { if ( gw->wolves[i] != NULL ) destruirWolf( gw->wolves[i] ); gw->wolves[i] = NULL; }
        gw->numWolves = 0;
        for ( int i = 0; i < gw->numIceShards; i++ ) { if ( gw->iceShards[i] != NULL ) destruirIceShard( gw->iceShards[i] ); gw->iceShards[i] = NULL; }
        gw->numIceShards = 0;
        for ( int i = 0; i < MAX_PROJETEIS; i++ ) gw->projeteis[i].ativo = false;
        free( gw );
    }
}

// --- Update ---

static Rectangle obterRetanguloBotaoMenu( int indiceBotao ) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    float scaleX = (float)screenW / 1672.0f;
    float scaleY = (float)screenH / 941.0f;
    
    float bx = 598.0f * scaleX;
    float bw = 472.0f * scaleX;
    float bh = 71.0f * scaleY;
    
    float by = 0.0f;
    switch ( indiceBotao ) {
        case 0: by = 495.0f; break; // 1 Player
        case 1: by = 573.0f; break; // 2 Players
        case 2: by = 651.0f; break; // Options
        case 3: by = 729.0f; break; // Credits
        case 4: by = 808.0f; break; // Quit Game
    }
    by *= scaleY;
    
    return (Rectangle){ bx, by, bw, bh };
}

static void updateMenu( GameWorld *gw ) {
    gw->tempoDeJogo += GetFrameTime();
    
    float scale = (float)GetScreenHeight() / 283.0f;
    float viewWidth = (float)GetScreenWidth() / scale;
    float maxOffset = 10603.0f - viewWidth;
    if ( maxOffset < 0 ) maxOffset = 0;
    gw->menuBgOffset = (sinf(gw->tempoDeJogo * 0.03f) * 0.5f + 0.5f) * maxOffset;
    
    // Detect click on "1 Player" (Button 0)
    Rectangle btn1Player = obterRetanguloBotaoMenu( 0 );
    if ( mouseSobreRect( btn1Player ) && IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        gw->modo2Jogadores = false;
        gw->faseAtual = 0;
        inicializar( gw );
        gw->estadoTela = TELA_JOGO;
    }
    
    // Detect click on "2 Players" (Button 1)
    Rectangle btn2Players = obterRetanguloBotaoMenu( 1 );
    if ( mouseSobreRect( btn2Players ) && IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        gw->modo2Jogadores = true;
        gw->faseAtual = 0;
        inicializar( gw );
        gw->estadoTela = TELA_JOGO;
    }
    
    // Detect click on "Quit Game" (Button 4)
    Rectangle btnQuit = obterRetanguloBotaoMenu( 4 );
    if ( mouseSobreRect( btnQuit ) && IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        gw->deveSair = true;
    }
}

static void updateJogo( GameWorld *gw, float delta ) {

    if ( IsKeyPressed( KEY_R ) ) {
        reiniciar( gw );
        return;
    }

    // Tecla P pausa o jogo
    if ( IsKeyPressed( KEY_P ) ) {
        gw->estadoTela = TELA_PAUSE;
        return;
    }

    Jogador *j = gw->jogador;
    Belial *b = gw->belial;
    gw->tempoDeJogo += delta;
    atualizarMapa( gw->mapa, gw, delta );
    entradaJogador( j, gw, delta );
    atualizarJogador( j, gw, delta );
    if ( gw->modo2Jogadores && b != NULL ) {
        entradaBelial( b, gw, delta );
        atualizarBelial( b, gw, delta );
    }
    
    // Atualiza inimigos fase 1
    if ( gw->faseAtual == 0 ) {
        for ( int i = 0; i < gw->numWolves; i++ ) {
            atualizarWolf( gw->wolves[i], gw, delta );
        }
        for ( int i = 0; i < gw->numIceShards; i++ ) {
            atualizarIceShard( gw->iceShards[i], gw, delta );
        }
        for ( int i = 0; i < MAX_PROJETEIS; i++ ) {
            if ( gw->projeteis[i].ativo ) {
                gw->projeteis[i].pos.x += gw->projeteis[i].vel.x * delta;
                gw->projeteis[i].pos.y += gw->projeteis[i].vel.y * delta;
                gw->projeteis[i].lifeTimer += delta;
                gw->projeteis[i].animTimer += delta * 12.0f;
                gw->projeteis[i].animFrame = ((int)gw->projeteis[i].animTimer) % 6;
                if ( gw->projeteis[i].lifeTimer >= 2.0f ) {
                    gw->projeteis[i].ativo = false;
                }
            }
        }
        // Resolucao de colisões fase 1
        // Wolf ataques e hit no jogador (PolarBear)
        for ( int i = 0; i < gw->numWolves; i++ ) {
            Wolf *w = gw->wolves[i];
            if ( w != NULL && w->ativo && w->estado != ESTADO_WOLF_MORRENDO ) {
                // PolarBear hit
                bool jogadorSocandoFrameDano = ( j->socando && j->socandoFrame == 2 ) ||
                                               ( j->socoAereo && !j->socoAereoAterrissou );
                if ( jogadorSocandoFrameDano && j->ativo ) {
                    Rectangle hitboxMao = obterHitboxSocoPolarBear( j );
                    Rectangle corpoWolf = wolfObterHitboxCorpo( w );
                    if ( CheckCollisionRecs( hitboxMao, corpoWolf ) ) {
                        wolfReceberDano( w );
                        if ( w->estado == ESTADO_WOLF_MORRENDO ) {
                            j->quantidadeVidas += 0.5f;
                            if ( j->quantidadeVidas > j->maxVidas ) j->quantidadeVidas = j->maxVidas;
                        }
                    }
                }
                // Belial hit
                if ( gw->modo2Jogadores && b != NULL && b->ativo ) {
                    bool belialSocandoFrameDano = ( b->socando && b->socandoFrame == 2 ) ||
                                                  ( b->socoAereo && !b->socoAereoAterrissou );
                    if ( belialSocandoFrameDano ) {
                        Rectangle hitboxMaoB = obterHitboxSocoPolarBear( (Jogador*)b );
                        Rectangle corpoWolf = wolfObterHitboxCorpo( w );
                        if ( CheckCollisionRecs( hitboxMaoB, corpoWolf ) ) {
                            wolfReceberDano( w );
                        }
                    }
                }
                // Wolf ataca jogador (apenas dano de ataque)
                if ( w->estado == ESTADO_WOLF_ATACANDO && !w->hasHitPlayer ) {
                    Rectangle ataqueWolf = wolfObterHitboxAtaque( w );

                    // --- Colisão com PolarBear ---
                    if ( j->ativo ) {
                        Rectangle visualJ = obterHitboxVisualJogador( j );
                        if ( CheckCollisionRecs( ataqueWolf, visualJ ) ) {
                            float depthW = w->ret.y + w->ret.height;
                            float depthJ = j->ret.y + j->ret.height;
                            if ( fabsf(depthW - depthJ) < 60.0f ) {
                                if ( j->invencibilidade <= 0.0f ) {
                                    j->quantidadeVidas -= 0.5f;
                                    j->invencibilidade = 1.5f;
                                    w->hasHitPlayer = true;
                                }
                            }
                        }
                    }

                    // --- Colisão com Belial ---
                    if ( gw->modo2Jogadores && b != NULL && b->ativo && !w->hasHitPlayer ) {
                        Rectangle visualB = obterHitboxVisualJogador( (Jogador*)b );
                        if ( CheckCollisionRecs( ataqueWolf, visualB ) ) {
                            float depthW = w->ret.y + w->ret.height;
                            float depthB = b->ret.y + b->ret.height;
                            if ( fabsf(depthW - depthB) < 60.0f ) {
                                if ( b->invencibilidade <= 0.0f ) {
                                    b->quantidadeVidas--;
                                    b->invencibilidade = 1.5f;
                                    w->hasHitPlayer = true;
                                }
                            }
                        }
                    }
                }
            } else if ( w != NULL && !w->ativo ) {
                // Minion terminou a animacao de morte -> destruir e liberar memoria
                destruirWolf( w );
                gw->wolves[i] = NULL;
            }
        }
        // Soco (normal ou aéreo) do jogador acerta Ice Shard
        for ( int i = 0; i < gw->numIceShards; i++ ) {
            IceShard *is = gw->iceShards[i];
            if ( is != NULL && is->ativo && is->estado != ESTADO_ICESHARD_MORRENDO ) {
                float isW = is->ret.width * 0.8f;
                float isH = is->ret.height * 0.8f;
                float isX = is->ret.x + (is->ret.width - isW) / 2.0f;
                float isY = is->ret.y + (is->ret.height - isH) / 2.0f;
                Rectangle hitboxIS = { isX, isY, isW, isH };

                // PolarBear hit
                bool socoDano = ( j->socando && j->socandoFrame == 2 ) || 
                                ( j->socoAereo && !j->socoAereoAterrissou );
                if ( socoDano && j->ativo ) {
                    Rectangle hitboxMao = obterHitboxSocoPolarBear( j );
                    if ( CheckCollisionRecs( hitboxMao, hitboxIS ) ) {
                        iceShardReceberDano( is, j->ret.x );
                        if ( is->estado == ESTADO_ICESHARD_MORRENDO ) {
                            j->quantidadeVidas += 0.5f;
                            if ( j->quantidadeVidas > j->maxVidas ) j->quantidadeVidas = j->maxVidas;
                        }
                    }
                }

                // Belial hit
                if ( gw->modo2Jogadores && b != NULL && b->ativo ) {
                    bool socoDanoB = ( b->socando && b->socandoFrame == 2 ) || 
                                     ( b->socoAereo && !b->socoAereoAterrissou );
                    if ( socoDanoB ) {
                        Rectangle hitboxMaoB = obterHitboxSocoPolarBear( (Jogador*)b );
                        if ( CheckCollisionRecs( hitboxMaoB, hitboxIS ) ) {
                            iceShardReceberDano( is, b->ret.x );
                        }
                    }
                }
            }
        }
        // Projéteis acertam jogador ou Belial
        for ( int i = 0; i < MAX_PROJETEIS; i++ ) {
            if ( gw->projeteis[i].ativo ) {
                Rectangle projRet = { gw->projeteis[i].pos.x - 10, gw->projeteis[i].pos.y - 10, 20, 20 };
                // --- Acerta PolarBear ---
                if ( j->ativo && CheckCollisionRecs( projRet, j->ret ) ) {
                    gw->projeteis[i].ativo = false;
                    if ( j->invencibilidade <= 0.0f ) {
                        j->quantidadeVidas -= 0.5f;
                        j->invencibilidade = 1.5f;
                    }
                // --- Acerta Belial ---
                } else if ( gw->modo2Jogadores && b != NULL && b->ativo && CheckCollisionRecs( projRet, b->ret ) ) {
                    gw->projeteis[i].ativo = false;
                    if ( b->invencibilidade <= 0.0f ) {
                        b->quantidadeVidas--;
                        b->invencibilidade = 1.5f;
                    }
                }
            }
        }
        
        // --- Gerenciador de Vidas (Life Manager) - Fase 1 ---
        // PolarBear
        if ( j->quantidadeVidas <= 0 ) {
            inicializar( gw );
            return;
        }
        // Belial
        if ( gw->modo2Jogadores && b != NULL && b->ativo && b->quantidadeVidas <= 0 ) {
            b->quantidadeVidas = 0;
            b->ativo = false;
            b->respawnTimer = 3.0f;
            b->ret.x = -999.0f; b->ret.y = -999.0f;
            b->vel = (Vector2){0, 0};
        }

    }

    // --- Gerenciador de Respawn (ambas as fases) ---
    {
        Jogador *j2 = gw->jogador;
        Belial  *b2 = gw->belial;

        // Respawn do PolarBear
        if ( j2 != NULL && !j2->ativo && j2->respawnTimer > 0.0f ) {
            j2->respawnTimer -= delta;
            if ( j2->respawnTimer <= 0.0f ) {
                j2->respawnTimer = 0.0f;
                if ( j2->quantidadeVidas > 0 ) {
                    // Reaparecer próximo ao Belial (se vivo) ou posição padrão
                    float spawnX = 150.0f;
                    float spawnY = ( gw->faseAtual == 0 ) ? 220.0f - j2->ret.height
                                  : (float)rm.ifsp_highschool.height - 63.0f - j2->ret.height;
                    if ( b2 != NULL && b2->ativo ) {
                        spawnX = b2->ret.x - 40.0f;
                        spawnY = b2->ret.y;
                    }
                    j2->ret.x = spawnX;
                    j2->ret.y = spawnY;
                    j2->vel = (Vector2){0,0};
                    j2->noPulo = false;
                    j2->puloY = 0.0f;
                    j2->estado = ESTADO_JOGADOR_PARADO;
                    j2->invencibilidade = 2.0f; // breve invencibilidade ao respawnar
                    j2->ativo = true;
                }
                // Se vidas <= 0, permanece inativo (morto definitivo)
            }
        }

        // Respawn do Belial
        if ( b2 != NULL && !b2->ativo && b2->respawnTimer > 0.0f ) {
            b2->respawnTimer -= delta;
            if ( b2->respawnTimer <= 0.0f ) {
                b2->respawnTimer = 0.0f;
                if ( b2->quantidadeVidas > 0 ) {
                    float spawnX = 180.0f;
                    float spawnY = ( gw->faseAtual == 0 ) ? 220.0f - b2->ret.height
                                  : (float)rm.ifsp_highschool.height - 63.0f - b2->ret.height;
                    if ( j2 != NULL && j2->ativo ) {
                        spawnX = j2->ret.x + 40.0f;
                        spawnY = j2->ret.y;
                    }
                    b2->ret.x = spawnX;
                    b2->ret.y = spawnY;
                    b2->vel = (Vector2){0,0};
                    b2->noPulo = false;
                    b2->puloY = 0.0f;
                    b2->estado = ESTADO_JOGADOR_PARADO;
                    b2->invencibilidade = 2.0f;
                    b2->ativo = true;
                }
            }
        }

        // --- Game Over Colaborativo ---
        // Só reinicia quando AMBOS os jogadores estão mortos definitivamente
        bool jMortoDefinitivo = ( j2 == NULL ) || ( !j2->ativo && j2->respawnTimer <= 0.0f && j2->quantidadeVidas <= 0 );
        bool bMortoDefinitivo = !gw->modo2Jogadores || ( b2 == NULL ) || ( !b2->ativo && b2->respawnTimer <= 0.0f && b2->quantidadeVidas <= 0 );
        if ( jMortoDefinitivo && bMortoDefinitivo ) {
            inicializar( gw );
            return;
        }
    }
    
    if ( gw->faseAtual == 1 && gw->earDog != NULL ) {
        atualizarEarDog( gw->earDog, gw, delta );
        resolverColisoesFase2( gw );
        
        // Spawn de Minions (lobos de 1 HP) quando EarDog late
        if ( gw->earDog->summonPendente && gw->earDog->estado != ESTADO_EARDOG_MORRENDO ) {
            gw->earDog->summonPendente = false;
            
            // Encontra 1 slot livre para o lobo minion
            float mapW = (float)gw->mapa->colunas;
            float spawnY = gw->jogador->ret.y + gw->jogador->ret.height / 2.0f - 10.0f;
            float spawnPosition = 20.0f; // Pode spawnar na esquerda ou onde for melhor
            
            for ( int i = 0; i < MAX_WOLVES; i++ ) {
                if ( gw->wolves[i] == NULL || !gw->wolves[i]->ativo ) {
                    if ( gw->wolves[i] != NULL ) {
                        destruirWolf( gw->wolves[i] );
                    }
                    gw->wolves[i] = criarWolf( spawnPosition, spawnY, 15, 10 );
                    gw->wolves[i]->quantidadeVidas = 1; // 1 soco = morreu
                    if ( gw->numWolves <= i ) gw->numWolves = i + 1;
                    break; // Spawna exatamente 1 minion
                }
            }
        }
        
        // Colisoes Minions na fase 2 (lobos convocados)
        Jogador *j2 = gw->jogador;
        for ( int i = 0; i < gw->numWolves; i++ ) {
            Wolf *w = gw->wolves[i];
            if ( w != NULL ) {
                atualizarWolf( w, gw, delta );
            }
            if ( w != NULL && w->ativo && w->estado != ESTADO_WOLF_MORRENDO ) {
                bool socoDano = ( j2->socando && j2->socandoFrame == 2 ) ||
                                ( j2->socoAereo && !j2->socoAereoAterrissou );
                if ( socoDano ) {
                    Rectangle hitboxMao = obterHitboxSocoPolarBear( j2 );
                    Rectangle corpoW    = wolfObterHitboxCorpo( w );
                    if ( CheckCollisionRecs( hitboxMao, corpoW ) ) {
                        wolfReceberDano( w );
                        if ( w->estado == ESTADO_WOLF_MORRENDO ) {
                            j2->quantidadeVidas += 0.5f;
                            if ( j2->quantidadeVidas > j2->maxVidas ) j2->quantidadeVidas = j2->maxVidas;
                        }
                    }
                }
                if ( w->estado == ESTADO_WOLF_ATACANDO && !w->hasHitPlayer ) {
                    Rectangle ataqueW = wolfObterHitboxAtaque( w );
                    Rectangle visualJ = obterHitboxVisualJogador( j2 );
                    if ( CheckCollisionRecs( ataqueW, visualJ ) ) {
                        float depthW = w->ret.y + w->ret.height;
                        float depthJ = j2->ret.y + j2->ret.height;
                        if ( fabsf(depthW - depthJ) < 60.0f ) {
                            if ( j2->invencibilidade <= 0.0f ) {
                                j2->quantidadeVidas -= 0.5f;
                                j2->invencibilidade = 1.5f;
                                w->hasHitPlayer = true;
                            }
                        }
                    }
                }
            } else if ( w != NULL && !w->ativo ) {
                // Destroi o minion morto para sumir da tela e limpar array
                destruirWolf( w );
                gw->wolves[i] = NULL;
            }
        }
        
        // Vida do jogador zerada na Fase 2 → reinicia a fase
        if ( j->quantidadeVidas <= 0 ) {
            ResetBossScene( gw );
            return;
        }
        
        // EarDog derrotado → O jogo continua para o boss ficar caído morto
        if ( gw->earDog != NULL &&
             gw->earDog->quantidadeVidas <= 0 &&
             gw->earDog->estado == ESTADO_EARDOG_MORRENDO &&
             gw->earDog->animFrame >= 3 ) {
            // A fase não deve reiniciar, ele deve ficar caído morto lá.
            // Spawns já estão inibidos.
        }
    }
    atualizarCamera( gw );

    // --- Limitador de Tela Dinâmico (Camera Clamping) ---
    // Impede que qualquer jogador saia dos limites visíveis da câmera
    {
        float halfW  = ( GetScreenWidth()  / gw->camera.zoom ) / 2.0f;
        float halfH  = ( GetScreenHeight() / gw->camera.zoom ) / 2.0f;
        float leftB  = gw->camera.target.x - halfW;
        float rightB = gw->camera.target.x + halfW;
        float topB   = gw->camera.target.y - halfH;
        float botB   = gw->camera.target.y + halfH;

        Jogador *jc = gw->jogador;
        if ( jc != NULL && jc->ativo ) {
            if ( jc->ret.x < leftB ) { jc->ret.x = leftB; jc->vel.x = 0; }
            if ( jc->ret.x + jc->ret.width > rightB ) { jc->ret.x = rightB - jc->ret.width; jc->vel.x = 0; }
            if ( jc->ret.y < topB ) { jc->ret.y = topB; jc->vel.y = 0; }
            if ( jc->ret.y + jc->ret.height > botB ) { jc->ret.y = botB - jc->ret.height; jc->vel.y = 0; }
        }
        Belial *bc = gw->belial;
        if ( bc != NULL && bc->ativo ) {
            if ( bc->ret.x < leftB ) { bc->ret.x = leftB; bc->vel.x = 0; }
            if ( bc->ret.x + bc->ret.width > rightB ) { bc->ret.x = rightB - bc->ret.width; bc->vel.x = 0; }
            if ( bc->ret.y < topB ) { bc->ret.y = topB; bc->vel.y = 0; }
            if ( bc->ret.y + bc->ret.height > botB ) { bc->ret.y = botB - bc->ret.height; bc->vel.y = 0; }
        }
    }
}

static void updatePause( GameWorld *gw ) {
    // Tecla P ou ESC despausa
    if ( IsKeyPressed( KEY_P ) || IsKeyPressed( KEY_ESCAPE ) ) {
        gw->estadoTela = TELA_JOGO;
    }
    // O clique no botão "Menu" é tratado no draw
}

/**
 * @brief Lê a entrada do usuário e atualiza o estado do jogo.
 */
void updateGameWorld( GameWorld *gw, float delta ) {
    switch ( gw->estadoTela ) {
        case TELA_MENU:
            updateMenu( gw );
            break;
        case TELA_JOGO:
            updateJogo( gw, delta );
            break;
        case TELA_PAUSE:
            updatePause( gw );
            break;
    }
}

// --- Draw ---

static void drawMenu( GameWorld *gw ) {

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // 1. Desenha o cenário de fundo frozen suburbs desfocado
    // O fundo se move lentamente de acordo com gw->menuBgOffset
    float bgScale = (float)screenH / 283.0f;
    float viewWidth = (float)screenW / bgScale;
    
    Rectangle srcBg = { gw->menuBgOffset, 30.0f, viewWidth, 283.0f };
    Rectangle destBg = { 0, 0, (float)screenW, (float)screenH };
    Vector2 origin = { 0, 0 };
    DrawTexturePro( rm.frozensuburbs_blurred, srcBg, destBg, origin, 0.0f, WHITE );

    // 2. Desenha a textura do menu (menu.png) por cima do fundo
    Rectangle srcMenu = { 0, 0, 1672.0f, 941.0f };
    Rectangle destMenu = { 0, 0, (float)screenW, (float)screenH };
    DrawTexturePro( rm.menu, srcMenu, destMenu, origin, 0.0f, WHITE );

    // 3. Desenha os efeitos hover e seletores para os botões do menu
    // São 5 botões: 0=1 Player, 1=2 Players, 2=Options, 3=Credits, 4=Quit Game
    for ( int i = 0; i < 5; i++ ) {
        Rectangle btnRect = obterRetanguloBotaoMenu( i );
        if ( mouseSobreRect( btnRect ) ) {
            // Efeito hover: destaque pulsante semi-transparente (azul glacial/neon)
            int alphaFill = (int)( 35 + 15 * sinf( gw->tempoDeJogo * 6.0f ) );
            DrawRectangleRounded( btnRect, 0.25f, 8, (Color){ 130, 200, 255, alphaFill } );

            // Borda brilhante pulsante
            int alphaBorder = (int)( 180 + 75 * sinf( gw->tempoDeJogo * 6.0f ) );
            Color borderCor = (Color){ 130, 220, 255, alphaBorder };
            DrawRectangleRoundedLines( btnRect, 0.25f, 8, borderCor );

            // Seletores em forma de diamante nas laterais com pulsação horizontal
            float pointerLeftX = btnRect.x - 25.0f - 5.0f * sinf( gw->tempoDeJogo * 6.0f );
            float pointerRightX = btnRect.x + btnRect.width + 25.0f + 5.0f * sinf( gw->tempoDeJogo * 6.0f );
            float pointerY = btnRect.y + btnRect.height / 2.0f;

            // Desenha diamante esquerdo
            Vector2 p1L = { pointerLeftX, pointerY - 8.0f };
            Vector2 p2L = { pointerLeftX + 10.0f, pointerY };
            Vector2 p3L = { pointerLeftX, pointerY + 8.0f };
            Vector2 p4L = { pointerLeftX - 10.0f, pointerY };
            DrawTriangle( p1L, p4L, p2L, borderCor );
            DrawTriangle( p3L, p2L, p4L, (Color){ 100, 200, 255, alphaBorder } );

            // Desenha diamante direito
            Vector2 p1R = { pointerRightX, pointerY - 8.0f };
            Vector2 p2R = { pointerRightX + 10.0f, pointerY };
            Vector2 p3R = { pointerRightX, pointerY + 8.0f };
            Vector2 p4R = { pointerRightX - 10.0f, pointerY };
            DrawTriangle( p1R, p4R, p2R, borderCor );
            DrawTriangle( p3R, p2R, p4R, (Color){ 100, 200, 255, alphaBorder } );
        }
    }
}

static void drawJogo( GameWorld *gw ) {

    ClearBackground( (Color) { 135, 206, 235, 255 } ); // Sky blue color

    BeginMode2D( gw->camera );
    desenharFundo( gw );
    desenharMapa( gw->mapa );

    // Cerca: fica ATRÁS do jogador quando ele está na rua, e À FRENTE quando está no mezanino
    // (apenas na fase 0 que tem mezanino)
    if ( gw->faseAtual == 0 ) {
        bool noMezanino = gw->jogador->noMezanino;
        if ( !noMezanino ) {
            DrawTexture( rm.mezzanine_railing, 8300, 130, WHITE );
        }
        
        // Ice Shards flutuam atrás do mezanino e cenário
        for ( int i = 0; i < gw->numIceShards; i++ ) {
            desenharIceShard( gw->iceShards[i] );
        }
        
        // Projéteis de gelo
        static const Rectangle proj_frames[6] = {
            { 170, 108, 12, 22 }, { 192, 108, 16, 22 }, { 221, 108, 22, 22 },
            { 256, 108, 16, 22 }, { 282, 108, 12, 22 }, { 306, 108, 12, 22 }
        };
        for ( int i = 0; i < MAX_PROJETEIS; i++ ) {
            if ( gw->projeteis[i].ativo ) {
                Rectangle src = proj_frames[gw->projeteis[i].animFrame % 6];
                float scale = 0.8f; // Projéteis reduzidos de 1.5 para 0.8
                Rectangle dest = { gw->projeteis[i].pos.x, gw->projeteis[i].pos.y, src.width*scale, src.height*scale };
                DrawTexturePro( rm.iceShard, src, dest, (Vector2){ src.width*scale/2, src.height*scale/2 }, 0, WHITE );
            }
        }
        
        // Wolves
        for ( int i = 0; i < gw->numWolves; i++ ) {
            desenharWolf( gw->wolves[i] );
        }

        desenharJogador( gw->jogador );
        if ( gw->belial != NULL ) {
            desenharBelial( gw->belial );
        }

        if ( noMezanino ) {
            DrawTexture( rm.mezzanine_railing, 8300, 130, WHITE );
        }
    } else {
        // Fase 2: EarDog e minions atras, PolarBear sempre na frente
        if ( gw->earDog != NULL ) {
            desenharEarDog( gw->earDog );
        }
        // Minions (lobos convocados) também desenhados aqui dentro do BeginMode2D
        for ( int i = 0; i < gw->numWolves; i++ ) {
            if ( gw->wolves[i] != NULL ) desenharWolf( gw->wolves[i] );
        }
        desenharJogador( gw->jogador );
        if ( gw->belial != NULL ) {
            desenharBelial( gw->belial );
        }
    }

    // ════════════════════════════════════════════
    // UI SOBREPOSTA — HUD PERSONALIZADO
    // ════════════════════════════════════════════
    EndMode2D();

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();


    // ─── HUD Vidas do Jogador (todas as fases) ───
    if ( gw->jogador != NULL ) {
        float pbVidas = gw->jogador->quantidadeVidas;
        if ( pbVidas < 0.0f ) pbVidas = 0.0f;

        // Painel de fundo da HUD
        Rectangle hudPanel = { 8, 8, 160, 52 };
        DrawRectangleRounded( hudPanel, 0.25f, 6, (Color){ 0, 0, 0, 160 } );
        DrawRectangleRoundedLines( hudPanel, 0.25f, 6, (Color){ 200, 200, 220, 80 } );

        // Ícone do Urso (rosto)
    // --- UI sobreposta (coordenadas de tela) ---

    // Canvas de HUD (Apenas na Fase 1 - Frozen Suburbs)
    if ( gw->faseAtual == 0 ) {
        // --- HUD PolarBear (canto superior esquerdo) ---
        Texture2D iconTex = rm.polarbear;
        Rectangle iconSrc  = { 216, 396, 23, 20 };
        Rectangle iconDest = { 20, 20, 23 * 3.0f, 20 * 3.0f };
        DrawTexturePro( iconTex, iconSrc, iconDest, (Vector2){0,0}, 0.0f, WHITE );

        // Segmentos de vida (corações / caixas divididas)
        int maxVidas = (int)gw->jogador->maxVidas;
        float segW = 28.0f, segH = 18.0f, segPad = 4.0f;
        float segStartX = 66.0f;
        float segStartY = 20.0f;
        for ( int i = 0; i < maxVidas; i++ ) {
            float sx = segStartX + i * (segW + segPad);
            
            // Fundo escuro (vida vazia)
            DrawRectangleRounded( (Rectangle){sx, segStartY, segW, segH}, 0.3f, 4, (Color){60, 30, 30, 200} );
            
            float fillRatio = 0.0f;
            if ( pbVidas >= i + 1.0f ) {
                fillRatio = 1.0f;
            } else if ( pbVidas > i ) {
                fillRatio = pbVidas - i;
            }
            
            if ( fillRatio > 0.0f ) {
                float fillW = segW * fillRatio;
                // Recorta o desenho da barra cheia no tamanho de fillRatio
                BeginScissorMode( (int)sx, (int)segStartY, (int)fillW, (int)segH );
                DrawRectangleRounded( (Rectangle){sx, segStartY, segW, segH}, 0.3f, 4, (Color){220, 60, 60, 255} );
                DrawRectangle( (int)sx+3, (int)segStartY+2, (int)segW-6, 4, (Color){255,200,200,100} );
                EndScissorMode();
                
                // Glow na caixa cheia ou preenchida parcialmente
                DrawRectangleRoundedLines( (Rectangle){sx-1, segStartY-1, segW+2, segH+2}, 0.3f, 4, (Color){255,120,120, 80} );
            }
            
            // Borda
            DrawRectangleRoundedLines( (Rectangle){sx, segStartY, segW, segH}, 0.3f, 4, (Color){255,255,255, fillRatio > 0.0f ? 120 : 40} );
            
            // Partição vertical da metade (traço escuro indicando meio coração)
            DrawLine( (int)(sx + segW/2.0f), (int)segStartY, (int)(sx + segW/2.0f), (int)(segStartY + segH), (Color){0, 0, 0, 150} );
        }

        char livesTxt[16];
        int pbVidas = ( gw->jogador->quantidadeVidas > 0 ) ? gw->jogador->quantidadeVidas : 0;
        sprintf( livesTxt, "X %d", pbVidas );
        DrawText( livesTxt, 110, 35, 30, BLACK );
        DrawText( livesTxt, 108, 33, 30, WHITE );

        // Timer de respawn do PolarBear (abaixo do ícone)
        if ( !gw->jogador->ativo && gw->jogador->respawnTimer > 0.0f ) {
            char timerTxt[32];
            sprintf( timerTxt, "Respawn: %d s", (int)ceilf( gw->jogador->respawnTimer ) );
            DrawText( timerTxt, 18, 90, 20, BLACK );
            DrawText( timerTxt, 16, 88, 20, (Color){ 255, 220, 60, 255 } );
        }

        // --- HUD Belial (canto superior direito) ---
        if ( gw->modo2Jogadores && gw->belial != NULL ) {
            int bVidas = gw->belial->quantidadeVidas;
            if ( bVidas < 0 ) bVidas = 0;
            int maxVidasB = 3;

            // Painel de fundo da HUD (espelhado à direita)
            float bPanelW = 160.0f;
            float bPanelH = 52.0f;
            float bPanelX = (float)screenW - bPanelW - 8.0f;
            float bPanelY = 8.0f;
            Rectangle bHudPanel = { bPanelX, bPanelY, bPanelW, bPanelH };
            DrawRectangleRounded( bHudPanel, 0.25f, 6, (Color){ 0, 0, 0, 160 } );
            DrawRectangleRoundedLines( bHudPanel, 0.25f, 6, (Color){ 200, 200, 220, 80 } );

            // Ícone do Belial (canto superior direito do painel)
            Texture2D belialTex = rm.belial;
            float bIconScale = 2.0f;
            float bIconW = 23 * bIconScale;
            float bIconH = 20 * bIconScale;
            float bIconX = (float)screenW - bIconW - 12.0f;
            float bIconY = 14.0f;
            Rectangle bIconSrcFace = { 25, 50, 23, 20 };
            Rectangle bIconDest = { bIconX, bIconY, bIconW, bIconH };
            DrawTexturePro( belialTex, bIconSrcFace, bIconDest, (Vector2){0,0}, 0.0f, WHITE );

            // Segmentos de vida (idênticos ao PolarBear, alinhados à esquerda do ícone)
            float segW = 28.0f, segH = 18.0f, segPad = 4.0f;
            // Os segmentos crescem da direita para a esquerda
            float segEndX   = bIconX - 6.0f;   // lado direito do bloco de segmentos
            float segStartY = 20.0f;
            for ( int i = 0; i < maxVidasB; i++ ) {
                // Segmento mais próximo do ícone = última vida (i=0 é a mais à direita)
                float sx = segEndX - (i + 1) * segW - i * segPad;

                // Fundo escuro (vida vazia)
                DrawRectangleRounded( (Rectangle){sx, segStartY, segW, segH}, 0.3f, 4, (Color){60, 30, 30, 200} );

                float fillRatio = 0.0f;
                int revIdx = maxVidasB - 1 - i; // índice lógico (0 = primeira vida)
                if ( bVidas >= revIdx + 1 ) {
                    fillRatio = 1.0f;
                } else if ( bVidas > revIdx ) {
                    fillRatio = (float)(bVidas - revIdx);
                }

                if ( fillRatio > 0.0f ) {
                    float fillW = segW * fillRatio;
                    // Recorta no tamanho de fillRatio (fill cresce da esquerda)
                    BeginScissorMode( (int)sx, (int)segStartY, (int)fillW, (int)segH );
                    DrawRectangleRounded( (Rectangle){sx, segStartY, segW, segH}, 0.3f, 4, (Color){220, 140, 20, 255} );
                    DrawRectangle( (int)sx+3, (int)segStartY+2, (int)segW-6, 4, (Color){255,230,150,100} );
                    EndScissorMode();

                    // Glow laranja na caixa preenchida
                    DrawRectangleRoundedLines( (Rectangle){sx-1, segStartY-1, segW+2, segH+2}, 0.3f, 4, (Color){255,180,60, 80} );
                }

                // Borda
                DrawRectangleRoundedLines( (Rectangle){sx, segStartY, segW, segH}, 0.3f, 4, (Color){255,255,255, fillRatio > 0.0f ? 120 : 40} );

                // Partição vertical da metade
                DrawLine( (int)(sx + segW/2.0f), (int)segStartY, (int)(sx + segW/2.0f), (int)(segStartY + segH), (Color){0, 0, 0, 150} );
            }

            // Texto de vidas (à esquerda dos segmentos, com sombra)
            int bVidasInt = gw->belial->quantidadeVidas;
            if ( bVidasInt < 0 ) bVidasInt = 0;
            char bLivesTxt[16];
            sprintf( bLivesTxt, "%d X", bVidasInt );
            float segBlockLeft = segEndX - maxVidasB * segW - (maxVidasB - 1) * segPad;
            int bTxtW = MeasureText( bLivesTxt, 22 );
            DrawText( bLivesTxt, (int)(segBlockLeft - bTxtW - 4), 37, 22, BLACK );
            DrawText( bLivesTxt, (int)(segBlockLeft - bTxtW - 6), 35, 22, (Color){ 255, 180, 80, 255 } );

            // Timer de respawn do Belial (abaixo do painel)
            if ( !gw->belial->ativo && gw->belial->respawnTimer > 0.0f ) {
                char bTimerTxt[32];
                sprintf( bTimerTxt, "Respawn: %d s", (int)ceilf( gw->belial->respawnTimer ) );
                int bTimW = MeasureText( bTimerTxt, 20 );
                DrawText( bTimerTxt, screenW - bTimW - 18, 68, 20, BLACK );
                DrawText( bTimerTxt, screenW - bTimW - 20, 66, 20, (Color){ 255, 220, 60, 255 } );
            }
        }
    }
    }

    // ─── HUD Instrucao de Pause ───
    DrawText( "[P] Pausar", 8, screenH - 22, 14, (Color){ 255, 255, 255, 100 } );

    // ─── Botão de Troca de Fase (canto inferior direito) ───
    {
        char faseTxt[32];
        sprintf( faseTxt, "Fase %d", gw->faseAtual + 1 );
        float btnFaseW = 120.0f;
        float btnFaseH = 34.0f;
        Rectangle btnFase = {
            screenW - btnFaseW - 12.0f,
            screenH - btnFaseH - 12.0f,
            btnFaseW,
            btnFaseH
        };
        bool hovFase = mouseSobreRect( btnFase );
        DrawRectangleRounded( btnFase, 0.3f, 6,
            hovFase ? (Color){80, 60, 120, 230} : (Color){40, 30, 70, 200} );
        DrawRectangleRoundedLines( btnFase, 0.3f, 6,
            hovFase ? (Color){200, 160, 255, 200} : (Color){120, 90, 180, 120} );
        int ftw = MeasureText( faseTxt, 16 );
        DrawText( faseTxt,
            (int)(btnFase.x + (btnFaseW - ftw) / 2.0f),
            (int)(btnFase.y + (btnFaseH - 16) / 2.0f),
            16, hovFase ? WHITE : (Color){200, 170, 255, 230} );
        if ( hovFase && IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
            trocarFase( gw, (gw->faseAtual + 1) % 2 );
        }
    }


    // ─── Boss Health Bar — EarDog (Fase 2) ───
    if ( gw->faseAtual == 1 && gw->earDog != NULL ) {
        int maxVidas = 7;
        float vidas = gw->earDog->quantidadeVidas;
        if ( vidas < 0.0f ) vidas = 0.0f;

        float barW  = (float)screenW * 0.48f;  // 48% da tela
        float barH  = 22.0f;
        float barX  = (screenW - barW) / 2.0f;
        float barY  = 10.0f;

        // Fundo externo (borda escura)
        DrawRectangle( barX - 2, barY - 2, barW + 4, barH + 4, (Color){ 0, 0, 0, 200 } );

        // Fundo da barra
        DrawRectangle( barX, barY, barW, barH, (Color){ 30, 18, 10, 240 } );

        // Preenchimento gradiente — de vermelho sangue a laranja
        float fillW = ((float)vidas / maxVidas) * barW;
        // Degradê manual: duas fatias
        DrawRectangle( barX, barY, fillW * 0.5f, barH, (Color){ 140, 10, 10, 255 } );
        DrawRectangle( barX + fillW * 0.5f, barY, fillW * 0.5f, barH, (Color){ 200, 50, 10, 255 } );
        // Highlight no topo da barra
        DrawRectangle( barX, barY, fillW, 5, (Color){ 255, 120, 80, 80 } );

        // Segmentos internos (linhas divisórias a cada 10%)
        for ( int i = 1; i < maxVidas; i++ ) {
            float lx = barX + (barW / maxVidas) * i;
            DrawLine( lx, barY, lx, barY + barH, (Color){ 0, 0, 0, 120 } );
        }

        // Borda metálica
        DrawRectangleLines( barX, barY, barW, barH, (Color){ 180, 140, 90, 200 } );
        DrawRectangleLines( barX-1, barY-1, barW+2, barH+2, (Color){ 80, 60, 40, 150 } );

        // Nome do Boss acima da barra
        const char *bossName = "--- EarDog ---";
        int nameW = MeasureText( bossName, 16 );
        DrawText( bossName, barX + (barW - nameW)/2.0f, barY - 18, 16, (Color){255,200,100,255} );

        // Número de vidas restantes (pequeno, à direita)
        char vidasTxt[16];
        sprintf( vidasTxt, "%g/7", vidas );
        int vidasW = MeasureText( vidasTxt, 13 );
        DrawText( vidasTxt, barX + barW - vidasW - 4, barY + 4, 13, (Color){255,255,255,200} );

    }
}

static void drawPause( GameWorld *gw ) {

    // Desenha o jogo congelado por trás
    drawJogo( gw );

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // ── Overlay escuro com vinheta suave ──
    DrawRectangle( 0, 0, screenW, screenH, (Color){ 0, 0, 0, 190 } );

    // ── Painel central ──
    float painelW = 340.0f;
    float painelH = 260.0f;
    float painelX = ( screenW - painelW ) / 2.0f;
    float painelY = ( screenH - painelH ) / 2.0f;
    Rectangle painel = { painelX, painelY, painelW, painelH };

    // Fundo do painel — gradiente simulado com 2 retângulos
    DrawRectangleRounded( painel, 0.12f, 8, (Color){ 12, 8, 20, 245 } );
    DrawRectangleRounded(
        (Rectangle){ painelX+2, painelY+2, painelW-4, painelH/2 },
        0.12f, 8, (Color){ 30, 20, 50, 80 }
    );
    // Bordas: externa escura + interna brilhante
    DrawRectangleRoundedLines( painel, 0.12f, 8, (Color){ 40, 30, 70, 255 } );
    DrawRectangleRoundedLines(
        (Rectangle){ painelX+2, painelY+2, painelW-4, painelH-4 },
        0.10f, 8, (Color){ 160, 120, 255, 60 }
    );

    // ── Título "PAUSE" ──
    const char *tituloPause = "|| PAUSE ||";
    int tituloSize = 36;
    int tituloW    = MeasureText( tituloPause, tituloSize );
    // Sombra
    DrawText( tituloPause,
        (int)( painelX + (painelW - tituloW)/2 ) + 2,
        (int)( painelY + 24 ) + 2,
        tituloSize, (Color){ 0, 0, 0, 200 } );
    // Texto principal — roxo brilhante
    DrawText( tituloPause,
        (int)( painelX + (painelW - tituloW)/2 ),
        (int)( painelY + 24 ),
        tituloSize, (Color){ 200, 160, 255, 255 } );

    // Linha decorativa sob o título
    DrawLine(
        painelX + 20, painelY + 70,
        painelX + painelW - 20, painelY + 70,
        (Color){ 160, 120, 255, 100 }
    );

    // ── Botões ──
    float btnW   = 240.0f;
    float btnH   = 46.0f;
    float btnX   = painelX + (painelW - btnW) / 2.0f;
    float btn1Y  = painelY + 90.0f;
    float btn2Y  = painelY + 150.0f;
    float btn3Y  = painelY + 205.0f;

    // Botão CONTINUAR
    Rectangle btnContinuar = { btnX, btn1Y, btnW, btnH };
    bool hovCont = mouseSobreRect( btnContinuar );
    DrawRectangleRounded( btnContinuar, 0.2f, 6,
        hovCont ? (Color){50,160,100,255} : (Color){25,90,55,230} );
    DrawRectangleRoundedLines( btnContinuar, 0.2f, 6,
        hovCont ? (Color){160,255,180,200} : (Color){60,150,90,120} );
    {
        const char *t = "CONTINUAR";
        int tw = MeasureText( t, 20 );
        DrawText( t, btnX + (btnW-tw)/2, btn1Y + (btnH-20)/2, 20,
            hovCont ? WHITE : (Color){200,255,210,255} );
    }
    if ( hovCont && IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        gw->estadoTela = TELA_JOGO;
    }

    // Botão REINICIAR
    Rectangle btnReiniciar = { btnX, btn2Y, btnW, btnH };
    bool hovRein = mouseSobreRect( btnReiniciar );
    DrawRectangleRounded( btnReiniciar, 0.2f, 6,
        hovRein ? (Color){180,120,20,255} : (Color){100,65,10,230} );
    DrawRectangleRoundedLines( btnReiniciar, 0.2f, 6,
        hovRein ? (Color){255,220,100,200} : (Color){150,100,30,120} );
    {
        const char *t = "REINICIAR";
        int tw = MeasureText( t, 20 );
        DrawText( t, btnX + (btnW-tw)/2, btn2Y + (btnH-20)/2, 20,
            hovRein ? WHITE : (Color){255,230,170,255} );
    }
    if ( hovRein && IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        inicializar( gw );
        gw->estadoTela = TELA_JOGO;
    }

    // Botão MENU
    Rectangle btnMenu = { btnX, btn3Y, btnW, btnH };
    bool hovMenu = mouseSobreRect( btnMenu );
    DrawRectangleRounded( btnMenu, 0.2f, 6,
        hovMenu ? (Color){180,40,40,255} : (Color){90,20,20,230} );
    DrawRectangleRoundedLines( btnMenu, 0.2f, 6,
        hovMenu ? (Color){255,140,140,200} : (Color){140,60,60,120} );
    {
        const char *t = "MENU PRINCIPAL";
        int tw = MeasureText( t, 20 );
        DrawText( t, btnX + (btnW-tw)/2, btn3Y + (btnH-20)/2, 20,
            hovMenu ? WHITE : (Color){255,190,190,255} );
    }
    if ( hovMenu && IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        if ( gw->mapa    != NULL ) { destruirMapa( gw->mapa );       gw->mapa    = NULL; }
        if ( gw->jogador != NULL ) { destruirJogador( gw->jogador ); gw->jogador = NULL; }
        if ( gw->earDog  != NULL ) { destruirEarDog( gw->earDog );   gw->earDog  = NULL; }
        gw->estadoTela = TELA_MENU;
    }

    // Dica de teclado
    const char *dica = "ESC / P para continuar";
    int dicaW = MeasureText( dica, 12 );
    DrawText( dica, (screenW - dicaW)/2, painelY + painelH + 10, 12,
        (Color){ 180, 180, 200, 140 } );
}

/**
 * @brief Desenha o estado do jogo.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();

    switch ( gw->estadoTela ) {
        case TELA_MENU:
            drawMenu( gw );
            break;
        case TELA_JOGO:
            drawJogo( gw );
            break;
        case TELA_PAUSE:
            drawPause( gw );
            break;
    }

    EndDrawing();
}

// --- Fundo ---

static void desenharFundo( GameWorld *gw ) {
    if ( gw->faseAtual == 0 ) {
        // Fase 1: Desenha o cenário suburbs excluindo a borda azul externa (10px esquerda, 30px topo)
        DrawTextureRec(
            rm.frozensuburbs,
            (Rectangle){ 10, 30, 10603, 283 },
            (Vector2){ 0, 0 },
            WHITE
        );
    } else {
        // Fase 2 (Boss Fight): IFSP High School - imagem inteira
        DrawTexture( rm.ifsp_highschool, 0, 0, WHITE );
    }
}

// --- Câmera ---

static void atualizarCamera( GameWorld *gw ) {

    Jogador *j = gw->jogador;
    Belial *b = gw->belial;
    Camera2D *c = &gw->camera;

    float targetCamX = 0.0f;
    if ( j != NULL && j->ativo ) {
        targetCamX = j->ret.x + j->ret.width / 2.0f;
    } else if ( gw->modo2Jogadores && b != NULL && b->ativo ) {
        targetCamX = b->ret.x + b->ret.width / 2.0f;
    } else if ( j != NULL ) {
        targetCamX = j->ret.x + j->ret.width / 2.0f;
    }

    if ( gw->faseAtual == 0 ) {
        // Fase 1: Zoom dinâmico para 283px de altura
        c->zoom = GetScreenHeight() / 283.0f;

        c->offset.x = GetScreenWidth() / 2.0f;
        c->offset.y = GetScreenHeight() / 2.0f;

        c->target.x = roundf( targetCamX );
        c->target.y = 283.0f / 2.0f;

        int minX = (int)( (GetScreenWidth() / c->zoom) / 2 );
        int maxX = calcularLarguraMapa( gw->mapa ) - (int)( (GetScreenWidth() / c->zoom) / 2 );

        if ( c->target.x < minX ) {
            c->target.x = minX;
        } else if ( c->target.x > maxX ) {
            c->target.x = maxX;
        }
    } else {
        // Fase 2 (Boss Fight): Zoom para a altura da imagem IFSP (250px)
        float alturaFase = (float)rm.ifsp_highschool.height;
        c->zoom = GetScreenHeight() / alturaFase;

        c->offset.x = GetScreenWidth() / 2.0f;
        c->offset.y = GetScreenHeight() / 2.0f;

        c->target.x = roundf( targetCamX );
        c->target.y = alturaFase / 2.0f;

        float larguraFase = (float)rm.ifsp_highschool.width;
        float minX = (GetScreenWidth() / c->zoom) / 2.0f;
        float maxX = larguraFase - (GetScreenWidth() / c->zoom) / 2.0f;

        if ( minX > maxX ) {
            // Mapa menor que a tela: centraliza
            c->target.x = larguraFase / 2.0f;
        } else {
            if ( c->target.x < minX ) {
                c->target.x = minX;
            } else if ( c->target.x > maxX ) {
                c->target.x = maxX;
            }
        }
    }
}

// --- Inicialização e Reinício ---

static void inicializar( GameWorld *gw ) {

    // Limpa estado anterior se existir
    if ( gw->mapa != NULL ) {
        destruirMapa( gw->mapa );
        gw->mapa = NULL;
    }
    if ( gw->jogador != NULL ) {
        destruirJogador( gw->jogador );
        gw->jogador = NULL;
    }
    if ( gw->belial != NULL ) {
        destruirBelial( gw->belial );
        gw->belial = NULL;
    }
    if ( gw->earDog != NULL ) {
        destruirEarDog( gw->earDog );
        gw->earDog = NULL;
    }
    
    // Limpa todos os inimigos / projeteis antigos para um Full Scene Reset
    for ( int i = 0; i < MAX_WOLVES; i++ ) {
        if ( gw->wolves[i] != NULL ) {
            destruirWolf( gw->wolves[i] );
            gw->wolves[i] = NULL;
        }
    }
    gw->numWolves = 0;
    
    for ( int i = 0; i < MAX_ICE_SHARDS; i++ ) {
        if ( gw->iceShards[i] != NULL ) {
            destruirIceShard( gw->iceShards[i] );
            gw->iceShards[i] = NULL;
        }
    }
    gw->numIceShards = 0;
    
    for ( int i = 0; i < MAX_PROJETEIS; i++ ) {
        gw->projeteis[i].ativo = false;
    }

    if ( gw->faseAtual == 0 ) {
        // Fase 1: Frozen Suburbs
        gw->mapa = carregarMapa( "resources/mapas/mapa_modelo.txt" );

        float spawnX = 150.0f;
        float spawnY = 220.0f - 30.0f;
        gw->jogador = criarJogador( spawnX, spawnY, 20, 30 );
        if ( gw->modo2Jogadores ) {
            gw->belial = criarBelial( spawnX - 40.0f, spawnY, 20, 30 );
        }

        gw->camera = (Camera2D) {
            .offset = { 0 },
            .target = { 0 },
            .rotation = 0.0f,
            .zoom = GetScreenHeight() / 283.0f
        };
        
        // Spawn Wolves espalhados (Aumentada a densidade)
        gw->wolves[0] = criarWolf( 800.0f, 150.0f, 30, 20 );
        gw->wolves[1] = criarWolf( 1500.0f, 150.0f, 30, 20 );
        gw->wolves[2] = criarWolf( 2300.0f, 150.0f, 30, 20 );
        gw->wolves[3] = criarWolf( 3200.0f, 150.0f, 30, 20 );
        gw->wolves[4] = criarWolf( 4100.0f, 150.0f, 30, 20 );
        gw->wolves[5] = criarWolf( 5000.0f, 150.0f, 30, 20 );
        gw->wolves[6] = criarWolf( 6100.0f, 150.0f, 30, 20 );
        gw->wolves[7] = criarWolf( 7200.0f, 150.0f, 30, 20 );
        gw->wolves[8] = criarWolf( 8100.0f, 150.0f, 30, 20 );
        gw->numWolves = 9;
        
        // Spawn Ice Shards (Aumentada a densidade, e altura reduzida para serem alcançados)
        gw->iceShards[0] = criarIceShard( 1100.0f, 130.0f );
        gw->iceShards[1] = criarIceShard( 1900.0f, 130.0f );
        gw->iceShards[2] = criarIceShard( 2800.0f, 130.0f );
        gw->iceShards[3] = criarIceShard( 3700.0f, 130.0f );
        gw->iceShards[4] = criarIceShard( 4600.0f, 130.0f );
        gw->iceShards[5] = criarIceShard( 5600.0f, 130.0f );
        gw->iceShards[6] = criarIceShard( 6700.0f, 130.0f );
        gw->iceShards[7] = criarIceShard( 7700.0f, 130.0f );
        gw->numIceShards = 8;
        
    } else {
        // Fase 2: IFSP High School (Boss Fight) - mapa menor
        gw->mapa = carregarMapa( "resources/mapas/mapa_modelo.txt" );

        // Ajusta o mapa para as dimensões da imagem IFSP
        gw->mapa->linhas = rm.ifsp_highschool.height;   // 250
        gw->mapa->colunas = rm.ifsp_highschool.width;   // 909

        float alturaFase = (float)rm.ifsp_highschool.height;
        float spawnX = 100.0f;
        float spawnY = 220.0f; // meio do chão - altura do jogador
        gw->jogador = criarJogador( spawnX, spawnY, 20, 30 );
        if ( gw->modo2Jogadores ) {
            gw->belial = criarBelial( spawnX - 40.0f, spawnY, 20, 30 );
        }

        // Spawn EarDog na Fase 2: mesmo Y que o jogador (mesma profundidade)
        float earDogSpawnY = 220.0f; // alinhado ao jogador
        gw->earDog = criarEarDog( 600.0f, earDogSpawnY, 44, 27 );

        gw->camera = (Camera2D) {
            .offset = { 0 },
            .target = { 0 },
            .rotation = 0.0f,
            .zoom = GetScreenHeight() / alturaFase
        };
    }

    gw->gravidade = 900;
    gw->tempoDeJogo = 0.0f;
}

static void reiniciar( GameWorld *gw ) {
    inicializar( gw );
}

void ResetBossScene( GameWorld *gw ) {
    // Zera completamente o estado da fase e recria entidades
    inicializar( gw );
}

/**
 * @brief Calcula a hitbox da mão do urso durante o frame de soco ativo (socandoFrame == 2).
 *        A hitbox é proporcional ao sprite (punch_frames[2] = { 192, 195, 64, 69 }).
 *        A mão ocupa os últimos ~20px horizontais do frame (lado direito).
 */
static Rectangle obterHitboxSocoPolarBear( Jogador *j ) {
    // Frame de soco ativo: punch_frames[2] = { 192, 195, 64, 69 }
    // O sprite desenhado tem escala dependente de feet_y
    float feet_y = j->ret.y + j->ret.height;
    float t = ( feet_y - 220.0f ) / 103.0f;
    float scale = 1.0f + t * 0.25f;

    if ( j->socoAereo || j->socoAereoAterrissou ) {
        // --- HITBOX DO SOCO AÉREO ---
        // O soco aéreo agora é uma hitbox de corpo inteiro expandida.
        // Basicamente tudo que encostar no urso na ascendente e descendente toma dano.
        Rectangle hitbox = j->ret;
        
        // Expande levemente para facilitar acertar alvos menores ou em planos diferentes
        hitbox.x -= 15.0f;
        hitbox.width += 30.0f;
        hitbox.y -= 10.0f;
        hitbox.height += 20.0f;
        
        return hitbox;

    } else {
        // --- HITBOX DO SOCO TERRESTRE (Normal) ---
        float frameW = 64.0f * scale;
        float frameH = 69.0f * scale;
        float drawX = ( j->ret.x + j->ret.width / 2.0f ) - frameW / 2.0f;
        float drawY = feet_y - frameH;

        float handW = ( 20.0f / 64.0f ) * frameW;
        float handH = ( 22.0f / 69.0f ) * frameH;
        float handY = drawY + ( 35.0f / 69.0f ) * frameH;

        if ( j->olhandoParaDireita ) {
            return (Rectangle){ drawX + frameW - handW, handY, handW, handH };
        } else {
            return (Rectangle){ drawX, handY, handW, handH };
        }
    }
}

/**
 * @brief Obtém a hitbox visual do corpo do jogador para receber dano 2.5D
 */
static Rectangle obterHitboxVisualJogador( Jogador *j ) {
    float feet_y = j->ret.y + j->ret.height;
    float t = ( feet_y - 220.0f ) / 103.0f;
    float scale = 1.0f + t * 0.25f;
    
    float w = 60.0f * scale; 
    float h = 65.0f * scale; 
    float drawX = ( j->ret.x + j->ret.width / 2.0f ) - w / 2.0f;
    float drawY = ( feet_y + j->puloY * scale ) - h;
    
    return (Rectangle){ drawX, drawY, w, h };
}

/**
 * @brief Resolve todas as colisões de combate da Fase 2:
 *   1. Hitbox da mão do urso (socandoFrame == 2) vs hitbox do EarDog → dano no EarDog
 *   2. Contato corpo-a-corpo sem ataque → dano no PolarBear
 */
static void resolverColisoesFase2( GameWorld *gw ) {
    Jogador *j = gw->jogador;
    Belial *b = gw->belial;
    EarDog *ed = gw->earDog;
    if ( j == NULL || ed == NULL ) return;
    if ( ed->estado == ESTADO_EARDOG_MORRENDO ) return;
    if ( ed == NULL ) return;

    Rectangle hitboxEarDog = earDogObterHitbox( ed );

    // ── EarDog ataca jogador (dash ou salto) ──
    Rectangle ataqueEarDog = earDogObterHitboxAtaque( ed );
    bool hitboxAtivaEarDog = ( ataqueEarDog.width > 0.0f );
    if ( hitboxAtivaEarDog && !ed->hasHitPlayer ) {
        Rectangle visualJ = obterHitboxVisualJogador( j );
        if ( CheckCollisionRecs( ataqueEarDog, visualJ ) ) {
            float depthE = ed->ret.y + ed->ret.height;
            float depthJ = j->ret.y + j->ret.height;
            if ( fabsf(depthE - depthJ) < 70.0f ) {
                if ( j->invencibilidade <= 0.0f ) {
                    j->quantidadeVidas -= 0.5f;
                    if ( j->quantidadeVidas < 0.0f ) j->quantidadeVidas = 0.0f;
                    j->invencibilidade = 1.5f;
                }
                ed->hasHitPlayer = true;
            }
        }
    }

    // ── Soco do Urso Polar acerta EarDog ──
    bool socoAtivoFrame = ( j->socando && j->socandoFrame == 2 ) ||
                          ( j->socoAereo && !j->socoAereoAterrissou );
    if ( socoAtivoFrame ) {
        Rectangle hitboxMao = obterHitboxSocoPolarBear( j );
        if ( CheckCollisionRecs( hitboxMao, hitboxEarDog ) ) {
            earDogReceberDano( ed );
        }
    }

    // ── Belial acerta EarDog ──
    if ( gw->modo2Jogadores && b != NULL && b->ativo ) {
        bool socoAtivoFrameB = ( b->socando && b->socandoFrame == 2 ) ||
                               ( b->socoAereo && !b->socoAereoAterrissou );
        if ( socoAtivoFrameB ) {
            Rectangle hitboxMaoB = obterHitboxSocoPolarBear( (Jogador*)b );
            if ( CheckCollisionRecs( hitboxMaoB, hitboxEarDog ) ) {
                earDogReceberDano( ed );
            }
        }
    }
}

static void trocarFase( GameWorld *gw, int novaFase ) {
    gw->faseAtual = novaFase;
    inicializar( gw );
}
