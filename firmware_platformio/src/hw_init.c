#include "hw_init.h"
#include "pines.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG_NVS = "NVS_CFG";
#define ESPACIO_NOMBRES_NVS "robot_cfg"
#define VERSION_ESTRUCTURA_NVS 1U

typedef struct {
    uint32_t version;
    float kp[CANTIDAD_EJES_BRAZO];
    float ki[CANTIDAD_EJES_BRAZO];
    float kd[CANTIDAD_EJES_BRAZO];
    float k_grav_deg_s[CANTIDAD_EJES_BRAZO];
    configuracion_trabajo_brazo_t trabajo_brazo;
    configuracion_transporte_t transporte;
} paquete_parametros_nvs_t;

//=====================================================
//--------- Definición de Instancias Globales ---------
//=====================================================
hc595_t g_registro_desplazamiento;
hc595_stepper_t g_motor_cinta_1;
hc595_stepper_t g_motor_cinta_2;
tcs3200_t g_sensor_color;
gc9306_t g_pantalla_tft;
pca9685_t g_controlador_pwm;
pca9685_config_t g_configuracion_pwm;

#if !PCA9685_USE_LEGACY_I2C
static i2c_master_bus_handle_t s_manejador_bus_i2c = NULL;
#endif

as5600_bus_t g_bus_encoders_magneticos;
sec_axis_t g_ejes_brazo[CANTIDAD_EJES_BRAZO];
const char *g_nombres_ejes_brazo[CANTIDAD_EJES_BRAZO] = {
    "BASE", "HOMBRO", "CODO", "MUNECA"
};

hcsr04_group_t g_grupo_ultrasonicos;
static hcsr04_sensor_t s_sensores_ultrasonicos[2];
uint8_t g_id_ultrasonico_1_piston = 0;
uint8_t g_id_ultrasonico_2_buffer = 1;

//=====================================================
//------- Gestión de Memoria No Volátil (NVS) ---------
//=====================================================
esp_err_t guardar_parametros_en_nvs(void) {
    nvs_handle_t manejador_nvs;
    esp_err_t estado = nvs_open(ESPACIO_NOMBRES_NVS, NVS_READWRITE, &manejador_nvs);
    if (estado != ESP_OK) return estado;

    paquete_parametros_nvs_t paquete = {
        .version = VERSION_ESTRUCTURA_NVS,
        .transporte = g_configuracion_transporte,
    };

    xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
    paquete.trabajo_brazo = g_configuracion_trabajo_brazo;
    xSemaphoreGive(g_mutex_configuracion_brazo);

    for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
        paquete.kp[indice_eje]           = g_ejes_brazo[indice_eje].cfg.kp;
        paquete.ki[indice_eje]           = g_ejes_brazo[indice_eje].cfg.ki;
        paquete.kd[indice_eje]           = g_ejes_brazo[indice_eje].cfg.kd;
        paquete.k_grav_deg_s[indice_eje] = g_ejes_brazo[indice_eje].cfg.k_grav_deg_s;
    }

    estado = nvs_set_blob(manejador_nvs, "params_v1", &paquete, sizeof(paquete));
    if (estado == ESP_OK) {
        estado = nvs_commit(manejador_nvs);
    }
    nvs_close(manejador_nvs);

    if (estado == ESP_OK) {
        ESP_LOGI(TAG_NVS, "Parametros guardados en flash NVS.");
    }
    return estado;
}

