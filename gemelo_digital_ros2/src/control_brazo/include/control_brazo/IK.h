#ifndef IK_H
#define IK_H

#include "Global_Config.h"
#include "Vectores.h"
#include <math.h>
#include <stdbool.h>

#define IK_D_EPS 1e-4f
#define Z_MIN_MESA 10.0f

typedef struct {
  float base_ang;
  bool base_init;
} ik_continuidad_t;

// --- Validaciones Geométricas de Límites ---
static inline bool IK_es_alcanzable(vector_t a) {
  float az = a.z - L_SHOULDER_Z;
  float r_total = sqrtf((a.x * a.x) + (a.y * a.y));

  if (r_total < RADIO_MIN_IK)
    return false;

  // Evaluar ángulo de la base
  float a1 = atan2f(a.y, a.x);
  if (a1 > SM1_ANG_MAX || a1 < SM1_ANG_MIN)
    return false;

  float r_muneca = r_total - L_ESLABON_3 - L_HERRAMIENTA;
  float D = sqrtf((r_muneca * r_muneca) + (az * az));

  float D_max = L_ESLABON_1 + L_ESLABON_2;
  float D_min = fabsf(L_ESLABON_1 - L_ESLABON_2);
  if (D > D_max || D < D_min)
    return false;

  float aux3 =
      ((L_ESLABON_1 * L_ESLABON_1) + (L_ESLABON_2 * L_ESLABON_2) - (D * D)) /
      (2.0f * L_ESLABON_1 * L_ESLABON_2);
  if (aux3 > 1.0f || aux3 < -1.0f)
    return false;
  float angulo3 = PI - acosf(aux3);

  float aux2 =
      ((L_ESLABON_1 * L_ESLABON_1) + (D * D) - (L_ESLABON_2 * L_ESLABON_2)) /
      (2.0f * L_ESLABON_1 * D);
  if (aux2 > 1.0f || aux2 < -1.0f)
    return false;
  float angulo2 = atan2f(az, r_muneca) + acosf(aux2);

  float sm2_cmd = (PI / 2.0f) - angulo2;
  float sm3_cmd = angulo3;

  // Mantener la herramienta horizontal
  float sm4_cmd = angulo3 - angulo2;

  // Validar límites de las articulaciones
  if (sm2_cmd > SM2_ANG_MAX || sm2_cmd < SM2_ANG_MIN)
    return false;
  if (sm3_cmd > SM3_ANG_MAX || sm3_cmd < SM3_ANG_MIN)
    return false;
  if (sm4_cmd > SM4_ANG_MAX || sm4_cmd < SM4_ANG_MIN)
    return false;

  return true;
}

// Filtro previo para validar el vector cartesiano de entrada en el espacio de
// trabajo
static inline bool Validar_Vector_Entrada(vector_t v) {
  // Protección física de colisión contra el suelo o mesa
  if (v.z < Z_MIN_MESA)
    return false;

  // Ángulo azimutal de la base dentro de límites
  float theta_base = atan2f(v.y, v.x);
  if (theta_base < SM1_ANG_MIN || theta_base > SM1_ANG_MAX)
    return false;

  // Extensión radial válida
  float r = sqrtf(v.x * v.x + v.y * v.y);
  float r_max = L_ESLABON_1 + L_ESLABON_2 + L_ESLABON_3 + L_HERRAMIENTA;
  if (r < RADIO_MIN_IK || r > r_max)
    return false;

  // 4. Verificación kinemática completa
  return IK_es_alcanzable(v);
}

