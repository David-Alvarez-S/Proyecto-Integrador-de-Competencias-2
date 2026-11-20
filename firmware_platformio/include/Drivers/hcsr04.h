#ifndef HCSR04_H
#define HCSR04_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_idf_version.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#ifndef HCSR04_TRIGGER_US
#define HCSR04_TRIGGER_US 12U
#endif

#ifndef HCSR04_DEFAULT_TIMEOUT_MS
#define HCSR04_DEFAULT_TIMEOUT_MS 30U
#endif

#ifndef HCSR04_DEFAULT_GUARD_MS
#define HCSR04_DEFAULT_GUARD_MS 30U
#endif

#ifndef HCSR04_DEFAULT_LOCK_TIMEOUT_MS
#define HCSR04_DEFAULT_LOCK_TIMEOUT_MS 1000U
#endif

#ifndef HCSR04_STUCK_WAIT_MS
#define HCSR04_STUCK_WAIT_MS 60U
#endif

#ifndef HCSR04_MAX_SAMPLES
#define HCSR04_MAX_SAMPLES 9U
#endif

#define HCSR04_ERR_NO_ECHO ESP_ERR_TIMEOUT
#define HCSR04_ERR_OUT_OF_RANGE ESP_ERR_INVALID_SIZE
#define HCSR04_ERR_ECHO_STUCK ESP_ERR_INVALID_STATE
#define HCSR04_ERR_BUSY ESP_ERR_NOT_FINISHED
#define HCSR04_ERR_NO_DATA ESP_ERR_NOT_FOUND

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#define HCSR04_YIELD_FROM_ISR_(hpw) portYIELD_FROM_ISR(hpw)
#else
#define HCSR04_YIELD_FROM_ISR_(hpw)                                            \
  do {                                                                         \
    (void)(hpw);                                                               \
    portYIELD_FROM_ISR();                                                      \
  } while (0)
#endif

typedef struct {
  float distance_cm;    
  uint32_t echo_us;     
  esp_err_t err;        
  int64_t timestamp_us; 
  uint32_t ok_count;    
  uint32_t err_count;   
} hcsr04_reading_t;

typedef struct {
  uint32_t timeout_ms;      
  uint32_t guard_ms;        
  uint32_t lock_timeout_ms; 
  float min_cm;             
  float max_cm;             
  float temperature_c;      
  int isr_flags;            
} hcsr04_group_config_t;

#define HCSR04_GROUP_CONFIG_DEFAULT()                                          \
  {                                                                            \
      .timeout_ms = HCSR04_DEFAULT_TIMEOUT_MS,                                 \
      .guard_ms = HCSR04_DEFAULT_GUARD_MS,                                     \
      .lock_timeout_ms = HCSR04_DEFAULT_LOCK_TIMEOUT_MS,                       \
      .min_cm = 2.0f,                                                          \
      .max_cm = 400.0f,                                                        \
      .temperature_c = 20.0f,                                                  \
      .isr_flags = 0,                                                          \
  }

typedef enum {
  HCSR04_ST_IDLE = 0,
  HCSR04_ST_ARMED,
  HCSR04_ST_RISING,
  HCSR04_ST_DONE,
} hcsr04_state_t;

typedef struct {
  gpio_num_t trig;
  gpio_num_t echo;
  const char *name;     
  SemaphoreHandle_t done;
  portMUX_TYPE mux;      
  volatile hcsr04_state_t state;
  volatile int64_t t_rise;
  volatile uint32_t echo_us;
  hcsr04_reading_t last;
} hcsr04_sensor_t;

typedef void (*hcsr04_reading_cb_t)(uint8_t id, const hcsr04_reading_t *reading,
                                    void *ctx);

typedef struct {
  uint32_t period_ms; 
  uint8_t samples;   
  UBaseType_t priority;
  uint32_t stack_size;    
  BaseType_t core_id;    
  hcsr04_reading_cb_t on_reading;
  void *cb_ctx;
} hcsr04_poll_config_t;

#define HCSR04_POLL_CONFIG_DEFAULT()                                           \
  {                                                                            \
      .period_ms = 100,                                                        \
      .samples = 1,                                                            \
      .priority = 5,                                                           \
      .stack_size = 4096,                                                      \
      .core_id = -1,                                                           \
      .on_reading = NULL,                                                      \
      .cb_ctx = NULL,                                                          \
  }