esp_err_t cargar_parametros_desde_nvs(void) {
    nvs_handle_t manejador_nvs;
    esp_err_t estado = nvs_open(ESPACIO_NOMBRES_NVS, NVS_READONLY, &manejador_nvs);
    if (estado == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG_NVS, "Sin configuracion previa en NVS; usando valores por defecto.");
        return ESP_OK;
    }
    if (estado != ESP_OK) return estado;

    paquete_parametros_nvs_t paquete = {0};
    size_t tamano_leido = sizeof(paquete);
    estado = nvs_get_blob(manejador_nvs, "params_v1", &paquete, &tamano_leido);
    nvs_close(manejador_nvs);

    if (estado == ESP_ERR_NVS_NOT_FOUND) {
        return ESP_OK;
    }
    if (estado != ESP_OK || tamano_leido != sizeof(paquete) || paquete.version != VERSION_ESTRUCTURA_NVS) {
        ESP_LOGW(TAG_NVS, "Datos NVS incompatibles; manteniendo valores por defecto.");
        return ESP_OK;
    }

    for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
        sec_set_pid(&g_ejes_brazo[indice_eje],
                    paquete.kp[indice_eje],
                    paquete.ki[indice_eje],
                    paquete.kd[indice_eje]);
        sec_set_gravity(&g_ejes_brazo[indice_eje], paquete.k_grav_deg_s[indice_eje], 0.0f);
    }

    xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
    g_configuracion_trabajo_brazo = paquete.trabajo_brazo;
    xSemaphoreGive(g_mutex_configuracion_brazo);

    g_configuracion_transporte = paquete.transporte;
    ESP_LOGI(TAG_NVS, "Parametros cargados correctamente desde NVS.");
    return ESP_OK;
}

esp_err_t borrar_parametros_nvs_y_restaurar_fabrica(void) {
    nvs_handle_t manejador_nvs;
    esp_err_t estado = nvs_open(ESPACIO_NOMBRES_NVS, NVS_READWRITE, &manejador_nvs);
    if (estado == ESP_OK) {
        nvs_erase_all(manejador_nvs);
        nvs_commit(manejador_nvs);
        nvs_close(manejador_nvs);
    }
    return estado;
}

//=====================================================
//------ Control de Salidas Auxiliares (74HC595) ------
//=====================================================
void actualizar_leds_indicadores_color(color_pallet_t color) {
    uint16_t mascara_leds = (1U << SR_LED_RED) | (1U << SR_LED_BLUE) | (1U << SR_LED_YELLOW);
    uint16_t valor_encendido = 0;

    if (color == COLOR_PALLET_ROJO)          valor_encendido = (1U << SR_LED_RED);
    else if (color == COLOR_PALLET_AZUL)     valor_encendido = (1U << SR_LED_BLUE);
    else if (color == COLOR_PALLET_AMARILLO) valor_encendido = (1U << SR_LED_YELLOW);

    (void)hc595_write_masked(&g_registro_desplazamiento, mascara_leds, valor_encendido);
}

void aplicar_orden_hardware_piston(const orden_piston_t *orden) {
    if (!orden) return;

    if (orden->tipo_orden == ORDEN_PISTON_VELOCIDAD) {
        (void)pca9685_cr_set_speed(&g_controlador_pwm, PCA_CH_PISTON, orden->porcentaje_velocidad);
    } else if (orden->tipo_orden == ORDEN_PISTON_NEUTRAL) {
        (void)pca9685_cr_stop(&g_controlador_pwm, PCA_CH_PISTON);
    } else {
        if (PISTON_LIBERAR_AL_DETENER) {
            (void)pca9685_release(&g_controlador_pwm, PCA_CH_PISTON);
        } else {
            (void)pca9685_cr_stop(&g_controlador_pwm, PCA_CH_PISTON);
        }
    }
}

esp_err_t reinicializar_controlador_pca9685(void) {
    esp_err_t estado = pca9685_init(&g_controlador_pwm, &g_configuracion_pwm);
    if (estado == ESP_OK) {
        pca9685_release(&g_controlador_pwm, PCA_CH_PISTON);
    }
    return estado;
}

