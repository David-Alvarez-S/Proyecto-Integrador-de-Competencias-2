#ifndef VECTORES_H
#define VECTORES_H

#include "Aritmetica.h"
#include <math.h>
#include <stdint.h>

typedef struct {
  float x;
  float y;
  float z;
} vector_t;

static inline void vect_suma(vector_t *out, vector_t in1, vector_t in2) {
  if (!out)
    return;
  out->x = in1.x + in2.x;
  out->y = in1.y + in2.y;
  out->z = in1.z + in2.z;
}
static inline void vect_resta(vector_t *out, vector_t in1, vector_t in2) {
  if (!out)
    return;
  out->x = in1.x - in2.x;
  out->y = in1.y - in2.y;
  out->z = in1.z - in2.z;
}
static inline void vect_escala(vector_t *out, vector_t in, float escala) {
  if (!out)
    return;

  out->x = in.x * escala;
  out->y = in.y * escala;
  out->z = in.z * escala;
}

static inline void vect_norm(vector_t *out, vector_t in) {
  if (!out)
    return;
  float escala = sqrtf((in.x * in.x) + (in.y * in.y) + (in.z * in.z));
  out->x = in.x / escala;
  out->y = in.y / escala;
  out->z = in.z / escala;
}

static inline float vect_magnitud(vector_t in) {
  return sqrtf((in.x * in.x) + (in.y * in.y) + (in.z * in.z));
}

// Interpolación lineal entre 'ini' y 'fin'. t se espera en [0,1], pero
// no se clampea acá a propósito: si el llamador se pasa (t<0 o t>1) por
// error de redondeo en el lazo, prefiero que se note en el resultado
// antes que esconderlo silenciosamente.
static inline void vect_lerp(vector_t *out, vector_t ini, vector_t fin,
                             float t) {
  if (!out)
    return;
  out->x = ini.x + (fin.x - ini.x) * t;
  out->y = ini.y + (fin.y - ini.y) * t;
  out->z = ini.z + (fin.z - ini.z) * t;
}

#endif // VECTORES_H
