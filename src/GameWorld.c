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
    gw->tempoDeJogo += delta;
    atualizarMapa( gw->mapa, gw, delta );
    entradaJogador( j, gw, delta );
    atualizarJogador( j, gw, delta );
    
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
        // Wolf ataques e hit no jogador
        for ( int i = 0; i < gw->numWolves; i++ ) {
            Wolf *w = gw->wolves[i];
            if ( w != NULL && w->ativo && w->estado != ESTADO_WOLF_MORRENDO ) {
                // Soco do jogador acerta Wolf
                bool jogadorSocandoFrameDano = ( j->socando && j->socandoFrame == 2 ) || 
                                               ( j->socoAereo && !j->socoAereoAterrissou );
                if ( jogadorSocandoFrameDano ) {
                    Rectangle hitboxMao = obterHitboxSocoPolarBear( j );
                    Rectangle corpoWolf = wolfObterHitboxCorpo( w );
                    if ( CheckCollisionRecs( hitboxMao, corpoWolf ) ) {
                        wolfReceberDano( w );
                    }
                }
                // Wolf ataca jogador (apenas dano de ataque)
                if ( w->estado == ESTADO_WOLF_ATACANDO && !w->hasHitPlayer ) {
                    Rectangle ataqueWolf = wolfObterHitboxAtaque( w );
                    Rectangle visualJ = obterHitboxVisualJogador( j );
                    
                    // Colisão visual 2D
                    if ( CheckCollisionRecs( ataqueWolf, visualJ ) ) {
                        // Verificação de profundidade 2.5D (z-axis)
                        float depthW = w->ret.y + w->ret.height;
                        float depthJ = j->ret.y + j->ret.height;
                        if ( fabsf(depthW - depthJ) < 60.0f ) {
                            if ( j->invencibilidade <= 0.0f ) {
                                j->quantidadeVidas--;
                                j->invencibilidade = 1.5f;
                                w->hasHitPlayer = true;
                            }
                        }
                    }
                }
            }
        }
        // Soco (normal ou aéreo) do jogador acerta Ice Shard
        for ( int i = 0; i < gw->numIceShards; i++ ) {
            IceShard *is = gw->iceShards[i];
            if ( is != NULL && is->ativo && is->estado != ESTADO_ICESHARD_MORRENDO ) {
                bool socoDano = ( j->socando && j->socandoFrame == 2 ) || 
                                ( j->socoAereo && !j->socoAereoAterrissou );
                if ( socoDano ) {
                    Rectangle hitboxMao = obterHitboxSocoPolarBear( j );
                    // Hitbox do Ice Shard
                    float isW = is->ret.width * 0.8f;
                    float isH = is->ret.height * 0.8f;
                    float isX = is->ret.x + (is->ret.width - isW) / 2.0f;
                    float isY = is->ret.y + (is->ret.height - isH) / 2.0f;
                    Rectangle hitboxIS = { isX, isY, isW, isH };
                    if ( CheckCollisionRecs( hitboxMao, hitboxIS ) ) {
                        iceShardReceberDano( is, j->ret.x );
                    }
                }
            }
        }
        // Projéteis acertam jogador
        for ( int i = 0; i < MAX_PROJETEIS; i++ ) {
            if ( gw->projeteis[i].ativo ) {
                Rectangle projRet = { gw->projeteis[i].pos.x - 10, gw->projeteis[i].pos.y - 10, 20, 20 };
                if ( CheckCollisionRecs( projRet, j->ret ) ) {
                    gw->projeteis[i].ativo = false;
                    if ( j->invencibilidade <= 0.0f ) {
                        j->quantidadeVidas--;
                        j->invencibilidade = 1.5f;
                    }
                }
            }
        }
        
        // --- Gerenciador de Vidas (Life Manager) - Apenas Fase 1 ---
        if ( j->quantidadeVidas <= 0 ) {
            // Reset direto da fase sem tela de Game Over
            inicializar( gw );
            return; // Interrompe o update atual, pois tudo foi recriado
        }

    }
    
    if ( gw->faseAtual == 1 && gw->earDog != NULL ) {
        atualizarEarDog( gw->earDog, gw, delta );
        resolverColisoesFase2( gw );
    }
    atualizarCamera( gw );
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
        if ( noMezanino ) {
            DrawTexture( rm.mezzanine_railing, 8300, 130, WHITE );
        }
    } else {
        // Fase 2: sem profundidade — EarDog atras, PolarBear sempre na frente
        if ( gw->earDog != NULL ) {
            desenharEarDog( gw->earDog );
        }
        desenharJogador( gw->jogador );
    }

    EndMode2D();

    // --- UI sobreposta (coordenadas de tela) ---

    // Canvas de HUD (Apenas na Fase 1 - Frozen Suburbs)
    if ( gw->faseAtual == 0 ) {
        // Desenha Ícone do Urso (Rosto)
        Texture2D iconTex = rm.polarbear;
        Rectangle iconSrc = { 216, 396, 23, 20 }; // ln4col4 face frame
        Rectangle iconDest = { 20, 20, 23 * 3.0f, 20 * 3.0f }; // Scale by 3x for UI
        DrawTexturePro( iconTex, iconSrc, iconDest, (Vector2){0,0}, 0.0f, WHITE );
        
        // Texto dinâmico de Vidas
        char livesTxt[16];
        sprintf( livesTxt, "X %d", gw->jogador->quantidadeVidas );
        DrawText( livesTxt, 110, 35, 30, BLACK ); // Sombra
        DrawText( livesTxt, 108, 33, 30, WHITE ); // Texto principal
    }

    // Botão de alternar fase (canto superior direito)
    char faseTxt[32];
    sprintf( faseTxt, "Fase %d", gw->faseAtual + 1 );
    float btnFaseW = 120.0f;
    float btnFaseH = 36.0f;
    Rectangle btnFase = {
        GetScreenWidth() - btnFaseW - 16.0f,
        16.0f,
        btnFaseW,
        btnFaseH
    };

    int novaFase = ( gw->faseAtual + 1 ) % 2;
    if ( desenharBotao( btnFase, faseTxt, 18,
            (Color){ 40, 40, 60, 200 },
            (Color){ 70, 70, 100, 230 },
            WHITE ) ) {
        trocarFase( gw, novaFase );
    }

    // Instrução de pause
    DrawText( "P - Pausar", 16, 16, 16, (Color){ 255, 255, 255, 150 } );

    // --- Mini HUD de Vidas (apenas Fase 2) ---
    if ( gw->faseAtual == 1 && gw->jogador != NULL ) {
        int screenW = GetScreenWidth();
        int hudFontSize = 22;
        int hudPad = 10;

        // Fundo semi-transparente do HUD
        Rectangle hudBg = { 0, 0, (float)screenW, 44.0f };
        DrawRectangleRec( hudBg, (Color){ 0, 0, 0, 130 } );

        // Vidas do PolarBear (esquerda)
        char polarbearHud[64];
        int pbVidas = gw->jogador->quantidadeVidas;
        if ( pbVidas < 0 ) pbVidas = 0;
        sprintf( polarbearHud, "Urso: %d", pbVidas );
        DrawText( polarbearHud, hudPad, hudPad, hudFontSize, (Color){ 180, 220, 255, 255 } );

        // Ícones de vida (corações) do PolarBear
        for ( int i = 0; i < 3; i++ ) {
            Color heartColor = ( i < pbVidas ) ? (Color){ 255, 80, 80, 255 } : (Color){ 80, 80, 80, 160 };
            int hx = hudPad + MeasureText( polarbearHud, hudFontSize ) + 8 + i * 22;
            int hy = hudPad + hudFontSize / 2 - 8;
            DrawRectangle( hx, hy, 14, 14, heartColor );
            DrawRectangleLines( hx, hy, 14, 14, (Color){ 255, 255, 255, 80 } );
        }

        // Vidas do EarDog (direita)
        if ( gw->earDog != NULL ) {
            char earDogHud[64];
            int edVidas = gw->earDog->quantidadeVidas;
            if ( edVidas < 0 ) edVidas = 0;
            sprintf( earDogHud, "EarDog: %d", edVidas );
            int edTxtW = MeasureText( earDogHud, hudFontSize );
            int iconsW = 3 * 22;
            int edTotalW = iconsW + 8 + edTxtW;
            int edX = screenW - edTotalW - hudPad;
            DrawText( earDogHud, edX, hudPad, hudFontSize, (Color){ 255, 200, 100, 255 } );

            for ( int i = 0; i < 3; i++ ) {
                Color heartColor = ( i < edVidas ) ? (Color){ 255, 80, 80, 255 } : (Color){ 80, 80, 80, 160 };
                int hx = edX + edTxtW + 8 + i * 22;
                int hy = hudPad + hudFontSize / 2 - 8;
                DrawRectangle( hx, hy, 14, 14, heartColor );
                DrawRectangleLines( hx, hy, 14, 14, (Color){ 255, 255, 255, 80 } );
            }
        }
    }
}

