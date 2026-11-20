#include "tarea_cinta.h"
#include "hw_init.h"
#include "tipos_de_sistema.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG_CINTA = "CINTA_SYS";

//=====================================================
//----- Funciones Auxiliares de Detección y Envío -----
//=====================================================
static void encolar_orden_para_piston(tipo_orden_piston_t tipo_orden, float porcentaje_velocidad) {
    if (!g_cola_ordenes_piston) return;

    orden_piston_t orden = {
        .tipo_orden = tipo_orden,
        .porcentaje_velocidad = porcentaje_velocidad
    };
    (void)xQueueSend(g_cola_ordenes_piston, &orden, 0);
}

static color_pallet_t clasificar_color_pallet_calibrado(const tcs3200_sample_t *muestra) {
    if (!muestra || !muestra->valid) return COLOR_PALLET_NINGUNO;

    float frecuencia_clear = muestra->hz100[TCS3200_FILTER_CLEAR];
    if (frecuencia_clear < COLOR_FRECUENCIA_CLEAR_MINIMA_HZ) return COLOR_PALLET_NINGUNO;

    float matiz = muestra->h;
    float saturacion = muestra->s;

    if (matiz >= 226.0f && matiz <= 235.0f && saturacion >= 0.75f) {
        return COLOR_PALLET_AZUL;
    }
    if (matiz >= 241.5f && matiz <= 248.5f && saturacion >= 0.55f && saturacion <= 0.65f && frecuencia_clear >= 6000.0f) {
        return COLOR_PALLET_AMARILLO;
    }
    if (matiz >= 258.0f && matiz <= 276.0f && saturacion >= 0.70f && frecuencia_clear <= 4800.0f) {
        return COLOR_PALLET_ROJO;
    }
    return COLOR_PALLET_NINGUNO;
}

