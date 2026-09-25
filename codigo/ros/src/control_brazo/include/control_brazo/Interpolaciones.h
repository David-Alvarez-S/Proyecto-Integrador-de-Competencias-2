#ifndef INTERPOLACIONES_H
#define INTERPOLACIONES_H

#include "Cuaterniones.h"
#include "Vectores.h"
#include <math.h>
#include <stdint.h>

static inline void lerp(vector_t *out, vector_t v1, vector_t v2, float s) {
  if (!out)
    return;

  vector_t aux;
  vect_resta(&aux, v2, v1);  // aux = v2 - v1
  vect_escala(&aux, aux, s); // aux = s * (v2 - v1)
  vect_suma(out, v1, aux);   // out = v1 + aux
}

static inline void slerp(cuaternion_t *out, cuaternion_t c1,
                         const cuaternion_t *c2, float s) {
  if (!out || !c2)
    return;

  cuaternion_t c2_local = *c2;
  float d = cuat_dot(c1, c2_local); // Producto punto

  if (d < 0.0f) {
    c2_local.w = -c2_local.w;
    c2_local.x = -c2_local.x;
    c2_local.y = -c2_local.y;
    c2_local.z = -c2_local.z;
    d = -d;
  }

  // Prevenir desbordamiento de acosf por imprecisión en flotantes
  if (d > 1.0f) {
    d = 1.0f;
  }

  if (d > 0.9995f) {
    cuaternion_t a, b;
    cuat_escala(&a, c1, 1.0f - s); // a = (1 - s) * c1
    cuat_escala(&b, c2_local, s);  // b = s * c2_local
    cuat_suma(out, a, b);          // out = a + b
    norm_cuat(out);                // Garantiza norma unitaria exacta
    return;
  }

  float omega = acosf(d);
  float sin_omega = sinf(omega);

  float factor_a = sinf((1.0f - s) * omega) / sin_omega;
  float factor_b = sinf(s * omega) / sin_omega;

  cuaternion_t a, b;
  cuat_escala(&a, c1, factor_a);
  cuat_escala(&b, c2_local, factor_b);

  cuat_suma(out, a, b);
}

#endif // INTERPOLACIONES_H
