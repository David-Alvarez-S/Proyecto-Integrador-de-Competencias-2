#ifndef IK_H
#define IK_H

#include "Global_Config.h"
#include "Vectores.h"
#include <math.h>
#include <stdbool.h>

#define IK_D_EPS 1e-4f

#define IK_D_SINGULAR_EPS 1e-4f
static inline bool IK_es_alcanzable(vector_t a) {
  float az = a.z - L_SHOULDER_Z;
  float r_total = sqrtf((a.x * a.x) + (a.y * a.y));

  if (r_total < RADIO_MIN_IK)
    return false;

  float r_muneca = r_total - L_ESLABON_3 - L_HERRAMIENTA;
  float D = sqrtf((r_muneca * r_muneca) + (az * az));

  return (D <= (L_ESLABON_1 + L_ESLABON_2)) &&
         (D >= fabsf(L_ESLABON_1 - L_ESLABON_2));
}

static inline bool IK_ext(servomotores_h *out, vector_t a, float error_max,
                          float dt, float *error_out) {
  if (!out)
    return false;

  float angulo1, angulo2, angulo3, aux;
  float error_total = 0.0f;
  float az = a.z - L_SHOULDER_Z;
  float ax = a.x;
  float ay = a.y;

  float r_total = sqrtf((ax * ax) + (ay * ay));

  static float base_ang = 0.0f;
  static bool base_init = false;

  float a1;

  if (r_total < RADIO_MIN_IK) {
    float err_r = RADIO_MIN_IK - r_total;
    error_total += err_r;
    if (error_total > error_max) {
      if (error_out)
        *error_out = error_total;
      return false;
    }

    a1 = base_init ? base_ang : atan2f(ay, ax);

    ax = RADIO_MIN_IK * cosf(a1);
    ay = RADIO_MIN_IK * sinf(a1);
    r_total = RADIO_MIN_IK;
  } else {
    a1 = atan2f(ay, ax);
  }

  // Ángulo de la base con continuidad
  if (base_init) {
    float d = a1 - base_ang;
    while (d > PI) {
      a1 -= 2.0f * PI;
      d -= 2.0f * PI;
    }
    while (d < -PI) {
      a1 += 2.0f * PI;
      d += 2.0f * PI;
    }

    // Límite de velocidad angular de la base
    if (dt > 0.0f) {
      float d_max = SM1_VEL_MAX * dt;
      if (d > d_max)
        d = d_max;
      if (d < -d_max)
        d = -d_max;
      a1 = base_ang + d;
    }
  }
  base_ang = a1;
  base_init = true;
  angulo1 = base_ang;

  float r_muneca = r_total - L_ESLABON_3 - L_HERRAMIENTA;
  float D = sqrtf((r_muneca * r_muneca) + (az * az));

  float D_max = L_ESLABON_1 + L_ESLABON_2;
  float D_min = fabsf(L_ESLABON_1 - L_ESLABON_2);

  if (D > D_max || D < D_min) {
    float D_clamped = (D > D_max) ? (D_max - IK_D_EPS) : (D_min + IK_D_EPS);

    float err_D = fabsf(D - D_clamped);
    error_total += err_D;
    if (error_total > error_max) {
      if (error_out)
        *error_out = error_total;
      return false;
    }

    if (D > IK_D_SINGULAR_EPS) {
      float k = D_clamped / D;
      r_muneca *= k;
      az *= k;
    } else {
      r_muneca = D_clamped;
      az = 0.0f;
    }
    D = D_clamped;
  }

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
  angulo2 = atan2f(az, r_muneca) + acosf(aux);

  // Ángulo de la Muñeca (Servomotor 4)
  // sm2 + sm3 + sm4 - PI/2 = 0  =>  sm4 = angulo2 - angulo3
  float sm2_cmd = (PI / 2.0f) - angulo2;
  float sm3_cmd = angulo3;
  float sm4_cmd = angulo2 - angulo3;

  // Clamp final a los límites físicos del servo (Global_Config.h).
  // Esto es independiente del presupuesto de error_max: la solución
  // geométrica puede ser válida en el espacio de trabajo (D dentro de
  // rango) y aun así pedir un ángulo que el servo no puede alcanzar.
  // No se clampea sm1 porque su unwrap es deliberado para dar
  // continuidad de giro de la base, no representa el rango físico
  // ±PI de forma directa.
  if (sm2_cmd > SM2_ANG_MAX)
    sm2_cmd = SM2_ANG_MAX;
  if (sm2_cmd < SM2_ANG_MIN)
    sm2_cmd = SM2_ANG_MIN;
  if (sm3_cmd > SM3_ANG_MAX)
    sm3_cmd = SM3_ANG_MAX;
  if (sm3_cmd < SM3_ANG_MIN)
    sm3_cmd = SM3_ANG_MIN;
  if (sm4_cmd > SM4_ANG_MAX)
    sm4_cmd = SM4_ANG_MAX;
  if (sm4_cmd < SM4_ANG_MIN)
    sm4_cmd = SM4_ANG_MIN;

  out->sm1.angulo_objetivo = angulo1;
  out->sm2.angulo_objetivo = sm2_cmd;
  out->sm3.angulo_objetivo = sm3_cmd;
  out->sm4.angulo_objetivo = sm4_cmd;

  if (error_out)
    *error_out = error_total;
  return true;
}

static inline bool IK(servomotores_h *out, vector_t a) {
  return IK_ext(out, a, 0.0f, 0.0f, NULL);
}

#endif // IK_H
