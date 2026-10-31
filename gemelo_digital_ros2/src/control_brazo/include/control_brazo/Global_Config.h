#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include "Aritmetica.h"
#include <stdint.h>

#define RADIO_MIN_IK 40

//==========================================
//  VELOCIDAD , ACELERACION Y TIRON MAXIMO
//==========================================
#define V_MAX 1.0f
#define A_MAX 1.0f
#define J_MAX 5.0f

//===================================
// LONGITUD DE LAS PARTES DEL BRAZO
//===================================
#define L_ESLABON_1 100.0f
#define L_ESLABON_2 106.35f
#define L_ESLABON_3 109.35f
#define L_SHOULDER_Z 13.5f
#define L_HERRAMIENTA 74.99f

//==========================================
// LIMITES ANGULARES DE LAS ARTICULACIONES
// =========================================
#define SM1_ANG_MIN -2.2f
#define SM1_ANG_MAX 2.2f

#define SM2_ANG_MIN -2.0f
#define SM2_ANG_MAX 2.0f

#define SM3_ANG_MIN -0.85f
#define SM3_ANG_MAX 2.8f

#define SM4_ANG_MIN -2.3f
#define SM4_ANG_MAX 2.5f

typedef struct {
  uint8_t pwm_pin;
  float angulo_max;
  float angulo_min;
  float angulo_actual;
  float angulo_objetivo;
} Servomotor_t;

typedef struct {
  Servomotor_t sm1;
  Servomotor_t sm2;
  Servomotor_t sm3;
  Servomotor_t sm4;
} servomotores_h;
#endif // GLOBAL_CONFIG_H
