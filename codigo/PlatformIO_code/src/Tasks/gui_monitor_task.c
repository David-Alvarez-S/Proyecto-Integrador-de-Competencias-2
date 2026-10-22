#include "Tasks/gui_monitor_task.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

// Definición de color azul marino si no existe en la librería
#ifndef ILI9341_NAVY
#define ILI9341_NAVY ILI9341_RGB565(0, 0, 128)
#endif

static const char *TAG = "GUI_TASK";

// Instancia del controlador TFT2.4
static tft24_t s_tft;

// DIBUJO DE LA INTERFAZ ESTÁTICA
static void draw_static_dashboard(ili9341_t *lcd) {
  // Fondo negro general
  ili9341_fill_screen(lcd, ILI9341_BLACK);

  // BARRA DE TÍTULO SUPERIOR
  ili9341_fill_rect(lcd, 0, 0, 320, 28, ILI9341_DARKGRAY);
  ili9341_draw_string(lcd, 15, 6, "MONITOR DE CINTA Y PALLETS", ILI9341_WHITE,
                      ILI9341_TRANSPARENT, 2);
  ili9341_draw_hline(lcd, 0, 28, 320, ILI9341_CYAN);

  // PANEL IZQUIERDO: DETECCIÓN DE COLOR
  ili9341_draw_rect(lcd, 10, 35, 145, 150, ILI9341_GRAY);
  ili9341_fill_rect(lcd, 11, 36, 143, 18, ILI9341_NAVY);
  ili9341_draw_string(lcd, 18, 39, "ULTIMO PALLET", ILI9341_CYAN,
                      ILI9341_TRANSPARENT, 1);

  ili9341_draw_string(lcd, 18, 62, "ID:", ILI9341_WHITE, ILI9341_TRANSPARENT,
                      1);
  ili9341_draw_string(lcd, 18, 80, "Color:", ILI9341_WHITE, ILI9341_TRANSPARENT,
                      1);
  ili9341_draw_string(lcd, 18, 145, "RGB:", ILI9341_GRAY, ILI9341_TRANSPARENT,
                      1);

  // PANEL DERECHO: SENSORES Y ACTUADORES
  ili9341_draw_rect(lcd, 165, 35, 145, 150, ILI9341_GRAY);
  ili9341_fill_rect(lcd, 166, 36, 143, 18, ILI9341_NAVY);
  ili9341_draw_string(lcd, 175, 39, "ESTADO SISTEMA", ILI9341_CYAN,
                      ILI9341_TRANSPARENT, 1);

  ili9341_draw_string(lcd, 172, 62, "Sensor 1:", ILI9341_WHITE,
                      ILI9341_TRANSPARENT, 1);
  ili9341_draw_string(lcd, 172, 92, "Sensor 2:", ILI9341_WHITE,
                      ILI9341_TRANSPARENT, 1);
  ili9341_draw_string(lcd, 172, 122, "Cinta 1 :", ILI9341_WHITE,
                      ILI9341_TRANSPARENT, 1);
  ili9341_draw_string(lcd, 172, 152, "Piston  :", ILI9341_WHITE,
                      ILI9341_TRANSPARENT, 1);

  // BOTÓN TÁCTIL INFERIOR: RESET CONTADOR
  ili9341_fill_rect(lcd, 10, 195, 300, 38, ILI9341_DARKGRAY);
  ili9341_draw_rect(lcd, 10, 195, 300, 38, ILI9341_WHITE);
  ili9341_draw_string(lcd, 65, 207, "[ REINICIAR CONTADOR ]", ILI9341_YELLOW,
                      ILI9341_TRANSPARENT, 1);
}

// TAREA PRINCIPAL DE INTERFAZ Y MONITOREO