//=====================================================
//------ Inicialización Completa de Periféricos -------
//=====================================================
esp_err_t inicializar_todo_el_hardware(void) {
    gpio_config_t configuracion_cs_tactil = {
        .pin_bit_mask = (1ULL << PIN_TOUCH_CS),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&configuracion_cs_tactil));
    gpio_set_level(PIN_TOUCH_CS, 1);

    gc9306_config_t configuracion_pantalla = GC9306_CONFIG_DEFAULT(
        PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_CLK,
        PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST, GPIO_NUM_NC
    );
    configuracion_pantalla.host = SPI_SHARED_HOST;
    configuracion_pantalla.init_bus = true;
    configuracion_pantalla.bl_pwm = false;
    configuracion_pantalla.rotation = TFT_ROTATION_0;
    configuracion_pantalla.bgr = true;
    configuracion_pantalla.clock_hz = 4 * 1000 * 1000;
    configuracion_pantalla.dma_buf_bytes = 1024;
    ESP_ERROR_CHECK(gc9306_init(&g_pantalla_tft, &configuracion_pantalla));

    hc595_config_t configuracion_74hc595 = HC595_CONFIG_SHARED_SPI(
        SPI_SHARED_HOST, PIN_SR_CS, PIN_SR_OE
    );
    configuracion_74hc595.chips = 2;
    configuracion_74hc595.clock_hz = 4 * 1000 * 1000;
    ESP_ERROR_CHECK(hc595_init(&g_registro_desplazamiento, &configuracion_74hc595));

    hc595_stepper_config_t configuracion_motor_1 = HC595_STEPPER_CONFIG_DEFAULT(
        &g_registro_desplazamiento, SR_M1_IN1, SR_M1_IN2, SR_M1_IN3, SR_M1_IN4
    );
    configuracion_motor_1.mode = HC595_STEPPER_MODE_FULL;
    configuracion_motor_1.max_speed = 650.0f;
    configuracion_motor_1.accel = ACELERACION_CINTAS_PASOS_S2;
    configuracion_motor_1.reversed = true;
    configuracion_motor_1.hold_when_idle = false;
    ESP_ERROR_CHECK(hc595_stepper_init(&g_motor_cinta_1, &configuracion_motor_1));

    hc595_stepper_config_t configuracion_motor_2 = HC595_STEPPER_CONFIG_DEFAULT(
        &g_registro_desplazamiento, SR_M2_IN1, SR_M2_IN2, SR_M2_IN3, SR_M2_IN4
    );
    configuracion_motor_2.mode = HC595_STEPPER_MODE_FULL;
    configuracion_motor_2.max_speed = 650.0f;
    configuracion_motor_2.accel = ACELERACION_CINTAS_PASOS_S2;
    configuracion_motor_2.reversed = true;
    configuracion_motor_2.hold_when_idle = false;
    ESP_ERROR_CHECK(hc595_stepper_init(&g_motor_cinta_2, &configuracion_motor_2));

    tcs3200_config_t configuracion_color = TCS3200_CONFIG_DEFAULT(
        &g_registro_desplazamiento, SR_TCS_S0, SR_TCS_S1, SR_TCS_S2, SR_TCS_S3, PIN_TCS_OUT
    );
    configuracion_color.scale = TCS3200_SCALE_100;
    configuracion_color.gate_ms = 25;
    configuracion_color.settle_ms = 2;
    ESP_ERROR_CHECK(tcs3200_init(&g_sensor_color, &configuracion_color));

    hcsr04_group_config_t configuracion_grupo_us = HCSR04_GROUP_CONFIG_DEFAULT();
    configuracion_grupo_us.timeout_ms = 25;
    configuracion_grupo_us.guard_ms = 25;
    configuracion_grupo_us.max_cm = 100.0f;
    ESP_ERROR_CHECK(hcsr04_group_init(&g_grupo_ultrasonicos, s_sensores_ultrasonicos, 2, &configuracion_grupo_us));

    ESP_ERROR_CHECK(hcsr04_group_add(&g_grupo_ultrasonicos, PIN_US1_TRIG, PIN_US1_ECHO, "US1_PISTON", &g_id_ultrasonico_1_piston));
    ESP_ERROR_CHECK(hcsr04_group_add(&g_grupo_ultrasonicos, PIN_US2_TRIG, PIN_US2_ECHO, "US2_BUFFER", &g_id_ultrasonico_2_buffer));

    hcsr04_poll_config_t configuracion_muestreo_us = HCSR04_POLL_CONFIG_DEFAULT();
    configuracion_muestreo_us.period_ms = 30;
    configuracion_muestreo_us.samples = 3;
    configuracion_muestreo_us.priority = 4;
    configuracion_muestreo_us.core_id = 1;
    ESP_ERROR_CHECK(hcsr04_group_start_polling(&g_grupo_ultrasonicos, &configuracion_muestreo_us));

