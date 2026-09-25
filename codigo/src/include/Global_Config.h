#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <stdint.h>

#define V_MAX 1.0f
#define A_MAX 2.0f
#define J_MAX 5.0f

#define L_ESLABON_1 5
#define L_ESLABON_2 5
#define L_ESLABON_3 5
#define L_HERRAMIENTA 1

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
