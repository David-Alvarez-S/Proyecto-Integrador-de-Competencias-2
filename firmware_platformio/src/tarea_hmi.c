#include "tarea_hmi.h"
#include <math.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hw_init.h"
#include "tipos_de_sistema.h"

//=====================================================
//---------- Dibujo de Tarjeta de Color HMI -----------
//=====================================================
static void dibujar_tarjeta_color_detectado(color_pallet_t color) {
    uint16_t color_fondo = GC9306_DARKGRAY;
    uint16_t color_texto = GC9306_WHITE;

    if (color == COLOR_PALLET_ROJO) {
        color_fondo = GC9306_RED;
        color_texto = GC9306_WHITE;
    } else if (color == COLOR_PALLET_AZUL) {
        color_fondo = GC9306_BLUE;
        color_texto = GC9306_WHITE;
    } else if (color == COLOR_PALLET_AMARILLO) {
        color_fondo = GC9306_YELLOW;
        color_texto = GC9306_BLACK;
    }

    gc9306_fill_rect(&g_pantalla_tft, 10, 40, 220, 84, color_fondo);
    gc9306_draw_rect(&g_pantalla_tft, 10, 40, 220, 84, GC9306_WHITE);

    const char *etiqueta_color = obtener_nombre_color_pallet(color);
    int escala_fuente = 4;
    int ancho_texto = gc9306_text_width(etiqueta_color, escala_fuente);
    int alto_texto = gc9306_text_height(escala_fuente);
    int posicion_x = 10 + (220 - ancho_texto) / 2;
    int posicion_y = 40 + (84 - alto_texto) / 2;

    gc9306_draw_string(&g_pantalla_tft, posicion_x, posicion_y, etiqueta_color, color_texto, color_fondo, escala_fuente);
}

