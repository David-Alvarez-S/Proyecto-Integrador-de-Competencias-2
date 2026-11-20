#ifndef HW_INIT_H
#define HW_INIT_H

#include "esp_err.h"
#include "Drivers/as5600.h"
#include "Drivers/gc9306.h"
#include "Drivers/hc595.h"
#include "Drivers/hc595_stepper.h"
#include "Drivers/hcsr04.h"
#include "Drivers/pca9685.h"
#include "Drivers/tcs3200_hc595.h"
#include "Control_Brazo_Robotico/servo_enc_ctrl.h"
#include "tipos_de_sistema.h"

//=====================================================
//--------- Parámetros Base de Cintas y Pistón --------
//=====================================================
#define VELOCIDAD_CINTA_1_PASOS_S           450.0f
#define VELOCIDAD_CINTA_2_PASOS_S           450.0f
#define ACELERACION_CINTAS_PASOS_S2         600.0f

#define PISTON_PULSO_MINIMO_US              1000U
#define PISTON_PULSO_NEUTRAL_US             1500U
#define PISTON_PULSO_MAXIMO_US              2000U
#define PISTON_BANDA_MUERTA_US              30U
#define PISTON_LIBERAR_AL_DETENER           true

//=====================================================
//------- Instancias Globales de Periféricos ----------
//=====================================================
extern hc595_t g_registro_desplazamiento;
extern hc595_stepper_t g_motor_cinta_1;
extern hc595_stepper_t g_motor_cinta_2;
extern tcs3200_t g_sensor_color;
extern gc9306_t g_pantalla_tft;
extern pca9685_t g_controlador_pwm;
extern pca9685_config_t g_configuracion_pwm;

extern as5600_bus_t g_bus_encoders_magneticos;
extern sec_axis_t g_ejes_brazo[CANTIDAD_EJES_BRAZO];
extern const char *g_nombres_ejes_brazo[CANTIDAD_EJES_BRAZO];

extern hcsr04_group_t g_grupo_ultrasonicos;
extern uint8_t g_id_ultrasonico_1_piston;
extern uint8_t g_id_ultrasonico_2_buffer;

//=====================================================
//------ Inicialización Física y Memoria NVS ----------
//=====================================================
esp_err_t inicializar_todo_el_hardware(void);
esp_err_t reinicializar_controlador_pca9685(void);
void aplicar_orden_hardware_piston(const orden_piston_t *orden);
void actualizar_leds_indicadores_color(color_pallet_t color);

esp_err_t cargar_parametros_desde_nvs(void);
esp_err_t guardar_parametros_en_nvs(void);
esp_err_t borrar_parametros_nvs_y_restaurar_fabrica(void);

#endif /* HW_INIT_H */
