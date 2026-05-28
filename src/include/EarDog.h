/**
 * @file EarDog.h
 * @author thale
 * @brief Declarações das funções do personagem EarDog.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria uma instância alocada dinamicamente da struct EarDog.
 */
EarDog *criarEarDog( float x, float y, float w, float h );

/**
 * @brief Destrói um objeto EarDog e libera seus recursos.
 */
void destruirEarDog( EarDog *ed );

/**
 * @brief Atualiza a física, movimentação e animação do EarDog.
 */
void atualizarEarDog( EarDog *ed, GameWorld *gw, float delta );

/**
 * @brief Desenha o EarDog na tela.
 */
void desenharEarDog( EarDog *ed );

/**
 * @brief Aplica dano ao EarDog (chamado quando a hitbox de ataque do PolarBear colide).
 */
void earDogReceberDano( EarDog *ed );

/**
 * @brief Retorna o retângulo de hitbox visual do EarDog proporcional ao sprite.
 */
Rectangle earDogObterHitbox( EarDog *ed );
