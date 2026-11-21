#include "tarea_brazo.h"
#include <math.h>
#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hw_init.h"
#include "uart_cli.h"
#include "Global_Config.h"
#include "Control_Brazo_Robotico/Perfil_S.h"

static const char *TAG_BRAZO = "BRAZO_TEACH";

// ============================================================================
// PUNTOS TEACH-IN ARTICULARES [Base, Hombro, Codo, Muñeca] EN GRADOS
// Rellena estos arreglos leyendo "MED:[...]" con el comando STATUS
// mientras posicionas el brazo con la mano (con DIS activado).
// ============================================================================
static const float POSICION_HOME_GRADOS[CANTIDAD_EJES_BRAZO] = {
    0.3f,   -6.5f,  130.5f,   0.0f
};

// 1. Secuencia de recogida en cinta
static const float PUNTO_DEG_APROX_RECOGIDA[CANTIDAD_EJES_BRAZO] = {
    -7.8f,  55.5f,   73.9f,  -19.0f   // Encima / antes de entrar al pallet
};
static const float PUNTO_DEG_RECOGIDA[CANTIDAD_EJES_BRAZO] = {
    -7.8f,  62.3f,   2.9f,  -12.9f   // Horquilla metida en el pallet sobre la cinta
};
static const float PUNTO_DEG_ELEVACION[CANTIDAD_EJES_BRAZO] = {
    -7.1f,  35.3f,   5.9f,  -8.8f   // Pa llet levantado verticalmente
};
static const float PUNTO_DEG_TRANSITO_ORIGEN[CANTIDAD_EJES_BRAZO] = {
    -7.1f,  8.4f,  138.5f,   -41.6f   // Brazo replegado antes de girar la base
};

// 2. Estructura y puntos de depósito por contenedor
typedef struct {
    float transito_destino[CANTIDAD_EJES_BRAZO]; // Frente al contenedor (replegado/alto)
    float entrada[CANTIDAD_EJES_BRAZO];          // Boca del contenedor a su altura Z
    float avance[CANTIDAD_EJES_BRAZO];           // Adentro del contenedor
    float asentamiento[CANTIDAD_EJES_BRAZO];     // Bajada suave para apoyar el pallet
    float retroceso[CANTIDAD_EJES_BRAZO];        // Salida hacia atrás sin arrastrar
} puntos_contenedor_deg_t;

static const puntos_contenedor_deg_t PUNTO_CONTENEDOR_ROJO = {
    .transito_destino = {  85.4f,  -24.5f,  125.0f,   4.1f },
    .entrada          = {  85.4f,  -24.5f,  125.1f,   4.1f },
    .avance           = {  85.8f,  9.4f, 86.6f,  3.7f },
    .asentamiento     = {  85.8f,  11.1f,   92.6f,  0.7f },
    .retroceso        = {  85.4f,  -24.5f,   125.1f,  5.1f }
};

static const puntos_contenedor_deg_t PUNTO_CONTENEDOR_AZUL = {
    .transito_destino = {  85.9f,  15.0f,  110.0f,   10.0f },
    .entrada          = {  85.9f,  15.0f,  110.5f,   10.0f },
    .avance           = {  85.1f,  30.2f,   108.5f,  -21.8f },
    .asentamiento     = {  85.2f,  30.7f,   112.2f,  -21.6f },
    .retroceso        = {  85.9f,  15.0f,   110.5f,  10.0f }
};

static const puntos_contenedor_deg_t PUNTO_CONTENEDOR_AMARILLO = {
    .transito_destino = { 85.6f,  47.0f,  135.8f,   -107.1f },
    .entrada          = { 85.6f,  47.0f,   135.8f,   -107.1f },
    .avance           = { 85.6f,  44.9f,   99.7f,   -75.3f },
    .asentamiento     = { 85.6f,  44.8f,   101.7f,   -66.1f },
    .retroceso        = { 85.6f,  47.0f,   135.8f,   -107.1f }
};

//================================
//----- Estructuras Internas -----
//================================
typedef struct {
    planificador_perfil_s_t planificador;
    float angulo_inicial_rad[CANTIDAD_EJES_BRAZO];
    float angulo_final_rad[CANTIDAD_EJES_BRAZO];
    float tiempo_transcurrido_s;
    bool activa;
} trayectoria_articular_t;

