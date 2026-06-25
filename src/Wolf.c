/**
 * @file Wolf.c
 * @author thale
 * @brief Implementação do Wolf (Inimigo Terrestre).
 *
 * @copyright Copyright (c) 2026
 */
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"
#include "raymath.h"

#include "Wolf.h"
#include "Mapa.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void resolverColisaoWolfObstaculosMapaX( Wolf *w, Mapa *mapa );

// Retorna verdadeiro se há chão (y==220) sob o ponto x
static bool temChao( float x, Mapa *mapa ) {
    ElementoMapa *el = mapa->obstaculos;
    while ( el != NULL ) {
        Obstaculo *o = (Obstaculo*) el->objeto;
        if ( o->ret.y == 220.0f ) {
            float tolerancia = 12.0f; // Mesma tolerancia do jogador
            if ( x >= o->ret.x - tolerancia && x <= o->ret.x + o->ret.width + tolerancia ) {
                return true;
            }
        }
        el = el->proximo;
    }
    return false;
}

static bool wolfNoChao( Wolf *w, Mapa *mapa ) {
    float cx = w->ret.x + w->ret.width / 2.0f;
    return temChao( cx, mapa );
}

Wolf *criarWolf( float x, float y, float w, float h ) {
    Wolf *novoWolf = (Wolf*) malloc( sizeof( Wolf ) );
    novoWolf->ret.x = x;
    novoWolf->ret.y = y;
    novoWolf->ret.width = w;
    novoWolf->ret.height = h;
    novoWolf->vel.x = 0;
    novoWolf->vel.y = 0;
    novoWolf->olhandoParaDireita = false;
    novoWolf->animTimer = 0.0f;
    novoWolf->animFrame = 0;
    
    novoWolf->estado = ESTADO_WOLF_PARADO;
    novoWolf->quantidadeVidas = 1.0f; // 1 soco para morrer
    novoWolf->invencibilidade = 0.0f;
    
    novoWolf->puloY = 0.0f;
    novoWolf->puloVel = 0.0f;
    
    novoWolf->attackCooldown = 0.0f;
    novoWolf->hasHitPlayer = false;
    
    novoWolf->hitTimer = 0.0f;
    novoWolf->hitFrame = 0;
    novoWolf->ativo = true;
    
    return novoWolf;
}

void destruirWolf( Wolf *w ) {
    if ( w != NULL ) {
        free( w );
    }
}

