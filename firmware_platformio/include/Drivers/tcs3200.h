#ifndef TCS3200_H
#define TCS3200_H

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
#include "soc/soc_caps.h"

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)) &&                       \
    defined(SOC_PCNT_SUPPORTED) && !defined(TCS3200_FORCE_NO_PCNT)
#define TCS3200_HAVE_PCNT 1
#include "driver/pulse_cnt.h"
#else
#define TCS3200_HAVE_PCNT 0
#endif
#define TCS3200_MAX_SENSORS 8

/** Frecuencia maxima aproximada de OUT a escala 100 % (Hz). */
#ifndef TCS3200_FULL_SCALE_HZ
#define TCS3200_FULL_SCALE_HZ 500000.0f
#endif
/** Sensor saturado: frecuencia (normalizada a 100 %) por encima de esta
 * fraccion del fondo de escala. */
#ifndef TCS3200_SAT_FRACTION
#define TCS3200_SAT_FRACTION 0.85f
#endif
/** Poca luz: por debajo de esta fraccion hay pocas cuentas por ventana (menos
 * resolucion). */
#ifndef TCS3200_LOW_FRACTION
#define TCS3200_LOW_FRACTION 0.08f
#endif
/** Frecuencia maxima que aguanta cada contador (Hz) para la autoescala. */
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

/** Canales (mascara). El orden de los arrays hz[]/norm[] es R, G, B, C. */
#define TCS3200_CH_RED (1U << 0)
#define TCS3200_CH_GREEN (1U << 1)
#define TCS3200_CH_BLUE (1U << 2)
#define TCS3200_CH_CLEAR (1U << 3)
#define TCS3200_CH_ALL 0x0FU
#define TCS3200_CH_RGB 0x07U

/* ------------------------------------------------------------------------- */
/* Tipos                                                                     */
/* ------------------------------------------------------------------------- */

typedef enum {
  TCS3200_FILTER_RED = 0,
  TCS3200_FILTER_GREEN,
  TCS3200_FILTER_BLUE,
  TCS3200_FILTER_CLEAR,
  TCS3200_FILTER_COUNT,
} tcs3200_filter_t;

/** Valor = codigo S1S0 (S0 = bit 0, S1 = bit 1). */
typedef enum {
  TCS3200_SCALE_POWER_DOWN = 0,
  TCS3200_SCALE_2 = 1,
  TCS3200_SCALE_20 = 2,
  TCS3200_SCALE_100 = 3,
} tcs3200_scale_t;

typedef enum {
  TCS3200_COUNTER_AUTO = 0, /**< PCNT si existe; si no, interrupcion */
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
  gpio_num_t s0, s1; /**< Escala. GPIO_NUM_NC si estan cableados: escala FIJA
                        (ahorra 2 pines) */
  gpio_num_t s2, s3; /**< Filtro de color                                  */
  gpio_num_t out;    /**< Salida de frecuencia (entrada del ESP32)         */
  gpio_num_t oe[TCS3200_MAX_SENSORS]; /**< OE de cada sensor. NC = a GND (solo
                                         con 1 sensor) */
  uint8_t sensors; /**< Numero de sensores del banco (1..8)              */
  gpio_num_t led;  /**< Pin que enciende el LED blanco, o GPIO_NUM_NC    */
  bool led_active_high;
  tcs3200_counter_t counter;
  uint32_t gate_ms;   /**< Ventana de cuenta por canal (0 = 50 ms)          */
  uint32_t settle_ms; /**< Espera tras cambiar filtro/escala/sensor         */
  uint32_t glitch_ns; /**< Filtro de glitches del PCNT (0 = 100 ns)         */
  tcs3200_scale_t scale; /**< Escala inicial (si S0/S1 son NC: la cableada) */
  tcs3200_scale_t max_scale; /**< Tope de la autoescala (POWER_DOWN = 100 %) */
  uint32_t counter_max_hz; /**< Frecuencia maxima que aguanta el contador (0 = 1
                              MHz con PCNT, 25 kHz con interrupcion) */
  bool auto_scale;
  uint8_t channels; /**< Mascara de canales que mide tcs3200_read()       */
  uint32_t lock_timeout_ms; /**< 0 = 3000 ms */
} tcs3200_config_t;

