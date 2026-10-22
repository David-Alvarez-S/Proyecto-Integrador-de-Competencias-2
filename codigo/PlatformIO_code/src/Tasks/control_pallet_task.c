#include "Tasks/control_pallet_task.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG_PALLET = "PALLET_TASK";
static const char *TAG_COLOR = "COLOR_TASK";

QueueHandle_t g_pallet_color_queue = NULL;
pallet_color_data_t g_last_pallet_color = {0};
SemaphoreHandle_t g_pallet_color_mutex = NULL;

static uint32_t s_pallet_counter = 0;

typedef enum {
  COLOR_DETECT_NONE = 0, // Fondo naranja, blanco, o imperfecciones de la cinta
  COLOR_DETECT_RED,
  COLOR_DETECT_BLUE
} color_detection_t;

static color_detection_t classify_pallet_color(uint8_t r, uint8_t g, uint8_t b,
                                               const char **name_out) {
  // Valores de fondo/cinta: R:255, G:186, B:157
  // Valores de blanco/imperfecciones: R:255, G:203, B:186 (se descartan)

  // Azul: El canal B predomina marcadamente sobre el canal R (B > 230 y B >
  // R) Muestras registradas: [R:211-215, G:205-207, B:255]
  if (b > 230 && b > r) {
    if (name_out)
      *name_out = "azul";
    return COLOR_DETECT_BLUE;
  }

  // Rojo: R es alto (>200) y tanto G como B caen a niveles muy bajos (<130)
  // Muestras registradas: [R:255, G:92-93, B:87-88]
  if (r > 200 && g < 130 && b < 130) {
    if (name_out)
      *name_out = "rojo";
    return COLOR_DETECT_RED;
  }

  // Todo lo demás (Blanco, Cinta vacía, destellos) se clasifica como ninguno
  if (name_out)
    *name_out = "fondo";
  return COLOR_DETECT_NONE;
}