typedef struct {
  hcsr04_group_config_t cfg;
  hcsr04_sensor_t *sensors;
  uint8_t capacity;
  uint8_t count;
  SemaphoreHandle_t bus_lock;   
  portMUX_TYPE mux;            
  volatile float speed_of_sound; 
  int64_t last_end_us;          

  hcsr04_poll_config_t poll;
  TaskHandle_t poll_task;
  SemaphoreHandle_t stop_sem;
  SemaphoreHandle_t exited_sem;
  volatile bool polling;
  bool initialized;
} hcsr04_group_t;

static inline const char *hcsr04_err_to_str(esp_err_t err) {
  switch (err) {
  case ESP_OK:
    return "OK";
  case HCSR04_ERR_NO_ECHO:
    return "NO_ECHO";
  case HCSR04_ERR_OUT_OF_RANGE:
    return "OUT_OF_RANGE";
  case HCSR04_ERR_ECHO_STUCK:
    return "ECHO_STUCK";
  case HCSR04_ERR_BUSY:
    return "BUSY";
  case HCSR04_ERR_NO_DATA:
    return "NO_DATA";
  default:
    return "ERROR";
  }
}


__attribute__((unused)) static void IRAM_ATTR hcsr04_isr_(void *arg) {
  hcsr04_sensor_t *s = (hcsr04_sensor_t *)arg;
  int64_t now = esp_timer_get_time();
  int level = gpio_get_level(s->echo);

  if (level) {
    if (s->state == HCSR04_ST_ARMED) {
      s->t_rise = now;
      s->state = HCSR04_ST_RISING;
    }
  } else if (s->state == HCSR04_ST_RISING) {
    s->echo_us = (uint32_t)(now - s->t_rise);
    s->state = HCSR04_ST_DONE;
    BaseType_t hpw = pdFALSE;
    xSemaphoreGiveFromISR(s->done, &hpw);
    if (hpw == pdTRUE) {
      HCSR04_YIELD_FROM_ISR_(hpw);
    }
  }
}

static inline float hcsr04_echo_to_cm_(const hcsr04_group_t *g,
                                       uint32_t echo_us) {

  return (float)echo_us * g->speed_of_sound * 0.00005f;
}

static inline TickType_t hcsr04_ms_to_ticks_min1_(uint32_t ms) {
  TickType_t t = pdMS_TO_TICKS(ms);
  return (t == 0) ? 1 : t;
}


static inline esp_err_t hcsr04_measure_raw_locked_(hcsr04_group_t *g,
                                                   hcsr04_sensor_t *s,
                                                   uint32_t *echo_us) {
  esp_err_t err;

  int64_t guard_us = (int64_t)g->cfg.guard_ms * 1000;
  int64_t elapsed = esp_timer_get_time() - g->last_end_us;
  if (elapsed < guard_us) {
    uint32_t rem_ms = (uint32_t)((guard_us - elapsed + 999) / 1000);
    vTaskDelay(hcsr04_ms_to_ticks_min1_(rem_ms));
  }


  uint32_t waited = 0;
  while (gpio_get_level(s->echo) && waited < HCSR04_STUCK_WAIT_MS) {
    vTaskDelay(1);
    waited += portTICK_PERIOD_MS;
  }
  if (gpio_get_level(s->echo)) {
    g->last_end_us = esp_timer_get_time();
    return HCSR04_ERR_ECHO_STUCK;
  }


  (void)xSemaphoreTake(s->done, 0);
  s->echo_us = 0;
  s->t_rise = 0;
  s->state = HCSR04_ST_ARMED;
  gpio_intr_enable(s->echo);


  portENTER_CRITICAL(&g->mux);
  gpio_set_level(s->trig, 1);
  esp_rom_delay_us(HCSR04_TRIGGER_US);
  gpio_set_level(s->trig, 0);
  portEXIT_CRITICAL(&g->mux);


  (void)xSemaphoreTake(s->done, pdMS_TO_TICKS(g->cfg.timeout_ms) + 1);
  gpio_intr_disable(s->echo);

  hcsr04_state_t st = s->state;
  s->state = HCSR04_ST_IDLE;

  if (st == HCSR04_ST_DONE) {
    *echo_us = s->echo_us;
    err = ESP_OK;
  } else if (st == HCSR04_ST_RISING) {
    err = HCSR04_ERR_OUT_OF_RANGE; 
  } else {
    err = HCSR04_ERR_NO_ECHO; 
  }

  g->last_end_us = esp_timer_get_time();
  return err;
}