static Jogador *obterAlvoProximo( Wolf *w, GameWorld *gw ) {
    Jogador *j = gw->jogador;
    Belial *b = gw->belial;
    bool jValido = ( j != NULL && j->ativo && j->ret.x > -1000.0f );
    bool bValido = ( b != NULL && b->ativo && b->ret.x > -1000.0f );

    if ( jValido && bValido ) {
        float cxW = w->ret.x + w->ret.width / 2.0f;
        float cyW = w->ret.y + w->ret.height;
        float cxJ = j->ret.x + j->ret.width / 2.0f;
        float cyJ = j->ret.y + j->ret.height;
        float cxB = b->ret.x + b->ret.width / 2.0f;
        float cyB = b->ret.y + b->ret.height;

        // Distancia Manhattan com peso no eixo Y para priorizar quem esta na mesma profundidade
        float distJ = fabsf( cxJ - cxW ) + fabsf( cyJ - cyW ) * 1.5f;
        float distB = fabsf( cxB - cxW ) + fabsf( cyB - cyW ) * 1.5f;

        // Histerese para evitar que o lobo fique alternando alvos rapidamente
        if ( fabsf( distJ - distB ) < 60.0f ) {
            bool dirJ = ( cxJ > cxW );
            bool dirB = ( cxB > cxW );
            if ( w->olhandoParaDireita == dirJ ) return j;
            if ( w->olhandoParaDireita == dirB ) return (Jogador*) b;
        }

        if ( distJ < distB ) {
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

void atualizarWolf( Wolf *w, GameWorld *gw, float delta ) {
    if ( w == NULL || !w->ativo ) return;

    if ( w->invencibilidade > 0.0f ) {
        w->invencibilidade -= delta;
        if ( w->invencibilidade < 0.0f ) w->invencibilidade = 0.0f;
    }

    if ( w->estado == ESTADO_WOLF_MORRENDO ) {
        // Animação de morte: Penúltima linha do spritesheet (10 frames)
        static const float deathDuracao = 0.1f;
        w->hitTimer += delta;
        if ( w->hitTimer >= deathDuracao ) {
            w->hitTimer = 0.0f;
            w->hitFrame++;
            if ( w->hitFrame >= 10 ) {
                w->ativo = false; // Removido apos tocar inteira
            }
        }
        
        // Aplica gravidade mesmo morto
        w->puloVel += gw->gravidade * delta;
        w->puloY += w->puloVel * delta;
        
        if ( w->puloVel > 0 && wolfNoChao(w, gw->mapa) ) {
            if ( w->puloY >= 0.0f ) {
                w->puloY = 0.0f;
                w->puloVel = 0.0f;
            }
        }
        
        // Se cair muito no buraco, apaga
        if ( w->puloY > 600.0f ) {
            w->ativo = false;
        }
        return;
    }

    if ( w->estado == ESTADO_WOLF_TOMANDO_GOLPE ) {
        // Usa o frame inicial da morte como hit
        static const float hitDuracao = 0.3f;
        w->hitTimer += delta;
        if ( w->hitTimer >= hitDuracao ) {
            w->estado = ESTADO_WOLF_PARADO; // Volta
            w->animTimer = 0.0f;
            w->animFrame = 0;
        }
        
        w->vel.x = Lerp(w->vel.x, 0, delta * 5.0f);
        w->ret.x += w->vel.x * delta;
        w->vel.y = Lerp(w->vel.y, 0, delta * 5.0f);
        w->ret.y += w->vel.y * delta;
        
        w->puloVel += gw->gravidade * delta;
        w->puloY += w->puloVel * delta;
        
        if ( w->puloVel > 0 && wolfNoChao(w, gw->mapa) ) {
            if ( w->puloY >= 0.0f ) {
                w->puloY = 0.0f;
                w->puloVel = 0.0f;
            }
        }
        
        // Knockback em buraco -> ignora borda e morre se cair
        if ( w->puloY > 600.0f ) {
            w->quantidadeVidas = 0;
            w->estado = ESTADO_WOLF_MORRENDO;
            w->ativo = false;
        }
        
        resolverColisaoWolfObstaculosMapaX( w, gw->mapa );
        return;
    }

    // Cooldown
    if ( w->attackCooldown > 0.0f ) {
        w->attackCooldown -= delta;
    }

    // Gravidade 2.5D
    w->puloVel += gw->gravidade * delta;
    w->puloY += w->puloVel * delta;
    
    // Limite de chao validado
    if ( w->puloVel > 0 && wolfNoChao(w, gw->mapa) ) {
        if ( w->puloY >= 0.0f ) {
            w->puloY = 0.0f;
            w->puloVel = 0.0f;
        }
    }
    
    // Fallback: se caiu no buraco
    if ( w->puloY > 600.0f ) {
        w->ativo = false;
    }

    // AI logic: Target Player
    Jogador *j = obterAlvoProximo( w, gw );
    if ( j != NULL && w->estado != ESTADO_WOLF_ATACANDO ) {
        float cxW = w->ret.x + w->ret.width / 2.0f;
        float cyW = w->ret.y + w->ret.height; // Depth do lobo
        
        float cxJ = j->ret.x + j->ret.width / 2.0f;
        float cyJ = j->ret.y + j->ret.height; // Depth do jogador
        
        float dist = fabsf(cxJ - cxW);

        // Movement with Lerp
        float targetSpeedX = 0.0f;
        float targetSpeedY = 0.0f;

        if ( dist < 5000.0f ) {
            bool direcaoAlvoDireita = (cxJ > cxW);
            
            // Sweet spot para o pulo
            bool naDistanciaIdeal = (dist >= 200.0f && dist <= 260.0f);
            bool noEixoY = (fabsf(cyJ - cyW) < 50.0f);
            
            // Ataque apenas se no chão (puloY <= 0)
            if ( naDistanciaIdeal && noEixoY && w->attackCooldown <= 0.0f && w->puloY >= 0.0f && wolfNoChao(w, gw->mapa) ) {
                w->estado = ESTADO_WOLF_ATACANDO;
                w->animTimer = 0.0f;
                w->animFrame = 0;
                w->hasHitPlayer = false;
                
                w->puloVel = -350.0f; // Pulo para cima
                w->vel.x = direcaoAlvoDireita ? 300.0f : -300.0f; // Avanço do bote
                w->vel.y = 0.0f;
                w->olhandoParaDireita = direcaoAlvoDireita;
                w->somAtaqueTocado = false;
            } else {
                // IA de Movimentação para alcançar a distância ideal
                if ( w->attackCooldown <= 0.0f ) {
                    if ( dist < 200.0f ) {
                        // Muito perto! Caminha na direção oposta para pegar distância para o bote
                        targetSpeedX = direcaoAlvoDireita ? -150.0f : 150.0f;
                    } else if ( dist > 260.0f ) {
                        // Muito longe! Avança
                        targetSpeedX = direcaoAlvoDireita ? 150.0f : -150.0f;
                    } else {
                        // Na distância de X, mas desalinhado em Y
                        targetSpeedX = 0.0f;
                    }
                } else {
                    // Se está em cooldown, apenas fica a uma distância segura
                    if ( dist > 150.0f ) {
                        targetSpeedX = direcaoAlvoDireita ? 150.0f : -150.0f;
                    } else {
                        targetSpeedX = 0.0f;
                    }
                }
                
                // IA de Borda (Cliff Avoidance) baseada na direção real do movimento pretendido
                bool vaiParaDireita = (targetSpeedX > 0.0f);
                float sensorX = vaiParaDireita ? (w->ret.x + w->ret.width + 15.0f) : (w->ret.x - 15.0f);
                if ( targetSpeedX != 0.0f && !temChao( sensorX, gw->mapa ) && w->puloY >= 0.0f ) {
                    targetSpeedX = 0.0f;
                }
                
                // Perseguição Vertical (Depth)
                if ( cyJ > cyW + 10.0f ) {
                    targetSpeedY = 100.0f;
                } else if ( cyJ < cyW - 10.0f ) {
                    targetSpeedY = -100.0f;
                }
                
                w->estado = ESTADO_WOLF_ANDANDO;
                if ( targetSpeedX == 0.0f && targetSpeedY == 0.0f ) w->estado = ESTADO_WOLF_PARADO;
            }
        } else {
            w->estado = ESTADO_WOLF_PARADO;
        }

        // Smooth acceleration / deceleration
        if ( w->estado != ESTADO_WOLF_ATACANDO ) {
            w->vel.x = Lerp(w->vel.x, targetSpeedX, delta * 5.0f);
            w->vel.y = Lerp(w->vel.y, targetSpeedY, delta * 5.0f);
            
            // Apply vertical depth movement
            w->ret.y += w->vel.y * delta;
            
            // Define para onde o lobo olha baseado no seu movimento real (evita deslizar de costas)
            if ( w->vel.x > 10.0f ) {
                w->olhandoParaDireita = true;
            } else if ( w->vel.x < -10.0f ) {
                w->olhandoParaDireita = false;
            } else {
                w->olhandoParaDireita = ( cxJ > cxW );
            }
        }
    } else if ( w->estado != ESTADO_WOLF_ATACANDO ) {
        w->estado = ESTADO_WOLF_PARADO;
        w->vel.x = Lerp(w->vel.x, 0.0f, delta * 5.0f);
        w->vel.y = Lerp(w->vel.y, 0.0f, delta * 5.0f);
    }
    
    // Aplica gravidade se não tiver chão (em qualquer estado livre)
    if ( w->estado != ESTADO_WOLF_MORRENDO && w->estado != ESTADO_WOLF_TOMANDO_GOLPE && w->estado != ESTADO_WOLF_ATACANDO ) {
        if ( !wolfNoChao(w, gw->mapa) ) {
            w->puloVel += gw->gravidade * delta;
            w->puloY += w->puloVel * delta;
            if ( w->puloY > 600.0f ) {
                w->ativo = false; // Cai no buraco e morre
            }
        } else if ( w->puloY > 0.0f ) {
            w->puloY = 0.0f;
            w->puloVel = 0.0f;
        }
    }

    // Update de estados visuais
    if ( w->estado == ESTADO_WOLF_ATACANDO ) {
        if (!w->somAtaqueTocado) {
            PlaySound( rm.sfxWolfAttack );
            w->somAtaqueTocado = true;
        }
        // Animação de ataque: Linha 2 (index 1), frames 0 a 9
        w->ret.x += w->vel.x * delta;
        w->ret.y += w->vel.y * delta; // Ensure depth movement during attack arc
        
        static const float atkDuracao = 0.08f;
        w->animTimer += delta;
        if ( w->animTimer >= atkDuracao ) {
            w->animTimer = 0.0f;
            w->animFrame++;
            if ( w->animFrame >= 10 ) {
                w->estado = ESTADO_WOLF_PARADO;
                w->animFrame = 0;
                w->attackCooldown = 2.0f; // 2 segundos entre ataques
            }
        }
    } else {
        w->ret.x += w->vel.x * delta;
        resolverColisaoWolfObstaculosMapaX( w, gw->mapa );

        if ( w->estado == ESTADO_WOLF_ANDANDO ) {
            // Walk cycle: Agora com 10 frames (Row 0)
            w->animTimer += delta * 12.0f;
            w->animFrame = (int)w->animTimer % 10;
        }
    }
}

void desenharWolf( Wolf *w ) {
    if ( w == NULL || !w->ativo ) return;

    // Row 0 for walk (10 frames)
    static const Rectangle walk_frames[10] = { { 12, 36, 224, 116 }, { 280, 36, 212, 116 }, { 548, 32, 204, 120 }, { 808, 32, 212, 120 }, { 1076, 28, 212, 124 }, { 1344, 24, 200, 128 }, { 1608, 16, 196, 136 }, { 1872, 16, 192, 136 }, { 2132, 24, 196, 128 }, { 2392, 28, 212, 124 } };
    
    // Row 1 for attack
    static const Rectangle attack_frames[10] = { { 80, 236, 224, 116 }, { 436, 208, 208, 144 }, { 784, 204, 204, 148 }, { 1124, 208, 212, 136 }, { 1436, 200, 224, 136 }, { 1732, 192, 224, 136 }, { 2092, 224, 236, 128 }, { 2480, 228, 200, 128 }, { 2832, 212, 192, 140 }, { 3180, 224, 208, 128 } };
    
    // Row 5 for death (penultima visualmente)
    static const Rectangle death_frames[10] = { { 60, 1016, 116, 172 }, { 312, 1040, 96, 152 }, { 532, 1072, 152, 116 }, { 760, 1108, 164, 92 }, { 1000, 1092, 164, 92 }, { 1240, 1076, 164, 92 }, { 1492, 1056, 152, 116 }, { 1732, 1072, 152, 116 }, { 1928, 1144, 192, 64 }, { 2168, 1136, 192, 64 } };
    
    // Row 3 (frame 0) para hit
    static const Rectangle hit_frame = { 24, 596, 196, 136 };

    Texture2D tex = rm.wolfDummy;
    Rectangle src;

    if ( w->estado == ESTADO_WOLF_MORRENDO ) {
        int fi = w->hitFrame;
        if ( fi > 9 ) fi = 9;
        src = death_frames[fi];
    } else if ( w->estado == ESTADO_WOLF_TOMANDO_GOLPE ) {
        src = hit_frame;
    } else if ( w->estado == ESTADO_WOLF_ATACANDO ) {
        int fi = w->animFrame;
        if ( fi > 9 ) fi = 9;
        src = attack_frames[fi];
    } else {
        src = walk_frames[w->animFrame % 10];
    }

    // Se a imagem original olha para a ESQUERDA:
    // w->olhandoParaDireita = true -> precisamos inverter (width negativo)
    // w->olhandoParaDireita = false -> mantem original (width positivo)
    if ( w->olhandoParaDireita ) {
        src.width = -src.width;
    }

    float scale = 0.25f; // Diminuído pela metade (era 0.5f)
    float drawW = fabsf( src.width ) * scale;
    float drawH = src.height * scale;

    float feet_y = w->ret.y + w->ret.height;
    float drawX = ( w->ret.x + w->ret.width / 2.0f ) - drawW / 2.0f;
    float drawY = (feet_y + w->puloY) - drawH;

    Rectangle dest = { drawX, drawY, drawW, drawH };
    Vector2 origin = { 0, 0 };

    Color tint = WHITE;
    if ( w->invencibilidade > 0.0f ) {
        int phase = (int)( w->invencibilidade / 0.15f ) % 2;
        if ( phase == 0 ) tint = (Color){ 255, 120, 120, 200 };
    }

    // Outline embutido (desenha 4 vezes preto deslocado 2px)
    DrawTexturePro( tex, src, (Rectangle){ dest.x - 2, dest.y, dest.width, dest.height }, origin, 0.0f, BLACK );
    DrawTexturePro( tex, src, (Rectangle){ dest.x + 2, dest.y, dest.width, dest.height }, origin, 0.0f, BLACK );
    DrawTexturePro( tex, src, (Rectangle){ dest.x, dest.y - 2, dest.width, dest.height }, origin, 0.0f, BLACK );
    DrawTexturePro( tex, src, (Rectangle){ dest.x, dest.y + 2, dest.width, dest.height }, origin, 0.0f, BLACK );

    DrawTexturePro( tex, src, dest, origin, 0.0f, tint );
}

void wolfReceberDano( Wolf *w ) {
    if ( w == NULL || !w->ativo ) return;
    if ( w->invencibilidade > 0.0f ) return;
    if ( w->estado == ESTADO_WOLF_MORRENDO ) return;

    w->quantidadeVidas--;
    if ( w->quantidadeVidas <= 0 ) {
        w->quantidadeVidas = 0;
        w->estado = ESTADO_WOLF_MORRENDO;
        w->hitFrame = 0;
        w->hitTimer = 0.0f;
        w->vel.x = 0;
    } else {
        w->estado = ESTADO_WOLF_TOMANDO_GOLPE;
        w->hitFrame = 0;
        w->hitTimer = 0.0f;
        
        // Knockback proporcional a largura do lobo
        Rectangle hitbox = wolfObterHitboxCorpo( w );
        float width = hitbox.width; 
        float forcaX = width * 1.5f; // Fator de knockback (ex: 100 * 1.5 = 150)
        
        // Empurrao pra tras 2.5D
        w->vel.x = w->olhandoParaDireita ? -forcaX : forcaX;
        w->vel.y = 0.0f;
        w->puloVel = -150.0f; // Leve recuo pra cima
    }

    w->invencibilidade = 0.5f;
}

void wolfReceberDanoEspecial( Wolf *w, int dano ) {
    if ( w == NULL || !w->ativo ) return;
    if ( w->invencibilidade > 0.0f ) return;
    if ( w->estado == ESTADO_WOLF_MORRENDO ) return;

    w->quantidadeVidas -= dano;
    if ( w->quantidadeVidas <= 0 ) {
        w->quantidadeVidas = 0;
        w->estado = ESTADO_WOLF_MORRENDO;
        w->hitFrame = 0;
        w->hitTimer = 0.0f;
        w->vel.x = 0;
    } else {
        w->estado = ESTADO_WOLF_TOMANDO_GOLPE;
        w->hitFrame = 0;
        w->hitTimer = 0.0f;
        
        // Knockback proporcional a largura do lobo
        Rectangle hitbox = wolfObterHitboxCorpo( w );
        float width = hitbox.width; 
        float forcaX = width * 1.5f; // Fator de knockback (ex: 100 * 1.5 = 150)
        
        // Empurrao pra tras 2.5D
        w->vel.x = w->olhandoParaDireita ? -forcaX : forcaX;
        w->vel.y = 0.0f;
        w->puloVel = -150.0f; // Leve recuo pra cima
    }

    w->invencibilidade = 0.5f;
}

Rectangle wolfObterHitboxCorpo( Wolf *w ) {
    float scale = 0.25f;
    float spriteW = 200.0f * scale; // Approx scaled width
    float spriteH = 132.0f * scale;

    float feet_y = w->ret.y + w->ret.height;
    float drawX = ( w->ret.x + w->ret.width / 2.0f ) - spriteW / 2.0f;
    float drawY = (feet_y + w->puloY) - spriteH;

    return (Rectangle){ drawX, drawY, spriteW, spriteH };
}

Rectangle wolfObterHitboxAtaque( Wolf *w ) {
    if ( w->estado != ESTADO_WOLF_ATACANDO ) return (Rectangle){ 0, 0, 0, 0 };
    if ( w->animFrame < 4 || w->animFrame > 8 ) return (Rectangle){ 0, 0, 0, 0 };

    Rectangle corpo = wolfObterHitboxCorpo( w );
    float hw = corpo.width * 0.6f;
    float hh = corpo.height * 0.8f;
    
    if ( w->olhandoParaDireita ) {
        return (Rectangle){ corpo.x + corpo.width - hw/2.0f, corpo.y + 5.0f, hw, hh };
    } else {
        return (Rectangle){ corpo.x - hw/2.0f, corpo.y + 5.0f, hw, hh };
    }
}

static void resolverColisaoWolfObstaculosMapaX( Wolf *w, Mapa *mapa ) {
    ElementoMapa *el = mapa->obstaculos;
    while ( el != NULL ) {
        Obstaculo *o = (Obstaculo*) el->objeto;
        if ( o->ret.y == 220.0f ) {
            el = el->proximo;
            continue;
        }
        if ( CheckCollisionRecs( w->ret, o->ret ) ) {
            if ( w->ret.x + w->ret.width / 2.0f < o->ret.x + o->ret.width / 2.0f ) {
                w->ret.x = o->ret.x - w->ret.width;
            } else {
                w->ret.x = o->ret.x + o->ret.width;
            }
            w->vel.x = 0;
        }
        el = el->proximo;
    }
}
