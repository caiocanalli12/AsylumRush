/**
 * @file Mapa.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Mapa (Template Cru).
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Macros.h"
#include "Mapa.h"
#include "Obstaculo.h"
#include "Tipos.h"
#include "ResourceManager.h"

static void inserirObstaculo( Mapa *mapa, ElementoMapa *obstaculo );

/**
 * @brief Carrega um mapa a partir de um arquivo.
 */
Mapa *carregarMapa( const char *caminhoArquivo ) {

    Mapa *novoMapa = (Mapa*) malloc( sizeof( Mapa ) );

    novoMapa->obstaculos = NULL;
    novoMapa->quantidadeObstaculos = 0;

    novoMapa->dimensaoPadraoElementos = 1.0f;
    novoMapa->linhas = 283;
    novoMapa->colunas = 10603;

    // Segmento 1 do chao: 0 ate 1451
    ElementoMapa *el1 = (ElementoMapa*) malloc( sizeof( ElementoMapa ) );
    el1->proximo = NULL;
    el1->objeto = criarObstaculo( 
        (Rectangle) { 
            .x = 0, 
            .y = 220, 
            .width = 1451, 
            .height = 63
        },
        BLANK
    );
    inserirObstaculo( novoMapa, el1 );

    // Segmento 2 do chao: 1642 ate 9343
    ElementoMapa *el2 = (ElementoMapa*) malloc( sizeof( ElementoMapa ) );
    el2->proximo = NULL;
    el2->objeto = criarObstaculo( 
        (Rectangle) { 
            .x = 1642, 
            .y = 220, 
            .width = 7701, 
            .height = 63
        },
        BLANK
    );
    inserirObstaculo( novoMapa, el2 );

    // Segmento 3 do chao: 9415 ate 10603
    ElementoMapa *el3 = (ElementoMapa*) malloc( sizeof( ElementoMapa ) );
    el3->proximo = NULL;
    el3->objeto = criarObstaculo( 
        (Rectangle) { 
            .x = 9415, 
            .y = 220, 
            .width = 1188, 
            .height = 63
        },
        BLANK
    );
    inserirObstaculo( novoMapa, el3 );

    return novoMapa;

}

/**
 * @brief Destrói um mapa.
 */
void destruirMapa( Mapa *m ) {

    if ( m != NULL ) {
        ElementoMapa *el = m->obstaculos;
        while ( el != NULL ) {
            destruirObstaculo( (Obstaculo*) el->objeto );
            ElementoMapa *t = el;
            el = el->proximo;
            free( t );
        }
        free( m );
    }

}

/**
 * @brief Atualiza um mapa.
 */
void atualizarMapa( Mapa *m, GameWorld *gw, float delta ) {
    // Sem elementos dinâmicos no mapa padrão por enquanto
}

/**
 * @brief Desenha um mapa.
 */
void desenharMapa( Mapa *m ) {

    ElementoMapa *el = m->obstaculos;
    while ( el != NULL ) {
        desenharObstaculo( (Obstaculo*) el->objeto );
        el = el->proximo;
    }

}

/**
 * @brief Calcula a largura do mapa.
 */
int calcularLarguraMapa( Mapa *m ) {
    return (int) ( m->dimensaoPadraoElementos * m->colunas );
}

/**
 * @brief Calcula a altura do mapa.
 */
int calcularAlturaMapa( Mapa *m ) {
    return (int) ( m->dimensaoPadraoElementos * m->linhas );
}

/**
 * @brief Insere um obstáculo na lista de obstáculos.
 */
static void inserirObstaculo( Mapa *mapa, ElementoMapa *obstaculo ) {
    if ( mapa->obstaculos == NULL ) {
        mapa->obstaculos = obstaculo;
    } else {
        obstaculo->proximo = mapa->obstaculos;
        mapa->obstaculos = obstaculo;
    }
    mapa->quantidadeObstaculos++;
}