#if PCA9685_USE_LEGACY_I2C
    i2c_config_t configuracion_i2c = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = PIN_I2C_SDA,
        .scl_io_num = PIN_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000U,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_PCA_PORT, &configuracion_i2c));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PCA_PORT, configuracion_i2c.mode, 0, 0, 0));
    g_configuracion_pwm = (pca9685_config_t)PCA9685_CONFIG_DEFAULT(I2C_PCA_PORT);
#else
    i2c_master_bus_config_t configuracion_bus_i2c = {
        .i2c_port = I2C_PCA_PORT,
        .sda_io_num = PIN_I2C_SDA,
        .scl_io_num = PIN_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&configuracion_bus_i2c, &s_manejador_bus_i2c));
    g_configuracion_pwm = (pca9685_config_t)PCA9685_CONFIG_DEFAULT(s_manejador_bus_i2c);
#endif

    g_configuracion_pwm.addr    = PCA9685_ADDR;
    g_configuracion_pwm.freq_hz = 100U;
    g_configuracion_pwm.oe_pin  = PIN_PCA_OE;
    ESP_ERROR_CHECK(pca9685_init(&g_controlador_pwm, &g_configuracion_pwm));

    pca9685_cr_config_t configuracion_servo_piston = PCA9685_CR_CONFIG_DEFAULT();
    configuracion_servo_piston.min_us      = PISTON_PULSO_MINIMO_US;
    configuracion_servo_piston.neutral_us  = PISTON_PULSO_NEUTRAL_US;
    configuracion_servo_piston.max_us      = PISTON_PULSO_MAXIMO_US;
    configuracion_servo_piston.deadband_us = PISTON_BANDA_MUERTA_US;
    configuracion_servo_piston.reversed    = false;
    ESP_ERROR_CHECK(pca9685_cr_attach(&g_controlador_pwm, PCA_CH_PISTON, &configuracion_servo_piston));
    pca9685_release(&g_controlador_pwm, PCA_CH_PISTON);

    as5600_bus_config_t configuracion_bus_encoders = {
#if PCA9685_USE_LEGACY_I2C
        .hw_bus = I2C_PCA_PORT,
#else
        .hw_bus = s_manejador_bus_i2c,
#endif
        .i2c_port          = I2C_PCA_PORT,
        .sda_pin           = PIN_I2C_SDA,
        .i2c_hz            = 400000U,
        .i2c_timeout_ms    = 20U,
        .bb_half_period_us = 3U,
        .shared_lock       = g_mutex_bus_i2c,
    };
    ESP_ERROR_CHECK(as5600_bus_init(&g_bus_encoders_magneticos, &configuracion_bus_encoders));

    const sec_config_t configuraciones_ejes[CANTIDAD_EJES_BRAZO] = {
        {   /* Eje 1: BASE */
            .pca = &g_controlador_pwm, .pca_ch = 15U,
            .enc_bus = &g_bus_encoders_magneticos, .enc_use_hw_i2c = true, .enc_scl_pin = GPIO_NUM_NC,
            .neutral_us = 1625U, .us_umbral_pos = 110.0f, .us_umbral_neg = 110.0f,
            .us_max_offset = 620.0f, .vel_umbral_deg_s = 35.0f, .servo_reversed = true,
            .enc_zero_deg = -107.9f, .enc_reversed = false,
            .min_deg = -105.0f, .max_deg = 105.0f,
            .kp = 1.2f, .ki = 0.25f, .kd = 0.02f, .kff_vel = 1.0f,
            .k_grav_deg_s = 0.0f, .grav_zero_deg = 0.0f,
            .i_max_deg_s = 100.0f, .vel_max_deg_s = 25.0f,
            .error_deadband_deg = 2.0f, .lock_tol_deg = 2.2f,
        },
        {   /* Eje 2: HOMBRO */
            .pca = &g_controlador_pwm, .pca_ch = 14U,
            .enc_bus = &g_bus_encoders_magneticos, .enc_use_hw_i2c = false, .enc_scl_pin = PIN_AS5600_2_SCL,
            .neutral_us = 1618U, .us_umbral_pos = 80.0f, .us_umbral_neg = 62.0f,
            .us_max_offset = 600.0f, .vel_umbral_deg_s = 40.0f, .servo_reversed = false,
            .enc_zero_deg = -150.9f, .enc_reversed = true,
            .min_deg = -90.0f, .max_deg = 90.0f,
            .kp = 1.6f, .ki = 0.45f, .kd = 0.03f, .kff_vel = 1.0f,
            .k_grav_deg_s = 12.0f, .grav_zero_deg = 0.0f,
            .i_max_deg_s = 80.0f, .vel_max_deg_s = 25.0f,
            .error_deadband_deg = 2.0f, .lock_tol_deg = 2.2f,
        },
        {   /* Eje 3: CODO */
            .pca = &g_controlador_pwm, .pca_ch = 13U,
            .enc_bus = &g_bus_encoders_magneticos, .enc_use_hw_i2c = false, .enc_scl_pin = PIN_AS5600_3_SCL,
            .neutral_us = 1618U, .us_umbral_pos = 80.0f, .us_umbral_neg = 80.0f,
            .us_max_offset = 550.0f, .vel_umbral_deg_s = 45.0f, .servo_reversed = false,
            .enc_zero_deg = -137.5f, .enc_reversed = true,
            .min_deg = -135.0f, .max_deg = 160.0f,
            .kp = 1.4f, .ki = 0.35f, .kd = 0.02f, .kff_vel = 1.0f,
            .k_grav_deg_s = 10.0f, .grav_zero_deg = 0.0f,
            .i_max_deg_s = 120.0f, .vel_max_deg_s = 30.0f,
            .error_deadband_deg = 2.0f, .lock_tol_deg = 2.2f,
        },
        {   /* Eje 4: MUÑECA */
            .pca = &g_controlador_pwm, .pca_ch = 12U,
            .enc_bus = &g_bus_encoders_magneticos, .enc_use_hw_i2c = false, .enc_scl_pin = PIN_AS5600_4_SCL,
            .neutral_us = 1623U, .us_umbral_pos = 45.0f, .us_umbral_neg = 45.0f,
            .us_max_offset = 320.0f, .vel_umbral_deg_s = 42.7f, .servo_reversed = true,
            .enc_zero_deg = -107.8f, .enc_reversed = false,
            .min_deg = -105.0f, .max_deg = 110.0f,
            .kp = 1.2f, .ki = 0.25f, .kd = 0.02f, .kff_vel = 1.0f,
            .k_grav_deg_s = 3.0f, .grav_zero_deg = 0.0f,
            .i_max_deg_s = 60.0f, .vel_max_deg_s = 25.0f,
            .error_deadband_deg = 2.5f, .lock_tol_deg = 2.8f,
        },  
    };

    for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
        ESP_ERROR_CHECK(sec_init(&g_ejes_brazo[indice_eje], &configuraciones_ejes[indice_eje]));
        ESP_ERROR_CHECK(sec_poll_sensor(&g_ejes_brazo[indice_eje], 0.0f, true));
    }

    (void)hc595_blink(&g_registro_desplazamiento, SR_LED_GREEN, 500, 500, 0);
    return ESP_OK;
}