/** Un sensor (OE = _oe, o GPIO_NUM_NC si va a GND). */
#define TCS3200_CONFIG_DEFAULT(_s0, _s1, _s2, _s3, _out, _oe)                  \
  {                                                                            \
      .s0 = (_s0),                                                             \
      .s1 = (_s1),                                                             \
      .s2 = (_s2),                                                             \
      .s3 = (_s3),                                                             \
      .out = (_out),                                                           \
      .oe = {(_oe), GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC,        \
             GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC},                           \
      .sensors = 1,                                                            \
      .led = GPIO_NUM_NC,                                                      \
      .led_active_high = true,                                                 \
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

/** Calibracion de un sensor, en frecuencias normalizadas a escala 100 %. */
typedef struct {
  float dark_hz[4];  /**< Con el sensor tapado (R, G, B, C)    */
  float white_hz[4]; /**< Frente a una referencia blanca       */
  bool has_dark;
  bool has_white;
} tcs3200_cal_t;

typedef struct {
  uint8_t sensor;
  float hz[4];    /**< Frecuencia medida a la escala usada (R, G, B, C)    */
  float hz100[4]; /**< Normalizada a escala 100 % (hz / factor de escala)  */
  uint32_t pulses[4]; /**< Pulsos contados en la ventana */
  float
      norm[4]; /**< 0..1 (calibrado) o relativa al canal mayor (sin calibrar) */
  uint8_t r, g, b; /**< 0..255 (de norm)                                    */
  float h, s, v;   /**< HSV: h en grados 0..360, s y v en 0..1              */
  tcs3200_color_t color;
  tcs3200_scale_t scale; /**< Escala con la que se midio */
  uint8_t measured; /**< Mascara de canales medidos                          */
  bool calibrated;  /**< Hay calibracion de blanco aplicada                  */
  bool saturated;   /**< Cerca del fondo de escala incluso a la escala minima */
  bool low_light;   /**< Muy poca luz incluso a la escala maxima             */
  bool no_signal;   /**< Ni un solo pulso: sensor desconectado o apagado     */
  bool valid;       /**< Medicion correcta                                   */
  esp_err_t err;
  int64_t time_us;
} tcs3200_sample_t;

typedef void (*tcs3200_sample_cb_t)(uint8_t sensor, const tcs3200_sample_t *s,
                                    void *ctx);

typedef struct {
  uint32_t period_ms; /**< Pausa entre barridos completos del banco */
  UBaseType_t priority;
  uint32_t stack_size; /**< Bytes                                    */
  BaseType_t core_id;  /**< <0: sin afinidad                         */
  bool power_save;     /**< Apagar el sensor entre barridos          */
  tcs3200_sample_cb_t on_sample; /**< Opcional (se ejecuta en la tarea) */
  void *cb_ctx;
} tcs3200_poll_config_t;

#define TCS3200_POLL_CONFIG_DEFAULT()                                          \
  {                                                                            \
      .period_ms = 500,                                                        \
      .priority = 4,                                                           \
      .stack_size = 3072,                                                      \
      .core_id = -1,                                                           \
      .power_save = false,                                                     \
      .on_sample = NULL,                                                       \
      .cb_ctx = NULL,                                                          \
  }

typedef struct {
  tcs3200_config_t cfg;
  SemaphoreHandle_t lock;
  portMUX_TYPE mux; /**< Protege las muestras en cache y la lectura (contador,
                       instante) */
  tcs3200_counter_t kind; /**< PCNT o interrupcion (ya resuelto) */
  tcs3200_scale_t scale;
  tcs3200_scale_t max_scale;
  bool fixed_scale; /**< S0/S1 cableados */
  bool powered;
  int8_t sel_sensor; /**< Sensor con OE bajo (-1 ninguno) */
  int8_t cur_filter;
  bool settle_pending;
  bool led_on;
  tcs3200_cal_t cal[TCS3200_MAX_SENSORS];
  tcs3200_sample_t last[TCS3200_MAX_SENSORS];
#if TCS3200_HAVE_PCNT
  pcnt_unit_handle_t pcnt_unit;
  pcnt_channel_handle_t pcnt_chan;
#endif
  volatile uint32_t isr_count;
  bool isr_added;
  /* tarea de medida periodica */
  tcs3200_poll_config_t poll;
  TaskHandle_t task;
  SemaphoreHandle_t exited;
  volatile bool stop;
  bool polling;
  bool initialized;
} tcs3200_t;

/* ------------------------------------------------------------------------- */
/* Utilidades puras (sin hardware)                                           */
/* ------------------------------------------------------------------------- */

/** Factor nominal de escala de la frecuencia de salida. */
static inline float tcs3200_scale_factor(tcs3200_scale_t s) {
  switch (s) {
  case TCS3200_SCALE_100:
    return 1.0f;
  case TCS3200_SCALE_20:
    return 0.2f;
  case TCS3200_SCALE_2:
    return 0.02f;
  default:
    return 0.0f;
  }
}

static inline void tcs3200_rgb_to_hsv(float r, float g, float b, float *h,
                                      float *s, float *v) {
  float mx = fmaxf(r, fmaxf(g, b)), mn = fminf(r, fminf(g, b));
  float d = mx - mn, hh = 0.0f;
  if (d > 1e-6f) {
    if (mx == r)
      hh = 60.0f * fmodf((g - b) / d, 6.0f);
    else if (mx == g)
      hh = 60.0f * (((b - r) / d) + 2.0f);
    else
      hh = 60.0f * (((r - g) / d) + 4.0f);
    if (hh < 0.0f)
      hh += 360.0f;
  }
  if (h)
    *h = hh;
  if (s)
    *s = (mx > 1e-6f) ? d / mx : 0.0f;
  if (v)
    *v = mx;
}

/** Clasificacion por HSV (umbrales orientativos: calibra blanco y negro
 * primero). */
static inline tcs3200_color_t tcs3200_classify(float h, float s, float v) {
  if (v < 0.15f) {
    return TCS3200_COLOR_BLACK;
  }
  if (s < 0.18f) {
    return (v > 0.80f) ? TCS3200_COLOR_WHITE : TCS3200_COLOR_GRAY;
  }
  if (h < 15.0f || h >= 345.0f)
    return TCS3200_COLOR_RED;
  if (h < 40.0f)
    return TCS3200_COLOR_ORANGE;
  if (h < 70.0f)
    return TCS3200_COLOR_YELLOW;
  if (h < 165.0f)
    return TCS3200_COLOR_GREEN;
  if (h < 200.0f)
    return TCS3200_COLOR_CYAN;
  if (h < 255.0f)
    return TCS3200_COLOR_BLUE;
  if (h < 290.0f)
    return TCS3200_COLOR_PURPLE;
  return TCS3200_COLOR_MAGENTA;
}

static inline const char *tcs3200_color_name(tcs3200_color_t c) {
  switch (c) {
  case TCS3200_COLOR_BLACK:
    return "negro";
  case TCS3200_COLOR_WHITE:
    return "blanco";
  case TCS3200_COLOR_GRAY:
    return "gris";
  case TCS3200_COLOR_RED:
    return "rojo";
  case TCS3200_COLOR_ORANGE:
    return "naranja";
  case TCS3200_COLOR_YELLOW:
    return "amarillo";
  case TCS3200_COLOR_GREEN:
    return "verde";
  case TCS3200_COLOR_CYAN:
    return "cian";
  case TCS3200_COLOR_BLUE:
    return "azul";
  case TCS3200_COLOR_PURPLE:
    return "violeta";
  case TCS3200_COLOR_MAGENTA:
    return "magenta";
  default:
    return "desconocido";
  }
}

/* ------------------------------------------------------------------------- */
/* Internas: contador de pulsos                                              */
/* ------------------------------------------------------------------------- */

__attribute__((unused)) static void IRAM_ATTR tcs3200_isr_(void *arg) {
  ((tcs3200_t *)arg)->isr_count++;
}

static inline esp_err_t tcs3200_counter_init_(tcs3200_t *t) {
  if (t->kind == TCS3200_COUNTER_GPIO_ISR) {
    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
      return err;
    }
    gpio_set_intr_type(t->cfg.out, GPIO_INTR_POSEDGE);
    if ((err = gpio_isr_handler_add(t->cfg.out, tcs3200_isr_, t)) != ESP_OK) {
      return err;
    }
    t->isr_added = true;
    gpio_intr_disable(t->cfg.out); /* solo se habilita durante la ventana */
    return ESP_OK;
  }
#if TCS3200_HAVE_PCNT
  pcnt_unit_config_t uc = {
      .low_limit = -30000,
      .high_limit = 30000,
      .flags.accum_count = 1, /* acumula: supera el limite de 16 bits */
  };
  esp_err_t err = pcnt_new_unit(&uc, &t->pcnt_unit);
  if (err != ESP_OK) {
    return err;
  }
  pcnt_glitch_filter_config_t fc = {
      .max_glitch_ns = t->cfg.glitch_ns ? t->cfg.glitch_ns : 100U,
  };
  if ((err = pcnt_unit_set_glitch_filter(t->pcnt_unit, &fc)) != ESP_OK) {
    goto fail;
  }
  pcnt_chan_config_t cc = {
      .edge_gpio_num = t->cfg.out,
      .level_gpio_num = -1,
  };
  if ((err = pcnt_new_channel(t->pcnt_unit, &cc, &t->pcnt_chan)) != ESP_OK) {
    goto fail;
  }
  /* Flanco de subida: +1. Flanco de bajada: nada. */
  if ((err = pcnt_channel_set_edge_action(
           t->pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE,
           PCNT_CHANNEL_EDGE_ACTION_HOLD)) != ESP_OK ||
      (err = pcnt_unit_enable(t->pcnt_unit)) != ESP_OK) {
    goto fail;
  }
  if ((err = pcnt_unit_clear_count(t->pcnt_unit)) != ESP_OK ||
      (err = pcnt_unit_start(t->pcnt_unit)) != ESP_OK) {
    (void)pcnt_unit_disable(t->pcnt_unit);
    goto fail;
  }
  return ESP_OK;
fail:
  if (t->pcnt_chan) {
    pcnt_del_channel(t->pcnt_chan);
    t->pcnt_chan = NULL;
  }
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
    if (t->pcnt_chan) {
      pcnt_del_channel(t->pcnt_chan);
      t->pcnt_chan = NULL;
    }
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
    if (open) {
      gpio_intr_enable(t->cfg.out);
    } else {
      gpio_intr_disable(t->cfg.out);
    }
  }
}

