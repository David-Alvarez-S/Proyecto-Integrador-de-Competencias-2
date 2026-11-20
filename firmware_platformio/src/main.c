#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "hw_init.h"
#include "tarea_brazo.h"
#include "tarea_cinta.h"
#include "tarea_hmi.h"
#include "tipos_de_sistema.h"
static const char *TAG_MAIN = "MAIN_SYS";

//=====================================================
//---- Estado Global y Configuración por Defecto ------
//=====================================================
volatile bool g_sistema_en_marcha = SISTEMA_ARRANQUE_AUTOMATICO;

volatile estado_telemetria_hmi_t g_telemetria_hmi = {
    .color_detectado            = COLOR_PALLET_NINGUNO,
    .distancia_ultrasonico_1_cm = -1.0f,
    .distancia_ultrasonico_2_cm = -1.0f,
    .estado_cinta_2             = ESTADO_CINTA2_DETENIDA,
    .estado_piston              = ESTADO_PISTON_QUIETO,
    .estado_brazo               = SECUENCIA_BRAZO_LISTO,
    .angulos_actuales_grados    = { 0.0f, 0.0f, 0.0f, 0.0f }
};

configuracion_trabajo_brazo_t g_configuracion_trabajo_brazo = {
    .punto_recogida_m               = { 0.260f, 0.000f, 0.060f },
    .elevacion_vertical_z_m         = 0.050f,
    .contenedor_x_m                 = 0.000f,
    .contenedor_y_m                 = 0.260f,
    .altura_z_rojo_m                = 0.070f,
    .altura_z_azul_m                = 0.140f,
    .altura_z_amarillo_m            = 0.210f,
    .avance_horizontal_deposito_m   = 0.025f,
    .bajada_vertical_asentamiento_m = 0.020f,
};

SemaphoreHandle_t g_mutex_bus_i2c = NULL;
SemaphoreHandle_t g_mutex_configuracion_brazo = NULL;

QueueHandle_t g_cola_pallets_cinta_1 = NULL;
QueueHandle_t g_cola_pallets_cinta_2 = NULL;
QueueHandle_t g_cola_objetivos_brazo = NULL;
QueueHandle_t g_cola_ordenes_piston = NULL;

//=====================================================
//------ Implementación de Conversiones a Texto -------
//=====================================================
const char *obtener_nombre_color_pallet(color_pallet_t color) {
    switch (color) {
        case COLOR_PALLET_ROJO:     return "ROJO";
        case COLOR_PALLET_AZUL:     return "AZUL";
        case COLOR_PALLET_AMARILLO: return "AMARILLO";
        default:                    return "NINGUNO";
    }
}

const char *obtener_nombre_color_para_qt(color_pallet_t color) {
    switch (color) {
        case COLOR_PALLET_ROJO:     return "ROJO";
        case COLOR_PALLET_AZUL:     return "AZUL";
        case COLOR_PALLET_AMARILLO: return "AMARILLO";
        default:                    return "INDEFINIDO";
    }
}

const char *obtener_texto_estado_cinta2(estado_cinta2_t estado) {
    switch (estado) {
        case ESTADO_CINTA2_ESPERANDO_RETARDO: return "ESPERA";
        case ESTADO_CINTA2_EN_MARCHA:         return "MARCHA";
        default:                              return "PARADA";
    }
}

const char *obtener_texto_estado_piston(estado_piston_t estado) {
    switch (estado) {
        case ESTADO_PISTON_ESPERANDO_RETARDO: return "RETARDO";
        case ESTADO_PISTON_EXTENDIENDO:       return "IDA";
        case ESTADO_PISTON_PAUSA_INVERSION:   return "PAUSA";
        case ESTADO_PISTON_RETRAYENDO:        return "REGRESO";
        default:                              return "QUIETO";
    }
}

