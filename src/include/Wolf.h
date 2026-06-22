/**
 * @file Wolf.h
 * @author thale
 * @brief Declarações das funções do inimigo Wolf.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria uma instância alocada dinamicamente da struct Wolf.
 */
Wolf *criarWolf( float x, float y, float w, float h );

/**
 * @brief Destrói um objeto Wolf e libera seus recursos.
 */
void destruirWolf( Wolf *w );

/**
 * @brief Atualiza a física, movimentação e IA do Wolf.
 */
void atualizarWolf( Wolf *w, GameWorld *gw, float delta );

/**
 * @brief Desenha o Wolf na tela.
 */
void desenharWolf( Wolf *w );

/**
 * @brief Aplica dano ao Wolf.
 */
void wolfReceberDano( Wolf *w );
void wolfReceberDanoEspecial( Wolf *w, int dano );

/**
 * @brief Retorna o retângulo de hitbox visual do corpo do Wolf.
 */
Rectangle wolfObterHitboxCorpo( Wolf *w );

/**
 * @brief Retorna o retângulo de hitbox do ataque (mordida) do Wolf, se aplicável.
 */
Rectangle wolfObterHitboxAtaque( Wolf *w );