//=====================================================
//------- Tarea Principal de Pantalla (Core 0) --------
//=====================================================
void tarea_pantalla_hmi(void *parametro) {
    (void)parametro;

    const uint16_t color_cabecera = GC9306_RGB565(15, 40, 80);
    const uint16_t color_panel    = GC9306_RGB565(24, 24, 28);

    gc9306_fill_screen(&g_pantalla_tft, GC9306_BLACK);
    gc9306_fill_rect(&g_pantalla_tft, 0, 0, 240, 32, color_cabecera);
    gc9306_draw_string(&g_pantalla_tft, 10, 8, "CLASIFICADOR HMI", GC9306_CYAN, color_cabecera, 2);

    gc9306_fill_rect(&g_pantalla_tft, 10, 132, 220, 34, color_panel);
    gc9306_draw_rect(&g_pantalla_tft, 10, 132, 220, 34, GC9306_GRAY);

    gc9306_fill_rect(&g_pantalla_tft, 10, 174, 220, 66, color_panel);
    gc9306_draw_rect(&g_pantalla_tft, 10, 174, 220, 66, GC9306_GRAY);
    gc9306_draw_string(&g_pantalla_tft, 18, 180, "ULTRASONICOS", GC9306_CYAN, color_panel, 1);

    gc9306_fill_rect(&g_pantalla_tft, 10, 248, 220, 64, color_panel);
    gc9306_draw_rect(&g_pantalla_tft, 10, 248, 220, 64, GC9306_GRAY);
    gc9306_draw_string(&g_pantalla_tft, 18, 254, "CINTAS / PISTON / BRAZO", GC9306_CYAN, color_panel, 1);

    color_pallet_t color_previo = (color_pallet_t)0xFF;
    float matiz_previo = -999.0f;
    float ultrasonico1_previo = -999.0f;
    float ultrasonico2_previo = -999.0f;
    uint8_t cola1_previa = 0xFF;
    uint8_t cola2_previa = 0xFF;
    estado_cinta2_t estado_cinta2_previo = (estado_cinta2_t)0xFF;
    estado_piston_t estado_piston_previo = (estado_piston_t)0xFF;
    estado_secuencia_brazo_t estado_brazo_previo = (estado_secuencia_brazo_t)0xFF;
    bool indicador_latido_activo = false;
    char texto_linea[52];

    while (1) {
        estado_telemetria_hmi_t lectura;
        lectura.color_detectado            = g_telemetria_hmi.color_detectado;
        lectura.matiz_h                    = g_telemetria_hmi.matiz_h;
        lectura.saturacion_s               = g_telemetria_hmi.saturacion_s;
        lectura.frecuencia_clear_hz        = g_telemetria_hmi.frecuencia_clear_hz;
        lectura.distancia_ultrasonico_1_cm = g_telemetria_hmi.distancia_ultrasonico_1_cm;
        lectura.distancia_ultrasonico_2_cm = g_telemetria_hmi.distancia_ultrasonico_2_cm;
        lectura.pallets_en_cola_cinta_1    = g_telemetria_hmi.pallets_en_cola_cinta_1;
        lectura.pallets_en_cola_cinta_2    = g_telemetria_hmi.pallets_en_cola_cinta_2;
        lectura.estado_cinta_2             = g_telemetria_hmi.estado_cinta_2;
        lectura.estado_piston              = g_telemetria_hmi.estado_piston;
        lectura.estado_brazo               = g_telemetria_hmi.estado_brazo;

        indicador_latido_activo = !indicador_latido_activo;
        gc9306_fill_circle(&g_pantalla_tft, 224, 16, 6,
                           indicador_latido_activo ? GC9306_GREEN : color_cabecera);

        if (lectura.color_detectado != color_previo) {
            dibujar_tarjeta_color_detectado(lectura.color_detectado);
            color_previo = lectura.color_detectado;
        }

        if (fabsf(lectura.matiz_h - matiz_previo) >= 0.2f) {
            snprintf(texto_linea, sizeof(texto_linea), "H:%5.1f S:%4.2f Clr:%5.0fHz",
                     lectura.matiz_h, lectura.saturacion_s, lectura.frecuencia_clear_hz);
            gc9306_draw_string(&g_pantalla_tft, 18, 145, texto_linea, GC9306_WHITE, color_panel, 1);
            matiz_previo = lectura.matiz_h;
        }

        if (fabsf(lectura.distancia_ultrasonico_1_cm - ultrasonico1_previo) >= 0.2f ||
            fabsf(lectura.distancia_ultrasonico_2_cm - ultrasonico2_previo) >= 0.2f) {
            snprintf(texto_linea, sizeof(texto_linea), "US1 (Piston): %5.1f cm",
                     lectura.distancia_ultrasonico_1_cm);
            gc9306_draw_string(&g_pantalla_tft, 18, 198, texto_linea, GC9306_WHITE, color_panel, 1);

            snprintf(texto_linea, sizeof(texto_linea), "US2 (Buffer): %5.1f cm",
                     lectura.distancia_ultrasonico_2_cm);
            gc9306_draw_string(&g_pantalla_tft, 18, 218, texto_linea, GC9306_WHITE, color_panel, 1);

            ultrasonico1_previo = lectura.distancia_ultrasonico_1_cm;
            ultrasonico2_previo = lectura.distancia_ultrasonico_2_cm;
        }

        if (lectura.pallets_en_cola_cinta_1 != cola1_previa ||
            lectura.pallets_en_cola_cinta_2 != cola2_previa ||
            lectura.estado_cinta_2 != estado_cinta2_previo ||
            lectura.estado_piston != estado_piston_previo ||
            lectura.estado_brazo != estado_brazo_previo) {
            snprintf(texto_linea, sizeof(texto_linea), "C2:%-6s P:%-7s",
                     obtener_texto_estado_cinta2(lectura.estado_cinta_2),
                     obtener_texto_estado_piston(lectura.estado_piston));
            gc9306_draw_string(&g_pantalla_tft, 18, 272, texto_linea, GC9306_YELLOW, color_panel, 1);

            snprintf(texto_linea, sizeof(texto_linea), "Q1:%u Q2:%u  B:%-8s",
                     lectura.pallets_en_cola_cinta_1,
                     lectura.pallets_en_cola_cinta_2,
                     obtener_texto_estado_brazo(lectura.estado_brazo));
            gc9306_draw_string(&g_pantalla_tft, 18, 290, texto_linea, GC9306_WHITE, color_panel, 1);

            cola1_previa = lectura.pallets_en_cola_cinta_1;
            cola2_previa = lectura.pallets_en_cola_cinta_2;
            estado_cinta2_previo = lectura.estado_cinta_2;
            estado_piston_previo = lectura.estado_piston;
            estado_brazo_previo = lectura.estado_brazo;
        }

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