static estado_secuencia_brazo_t s_estado_secuencia = SECUENCIA_BRAZO_LISTO;
static color_pallet_t s_color_pallet_activo = COLOR_PALLET_NINGUNO;
static puntos_contenedor_deg_t s_puntos_contenedor_activo = {0};

static bool s_aproximacion_recogida_completada = false;
static bool s_giro_en_radio_minimo_completado = false;

static trayectoria_articular_t s_trayectoria_articular = {0};

static bool s_trayectoria_finalizada = false;
static uint32_t s_tiempo_espera_enclave_ms = 0;
static uint32_t s_tiempo_estable_continuo_ms = 0;

//=====================================================
//---- Validación Articular y Telemetría de Estado ----
//=====================================================
static float normalizar_angulo_deg(float deg) {
    while (deg > 180.0f)  deg -= 360.0f;
    while (deg < -180.0f) deg += 360.0f;
    return deg;
}

static bool validar_pose_articular_en_limites(const float pose_deg[CANTIDAD_EJES_BRAZO], const char *nombre_punto) {
    for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
        float ang = normalizar_angulo_deg(pose_deg[indice_eje]);
        if (ang < g_ejes_brazo[indice_eje].cfg.min_deg ||
            ang > g_ejes_brazo[indice_eje].cfg.max_deg) {
            if (nombre_punto != NULL) {
                printf("[ERROR LIMITE] Punto '%s' exige %s = %.1f deg (rango [%.1f, %.1f] deg)\n",
                       nombre_punto,
                       g_nombres_ejes_brazo[indice_eje],
                       ang,
                       g_ejes_brazo[indice_eje].cfg.min_deg,
                       g_ejes_brazo[indice_eje].cfg.max_deg);
            }
            return false;
        }
    }
    return true;
}

bool validar_configuracion_completa_trabajo(const configuracion_trabajo_brazo_t *configuracion) {
    (void)configuracion;

    if (!validar_pose_articular_en_limites(POSICION_HOME_GRADOS, "HOME")) return false;
    if (!validar_pose_articular_en_limites(PUNTO_DEG_APROX_RECOGIDA, "APROX_RECOGIDA")) return false;
    if (!validar_pose_articular_en_limites(PUNTO_DEG_RECOGIDA, "RECOGIDA")) return false;
    if (!validar_pose_articular_en_limites(PUNTO_DEG_ELEVACION, "ELEVACION")) return false;
    if (!validar_pose_articular_en_limites(PUNTO_DEG_TRANSITO_ORIGEN, "TRANSITO_ORIGEN")) return false;

    const puntos_contenedor_deg_t *contenedores[3] = {
        &PUNTO_CONTENEDOR_ROJO,
        &PUNTO_CONTENEDOR_AZUL,
        &PUNTO_CONTENEDOR_AMARILLO
    };
    const char *nombres[3] = { "ROJO", "AZUL", "AMARILLO" };

    for (int i = 0; i < 3; i++) {
        if (!validar_pose_articular_en_limites(contenedores[i]->transito_destino, nombres[i])) return false;
        if (!validar_pose_articular_en_limites(contenedores[i]->entrada, nombres[i])) return false;
        if (!validar_pose_articular_en_limites(contenedores[i]->avance, nombres[i])) return false;
        if (!validar_pose_articular_en_limites(contenedores[i]->asentamiento, nombres[i])) return false;
        if (!validar_pose_articular_en_limites(contenedores[i]->retroceso, nombres[i])) return false;
    }

    return true;
}

