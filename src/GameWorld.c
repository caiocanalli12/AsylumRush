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
    return gw;
}

/**
 * @brief Destrói um objeto GameWorld e suas dependências.
 */
void destroyGameWorld( GameWorld *gw ) {
    if ( gw != NULL ) {
        if ( gw->mapa != NULL ) destruirMapa( gw->mapa );
        if ( gw->jogador != NULL ) destruirJogador( gw->jogador );
        free( gw );
    }
}

// --- Update ---

static void updateMenu( GameWorld *gw ) {
    // A lógica de clique do botão é tratada no draw (para simplificar)
    // Nada a atualizar aqui
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

    // Fundo com gradiente escuro
    DrawRectangleGradientV( 0, 0, screenW, screenH,
        (Color){ 10, 10, 30, 255 },
        (Color){ 20, 40, 80, 255 }
    );

    // Partículas decorativas (estrelas estáticas)
    for ( int i = 0; i < 60; i++ ) {
        int sx = ( i * 137 + 53 ) % screenW;
        int sy = ( i * 211 + 97 ) % screenH;
        int tamanho = ( i % 3 ) + 1;
        unsigned char alpha = (unsigned char)( 80 + ( i * 37 ) % 176 );
        DrawCircle( sx, sy, (float)tamanho, (Color){ 255, 255, 255, alpha } );
    }

    // Título do jogo
    const char *titulo = "ASYLUM RUSH";
    int tituloFontSize = 72;
    int tituloW = MeasureText( titulo, tituloFontSize );
    int tituloX = ( screenW - tituloW ) / 2;
    int tituloY = screenH / 4;

    // Sombra do título
    DrawText( titulo, tituloX + 3, tituloY + 3, tituloFontSize, (Color){ 0, 0, 0, 150 } );
    // Título principal com cor vibrante
    DrawText( titulo, tituloX, tituloY, tituloFontSize, (Color){ 100, 180, 255, 255 } );

    // Subtítulo
    const char *subtitulo = "Plataforma de Acao";
    int subFontSize = 24;
    int subW = MeasureText( subtitulo, subFontSize );
    DrawText( subtitulo, ( screenW - subW ) / 2, tituloY + tituloFontSize + 10, subFontSize, (Color){ 180, 200, 230, 200 } );

    // Botão JOGAR
    float btnW = 260.0f;
    float btnH = 64.0f;
    Rectangle btnJogar = {
        ( screenW - btnW ) / 2.0f,
        screenH / 2.0f + 40.0f,
        btnW,
        btnH
    };

    if ( desenharBotao( btnJogar, "JOGAR", 32,
            (Color){ 30, 100, 200, 255 },
            (Color){ 50, 140, 255, 255 },
            WHITE ) ) {
        // Iniciar o jogo
        inicializar( gw );
        gw->estadoTela = TELA_JOGO;
    }

    // Instrução na parte inferior
    const char *instrucao = "Pressione JOGAR para comecar!";
    int instrFontSize = 16;
    int instrW = MeasureText( instrucao, instrFontSize );
    DrawText( instrucao, ( screenW - instrW ) / 2, screenH - 60, instrFontSize, (Color){ 150, 150, 180, 180 } );
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
        desenharJogador( gw->jogador );
        if ( noMezanino ) {
            DrawTexture( rm.mezzanine_railing, 8300, 130, WHITE );
        }
    } else {
        desenharJogador( gw->jogador );
    }

    EndMode2D();

    // --- UI sobreposta (coordenadas de tela) ---

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

static void trocarFase( GameWorld *gw, int novaFase ) {
    gw->faseAtual = novaFase;
    inicializar( gw );
}
