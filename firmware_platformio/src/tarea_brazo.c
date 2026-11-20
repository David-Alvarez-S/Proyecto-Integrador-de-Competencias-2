#include "tarea_brazo.h"
#include <math.h>
#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hw_init.h"
#include "uart_cli.h"
#include "Global_Config.h"
#include "Control_Brazo_Robotico/IK.h"
#include "Control_Brazo_Robotico/Movimiento.h"
#include "Control_Brazo_Robotico/Perfil_S.h"

static const char *TAG_BRAZO = "BRAZO_IK";

#define RADIO_BUSQUEDA_MINIMO_M             0.040f
#define PASO_BUSQUEDA_RADIO_M               0.005f
#define DISTANCIA_APROX_RECOGIDA_MAX_M      0.035f

static const float POSICION_HOME_GRADOS[CANTIDAD_EJES_BRAZO] = {
    27.0f,   
    2.5f,    
    116.3f,  
    59.7f    
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
static configuracion_trabajo_brazo_t s_configuracion_ciclo_actual = {0};

static vector_t s_punto_aprox_recogida_m = {0};
static vector_t s_punto_recogida_m = {0};
static vector_t s_punto_elevacion_m = {0};
static vector_t s_punto_radio_minimo_origen_m = {0};
static vector_t s_punto_radio_minimo_destino_m = {0};
static vector_t s_punto_entrada_contenedor_m = {0};
static vector_t s_punto_avance_contenedor_m = {0};
static vector_t s_punto_asentamiento_m = {0};
static vector_t s_punto_retroceso_m = {0};

static bool s_aproximacion_recogida_completada = false;
static bool s_giro_en_radio_minimo_completado = false;

static trayectoria_articular_t s_trayectoria_articular = {0};
static movimiento_t s_trayectoria_cartesiana = {0};
static ik_continuidad_t s_continuidad_ik = {0};
static float s_tiempo_trayectoria_cartesiana_s = 0.0f;
static float s_ultimo_comando_cartesiano_rad[CANTIDAD_EJES_BRAZO] = {0};

static bool s_trayectoria_finalizada = false;
static uint32_t s_tiempo_espera_enclave_ms = 0;
static uint32_t s_tiempo_estable_continuo_ms = 0;

//=====================================================
//---- Validación Cinemática y Lectura Cartesiana -----
//=====================================================
static const char *obtener_descripcion_error_ik(ik_status_t estado_ik) {
    switch (estado_ik) {
        case IK_OK:                          return "OK";
        case IK_ERROR_BASE_FUERA_DE_LIMITE:  return "BASE_FUERA_LIMITE";
        case IK_ERROR_SINGULARIDAD_MUNICA:   return "SINGULARIDAD_MUNECA_O_BASE";
        case IK_ERROR_FUERA_DE_ALCANCE:      return "FUERA_DE_ALCANCE";
        case IK_ERROR_SERVO_FUERA_DE_LIMITE: return "SERVO_FUERA_LIMITE";
        case IK_ERROR_COLISION_MESA:         return "COLISION_MESA";
        default:                             return "ERROR_IK";
    }
}

static bool validar_punto_cartesiano_con_ik(vector_t punto_m, servomotores_h *servos_salida, const char *nombre_punto) {
    servomotores_h servos_calculados = {0};
    ik_status_t estado_ik = IK_ext_debug(&servos_calculados, punto_m, NULL);
    if (estado_ik != IK_OK) {
        if (nombre_punto != NULL) {
            printf("[ERROR IK] Punto '%s' (%.3f, %.3f, %.3f m) invalido: %s\n",
                   nombre_punto, punto_m.x, punto_m.y, punto_m.z, obtener_descripcion_error_ik(estado_ik));
        }
        return false;
    }

    float angulos_grados[CANTIDAD_EJES_BRAZO] = {
        sec_rad2deg_(servos_calculados.sm1.angulo_objetivo),
        sec_rad2deg_(servos_calculados.sm2.angulo_objetivo),
        sec_rad2deg_(servos_calculados.sm3.angulo_objetivo),
        sec_rad2deg_(servos_calculados.sm4.angulo_objetivo),
    };

    for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
        if (angulos_grados[indice_eje] < g_ejes_brazo[indice_eje].cfg.min_deg ||
            angulos_grados[indice_eje] > g_ejes_brazo[indice_eje].cfg.max_deg) {
            if (nombre_punto != NULL) {
                printf("[ERROR LIMITE] Punto '%s' exige %s = %.1f deg (rango [%.1f, %.1f] deg)\n",
                       nombre_punto,
                       g_nombres_ejes_brazo[indice_eje],
                       angulos_grados[indice_eje],
                       g_ejes_brazo[indice_eje].cfg.min_deg,
                       g_ejes_brazo[indice_eje].cfg.max_deg);
            }
            return false;
        }
    }

    if (servos_salida) *servos_salida = servos_calculados;
    return true;
}