void imprimir_configuracion_posiciones_brazo(void) {
    printf("\n--- PUNTOS TEACH-IN ARTICULARES CONFIGURADOS (deg) ---\n");
    printf("  HOME            : [%6.1f, %6.1f, %6.1f, %6.1f]\n",
           POSICION_HOME_GRADOS[0], POSICION_HOME_GRADOS[1], POSICION_HOME_GRADOS[2], POSICION_HOME_GRADOS[3]);
    printf("  Aprox Recogida  : [%6.1f, %6.1f, %6.1f, %6.1f]\n",
           PUNTO_DEG_APROX_RECOGIDA[0], PUNTO_DEG_APROX_RECOGIDA[1], PUNTO_DEG_APROX_RECOGIDA[2], PUNTO_DEG_APROX_RECOGIDA[3]);
    printf("  Recogida Cinta  : [%6.1f, %6.1f, %6.1f, %6.1f]\n",
           PUNTO_DEG_RECOGIDA[0], PUNTO_DEG_RECOGIDA[1], PUNTO_DEG_RECOGIDA[2], PUNTO_DEG_RECOGIDA[3]);
    printf("  Elevacion       : [%6.1f, %6.1f, %6.1f, %6.1f]\n",
           PUNTO_DEG_ELEVACION[0], PUNTO_DEG_ELEVACION[1], PUNTO_DEG_ELEVACION[2], PUNTO_DEG_ELEVACION[3]);
    printf("  Transito Origen : [%6.1f, %6.1f, %6.1f, %6.1f]\n",
           PUNTO_DEG_TRANSITO_ORIGEN[0], PUNTO_DEG_TRANSITO_ORIGEN[1], PUNTO_DEG_TRANSITO_ORIGEN[2], PUNTO_DEG_TRANSITO_ORIGEN[3]);
    printf("  Entrada ROJO    : [%6.1f, %6.1f, %6.1f, %6.1f]\n",
           PUNTO_CONTENEDOR_ROJO.entrada[0], PUNTO_CONTENEDOR_ROJO.entrada[1], PUNTO_CONTENEDOR_ROJO.entrada[2], PUNTO_CONTENEDOR_ROJO.entrada[3]);
    printf("  Entrada AZUL    : [%6.1f, %6.1f, %6.1f, %6.1f]\n",
           PUNTO_CONTENEDOR_AZUL.entrada[0], PUNTO_CONTENEDOR_AZUL.entrada[1], PUNTO_CONTENEDOR_AZUL.entrada[2], PUNTO_CONTENEDOR_AZUL.entrada[3]);
    printf("  Entrada AMARILLO: [%6.1f, %6.1f, %6.1f, %6.1f]\n\n",
           PUNTO_CONTENEDOR_AMARILLO.entrada[0], PUNTO_CONTENEDOR_AMARILLO.entrada[1], PUNTO_CONTENEDOR_AMARILLO.entrada[2], PUNTO_CONTENEDOR_AMARILLO.entrada[3]);
}

void imprimir_estado_articular_y_cartesiano(void) {
    for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
        printf("Eje %d (%-6s): MED=%6.1f deg | SP=%6.1f deg | I=%+5.1f deg/s | ON=%d LOCK=%d\n",
               indice_eje + 1,
               g_nombres_ejes_brazo[indice_eje],
               sec_get_angle_deg(&g_ejes_brazo[indice_eje]),
               sec_rad2deg_(g_ejes_brazo[indice_eje].sp_rad),
               sec_rad2deg_(g_ejes_brazo[indice_eje].integrator_rad_s),
               g_ejes_brazo[indice_eje].enabled,
               g_ejes_brazo[indice_eje].locked);
    }

    printf("TEACH-IN LISTO PARA COPIAR -> { %.1ff, %.1ff, %.1ff, %.1ff }\n",
           sec_get_angle_deg(&g_ejes_brazo[0]),
           sec_get_angle_deg(&g_ejes_brazo[1]),
           sec_get_angle_deg(&g_ejes_brazo[2]),
           sec_get_angle_deg(&g_ejes_brazo[3]));
}

