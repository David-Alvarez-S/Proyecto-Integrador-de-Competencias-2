#ifndef TAREA_BRAZO_H
#define TAREA_BRAZO_H

#include <stdbool.h>
#include "tipos_de_sistema.h"

//=====================================================
//---- Parámetros de Control y Estabilización (Hz) ----
//=====================================================
#define PERIODO_LAZO_CONTROL_MS             10U
#define PASO_TIEMPO_CONTROL_S               0.010f
#define DIVISOR_REPORTE_QT_100MS            10U

#define TIEMPO_ESTABILIZACION_CONTINUA_MS   500U
#define TIEMPO_MAXIMO_ESPERA_ENCLAVE_MS     2200U

//=====================================================
//--- Límites Suavizados de Perfil-S (Anti-Rebote) ----
//=====================================================
#define PERFIL_S_VELOCIDAD_MAXIMA           0.8f
#define PERFIL_S_ACELERACION_MAXIMA         0.4f
#define PERFIL_S_JERK_MAXIMO                0.8f

//=====================================================
//------ Interfaz de Control Manual y Secuencia -------
//=====================================================
void tarea_control_brazo_robotico(void *parametro);

void cancelar_movimiento_y_secuencia_brazo(void);
void ordenar_movimiento_articular_suave(const float objetivos_grados[CANTIDAD_EJES_BRAZO], bool es_retorno_reposo);
void ordenar_objetivo_articular_directo(uint8_t indice_eje, float angulo_grados);

bool validar_configuracion_completa_trabajo(const configuracion_trabajo_brazo_t *configuracion);
void imprimir_configuracion_posiciones_brazo(void);
void imprimir_estado_articular_y_cartesiano(void);

#endif /* TAREA_BRAZO_H */
