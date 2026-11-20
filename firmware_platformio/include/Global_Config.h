#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include "Control_Brazo_Robotico/Aritmetica.h"
#include <stdint.h>

//==========================================
// LIMITES ESPACIALES DEL ESPACIO DE TRABAJO
//==========================================
#define R_MIN_ALCANCE   0.090f
#define R_MAX_ALCANCE   0.360f  
#define Z_MIN_MESA      0.040f  
//==========================================
//  VELOCIDAD , ACELERACION Y TIRON MAXIMO
//==========================================
#define V_MAX 1.0f
#define A_MAX 1.0f
#define J_MAX 5.0f

//===================================
// LONGITUD DE LAS PARTES DEL BRAZO
//===================================
#define L_ESLABON_1 0.16f
#define L_ESLABON_2 0.109f
#define L_ESLABON_3 0.10f
#define L_SHOULDER_Z 0.099f
#define L_HERRAMIENTA 0.0f

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
