#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct {
  float kp;
  float ki;
  float kd;
  float error_anterior;
  float integral;
  float limite_min;
  float limite_max;
  float resultado;
} PID_t;

static inline void Iniciar_PID(PID_t *pid, float kp, float ki, float kd,
                               float limite_min, float limite_max) {
  if (!pid)
    return;
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  pid->limite_min = limite_min;
  pid->limite_max = limite_max;
  pid->resultado = 0;
  pid->integral = 0;
  pid->error_anterior = 0;
}

static inline void Ejecutar_PID(PID_t *pid, float sp, float med) {
  if (!pid)
    return;
  float error = sp - med;

  float P = pid->kp * error;

  pid->integral = pid->integral + error;
  float I = pid->ki * pid->integral;

  float D = pid->kd * (error - pid->error_anterior);
  pid->error_anterior = error;

  float salida = P + I + D;

  if (salida > pid->limite_max) {
    salida = pid->limite_max;
    pid->integral = pid->integral - error;
  } else if (salida < pid->limite_min) {
    salida = pid->limite_min;
    pid->integral = pid->integral - error;
  }
  pid->resultado = salida;
}

#endif // PID_H
