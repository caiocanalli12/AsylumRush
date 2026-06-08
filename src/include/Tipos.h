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

    // Dano corpo-a-corpo
    float invencibilidade;    // cooldown de invencibilidade apos tomar dano de contato (1.5s)

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
    ESTADO_EARDOG_TOMANDO_GOLPE,
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

    // Vidas e sistema de dano
    int quantidadeVidas;   // 3 vidas iniciais
    bool tomandoGolpe;     // true enquanto a anim de hit roda
    int hitFrame;          // frame atual da anim de hit (0-2)
    float hitTimer;        // tempo acumulado no frame atual
    float invencibilidade; // cooldown de invencibilidade apos tomar dano (evita multi-hit)
} EarDog;

/**
 * @brief Representa o estado do Wolf.
 */
typedef enum EstadoWolf {
    ESTADO_WOLF_PARADO,
    ESTADO_WOLF_ANDANDO,
    ESTADO_WOLF_ATACANDO,
    ESTADO_WOLF_TOMANDO_GOLPE,
    ESTADO_WOLF_MORRENDO
} EstadoWolf;

/**
 * @brief Representa o inimigo terrestre Wolf.
 */
typedef struct Wolf {
    Rectangle ret;
    Vector2 vel;
    bool olhandoParaDireita;
    float animTimer;
    int animFrame;
    
    float puloY;
    float puloVel;
    
    EstadoWolf estado;
    int quantidadeVidas;
    float invencibilidade;
    
    // Attack properties
    float attackCooldown;
    bool hasHitPlayer; 
    
    // Hit/Death properties
    float hitTimer;
    int hitFrame;
    bool ativo; 
} Wolf;

/**
 * @brief Representa o estado do Ice Shard.
 */
typedef enum EstadoIceShard {
    ESTADO_ICESHARD_FLUTUANDO,
    ESTADO_ICESHARD_ABRINDO,
    ESTADO_ICESHARD_ATACANDO,
    ESTADO_ICESHARD_MORRENDO
} EstadoIceShard;

/**
 * @brief Representa o inimigo aéreo Ice Shard.
 */
typedef struct IceShard {
    Rectangle ret;
    float base_x;
    float base_y;
    float floatTimer;
    float moveTimer;
    float targetX;
    float targetY;
    
    EstadoIceShard estado;
    int quantidadeVidas;
    float invencibilidade;
    
    float animTimer;
    int animFrame;
    
    // Attack properties
    float attackTimer; 
    int shotsFired;    
    float shotCooldown; 
    
    // Smooth Aim
    float currentAimAngle;
    
    bool ativo;
} IceShard;

/**
 * @brief Representa um projétil atirado.
 */
typedef struct Projetil {
    Vector2 pos;
    Vector2 vel;
    bool ativo;
    float lifeTimer;
    float animTimer;
    int animFrame;
} Projetil;

#define MAX_WOLVES 10
#define MAX_ICE_SHARDS 10
#define MAX_PROJETEIS 50

/**
 * @brief Representa o mundo do jogo e seus elementos.
 */
typedef struct GameWorld {

    Mapa *mapa;
    Jogador *jogador;
    EarDog *earDog;

    Wolf *wolves[MAX_WOLVES];
    int numWolves;

    IceShard *iceShards[MAX_ICE_SHARDS];
    int numIceShards;

    Projetil projeteis[MAX_PROJETEIS];

    Camera2D camera;

    float gravidade;
    float tempoDeJogo;

    EstadoTela estadoTela;
    int faseAtual; // 0 = frozensuburbs, 1 = ifsp_highschool (boss fight)

    float menuBgOffset;
    bool deveSair;

} GameWorld;