/**
 * Lee el contador y el instante de forma ATOMICA (seccion critica de unos us):
 * si una tarea de mayor prioridad interrumpiera entre ambas lecturas, la
 * frecuencia saldria mal. Primero el instante y despues el contador.
 */
static inline esp_err_t tcs3200_snapshot_(tcs3200_t *t, uint32_t *count,
                                          int64_t *time_us) {
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
  if (ms == 0) {
    return;
  }
  TickType_t ticks = pdMS_TO_TICKS(ms);
  vTaskDelay(ticks ? ticks : 1);
}

/* ------------------------------------------------------------------------- */
/* Internas: pines                                                           */
/* ------------------------------------------------------------------------- */

static inline esp_err_t tcs3200_lock_(tcs3200_t *t) {
  return (xSemaphoreTake(t->lock, pdMS_TO_TICKS(t->cfg.lock_timeout_ms)) ==
          pdTRUE)
             ? ESP_OK
             : ESP_ERR_NOT_FINISHED;
}

static inline void tcs3200_unlock_(tcs3200_t *t) { xSemaphoreGive(t->lock); }

static inline esp_err_t tcs3200_check_(const tcs3200_t *t, uint8_t sensor) {
  if (t == NULL || !t->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  return (sensor < t->cfg.sensors) ? ESP_OK : ESP_ERR_INVALID_ARG;
}

static inline bool tcs3200_has_oe_(const tcs3200_t *t) {
  return t->cfg.oe[0] != GPIO_NUM_NC;
}

static inline void tcs3200_write_scale_pins_(tcs3200_t *t, tcs3200_scale_t s) {
  if (!t->fixed_scale) {
    gpio_set_level(t->cfg.s0, ((unsigned)s >> 0) & 1U);
    gpio_set_level(t->cfg.s1, ((unsigned)s >> 1) & 1U);
  }
}

static inline void tcs3200_apply_scale_locked_(tcs3200_t *t,
                                               tcs3200_scale_t s) {
  t->scale = s;
  if (t->powered) {
    tcs3200_write_scale_pins_(t, s);
  }
  t->settle_pending = true;
}

static inline void tcs3200_deselect_all_locked_(tcs3200_t *t) {
  if (tcs3200_has_oe_(t)) {
    for (uint8_t i = 0; i < t->cfg.sensors; i++) {
      gpio_set_level(t->cfg.oe[i], 1); /* OUT en alta impedancia */
    }
  }
  t->sel_sensor = -1;
}

/** Elige que sensor del banco emite: primero se sueltan los demas y despues se
 * habilita el elegido. */
static inline void tcs3200_select_locked_(tcs3200_t *t, uint8_t idx) {
  if (t->sel_sensor == (int8_t)idx) {
    return;
  }
  if (tcs3200_has_oe_(t)) {
    for (uint8_t i = 0; i < t->cfg.sensors; i++) {
      if (i != idx) {
        gpio_set_level(t->cfg.oe[i], 1);
      }
    }
    gpio_set_level(t->cfg.oe[idx], 0);
  }
  t->sel_sensor = (int8_t)idx;
  t->settle_pending = true;
}

static inline void tcs3200_filter_locked_(tcs3200_t *t, tcs3200_filter_t f) {
  if (t->cur_filter == (int8_t)f) {
    return;
  }
  static const uint8_t s2s3[TCS3200_FILTER_COUNT][2] = {
      [TCS3200_FILTER_RED] = {0, 0},
      [TCS3200_FILTER_GREEN] = {1, 1},
      [TCS3200_FILTER_BLUE] = {0, 1},
      [TCS3200_FILTER_CLEAR] = {1, 0},
  };
  gpio_set_level(t->cfg.s2, s2s3[f][0]);
  gpio_set_level(t->cfg.s3, s2s3[f][1]);
  t->cur_filter = (int8_t)f;
  t->settle_pending = true;
}

static inline esp_err_t tcs3200_power_locked_(tcs3200_t *t, bool on) {
  if (on) {
    t->powered = true;
    tcs3200_write_scale_pins_(t, t->scale);
    t->settle_pending = true;
    return ESP_OK;
  }
  if (t->fixed_scale && !tcs3200_has_oe_(t)) {
    return ESP_ERR_NOT_SUPPORTED; /* sin S0/S1 ni OE no hay forma de apagarlo */
  }
  if (!t->fixed_scale) {
    gpio_set_level(t->cfg.s0, 0);
    gpio_set_level(t->cfg.s1, 0);
  }
  tcs3200_deselect_all_locked_(t);
  t->powered = false;
  t->settle_pending = true;
  return ESP_OK;
}

/* ------------------------------------------------------------------------- */
/* Internas: medida                                                          */
/* ------------------------------------------------------------------------- */

/** Frecuencia de un canal de un sensor (mutex tomado). */
static inline esp_err_t tcs3200_measure_locked_(tcs3200_t *t, uint8_t idx,
                                                tcs3200_filter_t f, float *hz,
                                                uint32_t *pulses) {
  esp_err_t err;
  if (!t->powered && (err = tcs3200_power_locked_(t, true)) != ESP_OK) {
    return err;
  }
  tcs3200_select_locked_(t, idx);
  tcs3200_filter_locked_(t, f);

  if ((err = tcs3200_counter_clear_(t)) != ESP_OK) {
    return err;
  }
  if (t->settle_pending) {
    tcs3200_sleep_ms_(
        t->cfg.settle_ms); /* la salida tarda en estabilizarse tras un cambio */
    t->settle_pending = false;
  }

  uint32_t c1 = 0, c2 = 0;
  int64_t t1 = 0, t2 = 0;
  tcs3200_counter_gate_(t, true);
  err = tcs3200_snapshot_(t, &c1, &t1);
  if (err == ESP_OK) {
    tcs3200_sleep_ms_(t->cfg.gate_ms);
    err = tcs3200_snapshot_(t, &c2, &t2);
  }
  tcs3200_counter_gate_(t, false);
  if (err != ESP_OK) {
    return err;
  }
  if (t2 <= t1) {
    return ESP_ERR_INVALID_RESPONSE;
  }
  uint32_t n = c2 - c1;
  *pulses = n;
  *hz = (float)((double)n * 1e6 / (double)(t2 - t1));
  return ESP_OK;
}

static inline float tcs3200_clamp01_(float v) {
  return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

/** Medida completa de un sensor con autoescala, calibracion, HSV y
 * clasificacion. */
static inline esp_err_t tcs3200_read_locked_(tcs3200_t *t, uint8_t idx,
                                             uint8_t mask,
                                             tcs3200_sample_t *smp) {
  memset(smp, 0, sizeof(*smp));
  smp->sensor = idx;
  esp_err_t err = ESP_OK;

  for (int attempt = 0; attempt < 3; attempt++) {
    memset(smp->hz, 0, sizeof(smp->hz));
    memset(smp->pulses, 0, sizeof(smp->pulses));
    float peak = 0.0f;
    for (int c = 0; c < TCS3200_FILTER_COUNT; c++) {
      if (!((mask >> c) & 1U)) {
        continue;
      }
      if ((err = tcs3200_measure_locked_(t, idx, (tcs3200_filter_t)c,
                                         &smp->hz[c], &smp->pulses[c])) !=
          ESP_OK) {
        return err;
      }
      if (smp->hz[c] > peak) {
        peak = smp->hz[c];
      }
    }
    if (!t->cfg.auto_scale || t->fixed_scale) {
      break;
    }
    /* Escala mas alta que el contador aguanta (mas cuentas = mas resolucion) */
    const float limit = (float)t->cfg.counter_max_hz;
    if (peak > limit && t->scale > TCS3200_SCALE_2) {
      tcs3200_apply_scale_locked_(t, (tcs3200_scale_t)(t->scale - 1));
      continue;
    }
    if (t->scale < t->max_scale) {
      float ratio = tcs3200_scale_factor((tcs3200_scale_t)(t->scale + 1)) /
                    tcs3200_scale_factor(t->scale);
      if (peak * ratio <
          0.8f * limit) { /* margen del 20 %: sin oscilaciones entre escalas */
        tcs3200_apply_scale_locked_(t, (tcs3200_scale_t)(t->scale + 1));
        continue;
      }
    }
    break;
  }

  smp->scale = t->scale;
  smp->measured = mask;
  const float factor = tcs3200_scale_factor(t->scale);
  bool any_pulse = false;
  float peak100 = 0.0f;
  for (int c = 0; c < TCS3200_FILTER_COUNT; c++) {
    smp->hz100[c] = (factor > 0.0f) ? smp->hz[c] / factor : 0.0f;
    if (smp->hz100[c] > peak100) {
      peak100 = smp->hz100[c];
    }
    if (smp->pulses[c]) {
      any_pulse = true;
    }
  }
  smp->no_signal = !any_pulse;
  /* Saturacion y poca luz se miden con la frecuencia normalizada a 100 %: no
   * dependen de la escala */
  smp->saturated = peak100 > TCS3200_SAT_FRACTION * TCS3200_FULL_SCALE_HZ;
  smp->low_light =
      !smp->no_signal && peak100 < TCS3200_LOW_FRACTION * TCS3200_FULL_SCALE_HZ;

  /* Normalizacion: calibrada (negro/blanco) o relativa al canal RGB mayor */
  const tcs3200_cal_t *cal = &t->cal[idx];
  if (cal->has_white) {
    smp->calibrated = true;
    for (int c = 0; c < TCS3200_FILTER_COUNT; c++) {
      float dark = cal->has_dark ? cal->dark_hz[c] : 0.0f;
      float span = cal->white_hz[c] - dark;
      smp->norm[c] = (span > 1e-3f)
                         ? tcs3200_clamp01_((smp->hz100[c] - dark) / span)
                         : 0.0f;
    }
  } else {
    float m = fmaxf(smp->hz100[0], fmaxf(smp->hz100[1], smp->hz100[2]));
    for (int c = 0; c < 3; c++) {
      smp->norm[c] = (m > 1e-3f) ? smp->hz100[c] / m : 0.0f;
    }
    smp->norm[3] = tcs3200_clamp01_(smp->hz100[3] / TCS3200_FULL_SCALE_HZ);
  }
  smp->r = (uint8_t)lroundf(smp->norm[0] * 255.0f);
  smp->g = (uint8_t)lroundf(smp->norm[1] * 255.0f);
  smp->b = (uint8_t)lroundf(smp->norm[2] * 255.0f);
  tcs3200_rgb_to_hsv(smp->norm[0], smp->norm[1], smp->norm[2], &smp->h, &smp->s,
                     &smp->v);
  smp->color = smp->no_signal ? TCS3200_COLOR_UNKNOWN
                              : tcs3200_classify(smp->h, smp->s, smp->v);
  smp->valid = true;
  smp->err = ESP_OK;
  smp->time_us = esp_timer_get_time();
  return ESP_OK;
}

static inline void tcs3200_store_last_(tcs3200_t *t, uint8_t idx,
                                       const tcs3200_sample_t *s) {
  portENTER_CRITICAL(&t->mux);
  t->last[idx] = *s;
  portEXIT_CRITICAL(&t->mux);
}

/* ------------------------------------------------------------------------- */
/* Ciclo de vida                                                             */
/* ------------------------------------------------------------------------- */

static inline bool tcs3200_pins_ok_(const tcs3200_config_t *c) {
  int pins[8 + TCS3200_MAX_SENSORS + 2];
  int n = 0;
  pins[n++] = c->s0;
  pins[n++] = c->s1;
  pins[n++] = c->s2;
  pins[n++] = c->s3;
  pins[n++] = c->out;
  pins[n++] = c->led;
  for (int i = 0; i < c->sensors; i++) {
    pins[n++] = c->oe[i];
  }
  for (int i = 0; i < n; i++) {
    if (pins[i] < 0) {
      continue;
    }
    for (int j = 0; j < i; j++) {
      if (pins[j] == pins[i]) {
        return false;
      }
    }
  }
  return true;
}

static inline void tcs3200_config_out_(gpio_num_t pin, int level) {
  gpio_set_level(
      pin,
      (uint32_t)level); /* nivel ANTES de activar la salida: sin glitch en OE */
  gpio_config_t io = {
      .pin_bit_mask = 1ULL << pin,
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  gpio_config(&io);
  gpio_set_level(pin, (uint32_t)level);
}
static inline esp_err_t tcs3200_init(tcs3200_t *t,
                                     const tcs3200_config_t *cfg) {
  if (t == NULL || cfg == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if (t->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  if (cfg->sensors < 1 || cfg->sensors > TCS3200_MAX_SENSORS ||
      !GPIO_IS_VALID_OUTPUT_GPIO(cfg->s2) ||
      !GPIO_IS_VALID_OUTPUT_GPIO(cfg->s3) || !GPIO_IS_VALID_GPIO(cfg->out) ||
      (cfg->s0 != GPIO_NUM_NC && !GPIO_IS_VALID_OUTPUT_GPIO(cfg->s0)) ||
      (cfg->s1 != GPIO_NUM_NC && !GPIO_IS_VALID_OUTPUT_GPIO(cfg->s1)) ||
      ((cfg->s0 == GPIO_NUM_NC) !=
       (cfg->s1 == GPIO_NUM_NC)) || /* o los dos o ninguno */
      (cfg->led != GPIO_NUM_NC && !GPIO_IS_VALID_OUTPUT_GPIO(cfg->led)) ||
      !tcs3200_pins_ok_(cfg)) {
    return ESP_ERR_INVALID_ARG;
  }
  for (int i = 0; i < cfg->sensors; i++) {
    if (cfg->oe[i] == GPIO_NUM_NC ? (cfg->sensors > 1)
                                  : !GPIO_IS_VALID_OUTPUT_GPIO(cfg->oe[i])) {
      return ESP_ERR_INVALID_ARG; /* con varios sensores, cada uno necesita su
                                     OE */
    }
  }

  memset(t, 0, sizeof(*t));
  t->cfg = *cfg;
  if (t->cfg.gate_ms == 0)
    t->cfg.gate_ms = TCS3200_DEFAULT_GATE_MS;
  if (t->cfg.lock_timeout_ms == 0)
    t->cfg.lock_timeout_ms = TCS3200_DEFAULT_LOCK_TIMEOUT_MS;
  if (t->cfg.channels == 0)
    t->cfg.channels = TCS3200_CH_ALL;
  t->cfg.channels &= TCS3200_CH_ALL;
  if (t->cfg.glitch_ns == 0)
    t->cfg.glitch_ns = 100U;

  /* Contador */
  t->kind = cfg->counter;
  if (t->kind == TCS3200_COUNTER_AUTO) {
    t->kind =
        TCS3200_HAVE_PCNT ? TCS3200_COUNTER_PCNT : TCS3200_COUNTER_GPIO_ISR;
  }
  if (t->kind == TCS3200_COUNTER_PCNT && !TCS3200_HAVE_PCNT) {
    return ESP_ERR_NOT_SUPPORTED;
  }

  t->fixed_scale = (cfg->s0 == GPIO_NUM_NC);
  t->scale = cfg->scale;
  t->max_scale = (cfg->max_scale == TCS3200_SCALE_POWER_DOWN)
                     ? TCS3200_SCALE_100
                     : cfg->max_scale;
  if (t->cfg.counter_max_hz == 0) {
    t->cfg.counter_max_hz = (t->kind == TCS3200_COUNTER_PCNT)
                                ? TCS3200_PCNT_MAX_HZ
                                : TCS3200_ISR_MAX_HZ;
  }
  if (t->scale == TCS3200_SCALE_POWER_DOWN ||
      (unsigned)t->scale > TCS3200_SCALE_100) {
    if (t->fixed_scale) {
      return ESP_ERR_INVALID_ARG; /* hay que declarar la escala cableada */
    }
    t->scale = t->max_scale;
  }
  if (!t->fixed_scale && t->scale > t->max_scale) {
    t->scale = t->max_scale;
  }
  if (!t->fixed_scale && t->cfg.auto_scale &&
      t->kind == TCS3200_COUNTER_GPIO_ISR) {
    t->scale =
        TCS3200_SCALE_2; /* con interrupcion se empieza por la escala segura */
  }
  if (t->fixed_scale) {
    t->cfg.auto_scale = false;
  }

  portMUX_INITIALIZE(&t->mux);
  t->lock = xSemaphoreCreateMutex();
  if (t->lock == NULL) {
    return ESP_ERR_NO_MEM;
  }

  for (int i = 0; i < cfg->sensors; i++) {
    if (cfg->oe[i] != GPIO_NUM_NC) {
      tcs3200_config_out_(cfg->oe[i], 1);
    }
  }
  if (!t->fixed_scale) {
    tcs3200_config_out_(cfg->s0, ((unsigned)t->scale >> 0) & 1U);
    tcs3200_config_out_(cfg->s1, ((unsigned)t->scale >> 1) & 1U);
  }
  tcs3200_config_out_(cfg->s2, 0);
  tcs3200_config_out_(cfg->s3, 0);
  t->cur_filter = TCS3200_FILTER_RED;
  if (cfg->led != GPIO_NUM_NC) {
    tcs3200_config_out_(cfg->led, cfg->led_active_high ? 0 : 1);
  }
  gpio_config_t in = {
      .pin_bit_mask = 1ULL << cfg->out,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  gpio_config(&in);

  esp_err_t err = tcs3200_counter_init_(t);
  if (err != ESP_OK) {
    vSemaphoreDelete(t->lock);
    t->lock = NULL;
    return err;
  }

  t->powered = true;
  t->sel_sensor = tcs3200_has_oe_(t) ? -1 : 0;
  t->settle_pending = true;
  t->initialized = true;
  return ESP_OK;
}

static inline esp_err_t tcs3200_deinit(tcs3200_t *t) {
  esp_err_t err = tcs3200_check_(t, 0);
  if (err != ESP_OK) {
    return err;
  }
  if (t->polling) {
    t->stop = true;
    if (xSemaphoreTake(t->exited, pdMS_TO_TICKS(t->poll.period_ms + 2000U)) !=
        pdTRUE) {
      return ESP_ERR_TIMEOUT;
    }
    t->polling = false;
    vSemaphoreDelete(t->exited);
    t->exited = NULL;
  }
  if ((err = tcs3200_lock_(t)) != ESP_OK) {
    return err;
  }
  (void)tcs3200_power_locked_(t, false);
  if (t->cfg.led != GPIO_NUM_NC) {
    gpio_set_level(t->cfg.led, t->cfg.led_active_high ? 0 : 1);
  }
  tcs3200_counter_deinit_(t);
  t->initialized = false;
  tcs3200_unlock_(t);
  vSemaphoreDelete(t->lock);
  t->lock = NULL;
  return ESP_OK;
}

static inline esp_err_t tcs3200_read_channel_hz(tcs3200_t *t, uint8_t sensor,
                                                tcs3200_filter_t filter,
                                                float *hz) {
  esp_err_t err = tcs3200_check_(t, sensor);
  if (err != ESP_OK) {
    return err;
  }
  if (hz == NULL || (unsigned)filter >= TCS3200_FILTER_COUNT) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = tcs3200_lock_(t)) != ESP_OK) {
    return err;
  }
  uint32_t pulses;
  err = tcs3200_measure_locked_(t, sensor, filter, hz, &pulses);
  tcs3200_unlock_(t);
  return err;
}

static inline esp_err_t tcs3200_read(tcs3200_t *t, uint8_t sensor,
                                     tcs3200_sample_t *out) {
  esp_err_t err = tcs3200_check_(t, sensor);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = tcs3200_lock_(t)) != ESP_OK) {
    return err;
  }
  tcs3200_sample_t s;
  err = tcs3200_read_locked_(t, sensor, t->cfg.channels, &s);
  if (err != ESP_OK) {
    memset(&s, 0, sizeof(s));
    s.sensor = sensor;
    s.err = err;
    s.time_us = esp_timer_get_time();
  }
  tcs3200_unlock_(t);
  tcs3200_store_last_(t, sensor, &s);
  if (out) {
    *out = s;
  }
  return err;
}

static inline esp_err_t tcs3200_get_last(tcs3200_t *t, uint8_t sensor,
                                         tcs3200_sample_t *out) {
  esp_err_t err = tcs3200_check_(t, sensor);
  if (err != ESP_OK) {
    return err;
  }
  if (out == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  portENTER_CRITICAL(&t->mux);
  *out = t->last[sensor];
  portEXIT_CRITICAL(&t->mux);
  return ESP_OK;
}

static inline uint32_t tcs3200_read_time_ms(const tcs3200_t *t) {
  uint32_t n = 0;
  for (int c = 0; c < TCS3200_FILTER_COUNT; c++) {
    n += (t->cfg.channels >> c) & 1U;
  }
  return n * (t->cfg.gate_ms + t->cfg.settle_ms);
}

//  calibracion

typedef enum {
  TCS3200_CAL_DARK = 0,
  TCS3200_CAL_WHITE,
} tcs3200_cal_kind_t;

static inline esp_err_t tcs3200_calibrate(tcs3200_t *t, uint8_t sensor,
                                          tcs3200_cal_kind_t kind,
                                          uint8_t samples) {
  esp_err_t err = tcs3200_check_(t, sensor);
  if (err != ESP_OK) {
    return err;
  }
  if (samples < 1 || samples > 8 || (unsigned)kind > TCS3200_CAL_WHITE) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = tcs3200_lock_(t)) != ESP_OK) {
    return err;
  }
  float acc[4] = {0, 0, 0, 0};
  for (uint8_t i = 0; i < samples; i++) {
    tcs3200_sample_t s;
    if ((err = tcs3200_read_locked_(t, sensor, TCS3200_CH_ALL, &s)) != ESP_OK) {
      goto out;
    }
    if (s.no_signal) {
      err = ESP_ERR_NOT_FOUND;
      goto out;
    }
    if (kind == TCS3200_CAL_WHITE && s.saturated) {
      err = ESP_ERR_INVALID_RESPONSE;
      goto out;
    }
    for (int c = 0; c < 4; c++) {
      acc[c] += s.hz100[c];
    }
  }
  {
    tcs3200_cal_t *cal = &t->cal[sensor];
    for (int c = 0; c < 4; c++) {
      (kind == TCS3200_CAL_DARK ? cal->dark_hz : cal->white_hz)[c] =
          acc[c] / (float)samples;
    }
    if (kind == TCS3200_CAL_DARK) {
      cal->has_dark = true;
    } else {
      cal->has_white = true;
    }
  }
out:
  tcs3200_unlock_(t);
  return err;
}

static inline esp_err_t tcs3200_set_calibration(tcs3200_t *t, uint8_t sensor,
                                                const tcs3200_cal_t *cal) {
  esp_err_t err = tcs3200_check_(t, sensor);
  if (err != ESP_OK) {
    return err;
  }
  if (cal == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = tcs3200_lock_(t)) != ESP_OK) {
    return err;
  }
  t->cal[sensor] = *cal;
  tcs3200_unlock_(t);
  return ESP_OK;
}

