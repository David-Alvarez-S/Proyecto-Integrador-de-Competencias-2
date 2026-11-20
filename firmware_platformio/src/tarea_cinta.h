#ifndef TAREA_CINTA_H
#define TAREA_CINTA_H

//=====================================================
//----- Parámetros de Sensores, Cintas y Pistón -------
//=====================================================
#define UMBRAL_ULTRASONICO_1_CM             5.0f
#define UMBRAL_ULTRASONICO_2_CM             5.0f
#define RETARDO_ARRANQUE_CINTA_2_MS         500U

#define PISTON_RETARDO_ACTIVACION_MS        2000U
#define PISTON_TIEMPO_EXTENSION_MS          1000U
#define PISTON_PAUSA_INVERSION_GIRO_MS      200U
#define PISTON_TIEMPO_RETRACCION_MS         1850U

#define PISTON_DIRECCION_EXTENSION          (-1)
#define PISTON_DIRECCION_RETRACCION         (1)
#define PISTON_VELOCIDAD_EXTENSION_PCT      85.0f
#define PISTON_VELOCIDAD_RETRACCION_PCT     85.0f

#define COLOR_FRECUENCIA_CLEAR_MINIMA_HZ    1500.0f
#define COLOR_MUESTRAS_CONFIRMACION         2U
#define COLOR_ZONA_MUERTA_DETECCION_MS      800U
#define COLOR_MUESTRAS_LIBERACION_VACIO     5U

//=====================================================
//--------- Tarea de Lógica de Transporte -------------
//=====================================================
void tarea_logica_cintas_y_piston(void *parametro);

#endif /* TAREA_CINTA_H */
