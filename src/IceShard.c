/**
 * @file IceShard.c
 * @author thale
 * @brief Implementação do Ice Shard (Inimigo Aéreo e Projéteis).
 *
 * @copyright Copyright (c) 2026
 */
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"
#include "raymath.h"

#include "IceShard.h"
#include "ResourceManager.h"
#include "Tipos.h"

// Funções auxiliares para projéteis (o GameWorld gerencia o array)
static void dispararProjetil( GameWorld *gw, Vector2 pos, float angle ) {
    for ( int i = 0; i < MAX_PROJETEIS; i++ ) {
        if ( !gw->projeteis[i].ativo ) {
            gw->projeteis[i].ativo = true;
            gw->projeteis[i].pos = pos;
            
            float speed = 300.0f; // Velocidade do projétil aumentada levemente
            gw->projeteis[i].vel.x = cosf(angle) * speed;
            gw->projeteis[i].vel.y = sinf(angle) * speed;
            
            gw->projeteis[i].lifeTimer = 0.0f;
            gw->projeteis[i].animTimer = 0.0f;
            gw->projeteis[i].animFrame = 0;
            break;
        }
    }
}

IceShard *criarIceShard( float x, float y ) {
    IceShard *is = (IceShard*) malloc( sizeof( IceShard ) );
    is->ret.x = x;
    is->ret.y = y;
    // Escala menor: hitboxes menores
    is->ret.width = 30.0f;
    is->ret.height = 40.0f;
    is->base_x = x;
    is->base_y = y;
    is->targetX = x;
    is->targetY = y;
    is->moveTimer = 0.0f;
    is->floatTimer = (float)(rand() % 100) / 100.0f * PI; // Offset randomico
    
    is->estado = ESTADO_ICESHARD_FLUTUANDO;
    is->quantidadeVidas = 2.0f; // Morre com 2 socos aereos
    is->invencibilidade = 0.0f;
    
    is->animTimer = 0.0f;
    is->animFrame = 0;
    
    is->attackTimer = 0.0f;
    is->shotsFired = 0;
    is->shotCooldown = 0.0f;
    
    is->currentAimAngle = PI / 2.0f; // Mira pra baixo por padrao
    is->ativo = true;
    
    return is;
}

void destruirIceShard( IceShard *is ) {
    if ( is != NULL ) {
        free( is );
    }
}

void iceShardReceberDano( IceShard *is, float playerX ) {
    if ( is == NULL || !is->ativo ) return;
    if ( is->invencibilidade > 0.0f ) return;
    if ( is->estado == ESTADO_ICESHARD_MORRENDO ) return;
    
    // Imune a dano durante a animação de ataque e disparo
    if ( is->estado == ESTADO_ICESHARD_ABRINDO || is->estado == ESTADO_ICESHARD_ATACANDO ) return;

    is->quantidadeVidas--;

    // Knockback proporcional à largura (ret.width * 0.8f é a hitbox aprox)
    float width = is->ret.width * 0.8f;
    float forceX = width * 1.5f; 
    
    // Determina a direção baseada no jogador
    if ( is->ret.x > playerX ) {
        is->base_x += forceX;
        is->targetX += forceX;
        is->ret.x += forceX;
    } else {
        is->base_x -= forceX;
        is->targetX -= forceX;
        is->ret.x -= forceX;
    }

    if ( is->quantidadeVidas <= 0 ) {
        is->quantidadeVidas = 0;
        is->estado = ESTADO_ICESHARD_MORRENDO;
        is->animFrame = 0;
        is->animTimer = 0.0f;
    } else {
        is->invencibilidade = 0.5f;
        // Cooldown de 1 segundo para o próximo ataque 
        // O timer dispara em 4.0, então resetamos para 3.0 para faltar exato 1 segundo.
        is->attackTimer = 3.0f;
    }
}