//=====================================================
//---- Tarea Principal: Cintas, Sensores y Pistón -----
//=====================================================
void tarea_logica_cintas_y_piston(void *parametro) {
    (void)parametro;

    hcsr04_reading_t lectura_us1 = {0};
    hcsr04_reading_t lectura_us2 = {0};
    tcs3200_sample_t muestra_color = {0};
    TickType_t ultimo_despertar = xTaskGetTickCount();

    estado_cinta2_t estado_actual_cinta2 = ESTADO_CINTA2_DETENIDA;
    estado_cinta2_t estado_anterior_cinta2_log = ESTADO_CINTA2_DETENIDA;
    int64_t instante_arranque_cinta2_us = 0;

    estado_piston_t estado_actual_piston = ESTADO_PISTON_QUIETO;
    estado_piston_t estado_anterior_piston_log = ESTADO_PISTON_QUIETO;
    int64_t instante_cambio_piston_us = 0;

    bool us1_detectaba_antes = false;
    bool us2_detectaba_antes = false;

    bool deteccion_color_bloqueada = false;
    color_pallet_t color_crudo_anterior = COLOR_PALLET_NINGUNO;
    color_pallet_t color_pallet_confirmado = COLOR_PALLET_NINGUNO;
    uint8_t contador_muestras_estables = 0;
    uint8_t contador_muestras_vacias = 0;
    int64_t fin_zona_muerta_color_us = 0;

    while (1) {
        int64_t tiempo_actual_us = esp_timer_get_time();

        if (!g_sistema_en_marcha) {
            estado_actual_cinta2 = ESTADO_CINTA2_DETENIDA;
            estado_actual_piston = ESTADO_PISTON_QUIETO;
            deteccion_color_bloqueada = false;
            color_pallet_confirmado = COLOR_PALLET_NINGUNO;
            color_crudo_anterior = COLOR_PALLET_NINGUNO;
            contador_muestras_estables = 0;
            contador_muestras_vacias = 0;

            g_telemetria_hmi.color_detectado         = COLOR_PALLET_NINGUNO;
            g_telemetria_hmi.pallets_en_cola_cinta_1 = 0;
            g_telemetria_hmi.pallets_en_cola_cinta_2 = 0;
            g_telemetria_hmi.estado_cinta_2          = ESTADO_CINTA2_DETENIDA;
            g_telemetria_hmi.estado_piston           = ESTADO_PISTON_QUIETO;

            vTaskDelayUntil(&ultimo_despertar, pdMS_TO_TICKS(20));
            continue;
        }

        //-------------------------------------------------
        //     Lectura y Debounce del Sensor de Color
        //-------------------------------------------------
        if (tcs3200_read(&g_sensor_color, &muestra_color) == ESP_OK && muestra_color.valid) {
            color_pallet_t color_crudo = clasificar_color_pallet_calibrado(&muestra_color);

            if (!deteccion_color_bloqueada) {
                if (color_crudo != COLOR_PALLET_NINGUNO) {
                    if (color_crudo == color_crudo_anterior) {
                        if (contador_muestras_estables < COLOR_MUESTRAS_CONFIRMACION) {
                            contador_muestras_estables++;
                        }
                    } else {
                        color_crudo_anterior = color_crudo;
                        contador_muestras_estables = 1;
                    }

                    if (contador_muestras_estables >= COLOR_MUESTRAS_CONFIRMACION) {
                        color_pallet_confirmado = color_crudo;
                        deteccion_color_bloqueada = true;
                        fin_zona_muerta_color_us = tiempo_actual_us + ((int64_t)COLOR_ZONA_MUERTA_DETECCION_MS * 1000LL);
                        contador_muestras_vacias = 0;

                        actualizar_leds_indicadores_color(color_pallet_confirmado);
                        (void)xQueueSend(g_cola_pallets_cinta_1, &color_pallet_confirmado, 0);

                        ESP_LOGI("COLOR", "%s", obtener_nombre_color_para_qt(color_pallet_confirmado));
                        ESP_LOGI(TAG_CINTA, "Pallet confirmado: %s | H=%.1f S=%.2f Clr=%.0f Hz",
                                 obtener_nombre_color_pallet(color_pallet_confirmado),
                                 muestra_color.h, muestra_color.s,
                                 muestra_color.hz100[TCS3200_FILTER_CLEAR]);
                    }
                } else {
                    color_crudo_anterior = COLOR_PALLET_NINGUNO;
                    contador_muestras_estables = 0;
                }
            } else if (tiempo_actual_us >= fin_zona_muerta_color_us) {
                if (color_crudo == COLOR_PALLET_NINGUNO) {
                    if (contador_muestras_vacias < COLOR_MUESTRAS_LIBERACION_VACIO) {
                        contador_muestras_vacias++;
                    }
                    if (contador_muestras_vacias >= COLOR_MUESTRAS_LIBERACION_VACIO) {
                        color_pallet_confirmado = COLOR_PALLET_NINGUNO;
                        color_crudo_anterior = COLOR_PALLET_NINGUNO;
                        contador_muestras_estables = 0;
                        contador_muestras_vacias = 0;
                        deteccion_color_bloqueada = false;

                        actualizar_leds_indicadores_color(COLOR_PALLET_NINGUNO);
                        ESP_LOGI("COLOR", "%s", obtener_nombre_color_para_qt(COLOR_PALLET_NINGUNO));
                    }
                } else {
                    contador_muestras_vacias = 0;
                }
            }
        }

        //-------------------------------------------------
        //  Sensores Ultrasónicos y Transferencia de Cola
        //-------------------------------------------------
        bool pallet_en_ultrasonico_1 = false;
        if (hcsr04_group_get_last(&g_grupo_ultrasonicos, g_id_ultrasonico_1_piston, &lectura_us1) == ESP_OK &&
            lectura_us1.err == ESP_OK) {
            pallet_en_ultrasonico_1 = (lectura_us1.distance_cm < UMBRAL_ULTRASONICO_1_CM);
        }
        if (pallet_en_ultrasonico_1 != us1_detectaba_antes) {
            ESP_LOGI("ULTRA1", "%s", pallet_en_ultrasonico_1 ? "1" : "0");
        }

        bool pallet_en_ultrasonico_2 = false;
        if (hcsr04_group_get_last(&g_grupo_ultrasonicos, g_id_ultrasonico_2_buffer, &lectura_us2) == ESP_OK &&
            lectura_us2.err == ESP_OK) {
            pallet_en_ultrasonico_2 = (lectura_us2.distance_cm < UMBRAL_ULTRASONICO_2_CM);
        }
        if (pallet_en_ultrasonico_2 != us2_detectaba_antes) {
            ESP_LOGI("ULTRA2", "%s", pallet_en_ultrasonico_2 ? "1" : "0");
            us2_detectaba_antes = pallet_en_ultrasonico_2;
        }

        if (!us1_detectaba_antes && pallet_en_ultrasonico_1 && estado_actual_piston == ESTADO_PISTON_QUIETO) {
            instante_cambio_piston_us = tiempo_actual_us + ((int64_t)g_configuracion_transporte.piston_retardo_ms * 1000LL);
            estado_actual_piston = ESTADO_PISTON_ESPERANDO_RETARDO;
        }

        if (us1_detectaba_antes && !pallet_en_ultrasonico_1) {
            color_pallet_t color_transferido;
            if (xQueueReceive(g_cola_pallets_cinta_1, &color_transferido, 0) == pdTRUE) {
                (void)xQueueSend(g_cola_pallets_cinta_2, &color_transferido, 0);
            }

            if (!pallet_en_ultrasonico_2 && estado_actual_cinta2 == ESTADO_CINTA2_DETENIDA) {
                instante_arranque_cinta2_us = tiempo_actual_us + ((int64_t)RETARDO_ARRANQUE_CINTA_2_MS * 1000LL);
                estado_actual_cinta2 = ESTADO_CINTA2_ESPERANDO_RETARDO;
            }
        }
        us1_detectaba_antes = pallet_en_ultrasonico_1;

        if (estado_actual_cinta2 == ESTADO_CINTA2_DETENIDA &&
            !pallet_en_ultrasonico_2 &&
            uxQueueMessagesWaiting(g_cola_pallets_cinta_2) > 0) {
            instante_arranque_cinta2_us = tiempo_actual_us + ((int64_t)RETARDO_ARRANQUE_CINTA_2_MS * 1000LL);
            estado_actual_cinta2 = ESTADO_CINTA2_ESPERANDO_RETARDO;
        }

        //-------------------------------------------------
        //  Máquina de Estados del Pistón Clasificador
        //-------------------------------------------------
        switch (estado_actual_piston) {
            case ESTADO_PISTON_QUIETO:
                break;

            case ESTADO_PISTON_ESPERANDO_RETARDO:
                if (tiempo_actual_us >= instante_cambio_piston_us) {
                    encolar_orden_para_piston(ORDEN_PISTON_VELOCIDAD,
                                              (float)PISTON_DIRECCION_EXTENSION * PISTON_VELOCIDAD_EXTENSION_PCT);
                    instante_cambio_piston_us = tiempo_actual_us + ((int64_t)g_configuracion_transporte.piston_extension_ms * 1000LL);
                    estado_actual_piston = ESTADO_PISTON_EXTENDIENDO;
                }
                break;

            case ESTADO_PISTON_EXTENDIENDO:
                if (tiempo_actual_us >= instante_cambio_piston_us) {
                    if (PISTON_PAUSA_INVERSION_GIRO_MS > 0U) {
                        encolar_orden_para_piston(ORDEN_PISTON_NEUTRAL, 0.0f);
                        instante_cambio_piston_us = tiempo_actual_us + ((int64_t)PISTON_PAUSA_INVERSION_GIRO_MS * 1000LL);
                        estado_actual_piston = ESTADO_PISTON_PAUSA_INVERSION;
                    } else {
                        encolar_orden_para_piston(ORDEN_PISTON_VELOCIDAD,
                                                  (float)PISTON_DIRECCION_RETRACCION * PISTON_VELOCIDAD_RETRACCION_PCT);
                        instante_cambio_piston_us = tiempo_actual_us + ((int64_t)g_configuracion_transporte.piston_retraccion_ms * 1000LL);
                        estado_actual_piston = ESTADO_PISTON_RETRAYENDO;
                    }
                }
                break;

            case ESTADO_PISTON_PAUSA_INVERSION:
                if (tiempo_actual_us >= instante_cambio_piston_us) {
                    encolar_orden_para_piston(ORDEN_PISTON_VELOCIDAD,
                                              (float)PISTON_DIRECCION_RETRACCION * PISTON_VELOCIDAD_RETRACCION_PCT);
                    instante_cambio_piston_us = tiempo_actual_us + ((int64_t)g_configuracion_transporte.piston_retraccion_ms * 1000LL);
                    estado_actual_piston = ESTADO_PISTON_RETRAYENDO;
                }
                break;

            case ESTADO_PISTON_RETRAYENDO:
                if (tiempo_actual_us >= instante_cambio_piston_us) {
                    encolar_orden_para_piston(ORDEN_PISTON_LIBERAR, 0.0f);
                    estado_actual_piston = ESTADO_PISTON_QUIETO;
                }
                break;
        }

        if (estado_actual_piston != estado_anterior_piston_log) {
            ESP_LOGI("PISTON", "%s", obtener_texto_estado_piston(estado_actual_piston));
            estado_anterior_piston_log = estado_actual_piston;
        }

        //------------------------------------
        //   Máquina de Estados de la Cinta 2
        //------------------------------------
        switch (estado_actual_cinta2) {
            case ESTADO_CINTA2_DETENIDA:
                break;

            case ESTADO_CINTA2_ESPERANDO_RETARDO:
                if (pallet_en_ultrasonico_2) {
                    estado_actual_cinta2 = ESTADO_CINTA2_DETENIDA;
                } else if (tiempo_actual_us >= instante_arranque_cinta2_us) {
                    hc595_stepper_run(&g_motor_cinta_2, g_configuracion_transporte.velocidad_cintas_pasos_s);
                    estado_actual_cinta2 = ESTADO_CINTA2_EN_MARCHA;
                }
                break;

            case ESTADO_CINTA2_EN_MARCHA:
                if (pallet_en_ultrasonico_2) {
                    hc595_stepper_stop(&g_motor_cinta_2);
                    estado_actual_cinta2 = ESTADO_CINTA2_DETENIDA;

                    color_pallet_t color_listo_para_brazo;
                    if (xQueueReceive(g_cola_pallets_cinta_2, &color_listo_para_brazo, 0) == pdTRUE) {
                        (void)xQueueSend(g_cola_objetivos_brazo, &color_listo_para_brazo, 0);
                    }
                }
                break;
        }

        if ((estado_actual_cinta2 == ESTADO_CINTA2_EN_MARCHA) !=
            (estado_anterior_cinta2_log == ESTADO_CINTA2_EN_MARCHA)) {
            ESP_LOGI("CINTA2", "%s", (estado_actual_cinta2 == ESTADO_CINTA2_EN_MARCHA) ? "1" : "0");
        }
        estado_anterior_cinta2_log = estado_actual_cinta2;

        //-----------------------------
        //  Publicación de Telemetría 
        //-----------------------------
        g_telemetria_hmi.color_detectado         = color_pallet_confirmado;
        g_telemetria_hmi.matiz_h                 = muestra_color.h;
        g_telemetria_hmi.saturacion_s            = muestra_color.s;
        g_telemetria_hmi.frecuencia_clear_hz     = muestra_color.hz100[TCS3200_FILTER_CLEAR];
        g_telemetria_hmi.distancia_ultrasonico_1_cm = lectura_us1.distance_cm;
        g_telemetria_hmi.distancia_ultrasonico_2_cm = lectura_us2.distance_cm;
        g_telemetria_hmi.pallets_en_cola_cinta_1 = (uint8_t)uxQueueMessagesWaiting(g_cola_pallets_cinta_1);
        g_telemetria_hmi.pallets_en_cola_cinta_2 = (uint8_t)uxQueueMessagesWaiting(g_cola_pallets_cinta_2);
        g_telemetria_hmi.estado_cinta_2          = estado_actual_cinta2;
        g_telemetria_hmi.estado_piston           = estado_actual_piston;

        vTaskDelayUntil(&ultimo_despertar, pdMS_TO_TICKS(20));
    }
}
