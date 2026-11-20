#ifndef TIPOS_DE_SISTEMA_H
#define TIPOS_DE_SISTEMA_H

#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "Control_Brazo_Robotico/Vectores.h"

//=====================================================
//--------- Constantes Generales del Sistema ----------
//=====================================================
#define CANTIDAD_EJES_BRAZO                 4
#define SISTEMA_ARRANQUE_AUTOMATICO         true

#define CAPACIDAD_COLA_CINTA_1              3
#define CAPACIDAD_COLA_CINTA_2              4
#define CAPACIDAD_COLA_OBJETIVOS_BRAZO      4
#define CAPACIDAD_COLA_COMANDOS_PISTON      8

//=====================================================
//------- Tipos de Datos de Pallets y Estados ---------
//=====================================================
typedef enum {
    COLOR_PALLET_NINGUNO = 0,
    COLOR_PALLET_ROJO,
    COLOR_PALLET_AZUL,
    COLOR_PALLET_AMARILLO
} color_pallet_t;

typedef enum {
    ESTADO_CINTA2_DETENIDA = 0,
    ESTADO_CINTA2_ESPERANDO_RETARDO,
    ESTADO_CINTA2_EN_MARCHA
} estado_cinta2_t;

typedef enum {
    ESTADO_PISTON_QUIETO = 0,
    ESTADO_PISTON_ESPERANDO_RETARDO,
    ESTADO_PISTON_EXTENDIENDO,
    ESTADO_PISTON_PAUSA_INVERSION,
    ESTADO_PISTON_RETRAYENDO
} estado_piston_t;

typedef enum {
    SECUENCIA_BRAZO_LISTO = 0,
    SECUENCIA_BRAZO_IR_A_RECOGIDA,
    SECUENCIA_BRAZO_ELEVAR_PALLET,
    SECUENCIA_BRAZO_IR_A_REPOSO_CON_PALLET,
    SECUENCIA_BRAZO_IR_A_CONTENEDOR,
    SECUENCIA_BRAZO_AVANZAR_EN_CONTENEDOR,
    SECUENCIA_BRAZO_BAJAR_PARA_ASENTAR,
    SECUENCIA_BRAZO_RETROCEDER_DE_CONTENEDOR,
    SECUENCIA_BRAZO_IR_A_REPOSO_FINAL,
    SECUENCIA_BRAZO_MOVIMIENTO_MANUAL
} estado_secuencia_brazo_t;

//=====================================================
//---- Configuración de Posiciones de Trabajo (m) -----
//=====================================================
typedef struct {
    vector_t punto_recogida_m;
    float elevacion_vertical_z_m;
    float contenedor_x_m;
    float contenedor_y_m;
    float altura_z_rojo_m;
    float altura_z_azul_m;
    float altura_z_amarillo_m;
    float avance_horizontal_deposito_m;
    float bajada_vertical_asentamiento_m;
} configuracion_trabajo_brazo_t;

//=====================================================
//---- Configuración Persistente de Cintas y Pistón ---
//=====================================================
typedef struct {
    uint32_t piston_retardo_ms;
    uint32_t piston_extension_ms;
    uint32_t piston_retraccion_ms;
    float velocidad_cintas_pasos_s;
} configuracion_transporte_t;

//=====================================================
//--------- Comandos Asincrónicos de Pistón -----------
//=====================================================
typedef enum {
    ORDEN_PISTON_LIBERAR = 0,
    ORDEN_PISTON_NEUTRAL,
    ORDEN_PISTON_VELOCIDAD
} tipo_orden_piston_t;

typedef struct {
    tipo_orden_piston_t tipo_orden;
    float porcentaje_velocidad;
} orden_piston_t;

//=====================================================
//------- Telemetría Compartida hacia Pantalla --------
//=====================================================
typedef struct {
    color_pallet_t color_detectado;
    float matiz_h;
    float saturacion_s;
    float frecuencia_clear_hz;
    float distancia_ultrasonico_1_cm;
    float distancia_ultrasonico_2_cm;
    uint8_t pallets_en_cola_cinta_1;
    uint8_t pallets_en_cola_cinta_2;
    estado_cinta2_t estado_cinta_2;
    estado_piston_t estado_piston;
    estado_secuencia_brazo_t estado_brazo;
    float angulos_actuales_grados[CANTIDAD_EJES_BRAZO];
} estado_telemetria_hmi_t;

//=====================================================
//--------- Recursos Globales Compartidos -------------
//=====================================================
extern volatile bool g_sistema_en_marcha;
extern volatile estado_telemetria_hmi_t g_telemetria_hmi;
extern configuracion_trabajo_brazo_t g_configuracion_trabajo_brazo;
extern configuracion_transporte_t g_configuracion_transporte;

extern SemaphoreHandle_t g_mutex_bus_i2c;
extern SemaphoreHandle_t g_mutex_configuracion_brazo;

extern QueueHandle_t g_cola_pallets_cinta_1;
extern QueueHandle_t g_cola_pallets_cinta_2;
extern QueueHandle_t g_cola_objetivos_brazo;
extern QueueHandle_t g_cola_ordenes_piston;

//=====================================================
//--------- Conversión de Estados a Texto -------------
//=====================================================
const char *obtener_nombre_color_pallet(color_pallet_t color);
const char *obtener_nombre_color_para_qt(color_pallet_t color);
const char *obtener_texto_estado_cinta2(estado_cinta2_t estado);
const char *obtener_texto_estado_piston(estado_piston_t estado);
const char *obtener_texto_estado_brazo(estado_secuencia_brazo_t estado);

#endif /* TIPOS_DE_SISTEMA_H */