static vector_t calcular_punto_aproximacion_horizontal_ik(vector_t punto_recogida_m) {
    float radio_recogida = hypotf(punto_recogida_m.x, punto_recogida_m.y);
    if (radio_recogida < 1e-4f) return punto_recogida_m;

    float cos_dir = punto_recogida_m.x / radio_recogida;
    float sin_dir = punto_recogida_m.y / radio_recogida;

    for (float retroceso = DISTANCIA_APROX_RECOGIDA_MAX_M; retroceso >= 0.005f; retroceso -= 0.005f) {
        float radio_aprox = radio_recogida - retroceso;
        if (radio_aprox <= 0.01f) continue;

        vector_t candidato = {
            radio_aprox * cos_dir,
            radio_aprox * sin_dir,
            punto_recogida_m.z
        };
        if (validar_punto_cartesiano_con_ik(candidato, NULL, NULL)) {
            return candidato;
        }
    }

    return punto_recogida_m;
}

static bool calcular_puntos_transito_radio_minimo_ik(vector_t punto_origen_m,
                                                     vector_t punto_destino_m,
                                                     vector_t *punto_rmin_origen_salida,
                                                     vector_t *punto_rmin_destino_salida) {
    float radio_origen = hypotf(punto_origen_m.x, punto_origen_m.y);
    float radio_destino = hypotf(punto_destino_m.x, punto_destino_m.y);
    if (radio_origen < 1e-4f || radio_destino < 1e-4f) return false;

    float cos_origen = punto_origen_m.x / radio_origen;
    float sin_origen = punto_origen_m.y / radio_origen;
    float cos_destino = punto_destino_m.x / radio_destino;
    float sin_destino = punto_destino_m.y / radio_destino;

    float z_transito = fmaxf(punto_origen_m.z, punto_destino_m.z);
    float radio_limite_superior = fminf(radio_origen, radio_destino);

    for (float radio_prueba = RADIO_BUSQUEDA_MINIMO_M;
         radio_prueba <= radio_limite_superior + 1e-4f;
         radio_prueba += PASO_BUSQUEDA_RADIO_M) {
        float radio_actual = fminf(radio_prueba, radio_limite_superior);
        vector_t candidato_origen = {
            radio_actual * cos_origen,
            radio_actual * sin_origen,
            z_transito
        };
        vector_t candidato_destino = {
            radio_actual * cos_destino,
            radio_actual * sin_destino,
            z_transito
        };

        if (validar_punto_cartesiano_con_ik(candidato_origen, NULL, NULL) &&
            validar_punto_cartesiano_con_ik(candidato_destino, NULL, NULL)) {
            if (punto_rmin_origen_salida) *punto_rmin_origen_salida = candidato_origen;
            if (punto_rmin_destino_salida) *punto_rmin_destino_salida = candidato_destino;
            return true;
        }
    }

    return false;
}