static Jogador *obterAlvoProximoIceShard( IceShard *is, GameWorld *gw ) {
    Jogador *j = gw->jogador;
    Belial *b = gw->belial;
    bool jValido = ( j != NULL && j->ativo );
    bool bValido = ( gw->modo2Jogadores && b != NULL && b->ativo );

    if ( jValido && bValido ) {
        float cxIS = is->ret.x + is->ret.width / 2.0f;
        float cxJ = j->ret.x + j->ret.width / 2.0f;
        float cxB = b->ret.x + b->ret.width / 2.0f;
        if ( fabsf( cxJ - cxIS ) < fabsf( cxB - cxIS ) ) {
            return j;
        } else {
            return (Jogador*) b;
        }
    } else if ( jValido ) {
        return j;
    } else if ( bValido ) {
        return (Jogador*) b;
    }
    return NULL;
}

void atualizarIceShard( IceShard *is, GameWorld *gw, float delta ) {
    if ( is == NULL || !is->ativo ) return;

    if ( is->invencibilidade > 0.0f ) {
        is->invencibilidade -= delta;
        if ( is->invencibilidade < 0.0f ) is->invencibilidade = 0.0f;
    }

    if ( is->estado == ESTADO_ICESHARD_MORRENDO ) {
        // Morte: últimos 3 frames de piscar/escurecer (duracao aumentada para 0.25s)
        is->animTimer += delta;
        if ( is->animTimer >= 0.25f ) {
            is->animTimer = 0.0f;
            is->animFrame++;
            if ( is->animFrame >= 3 ) {
                is->ativo = false; // Removido
            }
        }
        return;
    }

    // Comportamento de Patrulha em X e Y (transitando no mapa)
    is->moveTimer -= delta;
    if ( is->moveTimer <= 0.0f ) {
        // Escolhe um novo destino próximo da base
        is->targetX = is->base_x + (float)(rand() % 400 - 200); // Raio X +- 200
        is->targetY = is->base_y + (float)(rand() % 100 - 50);  // Raio Y +- 50
        is->moveTimer = 2.0f + (float)(rand() % 200)/100.0f; // 2 a 4 segundos
    }

    // Move suavemente para o targetX / targetY
    is->ret.x = Lerp(is->ret.x, is->targetX, delta * 1.5f);
    is->ret.y = Lerp(is->ret.y, is->targetY, delta * 1.5f);

    // Efeito de flutuação suave (bob) adicionado à posição base de patrulha
    is->floatTimer += delta * 2.0f;
    float bobOffset = sinf(is->floatTimer) * 10.0f;
    is->ret.y += bobOffset * delta; // Pequeno incremento visual

    Jogador *j = obterAlvoProximoIceShard( is, gw );
    if ( j == NULL ) {
        if ( is->estado != ESTADO_ICESHARD_MORRENDO ) {
            is->estado = ESTADO_ICESHARD_FLUTUANDO;
            is->attackTimer = 0.0f;
        }
        return;
    }

    float cxIS = is->ret.x + is->ret.width / 2.0f;
    float cxJ = j->ret.x + j->ret.width / 2.0f;
    float cyIS = is->ret.y + is->ret.height / 2.0f;
    float cyJ = j->ret.y + j->ret.height / 2.0f;
    
    float dist = fabsf(cxJ - cxIS);

    // Calcular ângulo alvo
    float targetAngle = atan2f(cyJ - cyIS, cxJ - cxIS);
    
    // LerpAngle suave (Raymath Lerp pode não cobrir o wraparound corretamente, mas aqui 
    // geralmente targetAngle está sempre para baixo, de 0 a PI)
    is->currentAimAngle = Lerp(is->currentAimAngle, targetAngle, delta * 3.0f);

    if ( is->estado == ESTADO_ICESHARD_FLUTUANDO ) {
        is->attackTimer += delta;
        if ( dist < 500.0f && is->attackTimer >= 4.0f ) {
            is->estado = ESTADO_ICESHARD_ABRINDO;
            is->animTimer = 0.0f;
            is->animFrame = 0; // Vai da estrela para a forma complexa
        }
    } else if ( is->estado == ESTADO_ICESHARD_ABRINDO ) {
        is->animTimer += delta;
        if ( is->animTimer >= 0.5f ) { // Tempo de abertura
            is->estado = ESTADO_ICESHARD_ATACANDO;
            is->shotsFired = 0;
            is->shotCooldown = 0.5f; // Dispara o primeiro imediatamente
        }
    } else if ( is->estado == ESTADO_ICESHARD_ATACANDO ) {
        is->shotCooldown -= delta;
        if ( is->shotCooldown <= 0.0f ) {
            // Dispara projétil na direção atual suavizada
            Vector2 pos = { cxIS, cyIS };
            dispararProjetil( gw, pos, is->currentAimAngle );
            
            is->shotsFired++;
            if ( is->shotsFired >= 3 ) { // Finalizou rajada
                is->estado = ESTADO_ICESHARD_FLUTUANDO;
                is->attackTimer = 0.0f; // Reseta cooldown
            } else {
                is->shotCooldown = 0.5f; // Exatamente 0.5s entre espinhos
            }
        }
    }
}

