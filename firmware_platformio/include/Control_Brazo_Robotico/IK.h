#ifndef IK_H
#define IK_H

#include "Global_Config.h"
#include "Vectores.h"
#include <math.h>
#include <stdbool.h>

#define IK_D_EPS 1e-4f

// ==================================================
// PARAMETROS DE CALIBRACIÓN DE LA ZONA DE PROTECCION
// ==================================================
#define R_BASE_SEGURIDAD_MAX  0.210f  
#define Z_ALTURA_PROTECCION   0.190f  

typedef enum {
    IK_OK = 0,
    IK_ERROR_BASE_FUERA_DE_LIMITE,
    IK_ERROR_SINGULARIDAD_MUNICA,
    IK_ERROR_FUERA_DE_ALCANCE,
    IK_ERROR_SERVO_FUERA_DE_LIMITE,
    IK_ERROR_COLISION_MESA
} ik_status_t;

typedef struct {
  float base_ang;
  bool base_init;
} ik_continuidad_t;

static inline ik_status_t IK_ext_debug(servomotores_h *out, vector_t a, ik_continuidad_t *continuidad) {
  if (!out) return IK_ERROR_FUERA_DE_ALCANCE;

  float ax = a.x;
  float ay = a.y;
  float az = a.z - L_SHOULDER_Z;

  if (a.z < Z_MIN_MESA) {
    return IK_ERROR_COLISION_MESA;
  }

  float r_total = sqrtf((ax * ax) + (ay * ay));

  float a1 = atan2f(ay, ax);
  if (a1 > SM1_ANG_MAX || a1 < SM1_ANG_MIN) {
    return IK_ERROR_BASE_FUERA_DE_LIMITE;
  }

  float l_efector = L_ESLABON_3 + L_HERRAMIENTA;
  float r_muneca = r_total - l_efector;

  // =====================
  // FILTRO DINAMICO EN Z
  // =====================
  float r_min_seguridad_base = 0.001f;

  if (a.z < Z_ALTURA_PROTECCION) {
    float factor = a.z / Z_ALTURA_PROTECCION; 
    if (factor < 0.0f) factor = 0.0f;
    r_min_seguridad_base = R_BASE_SEGURIDAD_MAX - factor * (R_BASE_SEGURIDAD_MAX - 0.001f);
  }

  if (r_muneca < r_min_seguridad_base) {
    return IK_ERROR_SINGULARIDAD_MUNICA;
  }

  float D_sq = (r_muneca * r_muneca) + (az * az);
  float D = sqrtf(D_sq);

  float D_max = L_ESLABON_1 + L_ESLABON_2;        
  float D_min = fabsf(L_ESLABON_1 - L_ESLABON_2); 

  if (D > D_max || D < D_min) {
    return IK_ERROR_FUERA_DE_ALCANCE;
  }

  float aux3 = ((L_ESLABON_1 * L_ESLABON_1) + (L_ESLABON_2 * L_ESLABON_2) - D_sq) /
               (2.0f * L_ESLABON_1 * L_ESLABON_2);
  aux3 = fminf(1.0f, fmaxf(-1.0f, aux3));
  float angulo3 = PI - acosf(aux3);

  float aux2 = ((L_ESLABON_1 * L_ESLABON_1) + D_sq - (L_ESLABON_2 * L_ESLABON_2)) /
               (2.0f * L_ESLABON_1 * D);
  aux2 = fminf(1.0f, fmaxf(-1.0f, aux2));
  float angulo2 = atan2f(az, r_muneca) + acosf(aux2);

  float sm2_cmd = (PI / 2.0f) - angulo2;
  float sm3_cmd = angulo3;            
  float sm4_cmd = -(angulo3 + angulo2);  

  if (sm2_cmd < -PI || sm2_cmd > PI || 
      sm3_cmd < -PI || sm3_cmd > PI || 
      sm4_cmd < -PI || sm4_cmd > PI) {
    return IK_ERROR_SERVO_FUERA_DE_LIMITE;
  }

  if (continuidad) {
    continuidad->base_ang = a1;
    continuidad->base_init = true;
  }

  out->sm1.angulo_objetivo = a1;
  out->sm2.angulo_objetivo = sm2_cmd;
  out->sm3.angulo_objetivo = sm3_cmd;
  out->sm4.angulo_objetivo = sm4_cmd;

  return IK_OK;
}

static inline bool IK_ext(servomotores_h *out, vector_t a, float error_max,
                          float dt, ik_continuidad_t *continuidad,
                          float *error_out) {
  (void)dt; (void)error_max;
  ik_status_t status = IK_ext_debug(out, a, continuidad);
  if (error_out) *error_out = (status == IK_OK) ? 0.0f : 999.0f;
  return (status == IK_OK);
}

static inline bool IK(servomotores_h *out, vector_t a, ik_continuidad_t *continuidad) {
  return IK_ext(out, a, 0.0f, 0.0f, continuidad, NULL);
}

static inline bool Validar_Vector_Entrada(vector_t v) {
  servomotores_h dummy;
  return IK(&dummy, v, NULL);
}

static inline bool IK_es_alcanzable(vector_t a) {
  return Validar_Vector_Entrada(a);
}

#endif // IK_H