bool validar_configuracion_completa_trabajo(const configuracion_trabajo_brazo_t *configuracion) {
    if (!configuracion) return false;

    vector_t punto_recogida = configuracion->punto_recogida_m;
    vector_t punto_elevacion = {
        punto_recogida.x,
        punto_recogida.y,
        punto_recogida.z + configuracion->elevacion_vertical_z_m
    };

    if (!validar_punto_cartesiano_con_ik(punto_recogida, NULL, "RECOGIDA (x1,y1,z1)")) return false;
    if (!validar_punto_cartesiano_con_ik(punto_elevacion, NULL, "ELEVACION (x1,y1,z1+dz)")) return false;

    float angulo_contenedor_rad = atan2f(configuracion->contenedor_y_m, configuracion->contenedor_x_m);
    float direccion_radial_x = cosf(angulo_contenedor_rad);
    float direccion_radial_y = sinf(angulo_contenedor_rad);

    const float alturas_por_color[3] = {
        configuracion->altura_z_rojo_m,
        configuracion->altura_z_azul_m,
        configuracion->altura_z_amarillo_m
    };
    const char *etiquetas_contenedores[3] = {
        "CONT_ROJO", "CONT_AZUL", "CONT_AMARILLO"
    };

    for (int indice_color = 0; indice_color < 3; indice_color++) {
        float z_actual = alturas_por_color[indice_color];
        vector_t punto_entrada = { configuracion->contenedor_x_m, configuracion->contenedor_y_m, z_actual };
        vector_t punto_avance  = {
            configuracion->contenedor_x_m + configuracion->avance_horizontal_deposito_m * direccion_radial_x,
            configuracion->contenedor_y_m + configuracion->avance_horizontal_deposito_m * direccion_radial_y,
            z_actual
        };
        vector_t punto_asentado = {
            punto_avance.x,
            punto_avance.y,
            z_actual - configuracion->bajada_vertical_asentamiento_m
        };

        if (!validar_punto_cartesiano_con_ik(punto_entrada,  NULL, etiquetas_contenedores[indice_color])) return false;
        if (!validar_punto_cartesiano_con_ik(punto_avance,   NULL, "AVANCE_CONTENEDOR")) return false;
        if (!validar_punto_cartesiano_con_ik(punto_asentado, NULL, "ASENTAR_PALLET")) return false;
        if (!calcular_puntos_transito_radio_minimo_ik(punto_elevacion, punto_entrada, NULL, NULL)) {
            printf("[ERROR IK] Sin zona de radio minimo valida para transito hacia %s\n",
                   etiquetas_contenedores[indice_color]);
            return false;
        }
    }
    return true;
}