//=====================================================
//---- Planificador de Trayectoria con Perfil S -------
//=====================================================
static void iniciar_trayectoria_articular_suave(const float objetivos_grados[CANTIDAD_EJES_BRAZO],
                                                bool es_retorno_reposo) {
    limites_perfil_s_t limites = {
        .v_max = PERFIL_S_VELOCIDAD_MAXIMA,
        .a_max = PERFIL_S_ACELERACION_MAXIMA,
        .j_max = PERFIL_S_JERK_MAXIMO,
    };

    float desplazamiento_maximo_grados = 0.0f;
    for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
        if (!g_ejes_brazo[indice_eje].enabled) {
            (void)sec_enable(&g_ejes_brazo[indice_eje]);
        }
        float angulo_actual_deg = sec_get_angle_deg(&g_ejes_brazo[indice_eje]);
        float objetivo_norm_deg = normalizar_angulo_deg(objetivos_grados[indice_eje]);
        float angulo_limitado_deg = sec_clamp_(objetivo_norm_deg,
                                               g_ejes_brazo[indice_eje].cfg.min_deg,
                                               g_ejes_brazo[indice_eje].cfg.max_deg);

        s_trayectoria_articular.angulo_inicial_rad[indice_eje] = sec_deg2rad_(angulo_actual_deg);
        s_trayectoria_articular.angulo_final_rad[indice_eje]   = sec_deg2rad_(angulo_limitado_deg);

        float delta_grados = fabsf(angulo_limitado_deg - angulo_actual_deg);
        if (delta_grados > desplazamiento_maximo_grados) {
            desplazamiento_maximo_grados = delta_grados;
        }
    }

    if (es_retorno_reposo) {
        limites.v_max = 0.08f;
        limites.a_max = 0.06f;
        limites.j_max = 0.25f;
    } else if (desplazamiento_maximo_grados > 40.0f) {
        limites.v_max = PERFIL_S_VELOCIDAD_MAXIMA * (40.0f / desplazamiento_maximo_grados);
        if (limites.v_max < 0.12f) limites.v_max = 0.12f;
    }

    Iniciar_Perfil_S(&s_trayectoria_articular.planificador, limites);
    (void)Planificar_Perfil_s(&s_trayectoria_articular.planificador, 0.0f, 1.0f);
    s_trayectoria_articular.tiempo_transcurrido_s = 0.0f;
    s_trayectoria_articular.activa = true;
}

static bool evaluar_trayectoria_articular_suave(float paso_tiempo_s) {
    if (!s_trayectoria_articular.activa) return true;

    s_trayectoria_articular.tiempo_transcurrido_s += paso_tiempo_s;
    estado_perfil_s_t estado_perfil = Estado_Perfil_s(&s_trayectoria_articular.planificador,
                                                      s_trayectoria_articular.tiempo_transcurrido_s);

    for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
        float recorrido_rad = s_trayectoria_articular.angulo_final_rad[indice_eje] -
                              s_trayectoria_articular.angulo_inicial_rad[indice_eje];
        float posicion_rad = s_trayectoria_articular.angulo_inicial_rad[indice_eje] +
                             estado_perfil.posicion * recorrido_rad;
        float velocidad_rad_s = estado_perfil.termino ? 0.0f : (estado_perfil.velocidad * recorrido_rad);

        sec_set_trajectory_rad(&g_ejes_brazo[indice_eje], posicion_rad, velocidad_rad_s);
    }

    if (estado_perfil.termino) {
        s_trayectoria_articular.activa = false;
        return true;
    }
    return false;
}

//=====================================================
//---- Verificación de Estabilización Anti-Rebote -----
//=====================================================
static bool verificar_brazo_estable_continuamente(void) {
    bool todos_enclavados = true;
    for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
        if (!sec_is_locked(&g_ejes_brazo[indice_eje])) {
            todos_enclavados = false;
            break;
        }
    }

    if (todos_enclavados) {
        s_tiempo_estable_continuo_ms += PERIODO_LAZO_CONTROL_MS;
    } else {
        s_tiempo_estable_continuo_ms = 0;
    }

    s_tiempo_espera_enclave_ms += PERIODO_LAZO_CONTROL_MS;

    return (s_tiempo_estable_continuo_ms >= TIEMPO_ESTABILIZACION_CONTINUA_MS) ||
           (s_tiempo_espera_enclave_ms >= TIEMPO_MAXIMO_ESPERA_ENCLAVE_MS);
}

static void preparar_siguiente_paso_articular(const float objetivos_grados[CANTIDAD_EJES_BRAZO],
                                              estado_secuencia_brazo_t nuevo_estado) {
    bool es_reposo = (nuevo_estado == SECUENCIA_BRAZO_IR_A_REPOSO_FINAL) ||
                     (nuevo_estado == SECUENCIA_BRAZO_MOVIMIENTO_MANUAL);
    iniciar_trayectoria_articular_suave(objetivos_grados, es_reposo);
    s_trayectoria_finalizada = false;
    s_tiempo_espera_enclave_ms = 0;
    s_tiempo_estable_continuo_ms = 0;
    s_estado_secuencia = nuevo_estado;
}

