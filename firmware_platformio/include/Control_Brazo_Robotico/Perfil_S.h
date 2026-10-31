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

  float x0;
  float target_dist;
  float direccion;

  float t_j; // Tiempo de subida de aceleración (Jerk)
  float t_a; // Tiempo de aceleración constante
  float t_v; // Tiempo de velocidad constante

  float t1, t2, t3, t4, t5, t6, t7;
  float a_max_act;
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

static inline uint8_t Planificar_Perfil_s(planificador_perfil_s_t *p,
                                          float posicion_inicio,
                                          float posicion_final) {
  if (!p)
    return 0;

  float distancia = posicion_final - posicion_inicio;
  float L = fabsf(distancia);

  if (L < epsilon) {
    p->tiempo_total = 0.0f;
    p->x0 = posicion_final;
    p->target_dist = 0.0f;
    return 1;
  }

  p->x0 = posicion_inicio;
  p->target_dist = L;
  p->direccion = (distancia > 0.0f) ? 1.0f : -1.0f;

  float v_m = p->limites.v_max;
  float a_m = p->limites.a_max;
  float j_m = p->limites.j_max;

  // Verificación de aceleración máxima alcanzable
  if (v_m * j_m < a_m * a_m) {
    a_m = sqrtf(v_m * j_m);
  }

  float t_j = a_m / j_m;
  float t_a = (v_m - a_m * t_j) / a_m;

  // Distancia mínima para alcanzar v_max (acelerar + desacelerar)
  float L_v = 2.0f * a_m * t_j * t_j + 3.0f * a_m * t_j * t_a + a_m * t_a * t_a;

  float t_v = 0.0f;

  if (L >= L_v) {
    // Alcanza velocidad crucero
    t_v = (L - L_v) / v_m;
  } else {
    // No alcanza v_max, recalcular
    t_v = 0.0f;
    // Chequear si alcanza a_max
    float L_a = 2.0f * a_m * t_j * t_j;
    if (L < L_a) {
      // Perfil triangular en Jerk
      a_m = cbrtf(0.5f * L * j_m * j_m);
      t_j = a_m / j_m;
      t_a = 0.0f;
      v_m = j_m * t_j * t_j;
    } else {
      // Alcanza a_m pero no v_m
      t_a = (-3.0f * t_j + sqrtf(t_j * t_j + 4.0f * L / a_m)) / 2.0f;
      if (t_a < 0.0f)
        t_a = 0.0f;
      v_m = a_m * (t_j + t_a);
    }
  }

  p->t_j = t_j;
  p->t_a = t_a;
  p->t_v = t_v;
  p->a_max_act = a_m;
  p->v_max_act = v_m;

  // Marcas de tiempo acumuladas
  p->t1 = t_j;
  p->t2 = p->t1 + t_a;
  p->t3 = p->t2 + t_j;
  p->t4 = p->t3 + t_v;
  p->t5 = p->t4 + t_j;
  p->t6 = p->t5 + t_a;
  p->t7 = p->t6 + t_j;
  p->tiempo_total = p->t7;

  return 1;
}

static inline estado_perfil_s_t Estado_Perfil_s(planificador_perfil_s_t *p,
                                                float t) {
  estado_perfil_s_t estado = {0};

  if (t <= 0.0f) {
    estado.posicion = p->x0;
    return estado;
  }

  if (t >= p->tiempo_total) {
    estado.posicion = p->x0 + p->direccion * p->target_dist;
    estado.termino = 1;
    return estado;
  }

  float j_m = p->limites.j_max;
  float a_m = p->a_max_act;
  float v_m = p->v_max_act;
  float t_j = p->t_j;
  float t_a = p->t_a;

  // Puntos de cambio de estado integrados exactamente
  float x1 = (1.0f / 6.0f) * j_m * t_j * t_j * t_j;
  float v1 = 0.5f * j_m * t_j * t_j;

  float x2 = x1 + v1 * t_a + 0.5f * a_m * t_a * t_a;
  float v2 = v1 + a_m * t_a;

  float x3 = x2 + v2 * t_j + 0.5f * a_m * t_j * t_j -
             (1.0f / 6.0f) * j_m * t_j * t_j * t_j;
  float v3 = v_m;

  float x4 = x3 + v_m * p->t_v;

  float x5 = x4 + v_m * t_j - (1.0f / 6.0f) * j_m * t_j * t_j * t_j;
  float v5 = v_m - 0.5f * j_m * t_j * t_j;

  float x6 = x5 + v5 * t_a - 0.5f * a_m * t_a * t_a;
  float v6 = 0.5f * j_m * t_j * t_j;

  float x = 0.0f, v = 0.0f, a = 0.0f, j = 0.0f;

  if (t <= p->t1) {
    float dt = t;
    j = j_m;
    a = j_m * dt;
    v = 0.5f * j_m * dt * dt;
    x = (1.0f / 6.0f) * j_m * dt * dt * dt;
  } else if (t <= p->t2) {
    float dt = t - p->t1;
    j = 0.0f;
    a = a_m;
    v = v1 + a_m * dt;
    x = x1 + v1 * dt + 0.5f * a_m * dt * dt;
  } else if (t <= p->t3) {
    float dt = t - p->t2;
    j = -j_m;
    a = a_m - j_m * dt;
    v = v2 + a_m * dt - 0.5f * j_m * dt * dt;
    x = x2 + v2 * dt + 0.5f * a_m * dt * dt -
        (1.0f / 6.0f) * j_m * dt * dt * dt;
  } else if (t <= p->t4) {
    float dt = t - p->t3;
    j = 0.0f;
    a = 0.0f;
    v = v_m;
    x = x3 + v_m * dt;
  } else if (t <= p->t5) {
    float dt = t - p->t4;
    j = -j_m;
    a = -j_m * dt;
    v = v_m - 0.5f * j_m * dt * dt;
    x = x4 + v_m * dt - (1.0f / 6.0f) * j_m * dt * dt * dt;
  } else if (t <= p->t6) {
    float dt = t - p->t5;
    j = 0.0f;
    a = -a_m;
    v = v5 - a_m * dt;
    x = x5 + v5 * dt - 0.5f * a_m * dt * dt;
  } else {
    float dt = t - p->t6;
    j = j_m;
    a = -a_m + j_m * dt;
    v = v6 - a_m * dt + 0.5f * j_m * dt * dt;
    x = x6 + v6 * dt - 0.5f * a_m * dt * dt +
        (1.0f / 6.0f) * j_m * dt * dt * dt;
  }

  estado.tiron = p->direccion * j;
  estado.aceleracion = p->direccion * a;
  estado.velocidad = p->direccion * v;
  estado.posicion = p->x0 + p->direccion * x;
  estado.termino = 0;

  return estado;
}

#endif // SFPU_PERFIL_S_H
