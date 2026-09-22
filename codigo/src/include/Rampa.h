#ifndef SFPU_RAMPA_H
#define SFPU_RAMPA_H

#include <stdint.h>

typedef struct {
  float Velocidad_Actual;
  float Velocidad_Objetivo;
  float Aceleracion;
  float Desaceleracion;
} Rampa;

static inline void Iniciar_Rampa(Rampa *out, float Velocidad_Actual,
                                 float Velocidad_Objetivo, float Aceleracion,
                                 float Desaceleracion) {
  if (!out)
    return;
  out->Velocidad_Actual = Velocidad_Actual;
  out->Velocidad_Objetivo = Velocidad_Objetivo;
  out->Aceleracion = Aceleracion;
  out->Desaceleracion = Desaceleracion;
}

static inline void Actualizar_Aceleracion(Rampa *out, float Aceleracion) {
  if (!out)
    return;
  out->Aceleracion = Aceleracion;
}
static inline void Actualizar_Desaceleracion(Rampa *out, float Desaceleracion) {
  if (!out)
    return;
  out->Desaceleracion = Desaceleracion;
}
static inline void Actualizar_Velocidad_Objetivo(Rampa *out,
                                                 float Velocidad_Objetivo) {
  if (!out)
    return;
  out->Velocidad_Objetivo = Velocidad_Objetivo;
}

static inline void Ejecutar_Rampa(Rampa *out) {
  if (!out)
    return;

  if (out->Velocidad_Objetivo <=
      out->Velocidad_Actual) { // Si tengo que desacelerar
    if (out->Velocidad_Actual < out->Desaceleracion) { // Evitar Underflow
      out->Velocidad_Actual = out->Velocidad_Objetivo;
      return;
    } else if (out->Velocidad_Actual - out->Desaceleracion <=
               out->Velocidad_Objetivo) { // Si me paso
      out->Velocidad_Actual = out->Velocidad_Objetivo;
      return;
    }
    out->Velocidad_Actual -= out->Desaceleracion; // Desacelero
  } else {                                        // Si tengo que acelerar
    if (UINT16_MAX - out->Velocidad_Actual <
        out->Aceleracion) { // Evitar Overflow
      out->Velocidad_Actual = out->Velocidad_Objetivo;
      return;
    } else if (out->Velocidad_Actual + out->Aceleracion >
               out->Velocidad_Objetivo) { // Si me paso
      out->Velocidad_Actual = out->Velocidad_Objetivo;
      return;
    }
    out->Velocidad_Actual += out->Aceleracion;
  }
  return;
}
#endif // SFPU_RAMPA_H