//=====================================================
//------- Funciones Públicas de Control Manual --------
//=====================================================
void cancelar_movimiento_y_secuencia_brazo(void) {
    s_trayectoria_articular.activa = false;
    s_estado_secuencia = SECUENCIA_BRAZO_LISTO;
    s_color_pallet_activo = COLOR_PALLET_NINGUNO;
    s_aproximacion_recogida_completada = false;
    s_giro_en_radio_minimo_completado = false;
    s_tiempo_estable_continuo_ms = 0;
    s_tiempo_espera_enclave_ms = 0;
}

void ordenar_movimiento_articular_suave(const float objetivos_grados[CANTIDAD_EJES_BRAZO], bool es_retorno_reposo) {
    if (es_retorno_reposo) {
        preparar_siguiente_paso_articular(POSICION_HOME_GRADOS, SECUENCIA_BRAZO_MOVIMIENTO_MANUAL);
    } else {
        preparar_siguiente_paso_articular(objetivos_grados, SECUENCIA_BRAZO_LISTO);
    }
}

void ordenar_objetivo_articular_directo(uint8_t indice_eje, float angulo_grados) {
    if (indice_eje >= CANTIDAD_EJES_BRAZO) return;
    cancelar_movimiento_y_secuencia_brazo();
    if (!g_ejes_brazo[indice_eje].enabled) {
        (void)sec_enable(&g_ejes_brazo[indice_eje]);
    }
    sec_set_target_deg(&g_ejes_brazo[indice_eje], normalizar_angulo_deg(angulo_grados));
}

