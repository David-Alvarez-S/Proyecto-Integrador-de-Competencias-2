#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include "Aritmetica.h"
#include <stdint.h>

#define RADIO_MIN_IK 40

#define V_MAX 3.0f
#define A_MAX 8.0f
#define J_MAX 10.0f

#define L_ESLABON_1 100.0f
#define L_ESLABON_2 106.0f
#define L_ESLABON_3 254.243f
#define L_SHOULDER_Z 80.0f // altura del hombro sobre base_link
#define L_HERRAMIENTA 0.0f // el target ya es el frame 'herramienta'

#define SM1_ANG_MAX PI
#define SM1_ANG_MIN -PI

#define SM1_VEL_MAX 1.5f

#define SM2_ANG_MAX 1.5
#define SM2_ANG_MIN -1.5

#define SM3_ANG_MAX 2.4
#define SM3_ANG_MIN -2.4

#define SM4_ANG_MAX 2.3
#define SM4_ANG_MIN -2.3

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
