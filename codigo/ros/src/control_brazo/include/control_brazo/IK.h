#ifndef IK_H
#define IK_H

#include "Global_Config.h"
#include "Vectores.h"
#include <math.h>
#include <stdbool.h>

static inline bool IK(servomotores_h *out, vector_t a) {
  if (!out)
    return false;

  float angulo1, angulo2, angulo3, angulo4, aux;

  // Ángulo de la base
  angulo1 = atan2f(a.y, a.x);

  // Radio proyectado total en el plano XY
  float r_total = sqrtf((a.x * a.x) + (a.y * a.y));

  // Radio proyectado hasta la muñeca (Servomotor 4)
  float r_muñeca = r_total - L_ESLABON_3 - L_HERRAMIENTA;

  // Distancia D entre el hombro (SM2) y la muñeca (SM4)
  float D = sqrtf((r_muñeca * r_muñeca) + (a.z * a.z));

  // Verificar si la posición está al alcance del brazo (L1 + L2)
  if (D > (L_ESLABON_1 + L_ESLABON_2) || D < fabsf(L_ESLABON_1 - L_ESLABON_2))
    return false;

  // Ángulo del Codo (Servomotor 3) mediante Ley de Cosenos
  aux = ((L_ESLABON_1 * L_ESLABON_1) + (L_ESLABON_2 * L_ESLABON_2) - (D * D)) /
        (2.0f * L_ESLABON_1 * L_ESLABON_2);

  if (aux > 1.0f)
    aux = 1.0f;
  if (aux < -1.0f)
    aux = -1.0f;

  angulo3 = PI - acosf(aux);

  // Ángulo del Hombro (Servomotor 2)
  aux = ((L_ESLABON_1 * L_ESLABON_1) + (D * D) - (L_ESLABON_2 * L_ESLABON_2)) /
        (2.0f * L_ESLABON_1 * D);

  if (aux > 1.0f)
    aux = 1.0f;
  if (aux < -1.0f)
    aux = -1.0f;

  angulo2 = atan2f(a.z, r_muñeca) + acosf(aux);

  // Ángulo de la Muñeca (Servomotor 4) para mantener la herramienta
  // horizontal
  angulo4 = PI - (angulo2 + angulo3);

  // Asignar salidas con los offsets correspondientes para RViz
  out->sm1.angulo_objetivo = angulo1;
  out->sm2.angulo_objetivo = (PI / 2.0f) - angulo2; // Offset vertical de 90°
  out->sm3.angulo_objetivo = (PI / 2.0f) - angulo3; // Offset vertical de 90
  out->sm4.angulo_objetivo = (PI / 2.0f) - angulo4; // Offset vertical de 90°

  return true;
}
#endif // IK_H
