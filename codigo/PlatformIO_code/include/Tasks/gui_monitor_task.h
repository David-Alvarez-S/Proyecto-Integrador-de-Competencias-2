#ifndef GUI_MONITOR_TASK_H
#define GUI_MONITOR_TASK_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stdint.h>

// Drivers de Pantalla TFT y Táctil
#include "Drivers/tft24.h"
#include "Tasks/control_pallet_task.h"

// --- CONFIGURACIÓN DE PINES TFT ILI9341 + XPT2046
#define TFT_SPI_HOST SPI2_HOST
#define TFT_MOSI_PIN GPIO_NUM_23
#define TFT_MISO_PIN GPIO_NUM_19
#define TFT_SCLK_PIN GPIO_NUM_18

#define TFT_LCD_CS_PIN GPIO_NUM_15
#define TFT_LCD_DC_PIN GPIO_NUM_2
#define TFT_LCD_RST_PIN GPIO_NUM_4
#define TFT_LCD_BL_PIN GPIO_NUM_14

#define TFT_TOUCH_CS_PIN GPIO_NUM_5
#define TFT_TOUCH_IRQ_PIN GPIO_NUM_27

// PROTOTIPO DE LA TAREA
void gui_monitor_task(void *pvParameters);

#endif // GUI_MONITOR_TASK_H
