#ifndef CONTROL_PALLET_TASK_H
#define CONTROL_PALLET_TASK_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stdint.h>

// Drivers del sistema
#include "Drivers/hc595.h"
#include "Drivers/hc595_stepper.h"
#include "Drivers/hcsr04.h"
#include "Drivers/pca9685.h"
#include "Drivers/tcs3200.h"

// --- Registro de Desplazamiento 74HC595 ---
#define HC595_DATA_PIN GPIO_NUM_23  // SER
#define HC595_CLK_PIN GPIO_NUM_18   // SRCLK
#define HC595_LATCH_PIN GPIO_NUM_26 // CS
#define HC595_OE_PIN GPIO_NUM_25    // /OE

// --- Mapeo de Salidas del 74HC595 para los Motores Paso a Paso ---
#define STEPPER1_IN1_OUTPUT 0
#define STEPPER1_IN2_OUTPUT 1
#define STEPPER1_IN3_OUTPUT 2
#define STEPPER1_IN4_OUTPUT 3

#define STEPPER2_IN1_OUTPUT 4
#define STEPPER2_IN2_OUTPUT 5
#define STEPPER2_IN3_OUTPUT 6
#define STEPPER2_IN4_OUTPUT 7

// --- Sensores Ultrasónicos HC-SR04 ---
#define HCSR04_1_TRIG_PIN GPIO_NUM_32
#define HCSR04_1_ECHO_PIN GPIO_NUM_36

#define HCSR04_2_TRIG_PIN GPIO_NUM_33
#define HCSR04_2_ECHO_PIN GPIO_NUM_39

// --- Bus I2C y PCA9685 para Servomotor ---
#define I2C_SDA_PIN GPIO_NUM_21
#define I2C_SCL_PIN GPIO_NUM_22
#define SERVO_CHANNEL 0

// --- Sensor de Color TCS3200 ---
#define TCS3200_S0_PIN GPIO_NUM_16
#define TCS3200_S1_PIN GPIO_NUM_4
#define TCS3200_S2_PIN GPIO_NUM_17
#define TCS3200_S3_PIN GPIO_NUM_5
#define TCS3200_OUT_PIN GPIO_NUM_34

// --- Tiempos y Parámetros ---
#define SERVO_AVANCE_TIME 1400
#define SERVO_REGRESO_TIME 2490

#define ACTIVATION_OFFSET 9000
#define SERVO_SPEED_FORWARD_PCT 80.0f
#define SERVO_SPEED_REVERSE_PCT -80.0f

#define STEPPER1_SPEED_STEPS_PER_S 200.0f
#define STEPPER2_SPEED_STEPS_PER_S 200.0f
#define STEPPER2_MAX_RUN_TIMEOUT_MS 20000

#define PALLET_DETECTION_DIST_CM 6.0f
#define PALLET_POLL_INTERVAL_MS 50
#define PALLET_DEBOUNCE_DELAY_MS 500

// --- Estructura de Datos del Pallet ---
typedef struct {
  uint32_t id;           // ID correlativo del pallet
  tcs3200_color_t color; // Enum del color
  char color_name[16];   // Nombre del color ("rojo", "azul", "blanco",
                         // "desconocido")
  uint8_t r, g, b;       // Valores RGB
  int64_t timestamp_us;  // Tiempo exacto de paso por sensor TCS3200
} pallet_color_data_t;

// Estructura de Parámetros para la Tarea de Control
typedef struct {
  hc595_stepper_t *stepper2;  // Puntero al Motor Paso a Paso 2
  pca9685_t *pca_dev;         // Puntero al controlador PCA9685
  hcsr04_group_t *hcsr_group; // Puntero al grupo de ultrasónicos
  uint8_t sensor1_id;         // ID del sensor ultrasónico 1 (Inicio secuencia)
  uint8_t sensor2_id;         // ID del sensor ultrasónico 2 (Parada Stepper 2)
  tcs3200_t *tcs_dev;         // Puntero al sensor de color TCS3200
} pallet_task_args_t;

// --- Sincronización Global ---
extern QueueHandle_t g_pallet_color_queue;
extern pallet_color_data_t g_last_pallet_color;
extern SemaphoreHandle_t g_pallet_color_mutex;

// --- Prototipos de Tareas ---
void tcs3200_color_polling_task(void *pvParameters);
void control_pallet_task(void *pvParameters);

#endif // CONTROL_PALLET_TASK_H
