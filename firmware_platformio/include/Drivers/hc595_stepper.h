#ifndef HC595_STEPPER_H
#define HC595_STEPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "hc595.h"

#ifndef HC595_STEPPER_STEPS_PER_REV_FULL
#define HC595_STEPPER_STEPS_PER_REV_FULL 2048.0f
#endif
#ifndef HC595_STEPPER_STEPS_PER_REV_HALF
#define HC595_STEPPER_STEPS_PER_REV_HALF 4096.0f
#endif

#ifndef HC595_STEPPER_MAX_STEP_RATE
#define HC595_STEPPER_MAX_STEP_RATE 1500.0f
#endif

#ifndef HC595_STEPPER_DEFAULT_MAX_SPEED
#define HC595_STEPPER_DEFAULT_MAX_SPEED 1000.0f
#endif
#ifndef HC595_STEPPER_DEFAULT_ACCEL
#define HC595_STEPPER_DEFAULT_ACCEL 1000.0f
#endif
#ifndef HC595_STEPPER_DEFAULT_SETTLE_MS
#define HC595_STEPPER_DEFAULT_SETTLE_MS 15U 
#endif

#ifndef HC595_STEPPER_LOCK_TIMEOUT_MS
#define HC595_STEPPER_LOCK_TIMEOUT_MS 100U
#endif

#ifndef HC595_STEPPER_DEINIT_GRACE_MS
#define HC595_STEPPER_DEINIT_GRACE_MS 20U
#endif

#define HC595_STEPPER_EVT_IDLE (1U << 0)
#define HC595_STEPPER_START_DELAY_US 100
#define HC595_STEPPER_MIN_WAIT_US 50

typedef enum {
  HC595_STEPPER_MODE_WAVE = 0,
  HC595_STEPPER_MODE_FULL, 
  HC595_STEPPER_MODE_HALF, 
} hc595_stepper_mode_t;

typedef enum {
  HC595_STEPPER_RUN_POSITION = 0, 
  HC595_STEPPER_RUN_VELOCITY,     
} hc595_stepper_run_mode_t;

typedef struct hc595_stepper hc595_stepper_t;
typedef void (*hc595_stepper_done_cb_t)(hc595_stepper_t *motor, void *ctx);

typedef struct {
  hc595_t *chain; 
  uint8_t pin[4]; 
  hc595_stepper_mode_t mode;
  float steps_per_rev;            
  float max_speed;                
  float accel;                    
  bool reversed;                  
  bool hold_when_idle;            
  uint32_t settle_ms;              
  hc595_stepper_done_cb_t on_done; 
  void *cb_ctx;
} hc595_stepper_config_t;

#define HC595_STEPPER_CONFIG_DEFAULT(_chain, _in1, _in2, _in3, _in4)           \
  {                                                                            \
      .chain = (_chain),                                                       \
      .pin = {(_in1), (_in2), (_in3), (_in4)},                                 \
      .mode = HC595_STEPPER_MODE_HALF,                                         \
      .steps_per_rev = 0.0f,                                                   \
      .max_speed = HC595_STEPPER_DEFAULT_MAX_SPEED,                            \
      .accel = HC595_STEPPER_DEFAULT_ACCEL,                                    \
      .reversed = false,                                                       \
      .hold_when_idle = false,                                                 \
      .settle_ms = HC595_STEPPER_DEFAULT_SETTLE_MS,                            \
      .on_done = NULL,                                                         \
      .cb_ctx = NULL,                                                          \
  }

struct hc595_stepper {
  hc595_stepper_config_t cfg;
  hc595_t *chain;
  uint16_t pin_mask; 
  uint8_t pin[4];
  esp_err_t fault; 
  const uint8_t *seq;
  uint8_t seq_len;
  float steps_per_rev;

  SemaphoreHandle_t lock;    
  EventGroupHandle_t events; 
  esp_timer_handle_t timer;

  int32_t position; 
  int32_t target;
  float vel_target; 
  float max_speed;
  float accel;
  float v;         
  int8_t dir;      
  uint8_t phase;   
  int64_t next_us; 
  hc595_stepper_run_mode_t run_mode;
  bool running;
  bool closing;
  bool initialized;
};

// --------- 
// Internas  
// --------- 

