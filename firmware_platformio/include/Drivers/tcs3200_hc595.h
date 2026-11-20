#ifndef TCS3200_HC595_H
#define TCS3200_HC595_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_idf_version.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "hc595.h"
#include "soc/soc_caps.h"

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)) &&                       \
    defined(SOC_PCNT_SUPPORTED) && !defined(TCS3200_FORCE_NO_PCNT)
#define TCS3200_HAVE_PCNT 1
#include "driver/pulse_cnt.h"
#else
#define TCS3200_HAVE_PCNT 0
#endif

#define TCS3200_MAX_SENSORS 8

#ifndef TCS3200_FULL_SCALE_HZ
#define TCS3200_FULL_SCALE_HZ 500000.0f
#endif
#ifndef TCS3200_SAT_FRACTION
#define TCS3200_SAT_FRACTION 0.85f
#endif
#ifndef TCS3200_LOW_FRACTION
#define TCS3200_LOW_FRACTION 0.08f
#endif
#ifndef TCS3200_PCNT_MAX_HZ
#define TCS3200_PCNT_MAX_HZ 1000000U
#endif
#ifndef TCS3200_ISR_MAX_HZ
#define TCS3200_ISR_MAX_HZ 25000U
#endif
#ifndef TCS3200_DEFAULT_GATE_MS
#define TCS3200_DEFAULT_GATE_MS 50U
#endif
#ifndef TCS3200_DEFAULT_SETTLE_MS
#define TCS3200_DEFAULT_SETTLE_MS 3U
#endif
#ifndef TCS3200_DEFAULT_LOCK_TIMEOUT_MS
#define TCS3200_DEFAULT_LOCK_TIMEOUT_MS 3000U
#endif

#define TCS3200_CH_RED   (1U << 0)
#define TCS3200_CH_GREEN (1U << 1)
#define TCS3200_CH_BLUE  (1U << 2)
#define TCS3200_CH_CLEAR (1U << 3)
#define TCS3200_CH_ALL   0x0FU
#define TCS3200_CH_RGB   0x07U

typedef enum {
  TCS3200_FILTER_RED = 0,
  TCS3200_FILTER_GREEN,
  TCS3200_FILTER_BLUE,
  TCS3200_FILTER_CLEAR,
  TCS3200_FILTER_COUNT,
} tcs3200_filter_t;

typedef enum {
  TCS3200_SCALE_POWER_DOWN = 0,
  TCS3200_SCALE_2 = 1,
  TCS3200_SCALE_20 = 2,
  TCS3200_SCALE_100 = 3,
} tcs3200_scale_t;

typedef enum {
  TCS3200_COUNTER_AUTO = 0,
  TCS3200_COUNTER_PCNT,
  TCS3200_COUNTER_GPIO_ISR,
} tcs3200_counter_t;

typedef enum {
  TCS3200_COLOR_UNKNOWN = 0,
  TCS3200_COLOR_BLACK,
  TCS3200_COLOR_WHITE,
  TCS3200_COLOR_GRAY,
  TCS3200_COLOR_RED,
  TCS3200_COLOR_ORANGE,
  TCS3200_COLOR_YELLOW,
  TCS3200_COLOR_GREEN,
  TCS3200_COLOR_CYAN,
  TCS3200_COLOR_BLUE,
  TCS3200_COLOR_PURPLE,
  TCS3200_COLOR_MAGENTA,
} tcs3200_color_t;

typedef struct {
  hc595_t *chain;        /**< Instancia inicializada del 74HC595 */
  uint8_t pin_s0;        /**< Salida 0..15 del 74HC595 asignada a S0 */
  uint8_t pin_s1;        /**< Salida 0..15 del 74HC595 asignada a S1 */
  uint8_t pin_s2;        /**< Salida 0..15 del 74HC595 asignada a S2 */
  uint8_t pin_s3;        /**< Salida 0..15 del 74HC595 asignada a S3 */
  gpio_num_t out;        /**< Pin de lectura directo del ESP32 */
  tcs3200_counter_t counter;
  uint32_t gate_ms;
  uint32_t settle_ms;
  uint32_t glitch_ns;
  tcs3200_scale_t scale;
  tcs3200_scale_t max_scale;
  uint32_t counter_max_hz;
  bool auto_scale;
  uint8_t channels;
  uint32_t lock_timeout_ms;
} tcs3200_config_t;