void imprimir_configuracion_posiciones_brazo(void) {
    configuracion_trabajo_brazo_t cfg;
    xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
    cfg = g_configuracion_trabajo_brazo;
    xSemaphoreGive(g_mutex_configuracion_brazo);

    printf("\n--- POSICIONES CONFIGURADAS DEL BRAZO ---\n");
    printf("  Recogida (x1,y1,z1) : [%.3f, %.3f, %.3f] m  ([%.1f, %.1f, %.1f] cm)\n",
           cfg.punto_recogida_m.x, cfg.punto_recogida_m.y, cfg.punto_recogida_m.z,
           cfg.punto_recogida_m.x * 100.0f, cfg.punto_recogida_m.y * 100.0f, cfg.punto_recogida_m.z * 100.0f);
    printf("  Elevacion (+dz)     : +%.3f m (+%.1f cm)\n",
           cfg.elevacion_vertical_z_m, cfg.elevacion_vertical_z_m * 100.0f);
    printf("  Contenedor (x,y)    : [%.3f, %.3f] m  ([%.1f, %.1f] cm)\n",
           cfg.contenedor_x_m, cfg.contenedor_y_m,
           cfg.contenedor_x_m * 100.0f, cfg.contenedor_y_m * 100.0f);
    printf("  Alturas Z por color : ROJO=%.3f m (%.1f cm) | AZUL=%.3f m (%.1f cm) | AMARILLO=%.3f m (%.1f cm)\n",
           cfg.altura_z_rojo_m, cfg.altura_z_rojo_m * 100.0f,
           cfg.altura_z_azul_m, cfg.altura_z_azul_m * 100.0f,
           cfg.altura_z_amarillo_m, cfg.altura_z_amarillo_m * 100.0f);
    printf("  Deposito            : Avance=%.3f m (%.1f cm) | Bajada=%.3f m (%.1f cm)\n\n",
           cfg.avance_horizontal_deposito_m, cfg.avance_horizontal_deposito_m * 100.0f,
           cfg.bajada_vertical_asentamiento_m, cfg.bajada_vertical_asentamiento_m * 100.0f);
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

    float theta_1 = sec_get_angle_rad(&g_ejes_brazo[0]);
    float theta_2 = sec_get_angle_rad(&g_ejes_brazo[1]);
    float theta_3 = sec_get_angle_rad(&g_ejes_brazo[2]);
    float theta_4 = sec_get_angle_rad(&g_ejes_brazo[3]);

    float angulo_herramienta_rad = (SEC_PI * 0.5f) - theta_2 + theta_3 + theta_4;
    float longitud_efector_m     = L_ESLABON_3 + L_HERRAMIENTA;
    float radio_muneca_m         = L_ESLABON_1 * sinf(theta_2) + L_ESLABON_2 * sinf(theta_2 - theta_3);
    float radio_total_m          = radio_muneca_m + longitud_efector_m * cosf(angulo_herramienta_rad);

    float x_actual_m = radio_total_m * cosf(theta_1);
    float y_actual_m = radio_total_m * sinf(theta_1);
    float z_actual_m = L_SHOULDER_Z +
                       L_ESLABON_1 * cosf(theta_2) +
                       L_ESLABON_2 * cosf(theta_2 - theta_3) +
                       longitud_efector_m * sinf(angulo_herramienta_rad);

    printf("XYZ ACTUAL: [%.3f, %.3f, %.3f] m  ([%.1f, %.1f, %.1f] cm) | Herramienta: %.1f deg\n",
           x_actual_m, y_actual_m, z_actual_m,
           x_actual_m * 100.0f, y_actual_m * 100.0f, z_actual_m * 100.0f,
           sec_rad2deg_(angulo_herramienta_rad));
}

//=====================================================
//---- Planificadores de Trayectoria con Perfil S -----
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
        float angulo_limitado_deg = sec_clamp_(objetivos_grados[indice_eje],
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

static bool iniciar_trayectoria_cartesiana_lineal(vector_t punto_inicio_m, vector_t punto_fin_m) {
    servomotores_h servos_iniciales = {0};
    for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
        if (!g_ejes_brazo[indice_eje].enabled) {
            (void)sec_enable(&g_ejes_brazo[indice_eje]);
        }
        s_ultimo_comando_cartesiano_rad[indice_eje] = sec_get_angle_rad(&g_ejes_brazo[indice_eje]);
    }

    servos_iniciales.sm1.angulo_objetivo = s_ultimo_comando_cartesiano_rad[0];
    servos_iniciales.sm2.angulo_objetivo = s_ultimo_comando_cartesiano_rad[1];
    servos_iniciales.sm3.angulo_objetivo = s_ultimo_comando_cartesiano_rad[2];
    servos_iniciales.sm4.angulo_objetivo = s_ultimo_comando_cartesiano_rad[3];

    limites_perfil_s_t limites = {
        .v_max = PERFIL_S_VELOCIDAD_MAXIMA,
        .a_max = PERFIL_S_ACELERACION_MAXIMA,
        .j_max = PERFIL_S_JERK_MAXIMO,
    };

    if (!Movimiento_Iniciar(&s_trayectoria_cartesiana,
                            servos_iniciales,
                            punto_inicio_m,
                            punto_fin_m,
                            limites,
                            &s_continuidad_ik)) {
        return false;
    }

    s_tiempo_trayectoria_cartesiana_s = 0.0f;
    return true;
}

