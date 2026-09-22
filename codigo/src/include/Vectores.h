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

vector_t inicializar_vec();

void vect_suma(vector_t *out, vector_t in1, vector_t in2);
void vect_resta(vector_t *out, vector_t in1, vector_t in2);
void vect_escala(vector_t *out, vector_t in, float escala);
void vect_norm(vector_t *out, vector_t in);
float vect_magnitud(vector_t in);

#endif // VECTORES_H
#ifdef VECTORES_IMPLEMENTATION

void vect_suma(vector_t *out, vector_t in1, vector_t in2) {
  if (!out)
    return;
  out->x = in1.x + in2.x;
  out->y = in1.y + in2.y;
  out->z = in1.z + in2.z;
}
void vect_resta(vector_t *out, vector_t in1, vector_t in2) {
  if (!out)
    return;
  out->x = in1.x - in2.x;
  out->y = in1.y - in2.y;
  out->z = in1.z - in2.z;
}
void vect_escala(vector_t *out, vector_t in, float escala) {
  if (!out)
    return;

  out->x = in.x * escala;
  out->y = in.y * escala;
  out->z = in.z * escala;
}

void vect_norm(vector_t *out, vector_t in) {
  if (!out)
    return;
  float escala = sqrtf((in.x * in.x) + (in.y * in.y) + (in.z * in.z));
  out->x = in.x / escala;
  out->y = in.y / escala;
  out->z = in.z / escala;
}

float vect_magnitud(vector_t in) {
  return sqrtf((in.x * in.x) + (in.y * in.y) + (in.z * in.z));
}

#endif // VECTORES_IMPLEMENTATION