static inline const uint8_t *hc595_stepper_sequence_(hc595_stepper_mode_t mode,
                                                     uint8_t *len) {
  static const uint8_t wave[] = {0x1, 0x2, 0x4, 0x8};
  static const uint8_t full[] = {0x3, 0x6, 0xC, 0x9};
  static const uint8_t half[] = {0x1, 0x3, 0x2, 0x6, 0x4, 0xC, 0x8, 0x9};

  switch (mode) {
  case HC595_STEPPER_MODE_WAVE:
    *len = sizeof(wave);
    return wave;
  case HC595_STEPPER_MODE_FULL:
    *len = sizeof(full);
    return full;
  case HC595_STEPPER_MODE_HALF:
    *len = sizeof(half);
    return half;
  default:
    *len = 0;
    return NULL;
  }
}

static inline esp_err_t hc595_stepper_write_phase_(hc595_stepper_t *m,
                                                   uint8_t mask) {
  uint16_t val = 0;
  for (int i = 0; i < 4; i++) {
    if ((mask >> i) & 1U) {
      val |= (uint16_t)(1U << m->pin[i]);
    }
  }
  return hc595_write_claimed(m->chain, m->pin_mask, val);
}

static inline esp_err_t hc595_stepper_lock_(hc595_stepper_t *m) {
  if (xSemaphoreTake(m->lock, pdMS_TO_TICKS(HC595_STEPPER_LOCK_TIMEOUT_MS)) !=
      pdTRUE) {
    return ESP_ERR_TIMEOUT;
  }
  return ESP_OK;
}

static inline void hc595_stepper_unlock_(hc595_stepper_t *m) {
  xSemaphoreGive(m->lock);
}

static inline void hc595_stepper_finalize_locked_(hc595_stepper_t *m) {
  m->v = 0.0f;
  m->dir = 0;
  m->vel_target = 0.0f;
  m->target = m->position;
  m->run_mode = HC595_STEPPER_RUN_POSITION;
  m->running = false;
  if (!m->cfg.hold_when_idle) {
    (void)hc595_stepper_write_phase_(m, 0);
  }
  xEventGroupSetBits(m->events, HC595_STEPPER_EVT_IDLE);
}

