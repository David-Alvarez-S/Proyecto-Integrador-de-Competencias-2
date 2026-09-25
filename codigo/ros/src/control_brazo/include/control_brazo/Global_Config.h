#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <stdint.h>

#define V_MAX 0.5f
#define A_MAX 1.0f
#define J_MAX 2.0f

#define L_ESLABON_1 105.2f
#define L_ESLABON_2 105.2f
#define L_ESLABON_3 53.112f
#define L_HERRAMIENTA 36.76f

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
