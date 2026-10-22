#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H

#include "Global_Config.h"
#include "IK.h"
#include "Interpolaciones.h" // lerp_angulo
#include "Perfil_S.h"
#include "Vectores.h"
#include <stdbool.h>

typedef struct {
  servomotores_h inicio;
  servomotores_h fin;
  planificador_perfil_s_t perfil;
  bool activo;
} movimiento_t;

static inline bool Movimiento_Es_Alcanzable(vector_t destino) {
  return IK_es_alcanzable(destino);
}

static inline bool Movimiento_Iniciar(movimiento_t *m, servomotores_h actual,
                                      vector_t destino,
                                      limites_perfil_s_t limites,
                                      ik_continuidad_t *continuidad) {
  if (!m)
    return false;

  servomotores_h fin;
  if (!IK_ext(&fin, destino, 0.0f, 0.0f, continuidad, NULL))
    return false;

  m->inicio = actual;
  m->fin = fin;
  Iniciar_Perfil_S(&m->perfil, limites);
  Planificar_Perfil_s(&m->perfil, 0.0f, 1.0f);
  m->activo = true;
  return true;
}

static inline bool Movimiento_Evaluar(servomotores_h *out, movimiento_t *m,
                                      float t, float *progreso_out) {
  if (!out || !m || !m->activo)
    return false;

  estado_perfil_s_t estado = Estado_Perfil_s(&m->perfil, t);
  float s = estado.posicion;

  out->sm1.angulo_objetivo =
      lerp_angulo(m->inicio.sm1.angulo_objetivo, m->fin.sm1.angulo_objetivo, s);
  out->sm2.angulo_objetivo =
      lerp_angulo(m->inicio.sm2.angulo_objetivo, m->fin.sm2.angulo_objetivo, s);
  out->sm3.angulo_objetivo =
      lerp_angulo(m->inicio.sm3.angulo_objetivo, m->fin.sm3.angulo_objetivo, s);
  out->sm4.angulo_objetivo =
      lerp_angulo(m->inicio.sm4.angulo_objetivo, m->fin.sm4.angulo_objetivo, s);

  if (progreso_out)
    *progreso_out = s;

  if (estado.termino) {
    m->activo = false;
    return false;
  }
  return true;
}

#endif // MOVIMIENTO_H
