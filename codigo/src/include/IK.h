#ifndef IK_H
#define IK_H

#include "Global_Config.h"
#include "Vectores.h"
#include <math.h>

static inline void IK(servomotores_h *out, vector_t a) {
  if (!out)
    return;

  // 1. Ángulo de la base (Yaw)
  float angulo1 = atan2f(a.y, a.x);

  // 2. Radio total en el plano XY
  float r_total = sqrtf((a.x * a.x) + (a.y * a.y));

  // 3. Radio proyectado hasta la muñeca
  float r_muneca = r_total - L_ESLABON_3 - L_HERRAMIENTA;

  // Si el objetivo está tan cerca que la muñeca queda detrás del hombro
  if (r_muneca < 0.0f) {
    return; // Ignora el cálculo para prevenir valores indeterminados
  }

  // 4. Distancia D (entre hombro y muñeca)
  float D = sqrtf((r_muneca * r_muneca) + (a.z * a.z));

  // Validación de alcance físico (L1 + L2)
  float alcance_max = L_ESLABON_1 + L_ESLABON_2;
  float alcance_min = fabsf(L_ESLABON_1 - L_ESLABON_2);

  if (D > alcance_max || D < alcance_min || D < 1e-4f) {
    return; // Punto fuera de alcance: mantiene la última posición válida
  }

  // 5. Codo (Servomotor 3) mediante Ley de Cosenos
  float aux3 =
      ((L_ESLABON_1 * L_ESLABON_1) + (L_ESLABON_2 * L_ESLABON_2) - (D * D)) /
      (2.0f * L_ESLABON_1 * L_ESLABON_2);

  // Clampeo estricto para evitar NaN en acosf
  if (aux3 > 1.0f)
    aux3 = 1.0f;
  if (aux3 < -1.0f)
    aux3 = -1.0f;

  float angulo3 = PI - acosf(aux3);

  // 6. Hombro (Servomotor 2)
  float aux2 =
      ((L_ESLABON_1 * L_ESLABON_1) + (D * D) - (L_ESLABON_2 * L_ESLABON_2)) /
      (2.0f * L_ESLABON_1 * D);

  if (aux2 > 1.0f)
    aux2 = 1.0f;
  if (aux2 < -1.0f)
    aux2 = -1.0f;

  float angulo2 = atan2f(a.z, r_muneca) + acosf(aux2);

  // 7. Muñeca (Servomotor 4) para mantener la herramienta en horizontal
  float angulo4 = PI - (angulo2 + angulo3);

  // 8. Aplicar desfases y asignación final a los servomotores
  out->sm1.angulo_objetivo = angulo1;
  out->sm2.angulo_objetivo = (PI / 2.0f) - angulo2;
  out->sm3.angulo_objetivo = (PI / 2.0f) - angulo3;
  out->sm4.angulo_objetivo = (PI / 2.0f) - angulo4;
}

#endif // IK_H