static bool evaluar_trayectoria_cartesiana_lineal(float paso_tiempo_s, bool *hubo_error_ik) {
    *hubo_error_ik = false;
    if (!s_trayectoria_cartesiana.activo) return true;

    s_tiempo_trayectoria_cartesiana_s += paso_tiempo_s;
    servomotores_h servos_objetivo = {0};
    float progreso_normalizado = 0.0f;

    bool en_curso = Movimiento_Evaluar(&servos_objetivo,
                                       &s_trayectoria_cartesiana,
                                       s_tiempo_trayectoria_cartesiana_s,
                                       &s_continuidad_ik,
                                       &progreso_normalizado);

    if (!en_curso && progreso_normalizado < 0.999f) {
        *hubo_error_ik = true;
        for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
            sec_set_trajectory_rad(&g_ejes_brazo[indice_eje],
                                   s_ultimo_comando_cartesiano_rad[indice_eje],
                                   0.0f);
        }
        return true;
    }

    float angulos_objetivo_rad[CANTIDAD_EJES_BRAZO] = {
        servos_objetivo.sm1.angulo_objetivo,
        servos_objetivo.sm2.angulo_objetivo,
        servos_objetivo.sm3.angulo_objetivo,
        servos_objetivo.sm4.angulo_objetivo,
    };

    for (int indice_eje = 0; indice_eje < CANTIDAD_EJES_BRAZO; indice_eje++) {
        float velocidad_avance = en_curso
            ? ((angulos_objetivo_rad[indice_eje] - s_ultimo_comando_cartesiano_rad[indice_eje]) / paso_tiempo_s)
            : 0.0f;
        sec_set_trajectory_rad(&g_ejes_brazo[indice_eje], angulos_objetivo_rad[indice_eje], velocidad_avance);
        s_ultimo_comando_cartesiano_rad[indice_eje] = angulos_objetivo_rad[indice_eje];
    }

    return !en_curso;
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

static void preparar_siguiente_paso_cartesiano(vector_t inicio_m,
                                               vector_t fin_m,
                                               estado_secuencia_brazo_t nuevo_estado) {
    if (iniciar_trayectoria_cartesiana_lineal(inicio_m, fin_m)) {
        s_trayectoria_finalizada = false;
        s_tiempo_espera_enclave_ms = 0;
        s_tiempo_estable_continuo_ms = 0;
        s_estado_secuencia = nuevo_estado;
    } else {
        preparar_siguiente_paso_articular(POSICION_HOME_GRADOS, SECUENCIA_BRAZO_IR_A_REPOSO_FINAL);
    }
}

//=====================================================
//------- Funciones Públicas de Control Manual --------
//=====================================================
void cancelar_movimiento_y_secuencia_brazo(void) {
    s_trayectoria_articular.activa = false;
    s_trayectoria_cartesiana.activo = false;
    s_estado_secuencia = SECUENCIA_BRAZO_LISTO;
    s_color_pallet_activo = COLOR_PALLET_NINGUNO;
    s_aproximacion_recogida_completada = false;
    s_giro_en_radio_minimo_completado = false;
    s_tiempo_estable_continuo_ms = 0;
    s_tiempo_espera_enclave_ms = 0;
}

