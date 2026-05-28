/**
 * @file Tipos.h
 * @author Prof. Dr. David Buzatto
 * @brief Definição dos tipos utilizados no jogo (Template Cru).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include <stdbool.h>
#include "raylib/raylib.h"

/**
 * @brief Representa a tela/estado atual do jogo.
 */
typedef enum EstadoTela {
    TELA_MENU,
    TELA_JOGO,
    TELA_PAUSE,
} EstadoTela;

/**
 * @brief Representa o estado do jogador.
 */
typedef enum EstadoJogador {
    ESTADO_JOGADOR_PARADO,
    ESTADO_JOGADOR_ANDANDO,
    ESTADO_JOGADOR_PULANDO,
    ESTADO_JOGADOR_SOCANDO,
} EstadoJogador;

/**
 * @brief Representa o jogador controlado pelo usuário.
 */
typedef struct Jogador {

    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velPulo;
    float velMaxQueda;

    float aceleracao;
    float desaceleracao;

    int quantidadePulos;
    int quantidadeMaxPulos;

    int quantidadeVidas;
    int score;

    EstadoJogador estado;
    bool olhandoParaDireita;

    float animTimer;
    int animFrame;

    float puloY;
    float puloVel;
    bool noPulo;
    bool noMezanino;

    // Soco simples (chao)
    bool socando;          // true enquanto a animacao de soco esta ativa
    int socandoFrame;      // frame atual da sequencia (0-3)
    float socandoTimer;    // tempo acumulado no frame atual
    float socandoCooldown; // tempo restante de cooldown no frame final

    // Soco aereo (no ar)
    bool socoAereo;           // true: soco aereo ativo (F pressionado no ar)
    bool socoAereoAterrissou; // true: ja pousou, exibindo frame de pouso
    float socoAereoCooldown;  // tempo de cooldown apos aterrissar (0.5s)

} Jogador;

/**
 * @brief Representa um obstáculo estático do mapa.
 */
typedef struct Obstaculo {
    Rectangle ret;
    Color cor;
} Obstaculo;

/**
 * @brief Representa um elemento do mapa.
 */
typedef struct ElementoMapa ElementoMapa;
struct ElementoMapa {
    void *objeto; // Aponta para Obstaculo
    ElementoMapa *proximo;
};

/**
 * @brief Representa um mapa de fase do jogo.
 */
typedef struct Mapa {

    // Lista ligada de obstáculos do mapa
    ElementoMapa *obstaculos;
    int quantidadeObstaculos;

    float dimensaoPadraoElementos;
    int linhas;
    int colunas;

} Mapa;

/**
 * @brief Representa o estado do EarDog.
 */
typedef enum EstadoEarDog {
    ESTADO_EARDOG_PARADO,
    ESTADO_EARDOG_ATACANDO,
} EstadoEarDog;

/**
 * @brief Representa o personagem EarDog.
 */
typedef struct EarDog {
    Rectangle ret;
    Vector2 vel;
    bool olhandoParaDireita;
    float animTimer;
    int animFrame;
    float movementTimer;
    float puloY;
    float puloVel;
    EstadoEarDog estado;
} EarDog;

/**
 * @brief Representa o mundo do jogo e seus elementos.
 */
typedef struct GameWorld {

    Mapa *mapa;
    Jogador *jogador;
    EarDog *earDog;

    Camera2D camera;

    float gravidade;
    float tempoDeJogo;

    EstadoTela estadoTela;
    int faseAtual; // 0 = frozensuburbs, 1 = ifsp_highschool (boss fight)

    float menuBgOffset;
    bool deveSair;

} GameWorld;