#include "uart_cli.h"
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "hw_init.h"
#include "pines.h"
#include "tarea_brazo.h"
#include "tipos_de_sistema.h"

//=====================================================
//---- Utilidades de Conversión de Unidades (cm/m) ----
//=====================================================
static inline float convertir_coordenada_a_metros(float valor_entrada) {
    return (fabsf(valor_entrada) > 2.0f) ? (valor_entrada * 0.01f) : valor_entrada;
}

static inline float convertir_distancia_a_metros(float valor_entrada) {
    return (fabsf(valor_entrada) > 1.0f) ? (valor_entrada * 0.01f) : valor_entrada;
}

//=====================================================
//----------- Menú de Ayuda por Puerto Serie ----------
//=====================================================
void imprimir_ayuda_comandos_uart(void) {
    printf("\n=== SERVO_ENC_CTRL + CINTAS + CLASIFICADOR IK ===\n");
    printf("Comandos del Brazo:\n");
    printf("  ANG <1..4> <deg>          -> Mueve un eje al angulo indicado\n");
    printf("  ALL <d1> <d2> <d3> <d4>   -> Mueve los 4 ejes simultaneamente\n");
    printf("  HOME                      -> Regresa suave a [0, 0, 0, 0]\n");
    printf("  HOLD                      -> Captura y sostiene postura actual\n");
    printf("  STOP                      -> Libera motores y detiene cintas\n");
    printf("  PID <1..4> <kp> <ki> <kd> -> Ajusta ganancias PID en vivo\n");
    printf("  GRAV <1..4> <deg_s>       -> Ajusta compensacion de gravedad\n");
    printf("  STATUS                    -> Muestra estado angular y posicion XYZ\n");
    printf("  REINIT                    -> Reinicializa PCA9685 en vivo\n");
    printf("Comandos de Posiciones y Ciclo:\n");
    printf("  START                     -> Arranca cintas\n");
    printf("  PICK <x1> <y1> <z1>       -> Posicion de recogida (m o cm)\n");
    printf("  LIFT <dz>                 -> Elevacion en +Z (m o cm)\n");
    printf("  CONT <x> <y>              -> Posicion XY de contenedores (m o cm)\n");
    printf("  ZCOLOR <zR> <zA> <zAm>    -> Alturas Z por color (m o cm)\n");
    printf("  ASENTAR <avance> <bajada> -> Avance y bajada al depositar (m o cm)\n");
    printf("  POS                       -> Muestra coordenadas configuradas\n");
    printf("  TEST <ROJO|AZUL|AMARILLO> -> Prueba ciclo completo Pick & Place\n\n");
}