static inline esp_err_t hcsr04_group_init(hcsr04_group_t *g,
                                          hcsr04_sensor_t *storage,
                                          uint8_t capacity,
                                          const hcsr04_group_config_t *cfg) {
  if (g == NULL || storage == NULL || capacity == 0) {
    return ESP_ERR_INVALID_ARG;
  }

  hcsr04_group_config_t def = HCSR04_GROUP_CONFIG_DEFAULT();
  hcsr04_group_config_t use = cfg ? *cfg : def;
  if (use.timeout_ms == 0 || use.min_cm < 0.0f || use.max_cm <= use.min_cm) {
    return ESP_ERR_INVALID_ARG;
  }

  memset(g, 0, sizeof(*g));
  memset(storage, 0, sizeof(*storage) * capacity);
  g->cfg = use;
  g->sensors = storage;
  g->capacity = capacity;
  portMUX_INITIALIZE(&g->mux);
  g->speed_of_sound = 331.3f + 0.606f * use.temperature_c;

  g->bus_lock = xSemaphoreCreateMutex();
  g->stop_sem = xSemaphoreCreateBinary();
  g->exited_sem = xSemaphoreCreateBinary();
  if (g->bus_lock == NULL || g->stop_sem == NULL || g->exited_sem == NULL) {
    goto fail_nomem;
  }

  esp_err_t err = gpio_install_isr_service(use.isr_flags);
  if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
    if (g->bus_lock)
      vSemaphoreDelete(g->bus_lock);
    if (g->stop_sem)
      vSemaphoreDelete(g->stop_sem);
    if (g->exited_sem)
      vSemaphoreDelete(g->exited_sem);
    memset(g, 0, sizeof(*g));
    return err;
  }

  g->initialized = true;
  return ESP_OK;

fail_nomem:
  if (g->bus_lock)
    vSemaphoreDelete(g->bus_lock);
  if (g->stop_sem)
    vSemaphoreDelete(g->stop_sem);
  if (g->exited_sem)
    vSemaphoreDelete(g->exited_sem);
  memset(g, 0, sizeof(*g));
  return ESP_ERR_NO_MEM;
}

