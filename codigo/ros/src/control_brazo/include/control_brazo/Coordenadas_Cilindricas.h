#ifndef COORDENADAS_CILINDRICAS_H
#define COORDENADAS_CILINDRICAS_H

#include "Vectores.h"
#include <math.h>

typedef struct {
  float r, theta, z;
} coor_cilindricas_t;

// Conversión Cartesiano → Cilíndrico
static inline coor_cilindricas_t vect_a_cilindrico(vector_t v) {
  coor_cilindricas_t c;
  c.r = sqrtf(v.x * v.x + v.y * v.y);
  c.theta = atan2f(v.y, v.x);
  c.z = v.z;
  return c;
}

// Conversión Cilíndrico → Cartesiano
static inline vector_t cili_a_cartesiano(coor_cilindricas_t c) {
  vector_t v;
  v.x = c.r * cosf(c.theta);
  v.y = c.r * sinf(c.theta);
  v.z = c.z;
  return v;
}
#endif // COORDENADAS_CILINDRICAS_H
