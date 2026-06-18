/**
 * @file Belial.h
 * @author thales
 * @brief Declarações das funções do Jogador 2 (Belial).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria uma instância alocada dinamicamente da struct Belial.
 */
Belial *criarBelial( float x, float y, float w, float h );

/**
 * @brief Destrói um objeto Belial e libera seus recursos.
 */
void destruirBelial( Belial *b );

/**
 * @brief Lê a entrada do usuário e atualiza as velocidades do Belial.
 */
void entradaBelial( Belial *b, GameWorld *gw, float delta );

/**
 * @brief Aplica física e resolve colisões do Belial com o mundo.
 */
void atualizarBelial( Belial *b, GameWorld *gw, float delta );

/**
 * @brief Desenha o Belial.
 */
void desenharBelial( Belial *b );
