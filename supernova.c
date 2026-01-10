/**
 * ASCII Supernova
 * Simulação ASCII de uma Supernova de Colapso de Núcleo (Tipo II)
 *
 * Descrição:
 * Este programa exibe no terminal a evolução de uma estrela massiva
 * desde seu estado de supergigante até o colapso gravitacional,
 * explosão em supernova e formação do remanescente compacto.
 * A simulação usa renderização ASCII em tempo real para representar
 * expansão radial, ejeção de matéria e remanescente estelar.
 *
 * Modelo astrofísico representado (sequência simplificada e fiel ao modelo científico):
 *
 * 1) GIANT
 *    A estrela começa como uma supergigante massiva em estágio final.
 *    O raio oscila levemente simulando instabilidades térmicas causadas
 *    pela queima irregular de elementos pesados (Si, O, C).
 *
 * 2) COLLAPSE
 *    O núcleo atinge ferro, elemento que não gera energia por fusão.
 *    Sem suporte de pressão, ocorre colapso gravitacional rápido.
 *    A estrela contrai violentamente, representando o colapso real
 *    que acontece em milissegundos na natureza.
 *
 * 3) BOUNCE
 *    O núcleo atinge densidade nuclear e torna-se extremamente rígido.
 *    A matéria cai, encontra resistência e ocorre o "core bounce":
 *    um ricochete interno responsável por iniciar a onda de choque.
 *
 * 4) EXPLOSION
 *    A onda de choque atravessa a estrela e ejeta suas camadas externas.
 *    As partículas representam plasma, poeira cósmica e elementos pesados
 *    sintetizados durante a explosão (incluindo metais pesados como ouro).
 *
 * 5) NEBULA
 *    O material ejetado continua se expandindo e se torna difuso,
 *    formando o remanescente de supernova (análoga à Nebulosa do Caranguejo).
 *    O caractere 'O' simboliza a estrela de nêutrons remanescente,
 *    que pode se tornar um pulsar em evoluções futuras do projeto.
 *
 * Observação:
 * Embora seja uma abstração artística, segue a lógica aceita
 * na astrofísica moderna para supernovas de colapso de núcleo (Tipo II).
 *
 * Uso educacional:
 * - Demonstração científica
 * - Ferramenta didática de astronomia
 * - Exemplo de animação em C no terminal
 *
 * Compilação:
 *     gcc supernova.c -o supernova -lm
 *
 * Execução:
 *     ./supernova
 *
 * Requisitos:
 * - GCC ou Clang
 * - Terminal com suporte ANSI
 * - Sistemas Linux, macOS ou Windows (via WSL ou similar)
 *
 * Licença:
 * MIT License — livre para uso acadêmico, científico e educacional.
 *
 * Autor:
 * Bandeirinha, 2026
 * Projeto aberto para estudo, colaboração e evolução.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#define WIDTH 90
#define HEIGHT 32
#define FPS 30

// Estados evolutivos da estrela
#define GIANT 0
#define COLLAPSE 1
#define BOUNCE 2
#define EXPLOSION 3
#define NEBULA 4

#define MAX_PARTICLES 450

typedef struct {
    float x, y;
    float vx, vy;
    float life;
} Particle;

typedef struct {
    float radius;            // Raio visível da estrela
    float core_radius;       // Tamanho do núcleo compacto (estrela de nêutrons)
    float explosion_radius;  // Raio da frente de choque da supernova
    float velocity;          // Velocidade de contração durante colapso
    float time;              // Tempo interno do estágio
    float density;           // (Reservado para futuras expansões científicas)
    int state;

    Particle particles[MAX_PARTICLES];
    int particle_count;

} Star;

// Função utilitária
float clamp(float v, float a, float b){
    if(v < a) return a;
    if(v > b) return b;
    return v;
}

/**
 * Geração das partículas ejetadas pela explosão.
 * Representam o "ejecta" rico em elementos pesados.
 */
void spawn_particles(Star *s){
    s->particle_count = MAX_PARTICLES;
    float cx = WIDTH / 2.0;
    float cy = HEIGHT / 2.0;

    for(int i=0;i<MAX_PARTICLES;i++){
        float angle = ((float)rand()/RAND_MAX) * 2*M_PI;
        float speed = 10 + rand()%40; // velocidades variadas → explosão irregular

        s->particles[i].x = cx;
        s->particles[i].y = cy;
        s->particles[i].vx = cos(angle) * speed;
        s->particles[i].vy = sin(angle) * speed * 0.55;
        s->particles[i].life = 2.5 + ((float)rand()/RAND_MAX)*1.5;
    }
}