static inline esp_err_t tcs3200_get_calibration(tcs3200_t *t, uint8_t sensor,
                                                tcs3200_cal_t *cal) {
  esp_err_t err = tcs3200_check_(t, sensor);
  if (err != ESP_OK) {
    return err;
  }
  if (cal == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = tcs3200_lock_(t)) != ESP_OK) {
    return err;
  }
  *cal = t->cal[sensor];
  tcs3200_unlock_(t);
  return ESP_OK;
}

// API: escala, energia y LED
static inline esp_err_t tcs3200_set_scale(tcs3200_t *t, tcs3200_scale_t scale) {
  esp_err_t err = tcs3200_check_(t, 0);
  if (err != ESP_OK) {
    return err;
  }
  if (t->fixed_scale) {
    return ESP_ERR_NOT_SUPPORTED;
  }
  if (scale < TCS3200_SCALE_2 || scale > TCS3200_SCALE_100) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = tcs3200_lock_(t)) != ESP_OK) {
    return err;
  }
  t->cfg.auto_scale = false;
  tcs3200_apply_scale_locked_(t, scale);
  tcs3200_unlock_(t);
  return ESP_OK;
}

static inline esp_err_t tcs3200_set_auto_scale(tcs3200_t *t, bool enable) {
  esp_err_t err = tcs3200_check_(t, 0);
  if (err != ESP_OK) {
    return err;
  }
  if (t->fixed_scale) {
    return ESP_ERR_NOT_SUPPORTED;
  }
  if ((err = tcs3200_lock_(t)) != ESP_OK) {
    return err;
  }
  t->cfg.auto_scale = enable;
  tcs3200_unlock_(t);
  return ESP_OK;
}

