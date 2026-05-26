/**
 * @file Obstaculo.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Obstáculo (Template Cru).
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Obstaculo.h"
#include "Tipos.h"

/**
 * @brief Cria um novo obstáculo.
 */
Obstaculo *criarObstaculo( Rectangle ret, Color cor ) {

    Obstaculo *novoObstaculo = (Obstaculo*) malloc( sizeof( Obstaculo ) );

    novoObstaculo->ret = ret;
    novoObstaculo->cor = cor;

    return novoObstaculo;

}

/**
 * @brief Destroi um obstáculo.
 */
void destruirObstaculo( Obstaculo *o ) {
    free( o );
}

/**
 * @brief Desenha um obstáculo.
 */
void desenharObstaculo( Obstaculo *o ) {
    if ( o->cor.a > 0 ) {
        DrawRectangleRec( o->ret, o->cor );
        DrawRectangleLines( o->ret.x, o->ret.y, o->ret.width, o->ret.height, BLACK );
    }
}