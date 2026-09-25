#ifndef SFPU_PERFIL_S_H
#define SFPU_PERFIL_S_H

#include <math.h>
#include <stdint.h>

#define epsilon 1e-9f

typedef struct {
  float posicion;
  float velocidad;
  float aceleracion;
  float tiron;
  uint8_t termino;
} estado_perfil_s_t;

typedef struct {
  float v_max;
  float a_max;
  float j_max;
} limites_perfil_s_t;

typedef struct {
  limites_perfil_s_t limites;

  float x0; // Posicion inicial
  float direccion;

  // Tiempos para cada fase
  float t_1_3; // Fase 1 y 3 (subida/bajada de acel. en aceleración)
  float t_2;   //  Fase 2 (aceleración constante)
  float t_4;   // Fase 4 (velocidad constante)
  float t_5_7; // Fase 5 y 7 (subida/bajada de acel. en desaceleración)
  float t_6;   // Fase 6 (desaceleración constante)

  // Marca de tiempos
  float t1, t2, t3, t4, t5, t6, t7;

  float a_max_act;
  float a_min_act;
  float v_max_act;

  float tiempo_total;

} planificador_perfil_s_t;

static inline void Iniciar_Perfil_S(planificador_perfil_s_t *planificador,
                                    limites_perfil_s_t limites) {
  if (!planificador)
    return;
  planificador->limites = limites;
  planificador->tiempo_total = 0.0f;
}

static inline uint8_t Planificar_Perfil_s(planificador_perfil_s_t *planificador,
                                          float posicion_inicio,
                                          float posicion_final) {
  if (!planificador)
    return 0;

  float distancia = posicion_final - posicion_inicio;
  float abs_dist = fabsf(distancia);

  if (abs_dist < epsilon) {
    planificador->tiempo_total = 0;
    planificador->x0 = posicion_final;
    return 1;
  }
  planificador->direccion = (distancia > 0.0) ? 1.0 : -1.0;

  planificador->x0 = posicion_inicio;

  float v_max = planificador->limites.v_max;
  float a_max = planificador->limites.a_max;
  float j_max = planificador->limites.j_max;

  if (v_max * j_max < a_max * a_max) {
    a_max = sqrtf(v_max * j_max);
  }

  float t_j = a_max / j_max;
  float s_acc = a_max * (t_j * t_j + (v_max / a_max) * t_j);

  // Caso estándar: Se asume que alcanza v_max
  float t_a = (v_max - (a_max * a_max / j_max)) / a_max;

  // Si el desplazamiento es muy pequeño para alcanzar v_max
  if (abs_dist < 2.0f * (a_max * t_j * t_j + 0.5f * a_max * t_a * t_a +
                         1.5f * a_max * t_j * t_a)) {

    // Descontar fase de velocidad crucero y recalcular v_max pico
    t_a = 0.0;

    // Resolver ecuación cúbica reducida para aceleración pico
    float a_pico = cbrtf(abs_dist * j_max * j_max / 2.0);
    if (a_pico < a_max) {
      a_max = a_pico;
      t_j = a_max / j_max;
    } else {
      t_a = (abs_dist - 2.0 * a_max * t_j * t_j) /
            (a_max * t_j + a_max * a_max / j_max);
      if (t_a < 0.0)
        t_a = 0.0;
    }
    v_max = a_max * t_j + a_max * t_a;
  }
  planificador->t_1_3 = t_j;
  planificador->t_2 = t_a;
  planificador->t_5_7 = t_j;
  planificador->t_6 = t_a;

  planificador->a_max_act = a_max;
  planificador->a_min_act = -a_max;
  planificador->v_max_act = v_max;

  // Calcular tiempo de velocidad constante
  float d_acc = a_max * t_j * t_j + a_max * t_j * t_a + 0.5 * a_max * t_a * t_a;
  float d_dec = d_acc;
  float d_cruise = abs_dist - (d_acc + d_dec);

  if (d_cruise < 0.0)
    d_cruise = 0.0;
  planificador->t_4 = d_cruise / v_max;

  // Calcular marcas de tiempo absolutas acumuladas
  planificador->t1 = planificador->t_1_3;
  planificador->t2 = planificador->t1 + planificador->t_2;
  planificador->t3 = planificador->t2 + planificador->t_1_3;
  planificador->t4 = planificador->t3 + planificador->t_4;
  planificador->t5 = planificador->t4 + planificador->t_5_7;
  planificador->t6 = planificador->t5 + planificador->t_6;
  planificador->t7 = planificador->t6 + planificador->t_5_7;
  planificador->tiempo_total = planificador->t7;
  return 1;
}