static inline tcs3200_scale_t tcs3200_get_scale(const tcs3200_t *t) {
  return (t != NULL && t->initialized) ? t->scale : TCS3200_SCALE_POWER_DOWN;
}

static inline esp_err_t tcs3200_power(tcs3200_t *t, bool on) {
  esp_err_t err = tcs3200_check_(t, 0);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = tcs3200_lock_(t)) != ESP_OK) {
    return err;
  }
  err = tcs3200_power_locked_(t, on);
  tcs3200_unlock_(t);
  return err;
}

// LED blanco del modulo (si se indico el pin 'led').
static inline esp_err_t tcs3200_set_led(tcs3200_t *t, bool on) {
  esp_err_t err = tcs3200_check_(t, 0);
  if (err != ESP_OK) {
    return err;
  }
  if (t->cfg.led == GPIO_NUM_NC) {
    return ESP_ERR_NOT_SUPPORTED;
  }
  if ((err = tcs3200_lock_(t)) != ESP_OK) {
    return err;
  }
  gpio_set_level(t->cfg.led, (on == t->cfg.led_active_high) ? 1 : 0);
  t->led_on = on;
  tcs3200_unlock_(t);
  return ESP_OK;
}

// Tarea de medida periodica

static inline void tcs3200_poll_sweep(tcs3200_t *t) {
  if (t->poll.power_save) {
    (void)tcs3200_power(t, true);
  }
  for (uint8_t i = 0; i < t->cfg.sensors; i++) {
    tcs3200_sample_t s;
    (void)tcs3200_read(t, i, &s);
    if (t->poll.on_sample) {
      t->poll.on_sample(i, &s, t->poll.cb_ctx);
    }
  }
  if (t->poll.power_save) {
    (void)tcs3200_power(t, false);
  }
}