//=====================================================
//------- Intérprete de Comandos Recibidos UART -------
//=====================================================
static void ejecutar_linea_comando_uart(const char *linea_comando) {
    int numero_eje = 0;
    float param_1 = 0.0f, param_2 = 0.0f, param_3 = 0.0f, param_4 = 0.0f;

    if (strcmp(linea_comando, "STOP") == 0) {
        g_sistema_en_marcha = false;
        (void)hc595_stepper_stop(&g_motor_cinta_1);
        (void)hc595_stepper_stop(&g_motor_cinta_2);
        actualizar_leds_indicadores_color(COLOR_PALLET_NINGUNO);

        xQueueReset(g_cola_pallets_cinta_1);
        xQueueReset(g_cola_pallets_cinta_2);
        xQueueReset(g_cola_objetivos_brazo);

        cancelar_movimiento_y_secuencia_brazo();

        for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
            sec_disable(&g_ejes_brazo[indice_eje], true);
        }
        pca9685_release(&g_controlador_pwm, PCA_CH_PISTON);

        ESP_LOGI("CINTA1", "0");
        ESP_LOGI("CINTA2", "0");
        ESP_LOGI("PISTON", "QUIETO");
        ESP_LOGI("COLOR", "INDEFINIDO");
        ESP_LOGI("PWM", "0, 0, 0, 0");
        printf("[STOP] Motores y cintas detenidos.\n");

    } else if (strcmp(linea_comando, "START") == 0) {
        if (!g_sistema_en_marcha) {
            g_sistema_en_marcha = true;
            (void)hc595_stepper_run(&g_motor_cinta_1, g_configuracion_transporte.velocidad_cintas_pasos_s);
            ESP_LOGI("CINTA1", "1");
            printf("[START] Sistema de cintas en marcha.\n");
        }

    } else if (strcmp(linea_comando, "REINIT") == 0) {
        cancelar_movimiento_y_secuencia_brazo();
        for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
            sec_disable(&g_ejes_brazo[indice_eje], true);
        }
        ESP_ERROR_CHECK(reinicializar_controlador_pca9685());
        printf("[REINIT] PCA9685 reinicializado.\n");

    } else if (strcmp(linea_comando, "HOLD") == 0) {
        cancelar_movimiento_y_secuencia_brazo();
        for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
            sec_enable(&g_ejes_brazo[indice_eje]);
        }
        printf("[HOLD] Postura enclavada con PID activo.\n");

    } else if (strcmp(linea_comando, "HOME") == 0) {
        const float posicion_reposo[CANTIDAD_EJES_BRAZO] = { 0.0f, 0.0f, 0.0f, 0.0f };
        ordenar_movimiento_articular_suave(posicion_reposo, true);
        printf("[HOME] Moviendo a [0.0, 0.0, 0.0, 0.0] deg\n");

    } else if (sscanf(linea_comando, "ANG %d %f", &numero_eje, &param_1) == 2) {
        if (numero_eje >= 1 && numero_eje <= CANTIDAD_EJES_BRAZO) {
            ordenar_objetivo_articular_directo((uint8_t)(numero_eje - 1), param_1);
            printf("[ANG] Eje %d (%s) -> %.1f deg\n",
                   numero_eje, g_nombres_ejes_brazo[numero_eje - 1], param_1);
        }

    } else if (sscanf(linea_comando, "ALL %f %f %f %f", &param_1, &param_2, &param_3, &param_4) == 4) {
        const float objetivos[CANTIDAD_EJES_BRAZO] = { param_1, param_2, param_3, param_4 };
        ordenar_movimiento_articular_suave(objetivos, false);
        printf("[ALL] Objetivos suaves -> [%.1f, %.1f, %.1f, %.1f] deg\n",
               param_1, param_2, param_3, param_4);

    } else if (sscanf(linea_comando, "PID %d %f %f %f", &numero_eje, &param_1, &param_2, &param_3) == 4) {
        if (numero_eje >= 1 && numero_eje <= CANTIDAD_EJES_BRAZO) {
            sec_set_pid(&g_ejes_brazo[numero_eje - 1], param_1, param_2, param_3);
            printf("[PID] Eje %d: Kp=%.2f Ki=%.2f Kd=%.2f\n", numero_eje, param_1, param_2, param_3);
        }

    } else if (sscanf(linea_comando, "GRAV %d %f", &numero_eje, &param_1) == 2) {
        if (numero_eje >= 1 && numero_eje <= CANTIDAD_EJES_BRAZO) {
            sec_set_gravity(&g_ejes_brazo[numero_eje - 1], param_1, 0.0f);
            printf("[GRAV] Eje %d: K_grav=%.1f deg/s\n", numero_eje, param_1);
        }

    } else if (strcmp(linea_comando, "STATUS") == 0) {
        imprimir_estado_articular_y_cartesiano();

    } else if (sscanf(linea_comando, "RAW %d %f", &numero_eje, &param_1) == 2) {
        if (numero_eje >= 0 && numero_eje <= 15) {
            cancelar_movimiento_y_secuencia_brazo();
            for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
                sec_disable(&g_ejes_brazo[indice_eje], false);
            }
            pca9685_servo_write_us(&g_controlador_pwm, (uint8_t)numero_eje, param_1);
            printf("[RAW] Canal %d -> %.1f us\n", numero_eje, param_1);
        }

    } else if (strcmp(linea_comando, "POS") == 0) {
        imprimir_configuracion_posiciones_brazo();

    } else if (sscanf(linea_comando, "PICK %f %f %f", &param_1, &param_2, &param_3) == 3) {
        configuracion_trabajo_brazo_t candidata;
        xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
        candidata = g_configuracion_trabajo_brazo;
        xSemaphoreGive(g_mutex_configuracion_brazo);

        candidata.punto_recogida_m.x = convertir_coordenada_a_metros(param_1);
        candidata.punto_recogida_m.y = convertir_coordenada_a_metros(param_2);
        candidata.punto_recogida_m.z = convertir_coordenada_a_metros(param_3);

        if (validar_configuracion_completa_trabajo(&candidata)) {
            xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
            g_configuracion_trabajo_brazo = candidata;
            xSemaphoreGive(g_mutex_configuracion_brazo);
            printf("[PICK] Nueva recogida: [%.3f, %.3f, %.3f] m\n",
                   candidata.punto_recogida_m.x,
                   candidata.punto_recogida_m.y,
                   candidata.punto_recogida_m.z);
        }

    } else if (sscanf(linea_comando, "LIFT %f", &param_1) == 1) {
        configuracion_trabajo_brazo_t candidata;
        xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
        candidata = g_configuracion_trabajo_brazo;
        xSemaphoreGive(g_mutex_configuracion_brazo);

        candidata.elevacion_vertical_z_m = fabsf(convertir_distancia_a_metros(param_1));
        if (validar_configuracion_completa_trabajo(&candidata)) {
            xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
            g_configuracion_trabajo_brazo = candidata;
            xSemaphoreGive(g_mutex_configuracion_brazo);
            printf("[LIFT] Nueva elevacion Z: +%.3f m\n", candidata.elevacion_vertical_z_m);
        }

    } else if (sscanf(linea_comando, "CONT %f %f", &param_1, &param_2) == 2) {
        configuracion_trabajo_brazo_t candidata;
        xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
        candidata = g_configuracion_trabajo_brazo;
        xSemaphoreGive(g_mutex_configuracion_brazo);

        candidata.contenedor_x_m = convertir_coordenada_a_metros(param_1);
        candidata.contenedor_y_m = convertir_coordenada_a_metros(param_2);
        if (validar_configuracion_completa_trabajo(&candidata)) {
            xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
            g_configuracion_trabajo_brazo = candidata;
            xSemaphoreGive(g_mutex_configuracion_brazo);
            printf("[CONT] Nueva posicion XY contenedores: [%.3f, %.3f] m\n",
                   candidata.contenedor_x_m, candidata.contenedor_y_m);
        }

    } else if (sscanf(linea_comando, "ZCOLOR %f %f %f", &param_1, &param_2, &param_3) == 3) {
        configuracion_trabajo_brazo_t candidata;
        xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
        candidata = g_configuracion_trabajo_brazo;
        xSemaphoreGive(g_mutex_configuracion_brazo);

        candidata.altura_z_rojo_m     = convertir_coordenada_a_metros(param_1);
        candidata.altura_z_azul_m     = convertir_coordenada_a_metros(param_2);
        candidata.altura_z_amarillo_m = convertir_coordenada_a_metros(param_3);

        if (validar_configuracion_completa_trabajo(&candidata)) {
            xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
            g_configuracion_trabajo_brazo = candidata;
            xSemaphoreGive(g_mutex_configuracion_brazo);
            printf("[ZCOLOR] ROJO=%.3f m | AZUL=%.3f m | AMARILLO=%.3f m\n",
                   candidata.altura_z_rojo_m,
                   candidata.altura_z_azul_m,
                   candidata.altura_z_amarillo_m);
        }

    } else if (sscanf(linea_comando, "ASENTAR %f %f", &param_1, &param_2) == 2) {
        configuracion_trabajo_brazo_t candidata;
        xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
        candidata = g_configuracion_trabajo_brazo;
        xSemaphoreGive(g_mutex_configuracion_brazo);

        candidata.avance_horizontal_deposito_m   = fabsf(convertir_distancia_a_metros(param_1));
        candidata.bajada_vertical_asentamiento_m = fabsf(convertir_distancia_a_metros(param_2));

        if (validar_configuracion_completa_trabajo(&candidata)) {
            xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
            g_configuracion_trabajo_brazo = candidata;
            xSemaphoreGive(g_mutex_configuracion_brazo);
            printf("[ASENTAR] Avance=%.3f m | Bajada=%.3f m\n",
                   candidata.avance_horizontal_deposito_m,
                   candidata.bajada_vertical_asentamiento_m);
        }

    } else if (strncmp(linea_comando, "TEST ", 5) == 0) {
        color_pallet_t color_prueba = COLOR_PALLET_NINGUNO;
        if (strstr(linea_comando + 5, "ROJ") != NULL)      color_prueba = COLOR_PALLET_ROJO;
        else if (strstr(linea_comando + 5, "AZU") != NULL) color_prueba = COLOR_PALLET_AZUL;
        else if (strstr(linea_comando + 5, "AMA") != NULL) color_prueba = COLOR_PALLET_AMARILLO;

        if (color_prueba != COLOR_PALLET_NINGUNO) {
            (void)xQueueSend(g_cola_objetivos_brazo, &color_prueba, 0);
            printf("[TEST] Iniciando secuencia Pick & Place para %s\n",
                   obtener_nombre_color_pallet(color_prueba));
        }
    } else if (strcmp(linea_comando, "GET_CFG") == 0) {
        configuracion_trabajo_brazo_t cfg;
        xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
        cfg = g_configuracion_trabajo_brazo;
        xSemaphoreGive(g_mutex_configuracion_brazo);

        for (int i = 0; i < CANTIDAD_EJES_BRAZO; i++) {
            ESP_LOGI("CFG_PID", "%d,%.3f,%.3f,%.3f,%.2f",
                 i + 1,
                 g_ejes_brazo[i].cfg.kp,
                 g_ejes_brazo[i].cfg.ki,
                 g_ejes_brazo[i].cfg.kd,
                 g_ejes_brazo[i].cfg.k_grav_deg_s);
        }
        ESP_LOGI("CFG_POS", "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f",
                 cfg.punto_recogida_m.x, cfg.punto_recogida_m.y, cfg.punto_recogida_m.z,
                 cfg.elevacion_vertical_z_m,
                 cfg.contenedor_x_m, cfg.contenedor_y_m,
                 cfg.altura_z_rojo_m, cfg.altura_z_azul_m, cfg.altura_z_amarillo_m,
                 cfg.avance_horizontal_deposito_m);
        ESP_LOGI("CFG_CINTA", "%lu,%lu,%lu,%.1f,%.3f",
                 (unsigned long)g_configuracion_transporte.piston_retardo_ms,
                 (unsigned long)g_configuracion_transporte.piston_extension_ms,
                 (unsigned long)g_configuracion_transporte.piston_retraccion_ms,
                 g_configuracion_transporte.velocidad_cintas_pasos_s,
                 cfg.bajada_vertical_asentamiento_m);

    } else if (sscanf(linea_comando, "PISTON_CFG %f %f %f %f", &param_1, &param_2, &param_3, &param_4) == 4) {
        g_configuracion_transporte.piston_retardo_ms        = (uint32_t)fabsf(param_1);
        g_configuracion_transporte.piston_extension_ms      = (uint32_t)fabsf(param_2);
        g_configuracion_transporte.piston_retraccion_ms     = (uint32_t)fabsf(param_3);
        g_configuracion_transporte.velocidad_cintas_pasos_s = fabsf(param_4);
        if (g_sistema_en_marcha) {
            (void)hc595_stepper_run(&g_motor_cinta_1, g_configuracion_transporte.velocidad_cintas_pasos_s);
        }
        printf("[PISTON_CFG] Retardo=%lums Ext=%lums Ret=%lums VelCinta=%.0f\n",
               (unsigned long)g_configuracion_transporte.piston_retardo_ms,
               (unsigned long)g_configuracion_transporte.piston_extension_ms,
               (unsigned long)g_configuracion_transporte.piston_retraccion_ms,
               g_configuracion_transporte.velocidad_cintas_pasos_s);

    } else if (strcmp(linea_comando, "SAVE_NVS") == 0) {
        if (guardar_parametros_en_nvs() == ESP_OK) {
            ESP_LOGI("NVS", "SAVED_OK");
            printf("[NVS] Configuracion guardada en flash permanente.\n");
        } else {
            ESP_LOGE("NVS", "SAVE_ERR");
        }

    } else if (strcmp(linea_comando, "RESET_NVS") == 0) {
        (void)borrar_parametros_nvs_y_restaurar_fabrica();
        ESP_LOGI("NVS", "RESET_OK");
        printf("[NVS] Memoria NVS borrada. Reinicia el ESP32 para cargar valores de fabrica.\n");
    }
}

void procesar_comandos_uart_disponibles(void) {
    static char buffer_comando[96];
    static size_t longitud_actual = 0;

    int caracter_recibido;
    while ((caracter_recibido = getchar()) != EOF) {
        if (caracter_recibido == '\r' || caracter_recibido == '\n') {
            if (longitud_actual > 0) {
                buffer_comando[longitud_actual] = '\0';
                for (size_t indice = 0; indice < longitud_actual; indice++) {
                    buffer_comando[indice] = (char)toupper((unsigned char)buffer_comando[indice]);
                }
                ejecutar_linea_comando_uart(buffer_comando);
                longitud_actual = 0;
            }
        } else if (longitud_actual < sizeof(buffer_comando) - 1) {
            buffer_comando[longitud_actual++] = (char)caracter_recibido;
        }
    }
}