#define TCS3200_CONFIG_DEFAULT(_chain, _s0, _s1, _s2, _s3, _out)              \
  {                                                                            \
      .chain = (_chain),                                                       \
      .pin_s0 = (_s0),                                                         \
      .pin_s1 = (_s1),                                                         \
      .pin_s2 = (_s2),                                                         \
      .pin_s3 = (_s3),                                                         \
      .out = (_out),                                                           \
      .counter = TCS3200_COUNTER_AUTO,                                         \
      .gate_ms = TCS3200_DEFAULT_GATE_MS,                                      \
      .settle_ms = TCS3200_DEFAULT_SETTLE_MS,                                  \
      .glitch_ns = 0,                                                          \
      .scale = TCS3200_SCALE_100,                                              \
      .max_scale = TCS3200_SCALE_POWER_DOWN,                                   \
      .counter_max_hz = 0,                                                     \
      .auto_scale = true,                                                      \
      .channels = TCS3200_CH_ALL,                                              \
      .lock_timeout_ms = 0,                                                    \
  }

typedef struct {
  float dark_hz[4];
  float white_hz[4];
  bool has_dark;
  bool has_white;
} tcs3200_cal_t;

typedef struct {
  float hz[4];
  float hz100[4];
  uint32_t pulses[4];
  float norm[4];
  uint8_t r, g, b;
  float h, s, v;
  tcs3200_color_t color;
  tcs3200_scale_t scale;
  uint8_t measured;
  bool calibrated;
  bool saturated;
  bool low_light;
  bool no_signal;
  bool valid;
  esp_err_t err;
  int64_t time_us;
} tcs3200_sample_t;

typedef struct {
  tcs3200_config_t cfg;
  hc595_t *chain;
  uint16_t pin_mask;
  SemaphoreHandle_t lock;
  portMUX_TYPE mux;
  tcs3200_counter_t kind;
  tcs3200_scale_t scale;
  tcs3200_scale_t max_scale;
  bool powered;
  int8_t cur_filter;
  bool settle_pending;
  tcs3200_cal_t cal;
  tcs3200_sample_t last;
#if TCS3200_HAVE_PCNT
  pcnt_unit_handle_t pcnt_unit;
  pcnt_channel_handle_t pcnt_chan;
#endif
  volatile uint32_t isr_count;
  bool isr_added;
  bool initialized;
} tcs3200_t;

/* ------------------------------------------------------------------------- */
/* Funciones Auxiliares                                                      */
/* ------------------------------------------------------------------------- */

static inline float tcs3200_scale_factor(tcs3200_scale_t s) {
  switch (s) {
  case TCS3200_SCALE_100: return 1.0f;
  case TCS3200_SCALE_20:  return 0.2f;
  case TCS3200_SCALE_2:   return 0.02f;
  default:                return 0.0f;
  }
}

static inline void tcs3200_rgb_to_hsv(float r, float g, float b, float *h, float *s, float *v) {
  float mx = fmaxf(r, fmaxf(g, b)), mn = fminf(r, fminf(g, b));
  float d = mx - mn, hh = 0.0f;
  if (d > 1e-6f) {
    if (mx == r)      hh = 60.0f * fmodf((g - b) / d, 6.0f);
    else if (mx == g) hh = 60.0f * (((b - r) / d) + 2.0f);
    else              hh = 60.0f * (((r - g) / d) + 4.0f);
    if (hh < 0.0f) hh += 360.0f;
  }
  if (h) *h = hh;
  if (s) *s = (mx > 1e-6f) ? d / mx : 0.0f;
  if (v) *v = mx;
}

static inline tcs3200_color_t tcs3200_classify(float h, float s, float v) {
  if (v < 0.15f) return TCS3200_COLOR_BLACK;
  if (s < 0.18f) return (v > 0.80f) ? TCS3200_COLOR_WHITE : TCS3200_COLOR_GRAY;
  if (h < 15.0f || h >= 345.0f) return TCS3200_COLOR_RED;
  if (h < 40.0f)  return TCS3200_COLOR_ORANGE;
  if (h < 70.0f)  return TCS3200_COLOR_YELLOW;
  if (h < 165.0f) return TCS3200_COLOR_GREEN;
  if (h < 200.0f) return TCS3200_COLOR_CYAN;
  if (h < 255.0f) return TCS3200_COLOR_BLUE;
  if (h < 290.0f) return TCS3200_COLOR_PURPLE;
  return TCS3200_COLOR_MAGENTA;
}