void ordenar_movimiento_articular_suave(const float objetivos_grados[CANTIDAD_EJES_BRAZO], bool es_retorno_reposo) {
    s_trayectoria_cartesiana.activo = false;
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
    sec_set_target_deg(&g_ejes_brazo[indice_eje], angulo_grados);
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
                    xSemaphoreTake(g_mutex_configuracion_brazo, portMAX_DELAY);
                    s_configuracion_ciclo_actual = g_configuracion_trabajo_brazo;
                    xSemaphoreGive(g_mutex_configuracion_brazo);

                    if (!validar_configuracion_completa_trabajo(&s_configuracion_ciclo_actual)) {
                        printf("[ARM] Ciclo abortado: coordenadas fuera de alcance IK.\n");
                        break;
                    }

                    s_color_pallet_activo = siguiente_pallet;
                    s_aproximacion_recogida_completada = false;
                    s_giro_en_radio_minimo_completado = false;

                    s_punto_recogida_m = s_configuracion_ciclo_actual.punto_recogida_m;
                    s_punto_aprox_recogida_m = calcular_punto_aproximacion_horizontal_ik(s_punto_recogida_m);
                    s_punto_elevacion_m = (vector_t){
                        s_punto_recogida_m.x,
                        s_punto_recogida_m.y,
                        s_punto_recogida_m.z + s_configuracion_ciclo_actual.elevacion_vertical_z_m
                    };

                    float altura_destino_z = s_configuracion_ciclo_actual.altura_z_rojo_m;
                    if (s_color_pallet_activo == COLOR_PALLET_AZUL) {
                        altura_destino_z = s_configuracion_ciclo_actual.altura_z_azul_m;
                    } else if (s_color_pallet_activo == COLOR_PALLET_AMARILLO) {
                        altura_destino_z = s_configuracion_ciclo_actual.altura_z_amarillo_m;
                    }

                    float angulo_contenedor = atan2f(s_configuracion_ciclo_actual.contenedor_y_m,
                                                     s_configuracion_ciclo_actual.contenedor_x_m);
                    float coseno_radial = cosf(angulo_contenedor);
                    float seno_radial   = sinf(angulo_contenedor);

                    s_punto_entrada_contenedor_m = (vector_t){
                        s_configuracion_ciclo_actual.contenedor_x_m,
                        s_configuracion_ciclo_actual.contenedor_y_m,
                        altura_destino_z
                    };
                    s_punto_avance_contenedor_m = (vector_t){
                        s_configuracion_ciclo_actual.contenedor_x_m +
                            s_configuracion_ciclo_actual.avance_horizontal_deposito_m * coseno_radial,
                        s_configuracion_ciclo_actual.contenedor_y_m +
                            s_configuracion_ciclo_actual.avance_horizontal_deposito_m * seno_radial,
                        altura_destino_z
                    };
                    s_punto_asentamiento_m = (vector_t){
                        s_punto_avance_contenedor_m.x,
                        s_punto_avance_contenedor_m.y,
                        altura_destino_z - s_configuracion_ciclo_actual.bajada_vertical_asentamiento_m
                    };
                    s_punto_retroceso_m = (vector_t){
                        s_configuracion_ciclo_actual.contenedor_x_m,
                        s_configuracion_ciclo_actual.contenedor_y_m,
                        altura_destino_z - s_configuracion_ciclo_actual.bajada_vertical_asentamiento_m
                    };

                    if (!calcular_puntos_transito_radio_minimo_ik(s_punto_elevacion_m,
                                                                  s_punto_entrada_contenedor_m,
                                                                  &s_punto_radio_minimo_origen_m,
                                                                  &s_punto_radio_minimo_destino_m)) {
                        printf("[ARM] Ciclo abortado: no existe zona IK de radio minimo.\n");
                        break;
                    }

                    servomotores_h servos_aprox = {0};
                    (void)IK_ext_debug(&servos_aprox, s_punto_aprox_recogida_m, &s_continuidad_ik);

                    float angulos_aprox_deg[CANTIDAD_EJES_BRAZO] = {
                        sec_rad2deg_(servos_aprox.sm1.angulo_objetivo),
                        sec_rad2deg_(servos_aprox.sm2.angulo_objetivo),
                        sec_rad2deg_(servos_aprox.sm3.angulo_objetivo),
                        sec_rad2deg_(servos_aprox.sm4.angulo_objetivo),
                    };

                    printf("[ARM] Pallet %s -> Paso 1a: Pre-recogida horizontal [%.3f, %.3f, %.3f] m\n",
                           obtener_nombre_color_pallet(s_color_pallet_activo),
                           s_punto_aprox_recogida_m.x, s_punto_aprox_recogida_m.y, s_punto_aprox_recogida_m.z);

                    preparar_siguiente_paso_articular(angulos_aprox_deg, SECUENCIA_BRAZO_IR_A_RECOGIDA);
                }
                break;
            }

            case SECUENCIA_BRAZO_IR_A_RECOGIDA:
                if (!s_aproximacion_recogida_completada) {
                    if (!s_trayectoria_finalizada) {
                        s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                    } else if (verificar_brazo_estable_continuamente()) {
                        s_aproximacion_recogida_completada = true;
                        printf("[ARM] Paso 1b: Avance recto horizontal hacia pallet [%.3f, %.3f, %.3f] m (IK)\n",
                               s_punto_recogida_m.x, s_punto_recogida_m.y, s_punto_recogida_m.z);
                        preparar_siguiente_paso_cartesiano(s_punto_aprox_recogida_m,
                                                           s_punto_recogida_m,
                                                           SECUENCIA_BRAZO_IR_A_RECOGIDA);
                    }
                } else {
                    if (!s_trayectoria_finalizada) {
                        bool hubo_error_ik = false;
                        s_trayectoria_finalizada = evaluar_trayectoria_cartesiana_lineal(PASO_TIEMPO_CONTROL_S, &hubo_error_ik);
                        if (hubo_error_ik) s_trayectoria_finalizada = true;
                    } else if (verificar_brazo_estable_continuamente()) {
                        printf("[ARM] Paso 2: Levantando pallet horizontal en +Z hasta z=%.3f m (IK)\n", s_punto_elevacion_m.z);
                        preparar_siguiente_paso_cartesiano(s_punto_recogida_m,
                                                           s_punto_elevacion_m,
                                                           SECUENCIA_BRAZO_ELEVAR_PALLET);
                    }
                }
                break;

            case SECUENCIA_BRAZO_ELEVAR_PALLET:
                if (!s_trayectoria_finalizada) {
                    bool hubo_error_ik = false;
                    s_trayectoria_finalizada = evaluar_trayectoria_cartesiana_lineal(PASO_TIEMPO_CONTROL_S, &hubo_error_ik);
                    if (hubo_error_ik) s_trayectoria_finalizada = true;
                } else if (verificar_brazo_estable_continuamente()) {
                    printf("[ARM] Paso 3: Replegando a radio minimo IK [%.3f, %.3f, %.3f] m\n",
                           s_punto_radio_minimo_origen_m.x,
                           s_punto_radio_minimo_origen_m.y,
                           s_punto_radio_minimo_origen_m.z);
                    preparar_siguiente_paso_cartesiano(s_punto_elevacion_m,
                                                       s_punto_radio_minimo_origen_m,
                                                       SECUENCIA_BRAZO_IR_A_REPOSO_CON_PALLET);
                }
                break;

            case SECUENCIA_BRAZO_IR_A_REPOSO_CON_PALLET:
                if (!s_trayectoria_finalizada) {
                    bool hubo_error_ik = false;
                    s_trayectoria_finalizada = evaluar_trayectoria_cartesiana_lineal(PASO_TIEMPO_CONTROL_S, &hubo_error_ik);
                    if (hubo_error_ik) s_trayectoria_finalizada = true;
                } else if (verificar_brazo_estable_continuamente()) {
                    servomotores_h servos_rmin_destino = {0};
                    (void)IK_ext_debug(&servos_rmin_destino, s_punto_radio_minimo_destino_m, &s_continuidad_ik);

                    float angulos_rmin_destino_deg[CANTIDAD_EJES_BRAZO] = {
                        sec_rad2deg_(servos_rmin_destino.sm1.angulo_objetivo),
                        sec_rad2deg_(servos_rmin_destino.sm2.angulo_objetivo),
                        sec_rad2deg_(servos_rmin_destino.sm3.angulo_objetivo),
                        sec_rad2deg_(servos_rmin_destino.sm4.angulo_objetivo),
                    };

                    printf("[ARM] Paso 4a: Girando en radio minimo IK hacia [%.3f, %.3f, %.3f] m\n",
                           s_punto_radio_minimo_destino_m.x,
                           s_punto_radio_minimo_destino_m.y,
                           s_punto_radio_minimo_destino_m.z);

                    s_giro_en_radio_minimo_completado = false;
                    preparar_siguiente_paso_articular(angulos_rmin_destino_deg, SECUENCIA_BRAZO_IR_A_CONTENEDOR);
                }
                break;

            case SECUENCIA_BRAZO_IR_A_CONTENEDOR:
                if (!s_giro_en_radio_minimo_completado) {
                    if (!s_trayectoria_finalizada) {
                        s_trayectoria_finalizada = evaluar_trayectoria_articular_suave(PASO_TIEMPO_CONTROL_S);
                    } else if (verificar_brazo_estable_continuamente()) {
                        s_giro_en_radio_minimo_completado = true;
                        printf("[ARM] Paso 4b: Extendiendo en IK hacia contenedor %s [%.3f, %.3f, %.3f] m\n",
                               obtener_nombre_color_pallet(s_color_pallet_activo),
                               s_punto_entrada_contenedor_m.x,
                               s_punto_entrada_contenedor_m.y,
                               s_punto_entrada_contenedor_m.z);
                        preparar_siguiente_paso_cartesiano(s_punto_radio_minimo_destino_m,
                                                           s_punto_entrada_contenedor_m,
                                                           SECUENCIA_BRAZO_IR_A_CONTENEDOR);
                    }
                } else {
                    if (!s_trayectoria_finalizada) {
                        bool hubo_error_ik = false;
                        s_trayectoria_finalizada = evaluar_trayectoria_cartesiana_lineal(PASO_TIEMPO_CONTROL_S, &hubo_error_ik);
                        if (hubo_error_ik) s_trayectoria_finalizada = true;
                    } else if (verificar_brazo_estable_continuamente()) {
                        printf("[ARM] Paso 5: Avanzando %.1f cm hacia el contenedor (IK)\n",
                               s_configuracion_ciclo_actual.avance_horizontal_deposito_m * 100.0f);
                        preparar_siguiente_paso_cartesiano(s_punto_entrada_contenedor_m,
                                                           s_punto_avance_contenedor_m,
                                                           SECUENCIA_BRAZO_AVANZAR_EN_CONTENEDOR);
                    }
                }
                break;

            case SECUENCIA_BRAZO_AVANZAR_EN_CONTENEDOR:
                if (!s_trayectoria_finalizada) {
                    bool hubo_error_ik = false;
                    s_trayectoria_finalizada = evaluar_trayectoria_cartesiana_lineal(PASO_TIEMPO_CONTROL_S, &hubo_error_ik);
                    if (hubo_error_ik) s_trayectoria_finalizada = true;
                } else if (verificar_brazo_estable_continuamente()) {
                    printf("[ARM] Paso 6: Bajando %.1f cm para asentar pallet (IK)\n",
                           s_configuracion_ciclo_actual.bajada_vertical_asentamiento_m * 100.0f);
                    preparar_siguiente_paso_cartesiano(s_punto_avance_contenedor_m,
                                                       s_punto_asentamiento_m,
                                                       SECUENCIA_BRAZO_BAJAR_PARA_ASENTAR);
                }
                break;

            case SECUENCIA_BRAZO_BAJAR_PARA_ASENTAR:
                if (!s_trayectoria_finalizada) {
                    bool hubo_error_ik = false;
                    s_trayectoria_finalizada = evaluar_trayectoria_cartesiana_lineal(PASO_TIEMPO_CONTROL_S, &hubo_error_ik);
                    if (hubo_error_ik) s_trayectoria_finalizada = true;
                } else if (verificar_brazo_estable_continuamente()) {
                    printf("[ARM] Paso 7: Retrocediendo %.1f cm (IK)\n",
                           s_configuracion_ciclo_actual.avance_horizontal_deposito_m * 100.0f);
                    preparar_siguiente_paso_cartesiano(s_punto_asentamiento_m,
                                                       s_punto_retroceso_m,
                                                       SECUENCIA_BRAZO_RETROCEDER_DE_CONTENEDOR);
                }
                break;

            case SECUENCIA_BRAZO_RETROCEDER_DE_CONTENEDOR:
                if (!s_trayectoria_finalizada) {
                    bool hubo_error_ik = false;
                    s_trayectoria_finalizada = evaluar_trayectoria_cartesiana_lineal(PASO_TIEMPO_CONTROL_S, &hubo_error_ik);
                    if (hubo_error_ik) s_trayectoria_finalizada = true;
                } else if (verificar_brazo_estable_continuamente()) {
                    printf("[ARM] Paso 8: Regresando a posicion final [90.0, 0.0, 148.8, 47.2]\n");
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
