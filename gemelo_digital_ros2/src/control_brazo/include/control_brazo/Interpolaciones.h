#ifndef INTERPOLACIONES_H
#define INTERPOLACIONES_H

#include "Coordenadas_Cilindricas.h"
#include "Global_Config.h"
#include "Vectores.h"
#include <math.h>
#include <stdint.h>

static inline void lerp(vector_t *out, vector_t v1, vector_t v2, float s) {
  if (!out)
    return;

  vector_t aux;
  vect_resta(&aux, v2, v1);
  vect_escala(&aux, aux, s);
  vect_suma(out, v1, aux);
}

static inline float lerp_angulo_limitado(float ini, float fin, float s,
                                         float min_lim, float max_lim) {
  float d_directo = fin - ini;

  float pos_intermedia = ini + s * d_directo;

  if (pos_intermedia < min_lim || pos_intermedia > max_lim) {
    if (d_directo > 0.0f && (ini + d_directo) > max_lim) {
      d_directo -= 2.0f * PI;
    } else if (d_directo < 0.0f && (ini + d_directo) < min_lim) {
      d_directo += 2.0f * PI;
    }
  }

  return ini + s * d_directo;
}

static inline coor_cilindricas_t
lerp_cilindrico(coor_cilindricas_t c1, coor_cilindricas_t c2, float s) {
  coor_cilindricas_t out;
  out.r = c1.r + s * (c2.r - c1.r);
  out.z = c1.z + s * (c2.z - c1.z);

  out.theta =
      lerp_angulo_limitado(c1.theta, c2.theta, s, SM1_ANG_MIN, SM1_ANG_MAX);

  return out;
}

static inline float lerp_angulo(float ini, float fin, float s) {
  return ini + (fin - ini) * s;
}

#endif // INTERPOLACIONES_H