/* ------------------------------------------------------------------------- */
/* Control de Escritura vía HC595                                            */
/* ------------------------------------------------------------------------- */

static inline esp_err_t tcs3200_write_pins_locked_(tcs3200_t *t, tcs3200_scale_t s, tcs3200_filter_t f) {
  static const uint8_t s2s3[4][2] = {
      [TCS3200_FILTER_RED]   = {0, 0},
      [TCS3200_FILTER_GREEN] = {1, 1},
      [TCS3200_FILTER_BLUE]  = {0, 1},
      [TCS3200_FILTER_CLEAR] = {1, 0},
  };

  uint8_t s0_val = ((unsigned)s >> 0) & 1U;
  uint8_t s1_val = ((unsigned)s >> 1) & 1U;
  uint8_t s2_val = s2s3[f][0];
  uint8_t s3_val = s2s3[f][1];

  uint16_t val = 0;
  if (s0_val) val |= (1U << t->cfg.pin_s0);
  if (s1_val) val |= (1U << t->cfg.pin_s1);
  if (s2_val) val |= (1U << t->cfg.pin_s2);
  if (s3_val) val |= (1U << t->cfg.pin_s3);

  return hc595_write_claimed(t->chain, t->pin_mask, val);
}

/* ------------------------------------------------------------------------- */
/* Conteo e Interrupciones                                                   */
/* ------------------------------------------------------------------------- */

__attribute__((unused)) static void IRAM_ATTR tcs3200_isr_(void *arg) {
  ((tcs3200_t *)arg)->isr_count++;
}

static inline esp_err_t tcs3200_counter_init_(tcs3200_t *t) {
  if (t->kind == TCS3200_COUNTER_GPIO_ISR) {
    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return err;
    gpio_set_intr_type(t->cfg.out, GPIO_INTR_POSEDGE);
    if ((err = gpio_isr_handler_add(t->cfg.out, tcs3200_isr_, t)) != ESP_OK) return err;
    t->isr_added = true;
    gpio_intr_disable(t->cfg.out);
    return ESP_OK;
  }
#if TCS3200_HAVE_PCNT
  pcnt_unit_config_t uc = {
      .low_limit = -30000,
      .high_limit = 30000,
      .flags.accum_count = 1,
  };
  esp_err_t err = pcnt_new_unit(&uc, &t->pcnt_unit);
  if (err != ESP_OK) return err;

  pcnt_glitch_filter_config_t fc = {
      .max_glitch_ns = t->cfg.glitch_ns ? t->cfg.glitch_ns : 100U,
  };
  if ((err = pcnt_unit_set_glitch_filter(t->pcnt_unit, &fc)) != ESP_OK) goto fail;

  pcnt_chan_config_t cc = {
      .edge_gpio_num = t->cfg.out,
      .level_gpio_num = -1,
  };
  if ((err = pcnt_new_channel(t->pcnt_unit, &cc, &t->pcnt_chan)) != ESP_OK) goto fail;

  if ((err = pcnt_channel_set_edge_action(t->pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD)) != ESP_OK ||
      (err = pcnt_unit_enable(t->pcnt_unit)) != ESP_OK) goto fail;

  if ((err = pcnt_unit_clear_count(t->pcnt_unit)) != ESP_OK ||
      (err = pcnt_unit_start(t->pcnt_unit)) != ESP_OK) {
    (void)pcnt_unit_disable(t->pcnt_unit);
    goto fail;
  }
  return ESP_OK;

fail:
  if (t->pcnt_chan) { pcnt_del_channel(t->pcnt_chan); t->pcnt_chan = NULL; }
  pcnt_del_unit(t->pcnt_unit);
  t->pcnt_unit = NULL;
  return err;
#else
  return ESP_ERR_NOT_SUPPORTED;
#endif
}

static inline void tcs3200_counter_deinit_(tcs3200_t *t) {
  if (t->isr_added) {
    gpio_intr_disable(t->cfg.out);
    gpio_isr_handler_remove(t->cfg.out);
    t->isr_added = false;
  }
#if TCS3200_HAVE_PCNT
  if (t->pcnt_unit) {
    (void)pcnt_unit_stop(t->pcnt_unit);
    (void)pcnt_unit_disable(t->pcnt_unit);
    if (t->pcnt_chan) { pcnt_del_channel(t->pcnt_chan); t->pcnt_chan = NULL; }
    pcnt_del_unit(t->pcnt_unit);
    t->pcnt_unit = NULL;
  }
#endif
}

