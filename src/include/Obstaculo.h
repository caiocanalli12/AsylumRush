/**
 * @file Obstaculo.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Obstáculo (Template Cru).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "raylib/raylib.h"
#include "Tipos.h"

/**
 * @brief Cria um novo obstáculo.
 */
Obstaculo *criarObstaculo( Rectangle ret, Color cor );

/**
 * @brief Destroi um obstáculo.
 */
void destruirObstaculo( Obstaculo *o );

/**
 * @brief Desenha um obstáculo.
 */
void desenharObstaculo( Obstaculo *o );