static inline float hc595_stepper_decel_(float v, float a, float floor_v) {
  float v2 = v * v - 2.0f * a;
  float f2 = floor_v * floor_v;
  return sqrtf(v2 > f2 ? v2 : f2);
}
static inline bool hc595_stepper_advance_locked_(hc595_stepper_t *m,
                                                 int64_t now,
                                                 int64_t *wait_us) {
  const float a = m->accel;
  const bool ramp = a > 0.0f;
  const float v0 = ramp ? sqrtf(2.0f * a) : 0.0f; 

  int8_t want_dir;
  float want_speed;
  int32_t dist = 0;
  if (m->run_mode == HC595_STEPPER_RUN_POSITION) {
    int32_t rem = m->target - m->position;
    want_dir = (int8_t)((rem > 0) - (rem < 0));
    dist = rem < 0 ? -rem : rem;
    want_speed = (rem != 0) ? m->max_speed : 0.0f;
  } else {
    float vt = m->vel_target;
    want_dir = (int8_t)((vt > 0.0f) - (vt < 0.0f));
    want_speed = fabsf(vt);
    if (want_speed > m->max_speed) {
      want_speed = m->max_speed;
    }
  }

  float v = m->v;
  int8_t dir = m->dir;
  bool was_at_rest = (v <= 0.0f);

  if (was_at_rest) {
    if (want_speed <= 0.0f) {
      hc595_stepper_finalize_locked_(m);
      return false;
    }
    dir = want_dir;
    v = ramp ? fminf(v0, want_speed) : want_speed;
  } else if (!ramp) {
    if (want_speed <= 0.0f) {
      hc595_stepper_finalize_locked_(m);
      return false;
    }
    dir = want_dir;
    v = want_speed;
  } else {
    const bool can_stop = (v <= v0 * 1.001f);
    const bool wrong_dir = (want_dir != 0) && (want_dir != dir);

    if (wrong_dir) {
      if (can_stop) { 
        dir = want_dir;
        v = fminf(v0, want_speed);
      } else {
        v = hc595_stepper_decel_(v, a, v0);
      }
    } else if (want_speed <= 0.0f) {
      if (can_stop) { 
        hc595_stepper_finalize_locked_(m);
        return false;
      }
      v = hc595_stepper_decel_(v, a, v0);
    } else {
      float v_nb = v;
      if (v < want_speed) {
        v_nb = fminf(sqrtf(v * v + 2.0f * a), want_speed);
      } else if (v > want_speed) {
        float v2 = v * v - 2.0f * a;
        v_nb = fmaxf(sqrtf(v2 > 0.0f ? v2 : 0.0f), want_speed);
      }
      bool brake = false;
      if (m->run_mode == HC595_STEPPER_RUN_POSITION) {
        float need = (v_nb * v_nb - v0 * v0) / (2.0f * a);
        brake = ((float)(dist - 1) + 0.001f < need);
      }
      v = brake ? hc595_stepper_decel_(v, a, v0) : v_nb;
    }
  }
  if (v < 0.5f) {
    v = 0.5f;
  }

  int ph = (int)m->phase + (m->cfg.reversed ? -(int)dir : (int)dir);
  ph %= (int)m->seq_len;
  if (ph < 0) {
    ph += (int)m->seq_len;
  }
  esp_err_t werr = hc595_stepper_write_phase_(m, m->seq[ph]);
  if (werr != ESP_OK) {
    m->fault = werr;
    hc595_stepper_finalize_locked_(m);
    return false;
  }
  m->dir = dir;
  m->v = v;
  m->position += dir;
  m->phase = (uint8_t)ph;
  int64_t delay = (int64_t)(1000000.0f / v);

  bool will_stop = (m->run_mode == HC595_STEPPER_RUN_POSITION)
                       ? (m->position == m->target)
                       : (m->vel_target == 0.0f);
  bool can_stop_now = !ramp || (v <= v0 * 1.001f);
  if (will_stop && can_stop_now) {
    int64_t settle = (int64_t)m->cfg.settle_ms * 1000;
    if (delay < settle) {
      delay = settle; 
  }

  if (was_at_rest) {
    m->next_us = now;
  }
  m->next_us += delay;
  int64_t wait = m->next_us - now;
  if (wait < HC595_STEPPER_MIN_WAIT_US) { 
    wait = HC595_STEPPER_MIN_WAIT_US;
    m->next_us = now + wait;
  }
  *wait_us = wait;
  return true;
}

static inline void hc595_stepper_timer_cb_(void *arg) {
  hc595_stepper_t *m = (hc595_stepper_t *)arg;

  if (xSemaphoreTake(m->lock, portMAX_DELAY) != pdTRUE) {
    return;
  }
  if (m->closing || !m->running) {
    xSemaphoreGive(m->lock);
    return;
  }

  hc595_stepper_done_cb_t done_cb = NULL;
  int64_t wait_us = 0;

  if (hc595_stepper_advance_locked_(m, esp_timer_get_time(), &wait_us)) {
    esp_err_t err = esp_timer_start_once(m->timer, (uint64_t)wait_us);
    if (err == ESP_ERR_INVALID_STATE) { /
      (void)esp_timer_stop(m->timer);
      err = esp_timer_start_once(m->timer, (uint64_t)wait_us);
    }
    if (err != ESP_OK) { 
      hc595_stepper_finalize_locked_(m);
      done_cb = m->cfg.on_done;
    }
  } else {
    done_cb = m->cfg.on_done;
  }
  void *ctx = m->cfg.cb_ctx;
  xSemaphoreGive(m->lock);

  if (done_cb) {
    done_cb(m, ctx);
  }
}

static inline esp_err_t hc595_stepper_kick_locked_(hc595_stepper_t *m) {
  if (m->running) {
    return ESP_OK;
  }
  m->running = true;
  xEventGroupClearBits(m->events, HC595_STEPPER_EVT_IDLE);
  esp_err_t err = esp_timer_start_once(m->timer, HC595_STEPPER_START_DELAY_US);
  if (err != ESP_OK) {
    m->running = false;
    xEventGroupSetBits(m->events, HC595_STEPPER_EVT_IDLE);
  }
  return err;
}

static inline esp_err_t hc595_stepper_check_(const hc595_stepper_t *m) {
  return (m != NULL && m->initialized && !m->closing) ? ESP_OK
                                                      : ESP_ERR_INVALID_STATE;
}

static inline esp_err_t hc595_stepper_init(hc595_stepper_t *m,
                                           const hc595_stepper_config_t *cfg) {
  if (m == NULL || cfg == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if (m->initialized) {
    return ESP_ERR_INVALID_STATE;
  }

  if (cfg->chain == NULL || !cfg->chain->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  uint16_t pmask = 0;
  for (int i = 0; i < 4; i++) {
    if (cfg->pin[i] >= HC595_MAX_OUTPUTS ||
        !((cfg->chain->valid_mask >> cfg->pin[i]) & 1U) ||
        ((pmask >> cfg->pin[i]) & 1U)) {
      return ESP_ERR_INVALID_ARG; 
    }
    pmask |= (uint16_t)(1U << cfg->pin[i]);
  }
  if (cfg->max_speed <= 0.0f || cfg->max_speed > HC595_STEPPER_MAX_STEP_RATE ||
      cfg->accel < 0.0f || cfg->steps_per_rev < 0.0f) {
    return ESP_ERR_INVALID_ARG;
  }

  uint8_t len = 0;
  const uint8_t *seq = hc595_stepper_sequence_(cfg->mode, &len);
  if (seq == NULL) {
    return ESP_ERR_INVALID_ARG;
  }

  memset(m, 0, sizeof(*m));
  m->cfg = *cfg;
  m->chain = cfg->chain;
  m->pin_mask = pmask;
  memcpy(m->pin, cfg->pin, sizeof(m->pin));
  m->seq = seq;
  m->seq_len = len;
  m->steps_per_rev = (cfg->steps_per_rev > 0.0f)
                         ? cfg->steps_per_rev
                         : (cfg->mode == HC595_STEPPER_MODE_HALF
                                ? HC595_STEPPER_STEPS_PER_REV_HALF
                                : HC595_STEPPER_STEPS_PER_REV_FULL);
  m->max_speed = cfg->max_speed;
  m->accel = cfg->accel;
  m->run_mode = HC595_STEPPER_RUN_POSITION;

  m->lock = xSemaphoreCreateMutex();
  m->events = xEventGroupCreate();
  if (m->lock == NULL || m->events == NULL) {
    goto fail_nomem;
  }
  xEventGroupSetBits(m->events, HC595_STEPPER_EVT_IDLE);

  esp_err_t err = hc595_claim(m->chain, m->pin_mask);
  if (err != ESP_OK) {
    goto fail_res; 
  }
  if ((err = hc595_stepper_write_phase_(m, 0)) != ESP_OK) {
    (void)hc595_unclaim(m->chain, m->pin_mask);
    goto fail_res;
  }

  esp_timer_create_args_t targs = {
      .callback = hc595_stepper_timer_cb_,
      .arg = m,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "hc595_step",
  };
  err = esp_timer_create(&targs, &m->timer);
  if (err != ESP_OK) {
    (void)hc595_unclaim(m->chain, m->pin_mask);
    goto fail_res;
  }

  m->initialized = true;
  return ESP_OK;

fail_nomem:
  err = ESP_ERR_NO_MEM;
fail_res:
  if (m->lock)
    vSemaphoreDelete(m->lock);
  if (m->events)
    vEventGroupDelete(m->events);
  memset(m, 0, sizeof(*m));
  return err;
}
static inline esp_err_t hc595_stepper_deinit(hc595_stepper_t *m) {
  if (m == NULL || !m->initialized || m->closing) {
    return ESP_ERR_INVALID_STATE;
  }
  if (hc595_stepper_lock_(m) != ESP_OK) {
    return ESP_ERR_TIMEOUT;
  }
  m->closing = true;
  m->running = false;
  (void)esp_timer_stop(m->timer);
  (void)hc595_stepper_write_phase_(m, 0);
  hc595_stepper_unlock_(m);

  vTaskDelay(pdMS_TO_TICKS(HC595_STEPPER_DEINIT_GRACE_MS) + 1);

  esp_timer_delete(m->timer);
  (void)hc595_unclaim(m->chain,
                      m->pin_mask); 
  vSemaphoreDelete(m->lock);
  vEventGroupDelete(m->events);
  m->lock = NULL;
  m->events = NULL;
  m->timer = NULL;
  m->initialized = false;
  return ESP_OK;
}

static inline esp_err_t hc595_stepper_set_max_speed(hc595_stepper_t *m,
                                                    float steps_per_s) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if (steps_per_s <= 0.0f || steps_per_s > HC595_STEPPER_MAX_STEP_RATE) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  m->max_speed = steps_per_s;
  hc595_stepper_unlock_(m);
  return ESP_OK;
}

static inline esp_err_t hc595_stepper_set_acceleration(hc595_stepper_t *m,
                                                       float steps_per_s2) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if (steps_per_s2 < 0.0f) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  m->accel = steps_per_s2;
  hc595_stepper_unlock_(m);
  return ESP_OK;
}

static inline esp_err_t hc595_stepper_move_to(hc595_stepper_t *m,
                                              int32_t target_steps) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  m->run_mode = HC595_STEPPER_RUN_POSITION;
  m->vel_target = 0.0f;
  m->target = target_steps;
  if (m->running || target_steps != m->position) {
    err = hc595_stepper_kick_locked_(m);
  }
  hc595_stepper_unlock_(m);
  return err;
}

