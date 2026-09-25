#ifndef CUATERNIONES_H
#define CUATERNIONES_H

#include "Aritmetica.h"
#include <math.h>
#include <stdint.h>

typedef struct {
  float w;
  float x;
  float y;
  float z;
} cuaternion_t;

static inline cuaternion_t inicializar_cuat() {
  return (cuaternion_t){0, 0, 0, 0};
}

static inline void norm_cuat(cuaternion_t *in) {
  if (!in)
    return;
  float modulo =
      (in->w * in->w) + (in->x * in->x) + (in->y * in->y) + (in->z * in->z);
  modulo = sqrtf(modulo);
  in->w = in->w / modulo;
  in->x = in->x / modulo;
  in->y = in->y / modulo;
  in->z = in->z / modulo;
}

static inline void grad_2_cuat(cuaternion_t *out, euler_grad_t in) {
  if (!out)
    return;

  rad_t r = in.r / 2;
  rad_t p = in.p / 2;
  rad_t y = in.y / 2;

  out->w = cosf(r) * cosf(p) * cosf(y) + sinf(r) * sinf(p) * sinf(y);
  out->x = sinf(r) * cosf(p) * cosf(y) - cosf(r) * sinf(p) * sinf(y);
  out->y = cosf(r) * sinf(p) * cosf(y) + sinf(r) * cosf(p) * sinf(y);
  out->z = cosf(r) * cosf(p) * sinf(y) - sinf(r) * sinf(p) * cosf(y);
};

static inline void cuat_suma(cuaternion_t *out, cuaternion_t a,
                             cuaternion_t b) {
  if (!out)
    return;

  out->w = a.w + b.w;
  out->x = a.x + b.x;
  out->y = a.y + b.y;
  out->z = a.z + b.z;
}
static inline void cuat_mult(cuaternion_t *out, cuaternion_t a,
                             cuaternion_t b) {
  out->w = (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z);
  out->x = (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y);
  out->y = (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x);
  out->z = (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w);
}
static inline void cuat_inv(cuaternion_t *out, cuaternion_t in) {
  if (!out)
    return;
  out->w = in.w;
  out->x = -in.x;
  out->y = -in.y;
  out->z = -in.z;
};

static inline float cuat_dot(cuaternion_t a, cuaternion_t b) {
  return ((a.w * b.w) + (a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

static inline void cuat_escala(cuaternion_t *out, cuaternion_t a,
                               float escala) {
  if (!out)
    return;
  out->w = a.w * escala;
  out->x = a.x * escala;
  out->y = a.y * escala;
  out->z = a.z * escala;
}

#endif // CUATERNIONES_H
