#include <stdio.h>

#include "include/Global_Config.h"
#include "include/IK.h"
#include "include/Interpolaciones.h"
#include "include/Perfil_S.h"

int main() {
  printf("Primera Prueba \n");

  limites_perfil_s_t limites = {.a_max = A_MAX, .v_max = V_MAX, .j_max = J_MAX};

  planificador_perfil_s_t planificador_s;
  estado_perfil_s_t estado;
  Iniciar_Perfil_S(&planificador_s, limites);
  Planificar_Perfil_s(&planificador_s, 0.0f, 1.0f);

  // 200, 0, 50 y 140,0,50,
  vector_t V1 = {200.0f, 0.0f, 50.0f};
  vector_t V2 = {140.0f, 0.0f, 50.0f};
  vector_t out;

  servomotores_h servomotores = {
      {1, -3, 3, 0, 0},
      {1, -3, 3, 0, 0},
      {1, -3, 3, 0, 0},
      {1, -3, 3, 0, 0},
  };

  for (float i = 0; i < 4; i = i + 0.02) {
    estado = Estado_Perfil_s(&planificador_s, i);

    lerp(&out, V1, V2, estado.posicion);

    IK(&servomotores, out);
    float angulo1 = servomotores.sm1.angulo_objetivo;
    float angulo2 = servomotores.sm2.angulo_objetivo;
    float angulo3 = servomotores.sm3.angulo_objetivo;
    float angulo4 = servomotores.sm4.angulo_objetivo;
    printf("Angulo 1 = %f| Angulo 2 = %f| Angulo 3 = %f|Angulo 4 = %f \n",
           angulo1, angulo2, angulo3, angulo4);
  }

  return 0;
}