// POLLING CONSTANTE DE COLOR
void tcs3200_color_polling_task(void *pvParameters) {
  tcs3200_t *tcs_dev = (tcs3200_t *)pvParameters;

  if (tcs_dev == NULL) {
    ESP_LOGE(TAG_COLOR, "Puntero TCS3200 nulo en tarea de color.");
    vTaskDelete(NULL);
    return;
  }

  ESP_LOGI(TAG_COLOR, "Iniciando polling de color");

  tcs3200_sample_t sample;
  bool in_pallet_zone = false;
  uint8_t consecutive_color_hits = 0;

  while (1) {
    esp_err_t err = tcs3200_read(tcs_dev, 0, &sample);

    if (err == ESP_OK && sample.valid) {
      const char *color_name_str = "fondo";
      color_detection_t detected =
          classify_pallet_color(sample.r, sample.g, sample.b, &color_name_str);

      if (detected != COLOR_DETECT_NONE) {
        // Se detectó Rojo o Azul
        if (!in_pallet_zone) {
          consecutive_color_hits++;
          if (consecutive_color_hits >=
              2) { // Filtro anti-ruido (2 lecturas consecutivas ~120ms)
            in_pallet_zone = true;
            s_pallet_counter++;

            pallet_color_data_t pallet_data = {0};
            pallet_data.id = s_pallet_counter;
            pallet_data.r = sample.r;
            pallet_data.g = sample.g;
            pallet_data.b = sample.b;
            pallet_data.timestamp_us = sample.time_us;
            snprintf(pallet_data.color_name, sizeof(pallet_data.color_name),
                     "%s", color_name_str);

            if (detected == COLOR_DETECT_RED) {
              pallet_data.color = TCS3200_COLOR_RED;
            } else if (detected == COLOR_DETECT_BLUE) {
              pallet_data.color = TCS3200_COLOR_BLUE;
            }

            ESP_LOGI(TAG_COLOR,
                     ">> PALLET VÁLIDO DETECTADO #%lu | COLOR: %s [R:%d, G:%d, "
                     "B:%d] <<",
                     pallet_data.id, pallet_data.color_name, pallet_data.r,
                     pallet_data.g, pallet_data.b);

            // Encolar para la tarea de control
            if (g_pallet_color_queue != NULL) {
              if (xQueueSend(g_pallet_color_queue, &pallet_data, 0) != pdPASS) {
                ESP_LOGW(TAG_COLOR,
                         "Cola de pallets llena. Transmisión omitida.");
              }
            }

            // Actualizar variable global
            if (g_pallet_color_mutex != NULL &&
                xSemaphoreTake(g_pallet_color_mutex, pdMS_TO_TICKS(50)) ==
                    pdTRUE) {
              g_last_pallet_color = pallet_data;
              xSemaphoreGive(g_pallet_color_mutex);
            }
          }
        }
      } else {
        // Volvió al fondo o pasó un color no deseado
        consecutive_color_hits = 0;
        in_pallet_zone = false;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(60));
  }
}

void control_pallet_task(void *pvParameters) {
  pallet_task_args_t *args = (pallet_task_args_t *)pvParameters;

  if (args == NULL || args->stepper2 == NULL || args->pca_dev == NULL ||
      args->hcsr_group == NULL) {
    ESP_LOGE(TAG_PALLET, "Argumentos inválidos pasados a control_pallet_task.");
    vTaskDelete(NULL);
    return;
  }

  ESP_LOGI(TAG_PALLET, "Tarea de secuencia de pallets iniciada.");

  hcsr04_reading_t reading1;
  hcsr04_reading_t reading2;

  while (1) {
    // Monitorear llegada de pallet al Sensor Ultrasónico 1
    esp_err_t err1 =
        hcsr04_group_measure(args->hcsr_group, args->sensor1_id, 3, &reading1);

    if (err1 == ESP_OK && reading1.distance_cm > 0.0f &&
        reading1.distance_cm <= PALLET_DETECTION_DIST_CM) {

      // Obtener el color registrado por el TCS3200
      pallet_color_data_t active_pallet = {0};
      if (g_pallet_color_queue != NULL &&
          xQueueReceive(g_pallet_color_queue, &active_pallet, 0) == pdPASS) {
        ESP_LOGI(
            TAG_PALLET, "Procesando Pallet #%lu (%s) en Sensor 1 (%.2f cm)",
            active_pallet.id, active_pallet.color_name, reading1.distance_cm);
      } else {
        ESP_LOGW(TAG_PALLET,
                 "Pallet detectado en Sensor 1 (%.2f cm) sin registro de color "
                 "previo.",
                 reading1.distance_cm);
      }

      vTaskDelay(pdMS_TO_TICKS(ACTIVATION_OFFSET));

      // --- Encender Motor Paso a Paso 2 ---
      ESP_LOGI(TAG_PALLET, "Activando Motor Paso a Paso 2");
      hc595_stepper_run(args->stepper2, STEPPER2_SPEED_STEPS_PER_S);

      // --- Servomotor Pista/Pistón (Avanza) ---
      ESP_LOGI(TAG_PALLET, "Movilizando pallet con Servo");
      pca9685_cr_set_speed(args->pca_dev, SERVO_CHANNEL,
                           SERVO_SPEED_REVERSE_PCT);
      vTaskDelay(pdMS_TO_TICKS(SERVO_AVANCE_TIME));

      // --- Servomotor Pista/Pistón (Regresa) ---
      ESP_LOGI(TAG_PALLET, "Regresando Pistón");
      pca9685_cr_set_speed(args->pca_dev, SERVO_CHANNEL,
                           SERVO_SPEED_FORWARD_PCT);
      vTaskDelay(pdMS_TO_TICKS(SERVO_REGRESO_TIME));

      // --- Liberar Servomotor ---
      pca9685_release(args->pca_dev, SERVO_CHANNEL);

      // --- MONITOREAR SENSOR ULTRASÓNICO 2 PARA DETENER STEPPER 2 ---
      ESP_LOGI(TAG_PALLET, "Esperando llegada al Sensor Ultrasónico 2");
      TickType_t start_tick = xTaskGetTickCount();
      bool stopped_by_sensor = false;

      while ((xTaskGetTickCount() - start_tick) <
             pdMS_TO_TICKS(STEPPER2_MAX_RUN_TIMEOUT_MS)) {
        esp_err_t err2 = hcsr04_group_measure(args->hcsr_group,
                                              args->sensor2_id, 1, &reading2);

        if (err2 == ESP_OK && reading2.distance_cm > 0.0f &&
            reading2.distance_cm <= PALLET_DETECTION_DIST_CM) {
          ESP_LOGI(
              TAG_PALLET,
              "Pallet detectado en Sensor 2 (%.2f cm). Deteniendo Stepper 2.",
              reading2.distance_cm);
          stopped_by_sensor = true;
          break;
        }
        vTaskDelay(pdMS_TO_TICKS(30));
      }

      if (!stopped_by_sensor) {
        ESP_LOGW(
            TAG_PALLET,
            "Timeout esperando Sensor 2. Deteniendo Stepper 2 por seguridad.");
      }

      hc595_stepper_stop(args->stepper2);

      ESP_LOGI(TAG_PALLET, "Secuencia completada.");
      vTaskDelay(pdMS_TO_TICKS(PALLET_DEBOUNCE_DELAY_MS));
    }

    vTaskDelay(pdMS_TO_TICKS(PALLET_POLL_INTERVAL_MS));
  }
}