static inline esp_err_t hc595_stepper_move_by(hc595_stepper_t *m,
                                              int32_t steps) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  m->run_mode = HC595_STEPPER_RUN_POSITION;
  m->vel_target = 0.0f;
  m->target = m->position + steps;
  if (m->running || steps != 0) {
    err = hc595_stepper_kick_locked_(m);
  }
  hc595_stepper_unlock_(m);
  return err;
}

static inline esp_err_t hc595_stepper_run(hc595_stepper_t *m,
                                          float steps_per_s) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  m->run_mode = HC595_STEPPER_RUN_VELOCITY;
  m->vel_target = steps_per_s;
  if (m->running || steps_per_s != 0.0f) {
    err = hc595_stepper_kick_locked_(m);
  }
  hc595_stepper_unlock_(m);
  return err;
}

static inline esp_err_t hc595_stepper_stop(hc595_stepper_t *m) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  if (m->running) {
    if (m->run_mode == HC595_STEPPER_RUN_VELOCITY) {
      m->vel_target = 0.0f;
    } else if (m->v > 0.0f && m->accel > 0.0f) {
      int32_t stop_steps = (int32_t)ceilf((m->v * m->v) / (2.0f * m->accel));
      m->target = m->position + (int32_t)m->dir * stop_steps;
    } else {
      m->target = m->position;
    }
  }
  hc595_stepper_unlock_(m);
  return ESP_OK;
}