void desenharIceShard( IceShard *is ) {
    if ( is == NULL || !is->ativo ) return;

    Texture2D tex = rm.iceShard;
    
    // Frames extraídos
    // Estrela fechada (Frame 0 da imagem)
    Rectangle frame0 = { 14, 6, 53, 83 };
    // Cristal aberto/atacando (Row 0 frame 1)
    Rectangle frame1 = { 91, 6, 73, 83 };
    
    // Frames de morte (últimos 3 da row 0)
    Rectangle death_frames[3] = {
        { 183, 6, 66, 83 },
        { 263, 6, 48, 83 },
        { 330, 6, 27, 83 }
    };

    Rectangle src;
    if ( is->estado == ESTADO_ICESHARD_MORRENDO ) {
        int fi = is->animFrame;
        if (fi > 2) fi = 2;
        src = death_frames[fi];
    } else if ( is->estado == ESTADO_ICESHARD_FLUTUANDO ) {
        src = frame0;
    } else {
        src = frame1; // Aberto
    }

    float scale = 0.6f; // Reduzido
    float drawW = src.width * scale;
    float drawH = src.height * scale;
    float drawX = is->ret.x + (is->ret.width - drawW) / 2.0f;
    float drawY = is->ret.y + (is->ret.height - drawH) / 2.0f;

    Rectangle dest = { drawX, drawY, drawW, drawH };
    Vector2 origin = { 0, 0 };

    // Determine tint color (fade out during death, reddish during invincibility)
    Color tint = WHITE;
    if (is->estado == ESTADO_ICESHARD_MORRENDO) {
        // Fade out over 3 frames
        unsigned char alpha = (unsigned char)(255 - is->animFrame * 85);
        tint.a = alpha;
    } else if ( is->invencibilidade > 0.0f ) {
        // Ficar levemente avermelhado ao tomar dano
        tint = (Color){ 255, 150, 150, 255 };
    }

    bool isHitFlash = false;
    if ( is->invencibilidade > 0.0f ) {
        int phase = (int)( is->invencibilidade / 0.15f ) % 2;
        if ( phase == 0 ) isHitFlash = true;
    }

    // Outline embutido (desenha 4 vezes preto deslocado 2px)
    DrawTexturePro( tex, src, (Rectangle){ dest.x - 2, dest.y, dest.width, dest.height }, origin, 0.0f, BLACK );
    DrawTexturePro( tex, src, (Rectangle){ dest.x + 2, dest.y, dest.width, dest.height }, origin, 0.0f, BLACK );
    DrawTexturePro( tex, src, (Rectangle){ dest.x, dest.y - 2, dest.width, dest.height }, origin, 0.0f, BLACK );
    DrawTexturePro( tex, src, (Rectangle){ dest.x, dest.y + 2, dest.width, dest.height }, origin, 0.0f, BLACK );

    // Draw main sprite with tint (handles fade and damage colors)
    DrawTexturePro( tex, src, dest, origin, 0.0f, tint );
    
    // Efeito de blur/brilho branco quando toma dano
    if ( isHitFlash ) {
        BeginBlendMode( BLEND_ADDITIVE );
        Rectangle destBlur = { dest.x - 5, dest.y - 5, dest.width + 10, dest.height + 10 };
        DrawTexturePro( tex, src, destBlur, origin, 0.0f, (Color){ 255, 255, 255, 150 } );
        EndBlendMode();
    }
}
