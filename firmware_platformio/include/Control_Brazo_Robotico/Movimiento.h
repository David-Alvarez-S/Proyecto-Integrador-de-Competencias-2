#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H

#include "Coordenadas_Cilindricas.h"
#include "Global_Config.h"
#include "IK.h"
#include "Interpolaciones.h"
#include "Perfil_S.h"
#include "Vectores.h"
#include <stdbool.h>

typedef enum {
  INTERPOLACION_CARTESIANA, // Línea recta cartesiana 3D (para avances)
  INTERPOLACION_CILINDRICA  // Arco periférico (para grandes giros de la base)
} modo_interpolacion_t;

typedef struct {
  servomotores_h inicio;
  servomotores_h fin;
  vector_t v_inicio;
  vector_t v_fin;
  coor_cilindricas_t c_inicio;
  coor_cilindricas_t c_fin;
  planificador_perfil_s_t perfil;
  modo_interpolacion_t modo;
  bool activo;
} movimiento_t;

static inline bool Movimiento_Es_Alcanzable(vector_t destino) {
  return Validar_Vector_Entrada(destino);
}

static inline bool Movimiento_Iniciar(movimiento_t *m, servomotores_h actual,
                                      vector_t inicio, vector_t destino,
                                      limites_perfil_s_t limites,
                                      ik_continuidad_t *continuidad) {
  if (!m)
    return false;
  (void)continuidad;

  servomotores_h fin;
  if (!IK_ext(&fin, destino, 0.0f, 0.0f, NULL, NULL))
    return false;

  m->inicio = actual;
  m->fin = fin;

  m->v_inicio = inicio;
  m->v_fin = destino;

  m->c_inicio = vect_a_cilindrico(inicio);
  m->c_fin = vect_a_cilindrico(destino);

  float dtheta = fabsf(fin.sm1.angulo_objetivo - actual.sm1.angulo_objetivo);

  // Si la rotación de la base es pequeña (<= 45° / 0.785 rad),
  // se activa el MODO CARTESIANO para garantizar una LÍNEA RECTA perfecta.
  if (dtheta <= 0.785f) {
    m->modo = INTERPOLACION_CARTESIANA;
  } else {
    m->modo = INTERPOLACION_CILINDRICA;
  }

  Iniciar_Perfil_S(&m->perfil, limites);
  Planificar_Perfil_s(&m->perfil, 0.0f, 1.0f);
  m->activo = true;
  return true;
}

static inline bool Movimiento_Evaluar(servomotores_h *out, movimiento_t *m,
                                      float t, ik_continuidad_t *continuidad,
                                      float *progreso_out) {
  if (!out || !m || !m->activo)
    return false;

  estado_perfil_s_t estado = Estado_Perfil_s(&m->perfil, t);
  float s = estado.posicion;

  if (m->modo == INTERPOLACION_CARTESIANA) {
    // Interpolación LERP sobre el vector 3D cartesiano (Trayectoria en Línea
    // Recta)
    vector_t v_inter;
    vect_lerp(&v_inter, m->v_inicio, m->v_fin, s);

    //  Cinemática Inversa punto a punto en cada paso del Perfil S
    if (!IK_ext(out, v_inter, 0.0f, 0.0f, continuidad, NULL)) {
      m->activo = false;
      return false;
    }
  } else {
    // Interpolación en arco para cambios grandes de orientación de la base
    coor_cilindricas_t c_inter = lerp_cilindrico(m->c_inicio, m->c_fin, s);
    vector_t v_inter = cili_a_cartesiano(c_inter);

    if (!IK_ext(out, v_inter, 0.0f, 0.0f, continuidad, NULL)) {
      m->activo = false;
      return false;
    }
  }

  if (progreso_out)
    *progreso_out = s;

  if (estado.termino) {
    m->activo = false;
    return false;
  }
  return true;
}

#endif // MOVIMIENTO_H