static inline esp_err_t hcsr04_group_add(hcsr04_group_t *g, gpio_num_t trig,
                                         gpio_num_t echo, const char *name,
                                         uint8_t *out_id) {
  if (g == NULL || !g->initialized || g->polling) {
    return ESP_ERR_INVALID_STATE;
  }
  if (!GPIO_IS_VALID_OUTPUT_GPIO(trig) || !GPIO_IS_VALID_GPIO(echo) ||
      trig == echo) {
    return ESP_ERR_INVALID_ARG;
  }
  if (g->count >= g->capacity) {
    return ESP_ERR_NO_MEM;
  }
  for (uint8_t i = 0; i < g->count; i++) {
    const hcsr04_sensor_t *o = &g->sensors[i];
    if (o->trig == trig || o->trig == echo || o->echo == trig ||
        o->echo == echo) {
      return ESP_ERR_INVALID_ARG;
    }
  }

  if (xSemaphoreTake(g->bus_lock, pdMS_TO_TICKS(g->cfg.lock_timeout_ms)) !=
      pdTRUE) {
    return HCSR04_ERR_BUSY;
  }

  uint8_t id = g->count;
  hcsr04_sensor_t *s = &g->sensors[id];
  memset(s, 0, sizeof(*s));
  s->trig = trig;
  s->echo = echo;
  s->name = name ? name : "sensor";
  portMUX_INITIALIZE(&s->mux);
  s->last.err = HCSR04_ERR_NO_DATA;
  s->last.distance_cm = -1.0f;
  s->done = xSemaphoreCreateBinary();

  esp_err_t err = ESP_OK;
  if (s->done == NULL) {
    err = ESP_ERR_NO_MEM;
    goto out;
  }

  gpio_config_t trig_cfg = {
      .pin_bit_mask = 1ULL << trig,
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  err = gpio_config(&trig_cfg);
  if (err != ESP_OK) {
    goto fail_sem;
  }
  gpio_set_level(trig, 0);

  gpio_config_t echo_cfg = {
      .pin_bit_mask = 1ULL << echo,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_IS_VALID_OUTPUT_GPIO(echo) ? GPIO_PULLDOWN_ENABLE
                                                      : GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };
  err = gpio_config(&echo_cfg);
  if (err != ESP_OK) {
    goto fail_sem;
  }

  err = gpio_isr_handler_add(echo, hcsr04_isr_, s);
  if (err != ESP_OK) {
    goto fail_sem;
  }
  gpio_intr_disable(echo); 

  g->count++;
  if (out_id) {
    *out_id = id;
  }
  goto out;

fail_sem:
  vSemaphoreDelete(s->done);
  s->done = NULL;
out:
  xSemaphoreGive(g->bus_lock);
  return err;
}

static inline uint8_t hcsr04_group_count(const hcsr04_group_t *g) {
  return g ? g->count : 0;
}

static inline esp_err_t hcsr04_group_set_temperature(hcsr04_group_t *g,
                                                     float celsius) {
  if (g == NULL || !g->initialized || celsius < -40.0f || celsius > 85.0f) {
    return ESP_ERR_INVALID_ARG;
  }
  g->speed_of_sound =
      331.3f + 0.606f * celsius; 
  g->cfg.temperature_c = celsius;
  return ESP_OK;
}

static inline esp_err_t hcsr04_group_measure(hcsr04_group_t *g, uint8_t id,
                                             uint8_t samples,
                                             hcsr04_reading_t *out) {
  if (g == NULL || !g->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  if (id >= g->count) {
    return ESP_ERR_INVALID_ARG;
  }
  if (samples == 0) {
    samples = 1;
  } else if (samples > HCSR04_MAX_SAMPLES) {
    samples = HCSR04_MAX_SAMPLES;
  }

  hcsr04_sensor_t *s = &g->sensors[id];
  uint32_t echoes[HCSR04_MAX_SAMPLES];
  uint8_t n_ok = 0;
  esp_err_t last_err = HCSR04_ERR_NO_ECHO;

  for (uint8_t i = 0; i < samples; i++) {
    if (xSemaphoreTake(g->bus_lock, pdMS_TO_TICKS(g->cfg.lock_timeout_ms)) !=
        pdTRUE) {
      last_err = HCSR04_ERR_BUSY;
      break;
    }
    uint32_t echo = 0;
    esp_err_t e = hcsr04_measure_raw_locked_(g, s, &echo);
    xSemaphoreGive(g->bus_lock);

    if (e == ESP_OK) {
      float cm = hcsr04_echo_to_cm_(g, echo);
      if (cm < g->cfg.min_cm || cm > g->cfg.max_cm) {
        last_err = HCSR04_ERR_OUT_OF_RANGE;
      } else {
        echoes[n_ok++] = echo;
      }
    } else {
      last_err = e;
    }
  }

  esp_err_t result = last_err;
  uint32_t median = 0;
  float cm = -1.0f;
  if (n_ok > 0) {
    for (uint8_t i = 1; i < n_ok; i++) {  
      uint32_t v = echoes[i];
      int8_t j = (int8_t)i - 1;
      while (j >= 0 && echoes[j] > v) {
        echoes[j + 1] = echoes[j];
        j--;
      }
      echoes[j + 1] = v;
    }
    median = echoes[n_ok / 2];
    cm = hcsr04_echo_to_cm_(g, median);
    result = ESP_OK;
  }

  int64_t now = esp_timer_get_time();
  portENTER_CRITICAL(&s->mux);
  s->last.err = result;
  s->last.timestamp_us = now;
  s->last.echo_us = median;
  s->last.distance_cm = cm;
  if (result == ESP_OK) {
    s->last.ok_count++;
  } else {
    s->last.err_count++;
  }
  if (out) {
    *out = s->last;
  }
  portEXIT_CRITICAL(&s->mux);

  return result;
}

static inline esp_err_t hcsr04_group_get_last(hcsr04_group_t *g, uint8_t id,
                                              hcsr04_reading_t *out) {
  if (g == NULL || !g->initialized || out == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if (id >= g->count) {
    return ESP_ERR_INVALID_ARG;
  }
  hcsr04_sensor_t *s = &g->sensors[id];
  portENTER_CRITICAL(&s->mux);
  *out = s->last;
  portEXIT_CRITICAL(&s->mux);
  return ESP_OK;
}
static inline void hcsr04_poll_task_(void *arg) {
  hcsr04_group_t *g = (hcsr04_group_t *)arg;
  bool stop = false;

  while (!stop) {
    for (uint8_t id = 0; id < g->count; id++) {
      hcsr04_reading_t r;
      (void)hcsr04_group_measure(g, id, g->poll.samples, &r);
      if (g->poll.on_reading) {
        g->poll.on_reading(id, &r, g->poll.cb_ctx);
      }
      if (xSemaphoreTake(g->stop_sem, 0) == pdTRUE) {
        stop = true;
        break;
      }
    }
    if (!stop) {
      if (xSemaphoreTake(g->stop_sem, hcsr04_ms_to_ticks_min1_(
                                          g->poll.period_ms)) == pdTRUE) {
        stop = true;
      }
    }
  }

  xSemaphoreGive(g->exited_sem);
  vTaskDelete(NULL);
}

static inline esp_err_t
hcsr04_group_start_polling(hcsr04_group_t *g,
                           const hcsr04_poll_config_t *pcfg) {
  if (g == NULL || !g->initialized || g->polling || g->count == 0) {
    return ESP_ERR_INVALID_STATE;
  }
  hcsr04_poll_config_t def = HCSR04_POLL_CONFIG_DEFAULT();
  g->poll = pcfg ? *pcfg : def;
  if (g->poll.samples == 0) {
    g->poll.samples = 1;
  }

  (void)xSemaphoreTake(g->stop_sem, 0);
  (void)xSemaphoreTake(g->exited_sem, 0);

  BaseType_t ok;
  if (g->poll.core_id < 0) {
    ok = xTaskCreate(hcsr04_poll_task_, "hcsr04_poll", g->poll.stack_size, g,
                     g->poll.priority, &g->poll_task);
  } else {
    ok = xTaskCreatePinnedToCore(hcsr04_poll_task_, "hcsr04_poll",
                                 g->poll.stack_size, g, g->poll.priority,
                                 &g->poll_task, g->poll.core_id);
  }
  if (ok != pdPASS) {
    g->poll_task = NULL;
    return ESP_ERR_NO_MEM;
  }
  g->polling = true;
  return ESP_OK;
}

static inline esp_err_t hcsr04_group_stop_polling(hcsr04_group_t *g) {
  if (g == NULL || !g->initialized || !g->polling) {
    return ESP_ERR_INVALID_STATE;
  }
  xSemaphoreGive(g->stop_sem);

  uint32_t wait_ms = 1000U + (uint32_t)g->count * g->poll.samples *
                                 (g->cfg.timeout_ms + g->cfg.guard_ms + 100U);
  if (xSemaphoreTake(g->exited_sem, pdMS_TO_TICKS(wait_ms)) != pdTRUE) {
    return ESP_ERR_TIMEOUT; 
  }
  g->poll_task = NULL;
  g->polling = false;
  return ESP_OK;
}

static inline bool hcsr04_group_is_polling(const hcsr04_group_t *g) {
  return g != NULL && g->polling;
}

static inline esp_err_t hcsr04_group_deinit(hcsr04_group_t *g) {
  if (g == NULL || !g->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  if (g->polling) {
    esp_err_t err = hcsr04_group_stop_polling(g);
    if (err != ESP_OK) {
      return err;
    }
  }
  if (xSemaphoreTake(g->bus_lock, pdMS_TO_TICKS(g->cfg.lock_timeout_ms)) !=
      pdTRUE) {
    return HCSR04_ERR_BUSY;
  }
  for (uint8_t i = 0; i < g->count; i++) {
    hcsr04_sensor_t *s = &g->sensors[i];
    gpio_intr_disable(s->echo);
    gpio_isr_handler_remove(s->echo);
    gpio_reset_pin(s->echo);
    gpio_reset_pin(s->trig);
    if (s->done) {
      vSemaphoreDelete(s->done);
      s->done = NULL;
    }
  }
  g->initialized = false;
  g->count = 0;
  SemaphoreHandle_t lock = g->bus_lock;
  g->bus_lock = NULL;
  xSemaphoreGive(lock);
  vSemaphoreDelete(lock);
  vSemaphoreDelete(g->stop_sem);
  vSemaphoreDelete(g->exited_sem);
  g->stop_sem = g->exited_sem = NULL;
  return ESP_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* HCSR04_H */