static inline esp_err_t tcs3200_counter_clear_(tcs3200_t *t) {
  if (t->kind == TCS3200_COUNTER_GPIO_ISR) {
    t->isr_count = 0;
    return ESP_OK;
  }
#if TCS3200_HAVE_PCNT
  return pcnt_unit_clear_count(t->pcnt_unit);
#else
  return ESP_ERR_NOT_SUPPORTED;
#endif
}

static inline void tcs3200_counter_gate_(tcs3200_t *t, bool open) {
  if (t->kind == TCS3200_COUNTER_GPIO_ISR) {
    if (open) gpio_intr_enable(t->cfg.out);
    else      gpio_intr_disable(t->cfg.out);
  }
}

static inline esp_err_t tcs3200_snapshot_(tcs3200_t *t, uint32_t *count, int64_t *time_us) {
  esp_err_t err = ESP_OK;
  portENTER_CRITICAL(&t->mux);
  *time_us = esp_timer_get_time();
  if (t->kind == TCS3200_COUNTER_GPIO_ISR) {
    *count = t->isr_count;
  } else {
#if TCS3200_HAVE_PCNT
    int c = 0;
    err = pcnt_unit_get_count(t->pcnt_unit, &c);
    *count = (uint32_t)c;
#else
    err = ESP_ERR_NOT_SUPPORTED;
#endif
  }
  portEXIT_CRITICAL(&t->mux);
  return err;
}

static inline void tcs3200_sleep_ms_(uint32_t ms) {
  if (ms == 0) return;
  TickType_t ticks = pdMS_TO_TICKS(ms);
  vTaskDelay(ticks ? ticks : 1);
}

/* ------------------------------------------------------------------------- */
/* Ciclo de Vida y Medición                                                  */
/* ------------------------------------------------------------------------- */

static inline esp_err_t tcs3200_lock_(tcs3200_t *t) {
  return (xSemaphoreTake(t->lock, pdMS_TO_TICKS(t->cfg.lock_timeout_ms)) == pdTRUE)
             ? ESP_OK
             : ESP_ERR_NOT_FINISHED;
}

static inline void tcs3200_unlock_(tcs3200_t *t) { xSemaphoreGive(t->lock); }

static inline esp_err_t tcs3200_measure_locked_(tcs3200_t *t, tcs3200_filter_t f, float *hz, uint32_t *pulses) {
  esp_err_t err = tcs3200_write_pins_locked_(t, t->scale, f);
  if (err != ESP_OK) return err;

  if ((err = tcs3200_counter_clear_(t)) != ESP_OK) return err;
  tcs3200_sleep_ms_(t->cfg.settle_ms);

  uint32_t c1 = 0, c2 = 0;
  int64_t t1 = 0, t2 = 0;
  tcs3200_counter_gate_(t, true);
  err = tcs3200_snapshot_(t, &c1, &t1);
  if (err == ESP_OK) {
    tcs3200_sleep_ms_(t->cfg.gate_ms);
    err = tcs3200_snapshot_(t, &c2, &t2);
  }
  tcs3200_counter_gate_(t, false);

  if (err != ESP_OK) return err;
  if (t2 <= t1) return ESP_ERR_INVALID_RESPONSE;

  uint32_t n = c2 - c1;
  *pulses = n;
  *hz = (float)((double)n * 1e6 / (double)(t2 - t1));
  return ESP_OK;
}

