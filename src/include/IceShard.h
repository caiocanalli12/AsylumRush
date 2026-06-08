/**
 * @file IceShard.h
 * @author thale
 * @brief Declarações das funções do inimigo Ice Shard.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria uma instância alocada dinamicamente da struct IceShard.
 */
IceShard *criarIceShard( float x, float y );

/**
 * @brief Destrói um objeto IceShard e libera seus recursos.
 */
void destruirIceShard( IceShard *is );

/**
 * @brief Atualiza a animação e lógica de ataque do Ice Shard.
 */
void atualizarIceShard( IceShard *is, GameWorld *gw, float delta );

/**
 * @brief Desenha o Ice Shard na tela.
 */
void desenharIceShard( IceShard *is );

/**
 * @brief Aplica dano ao Ice Shard.
 */
void iceShardReceberDano( IceShard *is, float playerX );