static inline esp_err_t hc595_stepper_stop_now(hc595_stepper_t *m,
                                               bool release_coils) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  (void)esp_timer_stop(m->timer);
  m->running = false;
  m->v = 0.0f;
  m->dir = 0;
  m->vel_target = 0.0f;
  m->target = m->position;
  m->run_mode = HC595_STEPPER_RUN_POSITION;
  esp_err_t werr = ESP_OK;
  if (release_coils) {
    werr = hc595_stepper_write_phase_(m, 0);
  }
  xEventGroupSetBits(m->events, HC595_STEPPER_EVT_IDLE);
  hc595_stepper_unlock_(m);
  return werr;
}

static inline esp_err_t hc595_stepper_wait_idle(hc595_stepper_t *m,
                                                uint32_t timeout_ms) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  TickType_t ticks =
      (timeout_ms == UINT32_MAX) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
  EventBits_t bits = xEventGroupWaitBits(m->events, HC595_STEPPER_EVT_IDLE,
                                         pdFALSE, pdTRUE, ticks);
  return (bits & HC595_STEPPER_EVT_IDLE) ? ESP_OK : ESP_ERR_TIMEOUT;
}

static inline esp_err_t hc595_stepper_move_to_blocking(hc595_stepper_t *m,
                                                       int32_t target_steps,
                                                       uint32_t timeout_ms) {
  esp_err_t err = hc595_stepper_move_to(m, target_steps);
  return (err == ESP_OK) ? hc595_stepper_wait_idle(m, timeout_ms) : err;
}

static inline esp_err_t hc595_stepper_move_by_blocking(hc595_stepper_t *m,
                                                       int32_t steps,
                                                       uint32_t timeout_ms) {
  esp_err_t err = hc595_stepper_move_by(m, steps);
  return (err == ESP_OK) ? hc595_stepper_wait_idle(m, timeout_ms) : err;
}

