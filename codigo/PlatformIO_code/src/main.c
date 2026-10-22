#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stdio.h>

#include "Drivers/tcs3200.h"
#include "Tasks/control_pallet_task.h"
#include "Tasks/gui_monitor_task.h"

static const char *TAG = "MAIN_APP";

static hc595_t hc595_chain;
static hc595_stepper_t stepper1;
static hc595_stepper_t stepper2;
static pca9685_t pca_dev;
static hcsr04_group_t hcsr_group;
static hcsr04_sensor_t hcsr_sensors[2];
static tcs3200_t tcs_dev;

static pallet_task_args_t pallet_args;

static void test_leds_buzzer_task(void *pvParameters) {
  hc595_t *chain = (hc595_t *)pvParameters;

  while (1) {
    for (uint8_t pin = 8; pin <= 11; pin++) {
      hc595_set_pin(chain, pin, true);
      vTaskDelay(pdMS_TO_TICKS(80));
    }

    for (int8_t pin = 11; pin >= 8; pin--) {
      hc595_set_pin(chain, pin, false);
      vTaskDelay(pdMS_TO_TICKS(80));
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void app_main(void) {
  ESP_LOGI(TAG, "Iniciando Sistema de Cinta Transportadora y Monitor TFT...");

  // Crear Cola y Mutex
  g_pallet_color_queue = xQueueCreate(10, sizeof(pallet_color_data_t));
  g_pallet_color_mutex = xSemaphoreCreateMutex();

  // Inicializar 74HC595
  hc595_config_t hc595_cfg = HC595_CONFIG_DEFAULT(
      HC595_DATA_PIN, HC595_CLK_PIN, HC595_LATCH_PIN, HC595_OE_PIN);
  hc595_cfg.chips = 2;
  ESP_ERROR_CHECK(hc595_init(&hc595_chain, &hc595_cfg));
  hc595_all_off(&hc595_chain);

  // Inicializar Steppers
  hc595_stepper_config_t s1_cfg = HC595_STEPPER_CONFIG_DEFAULT(
      &hc595_chain, STEPPER1_IN1_OUTPUT, STEPPER1_IN2_OUTPUT,
      STEPPER1_IN3_OUTPUT, STEPPER1_IN4_OUTPUT);
  s1_cfg.max_speed = STEPPER1_SPEED_STEPS_PER_S;
  s1_cfg.accel = 1000.0f;
  s1_cfg.mode = HC595_STEPPER_MODE_HALF;
  ESP_ERROR_CHECK(hc595_stepper_init(&stepper1, &s1_cfg));

  hc595_stepper_config_t s2_cfg = HC595_STEPPER_CONFIG_DEFAULT(
      &hc595_chain, STEPPER2_IN1_OUTPUT, STEPPER2_IN2_OUTPUT,
      STEPPER2_IN3_OUTPUT, STEPPER2_IN4_OUTPUT);
  s2_cfg.max_speed = STEPPER2_SPEED_STEPS_PER_S;
  s2_cfg.accel = 1000.0f;
  s2_cfg.mode = HC595_STEPPER_MODE_HALF;
  ESP_ERROR_CHECK(hc595_stepper_init(&stepper2, &s2_cfg));

  // Arrancar Stepper 1 continuo
  ESP_ERROR_CHECK(hc595_stepper_run(&stepper1, STEPPER1_SPEED_STEPS_PER_S));

  // Inicializar Bus I2C y PCA9685
  i2c_master_bus_config_t i2c_bus_cfg = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .i2c_port = I2C_NUM_0,
      .scl_io_num = I2C_SCL_PIN,
      .sda_io_num = I2C_SDA_PIN,
      .glitch_ignore_cnt = 7,
      .flags.enable_internal_pullup = true,
  };
  i2c_master_bus_handle_t i2c_bus = NULL;
  ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus));

  pca9685_config_t pca_cfg = PCA9685_CONFIG_DEFAULT(i2c_bus);
  ESP_ERROR_CHECK(pca9685_init(&pca_dev, &pca_cfg));

  pca9685_cr_config_t cr_cfg = PCA9685_CR_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(pca9685_cr_attach(&pca_dev, SERVO_CHANNEL, &cr_cfg));

  // Inicializar Ultrasónicos 1 y 2
  hcsr04_group_config_t hcsr_cfg = HCSR04_GROUP_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(hcsr04_group_init(&hcsr_group, hcsr_sensors, 2, &hcsr_cfg));

  uint8_t sensor1_id = 0, sensor2_id = 0;
  ESP_ERROR_CHECK(hcsr04_group_add(&hcsr_group, HCSR04_1_TRIG_PIN,
                                   HCSR04_1_ECHO_PIN, "sensor_1", &sensor1_id));
  ESP_ERROR_CHECK(hcsr04_group_add(&hcsr_group, HCSR04_2_TRIG_PIN,
                                   HCSR04_2_ECHO_PIN, "sensor_2", &sensor2_id));

  // Inicializar Sensor de Color TCS3200
  tcs3200_config_t tcs_cfg =
      TCS3200_CONFIG_DEFAULT(TCS3200_S0_PIN, TCS3200_S1_PIN, TCS3200_S2_PIN,
                             TCS3200_S3_PIN, TCS3200_OUT_PIN, GPIO_NUM_NC);
  ESP_ERROR_CHECK(tcs3200_init(&tcs_dev, &tcs_cfg));

  // Lanzar Tareas FreeRTOS
  pallet_args.stepper2 = &stepper2;
  pallet_args.pca_dev = &pca_dev;
  pallet_args.hcsr_group = &hcsr_group;
  pallet_args.sensor1_id = sensor1_id;
  pallet_args.sensor2_id = sensor2_id;
  pallet_args.tcs_dev = &tcs_dev;

  // Tarea A: Polling constante de color
  xTaskCreatePinnedToCore(tcs3200_color_polling_task, "color_poll_task", 3072,
                          &tcs_dev, 4, NULL, 1);

  // Tarea B: Control de pallets
  xTaskCreatePinnedToCore(control_pallet_task, "control_pallet_task", 4096,
                          &pallet_args, 5, NULL, 1);

  // Tarea C: Interfaz Gráfica y Monitor Táctil TFT Horizontal
  xTaskCreatePinnedToCore(gui_monitor_task, "gui_monitor_task", 4096, NULL, 3,
                          NULL, 1);

  // Tarea D: Test secuencial LEDs/Buzzer
  xTaskCreatePinnedToCore(test_leds_buzzer_task, "test_leds_buzzer", 2048,
                          &hc595_chain, 1, NULL, 1);

  ESP_LOGI(TAG, "Sistema totalmente inicializado y listo.");
}
