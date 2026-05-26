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
 * @brief Representa o estado do jogador.
 */
typedef enum EstadoJogador {
    ESTADO_JOGADOR_PARADO,
    ESTADO_JOGADOR_ANDANDO,
    ESTADO_JOGADOR_PULANDO,
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
 * @brief Representa o mundo do jogo e seus elementos.
 */
typedef struct GameWorld {

    Mapa *mapa;
    Jogador *jogador;

    Camera2D camera;

    float gravidade;
    float tempoDeJogo;

} GameWorld;