static void drawPause( GameWorld *gw ) {

    // Desenha o jogo congelado por trás
    drawJogo( gw );

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Overlay escuro semi-transparente
    DrawRectangle( 0, 0, screenW, screenH, (Color){ 0, 0, 0, 160 } );

    // Painel centralizado
    float painelW = 360.0f;
    float painelH = 220.0f;
    Rectangle painel = {
        ( screenW - painelW ) / 2.0f,
        ( screenH - painelH ) / 2.0f,
        painelW,
        painelH
    };

    // Fundo do painel
    DrawRectangleRounded( painel, 0.15f, 8, (Color){ 20, 20, 40, 230 } );
    DrawRectangleRoundedLines( painel, 0.15f, 8, (Color){ 100, 150, 255, 150 } );

    // Texto "PAUSADO"
    const char *pauseTxt = "PAUSADO";
    int pauseFontSize = 48;
    int pauseW = MeasureText( pauseTxt, pauseFontSize );
    DrawText( pauseTxt,
        (int)( painel.x + ( painelW - pauseW ) / 2.0f ),
        (int)( painel.y + 30.0f ),
        pauseFontSize,
        (Color){ 100, 180, 255, 255 }
    );

    // Botão CONTINUAR
    float btnW = 200.0f;
    float btnH = 44.0f;
    float btnX = painel.x + ( painelW - btnW ) / 2.0f;

    Rectangle btnContinuar = { btnX, painel.y + 100.0f, btnW, btnH };
    if ( desenharBotao( btnContinuar, "CONTINUAR", 22,
            (Color){ 30, 120, 80, 255 },
            (Color){ 50, 180, 120, 255 },
            WHITE ) ) {
        gw->estadoTela = TELA_JOGO;
    }

    // Botão MENU
    Rectangle btnMenu = { btnX, painel.y + 155.0f, btnW, btnH };
    if ( desenharBotao( btnMenu, "MENU", 22,
            (Color){ 150, 40, 40, 255 },
            (Color){ 200, 60, 60, 255 },
            WHITE ) ) {
        // Destrói o estado atual e volta ao menu
        if ( gw->mapa != NULL ) {
            destruirMapa( gw->mapa );
            gw->mapa = NULL;
        }
        if ( gw->jogador != NULL ) {
            destruirJogador( gw->jogador );
            gw->jogador = NULL;
        }
        gw->estadoTela = TELA_MENU;
    }
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
    Camera2D *c = &gw->camera;

    if ( gw->faseAtual == 0 ) {
        // Fase 1: Zoom dinâmico para 283px de altura
        c->zoom = GetScreenHeight() / 283.0f;

        c->offset.x = GetScreenWidth() / 2.0f;
        c->offset.y = GetScreenHeight() / 2.0f;

        c->target.x = roundf( j->ret.x + j->ret.width / 2.0f );
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

        c->target.x = roundf( j->ret.x + j->ret.width / 2.0f );
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
    if ( gw->earDog != NULL ) {
        destruirEarDog( gw->earDog );
        gw->earDog = NULL;
    }

    if ( gw->faseAtual == 0 ) {
        // Fase 1: Frozen Suburbs
        gw->mapa = carregarMapa( "resources/mapas/mapa_modelo.txt" );

        float spawnX = 150.0f;
        float spawnY = 220.0f - 30.0f;
        gw->jogador = criarJogador( spawnX, spawnY, 20, 30 );

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
        float spawnY = alturaFase - 63.0f - 30.0f; // chão - altura do jogador
        gw->jogador = criarJogador( spawnX, spawnY, 20, 30 );

        // Spawn EarDog na Fase 2: mesmo Y que o jogador (mesma profundidade)
        // spawnY coloca os pes do jogador em (alturaFase - 63.0f), portanto ret.y = spawnY
        float earDogSpawnY = spawnY; // alinhado ao jogador
        gw->earDog = criarEarDog( 550.0f, earDogSpawnY, 30, 20 );

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
    EarDog *ed = gw->earDog;
    if ( j == NULL || ed == NULL ) return;

    Rectangle retJogador = j->ret;
    Rectangle hitboxEarDog = earDogObterHitbox( ed );

    bool socoAtivoFrame = ( j->socando && j->socandoFrame == 2 );

    if ( socoAtivoFrame ) {
        // --- Colisão da MÃO do urso com o EarDog ---
        Rectangle hitboxMao = obterHitboxSocoPolarBear( j );
        if ( CheckCollisionRecs( hitboxMao, hitboxEarDog ) ) {
            earDogReceberDano( ed );
        }
    } else {
        // --- Contato corpo-a-corpo sem ataque: o urso sofre dano ---
        if ( CheckCollisionRecs( retJogador, hitboxEarDog ) ) {
            if ( j->invencibilidade <= 0.0f ) {
                j->quantidadeVidas--;
                if ( j->quantidadeVidas < 0 ) j->quantidadeVidas = 0;
                j->invencibilidade = 1.5f;
            }
        }
    }
}

static void trocarFase( GameWorld *gw, int novaFase ) {
    gw->faseAtual = novaFase;
    inicializar( gw );
}