static inline estado_perfil_s_t Estado_Perfil_s(planificador_perfil_s_t *p,
                                                float t) {
  estado_perfil_s_t estado = {0};

  if (t <= 0.0) { // Si no inicio
    estado.posicion = p->x0;
    estado.velocidad = 0.0;
    estado.aceleracion = 0.0;
    estado.tiron = 0.0;
    estado.termino = 0;
    return estado;
  }

  if (t >= p->tiempo_total) { // Si termino
    float total_dist = p->v_max_act * (p->t_4 + p->t_1_3 + p->t_2);
    estado.posicion = p->x0 + p->direccion * total_dist;
    estado.velocidad = 0.0;
    estado.aceleracion = 0.0;
    estado.tiron = 0.0;
    estado.termino = 1;
    return estado;
  }

  float j = 0.0, a = 0.0, v = 0.0, x = 0.0;
  float j_m = p->limites.j_max;
  float a_m = p->a_max_act;
  float v_m = p->v_max_act;

  // Evaluación de las 7 fases
  if (t <= p->t1) {
    // Fase 1: Jerk positivo, subiendo aceleración
    float dt = t;
    j = j_m;
    a = j * dt;
    v = 0.5f * j * dt * dt;
    x = (1.0f / 6.0f) * j * dt * dt * dt;
  } else if (t <= p->t2) {
    // Fase 2: Aceleración constante
    float dt = t - p->t1;
    float v1 = 0.5f * j_m * p->t_1_3 * p->t_1_3;
    float x1 = (1.0f / 6.0f) * j_m * p->t_1_3 * p->t_1_3 * p->t_1_3;

    j = 0.0f;
    a = a_m;
    v = v1 + a_m * dt;
    x = x1 + v1 * dt + 0.5f * a_m * dt * dt;
  } else if (t <= p->t3) {
    // Fase 3: Jerk negativo, reduciendo aceleración a 0
    float dt = t - p->t2;
    float v2 = 0.5 * j_m * p->t_1_3 * p->t_1_3 + a_m * p->t_2;
    float x2 = (1.0f / 6.0f) * j_m * p->t_1_3 * p->t_1_3 * p->t_1_3 +
               (0.5f * j_m * p->t_1_3 * p->t_1_3) * p->t_2 +
               0.5f * a_m * p->t_2 * p->t_2;

    j = -j_m;
    a = a_m - j_m * dt;
    v = v2 + a_m * dt - 0.5f * j_m * dt * dt;
    x = x2 + v2 * dt + 0.5f * a_m * dt * dt -
        (1.0f / 6.0f) * j_m * dt * dt * dt;
  } else if (t <= p->t4) {
    // Fase 4: Velocidad crucero constante
    float dt = t - p->t3;
    float x3 = a_m * p->t_1_3 * p->t_1_3 + a_m * p->t_1_3 * p->t_2 +
               0.5f * a_m * p->t_2 * p->t_2;

    j = 0.0f;
    a = 0.0f;
    v = v_m;
    x = x3 + v_m * dt;
  } else if (t <= p->t5) {
    /* Fase 5: Jerk negativo, aumentando desaceleración */
    float dt = t - p->t4;
    float x4 = a_m * p->t_1_3 * p->t_1_3 + a_m * p->t_1_3 * p->t_2 +
               0.5f * a_m * p->t_2 * p->t_2 + v_m * p->t_4;

    j = -j_m;
    a = -j_m * dt;
    v = v_m - 0.5f * j_m * dt * dt;
    x = x4 + v_m * dt - (1.0f / 6.0f) * j_m * dt * dt * dt;
  } else if (t <= p->t6) {
    // Fase 6: Desaceleración constante
    float dt = t - p->t5;
    float v5 = v_m - 0.5f * j_m * p->t_5_7 * p->t_5_7;
    float x4 = a_m * p->t_1_3 * p->t_1_3 + a_m * p->t_1_3 * p->t_2 +
               0.5f * a_m * p->t_2 * p->t_2 + v_m * p->t_4;
    float x5 = x4 + v_m * p->t_5_7 -
               (1.0f / 6.0f) * j_m * p->t_5_7 * p->t_5_7 * p->t_5_7;

    j = 0.0f;
    a = -a_m;
    v = v5 - a_m * dt;
    x = x5 + v5 * dt - 0.5f * a_m * dt * dt;
  } else {
    // Fase 7: Jerk positivo, llevando desaceleración a 0
    float dt = t - p->t6;
    float v6 = 0.5 * j_m * p->t_5_7 * p->t_5_7;
    float total_dist = v_m * (p->t_4 + p->t_1_3 + p->t_2);
    float x6 =
        total_dist - (v6 * p->t_5_7 - 0.5f * a_m * p->t_5_7 * p->t_5_7 +
                      (1.0f / 6.0f) * j_m * p->t_5_7 * p->t_5_7 * p->t_5_7);

    j = j_m;
    a = -a_m + j_m * dt;
    v = v6 - a_m * dt + 0.5f * j_m * dt * dt;
    x = x6 + v6 * dt - 0.5f * a_m * dt * dt +
        (1.0f / 6.0f) * j_m * dt * dt * dt;
  }

  // Mapeo según dirección del movimiento
  estado.tiron = p->direccion * j;
  estado.aceleracion = p->direccion * a;
  estado.velocidad = p->direccion * v;
  estado.posicion = p->x0 + p->direccion * x;
  estado.termino = 0;

  return estado;
}

#endif // SFPU_PERFIL_S_H