const char *obtener_texto_estado_brazo(estado_secuencia_brazo_t estado) {
    switch (estado) {
        case SECUENCIA_BRAZO_IR_A_RECOGIDA:          return "IR_PICK";
        case SECUENCIA_BRAZO_ELEVAR_PALLET:          return "SUBIR_Z";
        case SECUENCIA_BRAZO_IR_A_REPOSO_CON_PALLET: return "HOME_PAL";
        case SECUENCIA_BRAZO_IR_A_CONTENEDOR:        return "IR_CONT";
        case SECUENCIA_BRAZO_AVANZAR_EN_CONTENEDOR:  return "AVANZAR";
        case SECUENCIA_BRAZO_BAJAR_PARA_ASENTAR:     return "ASENTAR";
        case SECUENCIA_BRAZO_RETROCEDER_DE_CONTENEDOR: return "RETROCED";
        case SECUENCIA_BRAZO_IR_A_REPOSO_FINAL:      return "HOME_FIN";
        case SECUENCIA_BRAZO_MOVIMIENTO_MANUAL:      return "MANUAL";
        default:                                     return "LISTO";
    }
}

//=====================================================
//--------- Sincronización Inicial hacia Qt -----------
//=====================================================
static void sincronizar_estado_inicial_con_interfaz_qt(bool cinta_1_activa) {
    ESP_LOGI("CINTA1", "%s", cinta_1_activa ? "1" : "0");
    ESP_LOGI("CINTA2", "0");
    ESP_LOGI("PISTON", "QUIETO");
    ESP_LOGI("ULTRA1", "0");
    ESP_LOGI("ULTRA2", "0");
    ESP_LOGI("COLOR", "INDEFINIDO");
    ESP_LOGI("PWM", "0, 0, 0, 0");
    ESP_LOGI("ANGULOS", "0.00, 0.00, 0.00, 0.00");
}

//=====================================================
//--------- Punto de Entrada de la Aplicación ---------
//=====================================================
configuracion_transporte_t g_configuracion_transporte = {
    .piston_retardo_ms        = 4300U,
    .piston_extension_ms      = 1000U,
    .piston_retraccion_ms     = 1850U,
    .velocidad_cintas_pasos_s = VELOCIDAD_CINTA_1_PASOS_S,
};

void app_main(void) {
    esp_err_t estado_nvs = nvs_flash_init();
    if (estado_nvs == ESP_ERR_NVS_NO_FREE_PAGES || estado_nvs == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        estado_nvs = nvs_flash_init();
    }
    ESP_ERROR_CHECK(estado_nvs);

    g_mutex_bus_i2c = xSemaphoreCreateMutex();
    configASSERT(g_mutex_bus_i2c != NULL);

    g_mutex_configuracion_brazo = xSemaphoreCreateMutex();
    configASSERT(g_mutex_configuracion_brazo != NULL);

    g_cola_pallets_cinta_1 = xQueueCreate(CAPACIDAD_COLA_CINTA_1, sizeof(color_pallet_t));
    g_cola_pallets_cinta_2 = xQueueCreate(CAPACIDAD_COLA_CINTA_2, sizeof(color_pallet_t));
    g_cola_objetivos_brazo = xQueueCreate(CAPACIDAD_COLA_OBJETIVOS_BRAZO, sizeof(color_pallet_t));
    g_cola_ordenes_piston  = xQueueCreate(CAPACIDAD_COLA_COMANDOS_PISTON, sizeof(orden_piston_t));

    ESP_ERROR_CHECK(inicializar_todo_el_hardware());
    ESP_ERROR_CHECK(cargar_parametros_desde_nvs());

    if (g_sistema_en_marcha) {
        ESP_ERROR_CHECK(hc595_stepper_run(&g_motor_cinta_1, g_configuracion_transporte.velocidad_cintas_pasos_s));
    }

    sincronizar_estado_inicial_con_interfaz_qt(g_sistema_en_marcha);
    imprimir_configuracion_posiciones_brazo();

    xTaskCreatePinnedToCore(tarea_control_brazo_robotico, "ctrl_brazo",  8192, NULL, 10, NULL, 1);
    xTaskCreatePinnedToCore(tarea_logica_cintas_y_piston, "logica_cinta", 4096, NULL,  5, NULL, 1);
    xTaskCreatePinnedToCore(tarea_pantalla_hmi,           "hmi_tft",     4096, NULL,  2, NULL, 0);
}