//===============================================
//---- Tarea Principal de Control del Brazo -----
//===============================================
void tarea_control_brazo_robotico(void *parametro) {
    (void)parametro;

    uint32_t divisor_verificacion_iman = 0;
    uint32_t divisor_log_diagnostico = 0;
    uint32_t divisor_telemetria_qt = 0;
    int32_t ultimos_centesimos_enviados[CANTIDAD_EJES_BRAZO] = { -99999, -99999, -99999, -99999 };
    uint8_t contador_keepalive_qt = 0;

    imprimir_ayuda_comandos_uart();
    TickType_t ultimo_despertar = xTaskGetTickCount();

    while (1) {
        bool verificar_iman_ahora = (++divisor_verificacion_iman >= 50U);
        if (verificar_iman_ahora) divisor_verificacion_iman = 0;

        orden_piston_t orden_piston;
        while (xQueueReceive(g_cola_ordenes_piston, &orden_piston, 0) == pdTRUE) {
            aplicar_orden_hardware_piston(&orden_piston);
        }

        procesar_comandos_uart_disponibles();

        switch (s_estado_secuencia) {
            case SECUENCIA_BRAZO_LISTO: {
                color_pallet_t siguiente_pallet;
                if (xQueueReceive(g_cola_objetivos_brazo, &siguiente_pallet, 0) == pdTRUE) {
                    if (!validar_configuracion_completa_trabajo(NULL)) {
                        printf("[ARM] Ciclo abortado: punto Teach-In fuera de limite articular.\n");
                        break;
                    }

                    s_color_pallet_activo = siguiente_pallet;
                    s_aproximacion_recogida_completada = false;
                    s_giro_en_radio_minimo_completado = false;

                    if (s_color_pallet_activo == COLOR_PALLET_AZUL) {
                        s_puntos_contenedor_activo = PUNTO_CONTENEDOR_AZUL;
                    } else if (s_color_pallet_activo == COLOR_PALLET_AMARILLO) {
                        s_puntos_contenedor_activo = PUNTO_CONTENEDOR_AMARILLO;
                    } else {
                        s_puntos_contenedor_activo = PUNTO_CONTENEDOR_ROJO;
                    }

                    printf("[ARM] Pallet %s -> Paso 1a: Aproximacion recogida [%.1f, %.1f, %.1f, %.1f] deg\n",
                           obtener_nombre_color_pallet(s_color_pallet_activo),
                           PUNTO_DEG_APROX_RECOGIDA[0], PUNTO_DEG_APROX_RECOGIDA[1],
                           PUNTO_DEG_APROX_RECOGIDA[2], PUNTO_DEG_APROX_RECOGIDA[3]);

                    preparar_siguiente_paso_articular(PUNTO_DEG_APROX_RECOGIDA, SECUENCIA_BRAZO_IR_A_RECOGIDA);
                }
                break;
            }

            case SECUENCIA_BRAZO_IR_A_RECOGIDA:
                if (!s_aproximacion_recogida_completada) {
                    if (!s_trayectoria_finalizada) {
                        s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                    } else if (verificar_brazo_estable_continuamente()) {
                        s_aproximacion_recogida_completada = true;
                        printf("[ARM] Paso 1b: Recogida en cinta [%.1f, %.1f, %.1f, %.1f] deg\n",
                               PUNTO_DEG_RECOGIDA[0], PUNTO_DEG_RECOGIDA[1],
                               PUNTO_DEG_RECOGIDA[2], PUNTO_DEG_RECOGIDA[3]);
                        preparar_siguiente_paso_articular(PUNTO_DEG_RECOGIDA, SECUENCIA_BRAZO_IR_A_RECOGIDA);
                    }
                } else {
                    if (!s_trayectoria_finalizada) {
                        s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                    } else if (verificar_brazo_estable_continuamente()) {
                        printf("[ARM] Paso 2: Elevando pallet [%.1f, %.1f, %.1f, %.1f] deg\n",
                               PUNTO_DEG_ELEVACION[0], PUNTO_DEG_ELEVACION[1],
                               PUNTO_DEG_ELEVACION[2], PUNTO_DEG_ELEVACION[3]);
                        preparar_siguiente_paso_articular(PUNTO_DEG_ELEVACION, SECUENCIA_BRAZO_ELEVAR_PALLET);
                    }
                }
                break;

            case SECUENCIA_BRAZO_ELEVAR_PALLET:
                if (!s_trayectoria_finalizada) {
                    s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                } else if (verificar_brazo_estable_continuamente()) {
                    printf("[ARM] Paso 3: Replegando en origen [%.1f, %.1f, %.1f, %.1f] deg\n",
                           PUNTO_DEG_TRANSITO_ORIGEN[0], PUNTO_DEG_TRANSITO_ORIGEN[1],
                           PUNTO_DEG_TRANSITO_ORIGEN[2], PUNTO_DEG_TRANSITO_ORIGEN[3]);
                    preparar_siguiente_paso_articular(PUNTO_DEG_TRANSITO_ORIGEN, SECUENCIA_BRAZO_IR_A_REPOSO_CON_PALLET);
                }
                break;

            case SECUENCIA_BRAZO_IR_A_REPOSO_CON_PALLET:
                if (!s_trayectoria_finalizada) {
                    s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                } else if (verificar_brazo_estable_continuamente()) {
                    printf("[ARM] Paso 4a: Girando hacia contenedor %s [%.1f, %.1f, %.1f, %.1f] deg\n",
                           obtener_nombre_color_pallet(s_color_pallet_activo),
                           s_puntos_contenedor_activo.transito_destino[0],
                           s_puntos_contenedor_activo.transito_destino[1],
                           s_puntos_contenedor_activo.transito_destino[2],
                           s_puntos_contenedor_activo.transito_destino[3]);
                    s_giro_en_radio_minimo_completado = false;
                    preparar_siguiente_paso_articular(s_puntos_contenedor_activo.transito_destino,
                                                      SECUENCIA_BRAZO_IR_A_CONTENEDOR);
                }
                break;

            case SECUENCIA_BRAZO_IR_A_CONTENEDOR:
                if (!s_giro_en_radio_minimo_completado) {
                    if (!s_trayectoria_finalizada) {
                        s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                    } else if (verificar_brazo_estable_continuamente()) {
                        s_giro_en_radio_minimo_completado = true;
                        printf("[ARM] Paso 4b: Entrada contenedor %s [%.1f, %.1f, %.1f, %.1f] deg\n",
                               obtener_nombre_color_pallet(s_color_pallet_activo),
                               s_puntos_contenedor_activo.entrada[0],
                               s_puntos_contenedor_activo.entrada[1],
                               s_puntos_contenedor_activo.entrada[2],
                               s_puntos_contenedor_activo.entrada[3]);
                        preparar_siguiente_paso_articular(s_puntos_contenedor_activo.entrada,
                                                          SECUENCIA_BRAZO_IR_A_CONTENEDOR);
                    }
                } else {
                    if (!s_trayectoria_finalizada) {
                        s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                    } else if (verificar_brazo_estable_continuamente()) {
                        printf("[ARM] Paso 5: Avanzando en contenedor [%.1f, %.1f, %.1f, %.1f] deg\n",
                               s_puntos_contenedor_activo.avance[0],
                               s_puntos_contenedor_activo.avance[1],
                               s_puntos_contenedor_activo.avance[2],
                               s_puntos_contenedor_activo.avance[3]);
                        preparar_siguiente_paso_articular(s_puntos_contenedor_activo.avance,
                                                          SECUENCIA_BRAZO_AVANZAR_EN_CONTENEDOR);
                    }
                }
                break;

            case SECUENCIA_BRAZO_AVANZAR_EN_CONTENEDOR:
                if (!s_trayectoria_finalizada) {
                    s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                } else if (verificar_brazo_estable_continuamente()) {
                    printf("[ARM] Paso 6: Bajando para asentar pallet [%.1f, %.1f, %.1f, %.1f] deg\n",
                           s_puntos_contenedor_activo.asentamiento[0],
                           s_puntos_contenedor_activo.asentamiento[1],
                           s_puntos_contenedor_activo.asentamiento[2],
                           s_puntos_contenedor_activo.asentamiento[3]);
                    preparar_siguiente_paso_articular(s_puntos_contenedor_activo.asentamiento,
                                                      SECUENCIA_BRAZO_BAJAR_PARA_ASENTAR);
                }
                break;

            case SECUENCIA_BRAZO_BAJAR_PARA_ASENTAR:
                if (!s_trayectoria_finalizada) {
                    s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                } else if (verificar_brazo_estable_continuamente()) {
                    printf("[ARM] Paso 7: Retrocediendo de contenedor [%.1f, %.1f, %.1f, %.1f] deg\n",
                           s_puntos_contenedor_activo.retroceso[0],
                           s_puntos_contenedor_activo.retroceso[1],
                           s_puntos_contenedor_activo.retroceso[2],
                           s_puntos_contenedor_activo.retroceso[3]);
                    preparar_siguiente_paso_articular(s_puntos_contenedor_activo.retroceso,
                                                      SECUENCIA_BRAZO_RETROCEDER_DE_CONTENEDOR);
                }
                break;

            case SECUENCIA_BRAZO_RETROCEDER_DE_CONTENEDOR:
                if (!s_trayectoria_finalizada) {
                    s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                } else if (verificar_brazo_estable_continuamente()) {
                    printf("[ARM] Paso 8: Regresando a HOME [%.1f, %.1f, %.1f, %.1f] deg\n",
                           POSICION_HOME_GRADOS[0], POSICION_HOME_GRADOS[1],
                           POSICION_HOME_GRADOS[2], POSICION_HOME_GRADOS[3]);
                    preparar_siguiente_paso_articular(POSICION_HOME_GRADOS, SECUENCIA_BRAZO_IR_A_REPOSO_FINAL);
                }
                break;

            case SECUENCIA_BRAZO_IR_A_REPOSO_FINAL:
            case SECUENCIA_BRAZO_MOVIMIENTO_MANUAL:
                if (!s_trayectoria_finalizada) {
                    s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                } else if (verificar_brazo_estable_continuamente()) {
                    if (s_estado_secuencia == SECUENCIA_BRAZO_IR_A_REPOSO_FINAL) {
                        printf("[ARM] Ciclo %s completado.\n", obtener_nombre_color_pallet(s_color_pallet_activo));
                    } else {
                        printf("[ARM] Movimiento HOME completado.\n");
                    }
                    s_color_pallet_activo = COLOR_PALLET_NINGUNO;
                    s_estado_secuencia = SECUENCIA_BRAZO_LISTO;
                }
                break;
        }

        bool algun_eje_habilitado = false;
        for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
            if (sec_update(&g_ejes_brazo[indice_eje], PASO_TIEMPO_CONTROL_S, verificar_iman_ahora) != ESP_OK) {
                ESP_LOGE(TAG_BRAZO, "Fallo en sensor del eje %d (%s)",
                         indice_eje + 1, g_nombres_ejes_brazo[indice_eje]);
            }
            if (g_ejes_brazo[indice_eje].enabled) algun_eje_habilitado = true;
            g_telemetria_hmi.angulos_actuales_grados[indice_eje] = sec_get_angle_deg(&g_ejes_brazo[indice_eje]);
        }
        g_telemetria_hmi.estado_brazo = s_estado_secuencia;

        //-----------------------------
        //  Logs Periódicos Para El QT
        //-----------------------------
        if (algun_eje_habilitado && (++divisor_log_diagnostico >= 50U)) {
            divisor_log_diagnostico = 0;
            printf("SP:[%5.1f,%5.1f,%5.1f,%5.1f] | MED:[%5.1f,%5.1f,%5.1f,%5.1f] | US0:%+5.1f | LOCK:[%d%d%d%d]\n",
                   sec_rad2deg_(g_ejes_brazo[0].sp_rad), sec_rad2deg_(g_ejes_brazo[1].sp_rad),
                   sec_rad2deg_(g_ejes_brazo[2].sp_rad), sec_rad2deg_(g_ejes_brazo[3].sp_rad),
                   sec_get_angle_deg(&g_ejes_brazo[0]), sec_get_angle_deg(&g_ejes_brazo[1]),
                   sec_get_angle_deg(&g_ejes_brazo[2]), sec_get_angle_deg(&g_ejes_brazo[3]),
                   g_ejes_brazo[0].last_offset_us,
                   sec_is_locked(&g_ejes_brazo[0]), sec_is_locked(&g_ejes_brazo[1]),
                   sec_is_locked(&g_ejes_brazo[2]), sec_is_locked(&g_ejes_brazo[3]));
        }

        if (++divisor_telemetria_qt >= DIVISOR_REPORTE_QT_100MS) {
            divisor_telemetria_qt = 0;
            bool hubo_cambio_angular = false;
            int32_t centesimos_actuales[CANTIDAD_EJES_BRAZO] = {0};
            float grados_normalizados[CANTIDAD_EJES_BRAZO] = {0.0f};
            int pwm_porcentaje[CANTIDAD_EJES_BRAZO] = {0};

            for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
                float valor_grados = sec_get_angle_deg(&g_ejes_brazo[indice_eje]);
                if (valor_grados < 0.0f) valor_grados += 360.0f;
                int32_t centesimos = (int32_t)lroundf(valor_grados * 100.0f) % 36000;
                centesimos_actuales[indice_eje] = centesimos;
                grados_normalizados[indice_eje] = (float)centesimos / 100.0f;

                if (centesimos_actuales[indice_eje] != ultimos_centesimos_enviados[indice_eje]) {
                    hubo_cambio_angular = true;
                }

                if (g_ejes_brazo[indice_eje].enabled && g_ejes_brazo[indice_eje].cfg.us_max_offset > 1.0f) {
                    float pct = (g_ejes_brazo[indice_eje].last_offset_us / g_ejes_brazo[indice_eje].cfg.us_max_offset) * 100.0f;
                    if (pct > 100.0f) pct = 100.0f;
                    if (pct < -100.0f) pct = -100.0f;
                    pwm_porcentaje[indice_eje] = (int)lroundf(pct);
                }
            }

            if (hubo_cambio_angular || (++contador_keepalive_qt >= 5U)) {
                contador_keepalive_qt = 0;
                for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
                    ultimos_centesimos_enviados[indice_eje] = centesimos_actuales[indice_eje];
                }
                ESP_LOGI("ANGULOS", "%.2f, %.2f, %.2f, %.2f",
                         grados_normalizados[0], grados_normalizados[1],
                         grados_normalizados[2], grados_normalizados[3]);
                ESP_LOGI("PWM", "%d, %d, %d, %d",
                         pwm_porcentaje[0], pwm_porcentaje[1],
                         pwm_porcentaje[2], pwm_porcentaje[3]);
            }
        }

        vTaskDelayUntil(&ultimo_despertar, pdMS_TO_TICKS(PERIODO_LAZO_CONTROL_MS));
    }
}