static inline void tcs3200_poll_task_(void *arg) {
  tcs3200_t *t = (tcs3200_t *)arg;
  while (!t->stop) {
    tcs3200_poll_sweep(t);
    tcs3200_sleep_ms_(t->poll.period_ms);
  }
  xSemaphoreGive(t->exited);
  vTaskDelete(NULL);
}

static inline esp_err_t tcs3200_poll_start(tcs3200_t *t,
                                           const tcs3200_poll_config_t *pcfg) {
  esp_err_t err = tcs3200_check_(t, 0);
  if (err != ESP_OK) {
    return err;
  }
  if (t->polling) {
    return ESP_ERR_INVALID_STATE;
  }
  tcs3200_poll_config_t def = TCS3200_POLL_CONFIG_DEFAULT();
  t->poll = pcfg ? *pcfg : def;
  t->stop = false;
  if (t->exited == NULL) {
    t->exited = xSemaphoreCreateBinary();
    if (t->exited == NULL) {
      return ESP_ERR_NO_MEM;
    }
  }
  (void)xSemaphoreTake(t->exited, 0);
  BaseType_t ok;
  if (t->poll.core_id < 0) {
    ok = xTaskCreate(tcs3200_poll_task_, "tcs3200", t->poll.stack_size, t,
                     t->poll.priority, &t->task);
  } else {
    ok = xTaskCreatePinnedToCore(tcs3200_poll_task_, "tcs3200",
                                 t->poll.stack_size, t, t->poll.priority,
                                 &t->task, t->poll.core_id);
  }
  if (ok != pdPASS) {
    return ESP_ERR_NO_MEM;
  }
  t->polling = true;
  return ESP_OK;
}

static inline esp_err_t tcs3200_poll_stop(tcs3200_t *t) {
  esp_err_t err = tcs3200_check_(t, 0);
  if (err != ESP_OK) {
    return err;
  }
  if (!t->polling) {
    return ESP_ERR_INVALID_STATE;
  }
  t->stop = true;
  uint32_t wait_ms =
      t->poll.period_ms + 1000U +
      (uint32_t)t->cfg.sensors * (tcs3200_read_time_ms(t) * 3U + 100U);
  if (xSemaphoreTake(t->exited, pdMS_TO_TICKS(wait_ms)) != pdTRUE) {
    return ESP_ERR_TIMEOUT;
  }
  t->task = NULL;
  t->polling = false;
  return ESP_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* TCS3200_H */