static inline esp_err_t tcs3200_init(tcs3200_t *t, const tcs3200_config_t *cfg) {
  if (t == NULL || cfg == NULL || cfg->chain == NULL) return ESP_ERR_INVALID_ARG;
  if (t->initialized) return ESP_ERR_INVALID_STATE;
  if (!GPIO_IS_VALID_GPIO(cfg->out)) return ESP_ERR_INVALID_ARG;

  /* Reservar salidas en el 74HC595 */
  uint16_t pmask = (1U << cfg->pin_s0) | (1U << cfg->pin_s1) | (1U << cfg->pin_s2) | (1U << cfg->pin_s3);
  esp_err_t err = hc595_claim(cfg->chain, pmask);
  if (err != ESP_OK) return err;

  memset(t, 0, sizeof(*t));
  t->cfg = *cfg;
  t->chain = cfg->chain;
  t->pin_mask = pmask;
  if (t->cfg.gate_ms == 0) t->cfg.gate_ms = TCS3200_DEFAULT_GATE_MS;
  if (t->cfg.lock_timeout_ms == 0) t->cfg.lock_timeout_ms = TCS3200_DEFAULT_LOCK_TIMEOUT_MS;

  t->kind = cfg->counter;
  if (t->kind == TCS3200_COUNTER_AUTO) {
    t->kind = TCS3200_HAVE_PCNT ? TCS3200_COUNTER_PCNT : TCS3200_COUNTER_GPIO_ISR;
  }

  t->scale = cfg->scale;
  t->max_scale = (cfg->max_scale == TCS3200_SCALE_POWER_DOWN) ? TCS3200_SCALE_100 : cfg->max_scale;

  portMUX_INITIALIZE(&t->mux);
  t->lock = xSemaphoreCreateMutex();
  if (t->lock == NULL) {
    hc595_unclaim(t->chain, t->pin_mask);
    return ESP_ERR_NO_MEM;
  }

  /* Pin de lectura GPIO */
  gpio_config_t in = {
      .pin_bit_mask = 1ULL << cfg->out,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  gpio_config(&in);

  if ((err = tcs3200_counter_init_(t)) != ESP_OK) {
    vSemaphoreDelete(t->lock);
    hc595_unclaim(t->chain, t->pin_mask);
    return err;
  }

  /* Estado inicial seguro */
  tcs3200_write_pins_locked_(t, t->scale, TCS3200_FILTER_RED);
  t->powered = true;
  t->initialized = true;
  return ESP_OK;
}

static inline esp_err_t tcs3200_deinit(tcs3200_t *t) {
  if (t == NULL || !t->initialized) return ESP_ERR_INVALID_STATE;
  if (tcs3200_lock_(t) != ESP_OK) return ESP_ERR_TIMEOUT;

  /* Power down vía S0/S1 = LOW */
  tcs3200_write_pins_locked_(t, TCS3200_SCALE_POWER_DOWN, TCS3200_FILTER_RED);
  tcs3200_counter_deinit_(t);
  
  hc595_unclaim(t->chain, t->pin_mask);
  t->initialized = false;
  tcs3200_unlock_(t);

  vSemaphoreDelete(t->lock);
  return ESP_OK;
}

static inline esp_err_t tcs3200_read(tcs3200_t *t, tcs3200_sample_t *out) {
  if (t == NULL || !t->initialized) return ESP_ERR_INVALID_STATE;
  esp_err_t err = tcs3200_lock_(t);
  if (err != ESP_OK) return err;

  tcs3200_sample_t smp;
  memset(&smp, 0, sizeof(smp));

  for (int c = 0; c < 4; c++) {
    err = tcs3200_measure_locked_(t, (tcs3200_filter_t)c, &smp.hz[c], &smp.pulses[c]);
    if (err != ESP_OK) {
      tcs3200_unlock_(t);
      return err;
    }
  }

  smp.scale = t->scale;
  const float factor = tcs3200_scale_factor(t->scale);
  float peak100 = 0.0f;

  for (int c = 0; c < 4; c++) {
    smp.hz100[c] = (factor > 0.0f) ? smp.hz[c] / factor : 0.0f;
    if (smp.hz100[c] > peak100) peak100 = smp.hz100[c];
  }

  float m = fmaxf(smp.hz100[0], fmaxf(smp.hz100[1], smp.hz100[2]));
  for (int c = 0; c < 3; c++) {
    smp.norm[c] = (m > 1e-3f) ? smp.hz100[c] / m : 0.0f;
  }
  smp.r = (uint8_t)lroundf(smp.norm[0] * 255.0f);
  smp.g = (uint8_t)lroundf(smp.norm[1] * 255.0f);
  smp.b = (uint8_t)lroundf(smp.norm[2] * 255.0f);

  tcs3200_rgb_to_hsv(smp.norm[0], smp.norm[1], smp.norm[2], &smp.h, &smp.s, &smp.v);
  smp.color = tcs3200_classify(smp.h, smp.s, smp.v);
  smp.valid = true;
  smp.time_us = esp_timer_get_time();

  t->last = smp;
  if (out) *out = smp;

  tcs3200_unlock_(t);
  return ESP_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* TCS3200_HC595_H */