// Limpa terminal
void clear(){
    printf("\033[H\033[J");
}

/**
 * Renderização da estrela e fenômenos associados
 * Cada símbolo representa um estado físico aproximado:
 * #  = envelope estelar estável
 * @  = estrela colapsando
 * *  = choque sendo propagado
 * +  = partículas de ejecta
 * .  = gás difuso (nebulosa)
 * O  = estrela de nêutrons remanescente
 */
void draw_star(Star *s){
    clear();

    float cx = WIDTH / 2.0;
    float cy = HEIGHT / 2.0;

    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){

            // Correção da proporção vertical do terminal
            float dy = (y - cy) * 1.5;
            float dx = (x - cx);
            float d = sqrt(dx*dx + dy*dy);

            char pixel = ' ';

            if(s->state == GIANT){
                // Supergigante massiva pulsando
                if(d <= s->radius) pixel = '#';
            }

            else if(s->state == COLLAPSE){
                // Contração do núcleo e esmagamento gravitacional
                if(d <= s->radius) pixel = '@';
            }

            else if(s->state == BOUNCE){
                // Frente de choque inicial
                if(d <= s->radius && d >= s->radius - 1.5) pixel = '*';
            }

            else if(s->state == EXPLOSION){
                // Onda de choque se expandindo rapidamente
                if(d <= s->explosion_radius && d >= s->explosion_radius - 1.6)
                    pixel = '*';
            }

            else if(s->state == NEBULA){
                // Remanescente difuso da supernova
                if(d <= s->explosion_radius && rand()%12==0)
                    pixel = '.';
            }

            // Núcleo compacto restante — estrela de nêutrons
            if(d <= s->core_radius)
                pixel = 'O';

            // Partículas de ejecta
            for(int i=0;i<s->particle_count;i++){
                int px = (int)s->particles[i].x;
                int py = (int)s->particles[i].y;

                if(px == x && py == y && s->particles[i].life > 0)
                    pixel = '+';
            }

            putchar(pixel);
        }
        putchar('\n');
    }
}

/**
 * Atualiza movimento das partículas ejetadas
 */
void update_particles(Star *s, float dt){
    for(int i=0;i<s->particle_count;i++){
        if(s->particles[i].life <= 0) continue;

        s->particles[i].x += s->particles[i].vx * dt;
        s->particles[i].y += s->particles[i].vy * dt;
        s->particles[i].life -= dt;
    }
}

/**
 * Evolução temporal do objeto astrofísico
 */
void update_star(Star *s, float dt){
    s->time += dt;

    // Fase de Supergigante instável
    if(s->state == GIANT){
        s->radius = 9 + sin(s->time*3)*1.5;

        // Após certo tempo → colapso catastrófico
        if(s->time > 5){
            s->state = COLLAPSE;
            s->time = 0;
            s->velocity = 0;
        }
    }

    // Colapso gravitacional do núcleo
    else if(s->state == COLLAPSE){
        s->velocity += 40 * dt;
        s->radius -= s->velocity * dt;

        // Núcleo atinge densidade nuclear → bounce
        if(s->radius < 3){
            s->state = BOUNCE;
            s->time = 0;
            s->core_radius = 2; // estrela de nêutrons formada
        }
    }

    // Choque de ricochete
    else if(s->state == BOUNCE){
        s->radius += 25 * dt;

        if(s->time > 0.8){
            s->state = EXPLOSION;
            s->time = 0;
            s->explosion_radius = 3;
            spawn_particles(s);
        }
    }

    // Supernova propriamente dita
    else if(s->state == EXPLOSION){
        s->explosion_radius += 30 * dt;
        update_particles(s, dt);

        if(s->explosion_radius > 32){
            s->state = NEBULA;
            s->time = 0;
        }
    }

    // Remanescente de Supernova
    else if(s->state == NEBULA){
        s->explosion_radius += 6 * dt;
        update_particles(s, dt);

        // Reinicia o ciclo apenas para fins de animação
        if(s->explosion_radius > 42){
            s->state = GIANT;
            s->time = 0;
            s->radius = 9;
        }
    }
}

int main(){
    srand(time(NULL));

    Star s;
    s.radius = 9;
    s.core_radius = 0;
    s.explosion_radius = 0;
    s.velocity = 0;
    s.time = 0;
    s.state = GIANT;
    s.particle_count = 0;

    float dt = 1.0 / FPS;

    while(1){
        update_star(&s, dt);
        draw_star(&s);
        usleep(1000000 / FPS);
    }

    return 0;
}