void gui_monitor_task(void *pvParameters) {
  ESP_LOGI(TAG, "Iniciando pantalla TFT e interfaz táctil horizontal...");

  // Configuración de la Pantalla TFT y Táctil compartiendo el bus SPI
  tft24_config_t cfg = TFT24_CONFIG_DEFAULT(
      TFT_MOSI_PIN, TFT_MISO_PIN, TFT_SCLK_PIN, TFT_LCD_CS_PIN, TFT_LCD_DC_PIN,
      TFT_LCD_RST_PIN, TFT_LCD_BL_PIN, TFT_TOUCH_CS_PIN, TFT_TOUCH_IRQ_PIN);

  cfg.host = TFT_SPI_HOST;
  cfg.rotation = TFT_ROTATION_1; // ROTACIÓN HORIZONTAL

  esp_err_t err = tft24_init(&s_tft, &cfg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error inicializando Pantalla TFT/Táctil: 0x%X", err);
    vTaskDelete(NULL);
    return;
  }

  // Inicializar tarea del panel táctil
  xpt2046_start(&s_tft.touch);

  // Dibujar elementos estáticos en pantalla
  draw_static_dashboard(&s_tft.lcd);

  // Variables de caché para refresco anti-parpadeo
  uint32_t last_rendered_id = 999999;
  tcs3200_color_t last_rendered_color = (tcs3200_color_t)-1;

  pallet_color_data_t current_pallet = {0};
  xpt2046_event_t touch_event;

  while (1) {
    // LEER ESTADO DE LA VARIABLE GLOBAL DE PALLET
    if (g_pallet_color_mutex != NULL &&
        xSemaphoreTake(g_pallet_color_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
      current_pallet = g_last_pallet_color;
      xSemaphoreGive(g_pallet_color_mutex);
    }

    // Actualizar datos del panel de color si cambió el pallet o el color
    if (current_pallet.id != last_rendered_id ||
        current_pallet.color != last_rendered_color) {
      last_rendered_id = current_pallet.id;
      last_rendered_color = current_pallet.color;

      // Borrar zonas de texto previas
      ili9341_fill_rect(&s_tft.lcd, 45, 62, 100, 15, ILI9341_BLACK);
      ili9341_fill_rect(&s_tft.lcd, 18, 98, 128, 20, ILI9341_BLACK);
      ili9341_fill_rect(&s_tft.lcd, 18, 160, 128, 15, ILI9341_BLACK);

      // Dibujar ID
      char id_buf[16];
      snprintf(id_buf, sizeof(id_buf), "#%lu", current_pallet.id);
      ili9341_draw_string(&s_tft.lcd, 45, 62, id_buf, ILI9341_YELLOW,
                          ILI9341_TRANSPARENT, 1);

      // Determinar color de la caja indicadora en pantalla
      uint16_t box_color = ILI9341_DARKGRAY;
      if (current_pallet.color == TCS3200_COLOR_RED) {
        box_color = ILI9341_RED;
      } else if (current_pallet.color == TCS3200_COLOR_BLUE) {
        box_color = ILI9341_BLUE;
      }

      // Dibujar recuadro de color visual
      ili9341_fill_rect(&s_tft.lcd, 18, 98, 128, 20, box_color);
      ili9341_draw_rect(&s_tft.lcd, 18, 98, 128, 20, ILI9341_WHITE);
      ili9341_draw_string(&s_tft.lcd, 25, 103, current_pallet.color_name,
                          ILI9341_WHITE, ILI9341_TRANSPARENT, 1);

      // Dibujar valores RGB
      char rgb_buf[24];
      snprintf(rgb_buf, sizeof(rgb_buf), "%d,%d,%d", current_pallet.r,
               current_pallet.g, current_pallet.b);
      ili9341_draw_string(&s_tft.lcd, 18, 160, rgb_buf, ILI9341_GREEN,
                          ILI9341_TRANSPARENT, 1);
    }

    // ATENDER EVENTOS TÁCTILES (BOTÓN REINICIAR CONTADOR)
    if (xpt2046_get_event(&s_tft.touch, &touch_event, 0) == ESP_OK) {
      if (touch_event.type == XPT2046_EVT_PRESS) {
        if (touch_event.y >= 195 && touch_event.y <= 235 &&
            touch_event.x >= 10 && touch_event.x <= 310) {

          ESP_LOGI(TAG, "¡Botón de reinicio presionado en pantalla!");

          // Feedback visual
          ili9341_fill_rect(&s_tft.lcd, 10, 195, 300, 38, ILI9341_BLUE);
          ili9341_draw_string(&s_tft.lcd, 85, 207, "REINICIADO", ILI9341_WHITE,
                              ILI9341_TRANSPARENT, 1);
          vTaskDelay(pdMS_TO_TICKS(200));

          // Reiniciar variable global protegida
          if (g_pallet_color_mutex != NULL &&
              xSemaphoreTake(g_pallet_color_mutex, pdMS_TO_TICKS(50)) ==
                  pdTRUE) {
            memset(&g_last_pallet_color, 0, sizeof(pallet_color_data_t));
            snprintf(g_last_pallet_color.color_name,
                     sizeof(g_last_pallet_color.color_name), "ninguno");
            xSemaphoreGive(g_pallet_color_mutex);
          }

          // Restaurar botón
          ili9341_fill_rect(&s_tft.lcd, 10, 195, 300, 38, ILI9341_DARKGRAY);
          ili9341_draw_rect(&s_tft.lcd, 10, 195, 300, 38, ILI9341_WHITE);
          ili9341_draw_string(&s_tft.lcd, 65, 207, "[ REINICIAR CONTADOR ]",
                              ILI9341_YELLOW, ILI9341_TRANSPARENT, 1);
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