static inline bool hc595_stepper_is_moving(hc595_stepper_t *m) {
  if (hc595_stepper_check_(m) != ESP_OK || hc595_stepper_lock_(m) != ESP_OK) {
    return false;
  }
  bool r = m->running;
  hc595_stepper_unlock_(m);
  return r;
}

static inline esp_err_t hc595_stepper_get_position(hc595_stepper_t *m,
                                                   int32_t *steps) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if (steps == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  *steps = m->position;
  hc595_stepper_unlock_(m);
  return ESP_OK;
}

static inline esp_err_t hc595_stepper_distance_to_go(hc595_stepper_t *m,
                                                     int32_t *steps) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if (steps == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  *steps = (m->run_mode == HC595_STEPPER_RUN_POSITION)
               ? (m->target - m->position)
               : 0;
  hc595_stepper_unlock_(m);
  return ESP_OK;
}

static inline esp_err_t hc595_stepper_get_speed(hc595_stepper_t *m,
                                                float *steps_per_s) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if (steps_per_s == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  *steps_per_s = m->v * (float)m->dir;
  hc595_stepper_unlock_(m);
  return ESP_OK;
}

static inline esp_err_t hc595_stepper_set_position(hc595_stepper_t *m,
                                                   int32_t steps) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  if (m->running) {
    err = ESP_ERR_INVALID_STATE;
  } else {
    m->position = steps;
    m->target = steps;
  }
  hc595_stepper_unlock_(m);
  return err;
}

static inline esp_err_t hc595_stepper_release(hc595_stepper_t *m) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  if (m->running) {
    err = ESP_ERR_INVALID_STATE;
  } else {
    err = hc595_stepper_write_phase_(m, 0);
  }
  hc595_stepper_unlock_(m);
  return err;
}

static inline esp_err_t hc595_stepper_hold(hc595_stepper_t *m) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  if (m->running) {
    err = ESP_ERR_INVALID_STATE;
  } else {
    err = hc595_stepper_write_phase_(m, m->seq[m->phase]);
  }
  hc595_stepper_unlock_(m);
  return err;
}

static inline esp_err_t hc595_stepper_take_fault(hc595_stepper_t *m) {
  esp_err_t err = hc595_stepper_check_(m);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_stepper_lock_(m)) != ESP_OK) {
    return err;
  }
  esp_err_t f = m->fault;
  m->fault = ESP_OK;
  hc595_stepper_unlock_(m);
  return f;
}

static inline float hc595_stepper_steps_per_rev(const hc595_stepper_t *m) {
  return m ? m->steps_per_rev : 0.0f;
}

static inline int32_t hc595_stepper_deg_to_steps(const hc595_stepper_t *m,
                                                 float degrees) {
  return (int32_t)lroundf(degrees / 360.0f * m->steps_per_rev);
}

static inline float hc595_stepper_steps_to_deg(const hc595_stepper_t *m,
                                               int32_t steps) {
  return (float)steps * 360.0f / m->steps_per_rev;
}

static inline esp_err_t hc595_stepper_move_by_deg(hc595_stepper_t *m,
                                                  float degrees) {
  return hc595_stepper_check_(m) == ESP_OK
             ? hc595_stepper_move_by(m, hc595_stepper_deg_to_steps(m, degrees))
             : ESP_ERR_INVALID_STATE;
}

static inline esp_err_t hc595_stepper_move_to_deg(hc595_stepper_t *m,
                                                  float degrees) {
  return hc595_stepper_check_(m) == ESP_OK
             ? hc595_stepper_move_to(m, hc595_stepper_deg_to_steps(m, degrees))
             : ESP_ERR_INVALID_STATE;
}

static inline float hc595_stepper_rpm_to_steps_per_s(const hc595_stepper_t *m,
                                                     float rpm) {
  return rpm * m->steps_per_rev / 60.0f;
}

static inline esp_err_t hc595_stepper_get_position_deg(hc595_stepper_t *m,
                                                       float *degrees) {
  int32_t s;
  esp_err_t err = hc595_stepper_get_position(m, &s);
  if (err == ESP_OK && degrees) {
    *degrees = hc595_stepper_steps_to_deg(m, s);
  }
  return err;
}

#ifdef __cplusplus
}
#endif

#endif /* HC595_STEPPER_H */