// --- Cálculo de Cinemática Inversa Extendida ---
static inline bool IK_ext(servomotores_h *out, vector_t a, float error_max,
                          float dt, ik_continuidad_t *continuidad,
                          float *error_out) {
  (void)dt;
  // Solo exigimos que 'out' no sea NULL (continuidad puede ser NULL)
  if (!out)
    return false;

  float error_total = 0.0f;
  float az = a.z - L_SHOULDER_Z;
  float ax = a.x;
  float ay = a.y;

  float r_total = sqrtf((ax * ax) + (ay * ay));

  if (r_total < RADIO_MIN_IK) {
    float err_r = RADIO_MIN_IK - r_total;
    error_total += err_r;
    if (error_total > error_max) {
      if (error_out)
        *error_out = error_total;
      return false;
    }
    ax = RADIO_MIN_IK * cosf(atan2f(ay, ax));
    ay = RADIO_MIN_IK * sinf(atan2f(ay, ax));
    r_total = RADIO_MIN_IK;
  }

  float a1 = atan2f(ay, ax);

  if (a1 > SM1_ANG_MAX || a1 < SM1_ANG_MIN) {
    if (error_out)
      *error_out = 999.0f;
    return false;
  }

  float a1_temp = a1;
  // Solo ejecutamos unwrapping si el puntero de continuidad NO es NULL
  if (continuidad && continuidad->base_init) {
    float d = a1_temp - continuidad->base_ang;
    while (d > PI) {
      a1_temp -= 2.0f * PI;
      d -= 2.0f * PI;
    }
    while (d < -PI) {
      a1_temp += 2.0f * PI;
      d += 2.0f * PI;
    }
  }

  float r_muneca = r_total - L_ESLABON_3 - L_HERRAMIENTA;
  float D = sqrtf((r_muneca * r_muneca) + (az * az));

  float D_max = L_ESLABON_1 + L_ESLABON_2;
  float D_min = fabsf(L_ESLABON_1 - L_ESLABON_2);

  if (D > D_max || D < D_min) {
    float D_clamped = (D > D_max) ? (D_max - IK_D_EPS) : (D_min + IK_D_EPS);
    error_total += fabsf(D - D_clamped);
    if (error_total > error_max) {
      if (error_out)
        *error_out = error_total;
      return false;
    }
    if (D > IK_D_EPS) {
      float k = D_clamped / D;
      r_muneca *= k;
      az *= k;
    } else {
      r_muneca = D_clamped;
      az = 0.0f;
    }
    D = D_clamped;
  }

  float aux3 =
      ((L_ESLABON_1 * L_ESLABON_1) + (L_ESLABON_2 * L_ESLABON_2) - (D * D)) /
      (2.0f * L_ESLABON_1 * L_ESLABON_2);
  aux3 = (aux3 > 1.0f) ? 1.0f : ((aux3 < -1.0f) ? -1.0f : aux3);
  float angulo3 = PI - acosf(aux3);

  float aux2 =
      ((L_ESLABON_1 * L_ESLABON_1) + (D * D) - (L_ESLABON_2 * L_ESLABON_2)) /
      (2.0f * L_ESLABON_1 * D);
  aux2 = (aux2 > 1.0f) ? 1.0f : ((aux2 < -1.0f) ? -1.0f : aux2);
  float angulo2 = atan2f(az, r_muneca) + acosf(aux2);

  float sm2_cmd = (PI / 2.0f) - angulo2;
  float sm3_cmd = angulo3;
  float sm4_cmd = angulo3 - angulo2;

  if (a1_temp < SM1_ANG_MIN || a1_temp > SM1_ANG_MAX || sm2_cmd < SM2_ANG_MIN ||
      sm2_cmd > SM2_ANG_MAX || sm3_cmd < SM3_ANG_MIN || sm3_cmd > SM3_ANG_MAX ||
      sm4_cmd < SM4_ANG_MIN || sm4_cmd > SM4_ANG_MAX) {
    if (error_out)
      *error_out = 999.0f;
    return false;
  }

  if (continuidad) {
    continuidad->base_ang = a1_temp;
    continuidad->base_init = true;
  }

  out->sm1.angulo_objetivo = a1_temp;
  out->sm2.angulo_objetivo = sm2_cmd;
  out->sm3.angulo_objetivo = sm3_cmd;
  out->sm4.angulo_objetivo = sm4_cmd;

  if (error_out)
    *error_out = error_total;
  return true;
}
static inline bool IK(servomotores_h *out, vector_t a,
                      ik_continuidad_t *continuidad) {
  return IK_ext(out, a, 0.0f, 0.0f, continuidad, NULL);
}

#endif // IK_H
