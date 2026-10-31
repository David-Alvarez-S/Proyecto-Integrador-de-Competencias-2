#ifndef XPT2046_H
#define XPT2046_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "tft_common.h"

// Parametros
#define XPT2046_CMD_X 0xD0 /* canal X, 12 bit, diferencial, PD=00 */
#define XPT2046_CMD_Y 0x90 /* canal Y */
#define XPT2046_CMD_Z1 0xB0
#define XPT2046_CMD_Z2 0xC0

#ifndef XPT2046_DEFAULT_CLOCK_HZ
#define XPT2046_DEFAULT_CLOCK_HZ 1000000
#endif
#ifndef XPT2046_MAX_SAMPLES
#define XPT2046_MAX_SAMPLES 15
#endif
#ifndef XPT2046_LOCK_TIMEOUT_MS
#define XPT2046_LOCK_TIMEOUT_MS 200U
#endif
#ifndef XPT2046_EVENT_QUEUE_LEN
#define XPT2046_EVENT_QUEUE_LEN 16
#endif

// Tipos

typedef struct {
  float a, b, c, d, e, f;
  bool valid; // true si proviene de una calibracion real
} xpt2046_cal_t;

typedef struct {
  spi_host_device_t host;
  bool init_bus;               //  true: esta libreria inicializa el bus
  gpio_num_t mosi, miso, sclk; //  Solo si init_bus
  gpio_num_t cs;
  gpio_num_t irq; // T_IRQ o GPIO_NUM_NC
  int clock_hz;   //  0 = XPT2046_DEFAULT_CLOCK_HZ
  tft_rotation_t rotation;
  uint16_t z_threshold; // Presion minima (0..4095). Tipico 400
  uint8_t samples;      // Muestras por eje (1..15). Tipico 5
  uint16_t max_spread;  // Dispersion raw maxima admitida
  // Mapeo por defecto (sin calibrar): rangos raw del panel
  uint16_t raw_x_min, raw_x_max, raw_y_min, raw_y_max;
  bool swap_xy, invert_x, invert_y;
  /* Tarea de eventos */
  uint16_t poll_ms;        // Periodo de muestreo mientras se toca
  uint8_t release_samples; // Lecturas sin toque para dar RELEASE
  uint8_t move_threshold;  // Pixeles minimos para emitir MOVE
  UBaseType_t task_priority;
  uint32_t task_stack;  // Bytes
  BaseType_t task_core; // <0: sin afinidad
} xpt2046_config_t;

#define XPT2046_CONFIG_DEFAULT(_mosi, _miso, _sclk, _cs, _irq)                 \
  {                                                                            \
      .host = SPI2_HOST,                                                       \
      .init_bus = true,                                                        \
      .mosi = (_mosi),                                                         \
      .miso = (_miso),                                                         \
      .sclk = (_sclk),                                                         \
      .cs = (_cs),                                                             \
      .irq = (_irq),                                                           \
      .clock_hz = 0,                                                           \
      .rotation = TFT_ROTATION_0,                                              \
      .z_threshold = 400,                                                      \
      .samples = 5,                                                            \
      .max_spread = 100,                                                       \
      .raw_x_min = 250,                                                        \
      .raw_x_max = 3850,                                                       \
      .raw_y_min = 250,                                                        \
      .raw_y_max = 3850,                                                       \
      .swap_xy = false,                                                        \
      .invert_x = false,                                                       \
      .invert_y = false,                                                       \
      .poll_ms = 10,                                                           \
      .release_samples = 3,                                                    \
      .move_threshold = 2,                                                     \
      .task_priority = 4,                                                      \
      .task_stack = 3072,                                                      \
      .task_core = -1,                                                         \
  }

typedef struct {
  int16_t x, y; // Coordenadas de la rotacion activa
  uint16_t z;   /// Presion (0..4095)
} xpt2046_point_t;

typedef enum {
  XPT2046_EVT_PRESS = 0,
  XPT2046_EVT_MOVE,
  XPT2046_EVT_RELEASE,
} xpt2046_evt_type_t;

typedef struct {
  xpt2046_evt_type_t type;
  int16_t x, y;
  uint16_t z;
  int64_t time_us; // esp_timer_get_time()
} xpt2046_event_t;

typedef struct {
  bool pressed;
  uint8_t miss;
  int16_t last_x, last_y;
} xpt2046_track_t;

typedef struct {
  xpt2046_config_t cfg;
  spi_device_handle_t spi;
  SemaphoreHandle_t lock;
  xpt2046_cal_t cal;
  tft_rotation_t rotation;
  bool bus_owned;
  bool initialized;

  QueueHandle_t queue;
  SemaphoreHandle_t irq_sem;
  SemaphoreHandle_t exited;
  TaskHandle_t task;
  volatile bool stop;
  bool running;
  bool irq_isr_added;
} xpt2046_t;

// Internas

static inline esp_err_t xpt2046_lock_(xpt2046_t *t) {
  return (xSemaphoreTake(t->lock, pdMS_TO_TICKS(XPT2046_LOCK_TIMEOUT_MS)) ==
          pdTRUE)
             ? ESP_OK
             : ESP_ERR_TIMEOUT;
}

static inline void xpt2046_unlock_(xpt2046_t *t) { xSemaphoreGive(t->lock); }

static inline esp_err_t xpt2046_check_(const xpt2046_t *t) {
  return (t != NULL && t->initialized) ? ESP_OK : ESP_ERR_INVALID_STATE;
}

// Una conversion de 12 bits (trama de 3 bytes).
static inline esp_err_t xpt2046_xfer_(xpt2046_t *t, uint8_t cmd,
                                      uint16_t *out) {
  spi_transaction_t x;
  memset(&x, 0, sizeof(x));
  x.length = 24;
  x.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
  x.tx_data[0] = cmd;
  esp_err_t err = spi_device_polling_transmit(t->spi, &x);
  if (err != ESP_OK) {
    return err;
  }
  *out =
      (uint16_t)((((uint16_t)x.rx_data[1] << 8) | x.rx_data[2]) >> 3) & 0x0FFFU;
  return ESP_OK;
}

static inline void xpt2046_sort_(uint16_t *v, int n) {
  for (int i = 1; i < n; i++) {
    uint16_t k = v[i];
    int j = i - 1;
    while (j >= 0 && v[j] > k) {
      v[j + 1] = v[j];
      j--;
    }
    v[j + 1] = k;
  }
}

// Media recortada. Devuelve false si la dispersion supera max_spread.
static inline bool xpt2046_trimmed_(uint16_t *v, int n, uint16_t max_spread,
                                    uint16_t *out) {
  xpt2046_sort_(v, n);
  int k = n / 4;
  int lo = k, hi = n - k - 1;
  if ((uint16_t)(v[hi] - v[lo]) > max_spread) {
    return false;
  }
  uint32_t sum = 0;
  for (int i = lo; i <= hi; i++) {
    sum += v[i];
  }
  *out = (uint16_t)(sum / (uint32_t)(hi - lo + 1));
  return true;
}

static inline esp_err_t xpt2046_pressure_(xpt2046_t *t, uint16_t *z) {
  uint16_t z1, z2;
  esp_err_t err = xpt2046_xfer_(t, XPT2046_CMD_Z1, &z1);
  if (err == ESP_OK) {
    err = xpt2046_xfer_(t, XPT2046_CMD_Z2, &z2);
  }
  if (err != ESP_OK) {
    return err;
  }
  int32_t p = (z1 > 0) ? ((int32_t)z1 + 4095 - (int32_t)z2) : 0;
  *z = (uint16_t)(p < 0 ? 0 : (p > 4095 ? 4095 : p));
  return ESP_OK;
}

static inline void xpt2046_lin_(float raw_lo, float raw_hi, float v0, float v1,
                                float *a, float *c) {
  float span = raw_hi - raw_lo;
  if (span < 1.0f) {
    span = 1.0f;
  }
  *a = (v1 - v0) / span;
  *c = v0 - *a * raw_lo;
}

static inline xpt2046_cal_t xpt2046_default_cal_(const xpt2046_config_t *cfg) {
  xpt2046_cal_t cal = {0};
  const float W = (float)(TFT_NATIVE_WIDTH - 1),
              H = (float)(TFT_NATIVE_HEIGHT - 1);
  float ax, cx, ay, cy;
  if (!cfg->swap_xy) { // x <- raw X ; y <- raw Y
    xpt2046_lin_(cfg->raw_x_min, cfg->raw_x_max, cfg->invert_x ? W : 0.0f,
                 cfg->invert_x ? 0.0f : W, &ax, &cx);
    xpt2046_lin_(cfg->raw_y_min, cfg->raw_y_max, cfg->invert_y ? H : 0.0f,
                 cfg->invert_y ? 0.0f : H, &ay, &cy);
    cal.a = ax;
    cal.b = 0;
    cal.c = cx;
    cal.d = 0;
    cal.e = ay;
    cal.f = cy;
  } else { // x <- raw Y ; y <- raw X
    xpt2046_lin_(cfg->raw_y_min, cfg->raw_y_max, cfg->invert_x ? W : 0.0f,
                 cfg->invert_x ? 0.0f : W, &ax, &cx);
    xpt2046_lin_(cfg->raw_x_min, cfg->raw_x_max, cfg->invert_y ? H : 0.0f,
                 cfg->invert_y ? 0.0f : H, &ay, &cy);
    cal.a = 0;
    cal.b = ax;
    cal.c = cx;
    cal.d = ay;
    cal.e = 0;
    cal.f = cy;
  }
  cal.valid = false;
  return cal;
}

// Lectura filtrada con el mutex tomado.
static inline esp_err_t xpt2046_read_raw_locked_(xpt2046_t *t, uint16_t *rx,
                                                 uint16_t *ry, uint16_t *rz) {
  // Descarte rapido con T_IRQ (activo en bajo)
  if (t->cfg.irq != GPIO_NUM_NC && gpio_get_level(t->cfg.irq) != 0) {
    return ESP_ERR_NOT_FOUND;
  }

  uint16_t z = 0;
  esp_err_t err = xpt2046_pressure_(t, &z);
  if (err != ESP_OK) {
    return err;
  }
  if (z < t->cfg.z_threshold) {
    return ESP_ERR_NOT_FOUND;
  }

  int n = t->cfg.samples;
  if (n < 1)
    n = 1;
  if (n > XPT2046_MAX_SAMPLES)
    n = XPT2046_MAX_SAMPLES;

  uint16_t xs[XPT2046_MAX_SAMPLES], ys[XPT2046_MAX_SAMPLES], dummy;

  // El primer resultado tras cambiar de canal no es fiable: se descarta
  if ((err = xpt2046_xfer_(t, XPT2046_CMD_X, &dummy)) != ESP_OK)
    return err;
  for (int i = 0; i < n; i++) {
    if ((err = xpt2046_xfer_(t, XPT2046_CMD_X, &xs[i])) != ESP_OK)
      return err;
  }
  if ((err = xpt2046_xfer_(t, XPT2046_CMD_Y, &dummy)) != ESP_OK)
    return err;
  for (int i = 0; i < n; i++) {
    if ((err = xpt2046_xfer_(t, XPT2046_CMD_Y, &ys[i])) != ESP_OK)
      return err;
  }

  // Si se levanto el dedo durante el muestreo, descartar
  uint16_t z_after = 0;
  if ((err = xpt2046_pressure_(t, &z_after)) != ESP_OK) {
    return err;
  }
  if (z_after < t->cfg.z_threshold) {
    return ESP_ERR_NOT_FOUND;
  }

  uint16_t vx, vy;
  if (!xpt2046_trimmed_(xs, n, t->cfg.max_spread, &vx) ||
      !xpt2046_trimmed_(ys, n, t->cfg.max_spread, &vy)) {
    return ESP_ERR_INVALID_RESPONSE;
  }
  if (rx)
    *rx = vx;
  if (ry)
    *ry = vy;
  if (rz)
    *rz = z;
  return ESP_OK;
}

// Calibracion

static inline esp_err_t xpt2046_cal_from_3pt(const float raw[3][2],
                                             const float native[3][2],
                                             xpt2046_cal_t *out) {
  if (raw == NULL || native == NULL || out == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  const double xt0 = raw[0][0], yt0 = raw[0][1], xt1 = raw[1][0],
               yt1 = raw[1][1];
  const double xt2 = raw[2][0], yt2 = raw[2][1];
  const double xd0 = native[0][0], yd0 = native[0][1], xd1 = native[1][0],
               yd1 = native[1][1];
  const double xd2 = native[2][0], yd2 = native[2][1];

  const double k = (xt0 - xt2) * (yt1 - yt2) - (xt1 - xt2) * (yt0 - yt2);
  if (fabs(k) < 1e-6) {
    return ESP_ERR_INVALID_ARG; // puntos colineales o repetidos
  }
  out->a = (float)(((xd0 - xd2) * (yt1 - yt2) - (xd1 - xd2) * (yt0 - yt2)) / k);
  out->b = (float)(((xt0 - xt2) * (xd1 - xd2) - (xd0 - xd2) * (xt1 - xt2)) / k);
  out->c =
      (float)((yt0 * (xt2 * xd1 - xt1 * xd2) + yt1 * (xt0 * xd2 - xt2 * xd0) +
               yt2 * (xt1 * xd0 - xt0 * xd1)) /
              k);
  out->d = (float)(((yd0 - yd2) * (yt1 - yt2) - (yd1 - yd2) * (yt0 - yt2)) / k);
  out->e = (float)(((xt0 - xt2) * (yd1 - yd2) - (yd0 - yd2) * (xt1 - xt2)) / k);
  out->f =
      (float)((yt0 * (xt2 * yd1 - xt1 * yd2) + yt1 * (xt0 * yd2 - xt2 * yd0) +
               yt2 * (xt1 * yd0 - xt0 * yd1)) /
              k);
  out->valid = true;
  return ESP_OK;
}

static inline esp_err_t xpt2046_set_calibration(xpt2046_t *t,
                                                const xpt2046_cal_t *cal) {
  esp_err_t err = xpt2046_check_(t);
  if (err != ESP_OK) {
    return err;
  }
  if (cal == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = xpt2046_lock_(t)) != ESP_OK) {
    return err;
  }
  t->cal = *cal;
  xpt2046_unlock_(t);
  return ESP_OK;
}

static inline esp_err_t xpt2046_get_calibration(xpt2046_t *t,
                                                xpt2046_cal_t *cal) {
  esp_err_t err = xpt2046_check_(t);
  if (err != ESP_OK) {
    return err;
  }
  if (cal == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = xpt2046_lock_(t)) != ESP_OK) {
    return err;
  }
  *cal = t->cal;
  xpt2046_unlock_(t);
  return ESP_OK;
}

// Ciclo de vida

static inline esp_err_t xpt2046_init(xpt2046_t *t,
                                     const xpt2046_config_t *cfg) {
  if (t == NULL || cfg == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if (t->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  if (!GPIO_IS_VALID_OUTPUT_GPIO(cfg->cs) ||
      (cfg->irq != GPIO_NUM_NC && !GPIO_IS_VALID_GPIO(cfg->irq)) ||
      cfg->samples < 1 || cfg->samples > XPT2046_MAX_SAMPLES ||
      cfg->raw_x_max <= cfg->raw_x_min || cfg->raw_y_max <= cfg->raw_y_min) {
    return ESP_ERR_INVALID_ARG;
  }

  memset(t, 0, sizeof(*t));
  t->cfg = *cfg;
  if (t->cfg.clock_hz <= 0) {
    t->cfg.clock_hz = XPT2046_DEFAULT_CLOCK_HZ;
  }
  if (t->cfg.poll_ms == 0) {
    t->cfg.poll_ms = 10;
  }
  if (t->cfg.release_samples == 0) {
    t->cfg.release_samples = 1;
  }
  t->rotation = t->cfg.rotation;
  t->cal = xpt2046_default_cal_(&t->cfg);

  esp_err_t err = ESP_OK;
  t->lock = xSemaphoreCreateMutex();
  if (t->lock == NULL) {
    return ESP_ERR_NO_MEM;
  }

  if (t->cfg.irq != GPIO_NUM_NC) {
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << t->cfg.irq,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    if ((err = gpio_config(&io)) != ESP_OK) {
      goto fail;
    }
  }

  if (t->cfg.init_bus) {
    spi_bus_config_t bus = {
        .mosi_io_num = t->cfg.mosi,
        .miso_io_num = t->cfg.miso,
        .sclk_io_num = t->cfg.sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
    err = spi_bus_initialize(t->cfg.host, &bus, SPI_DMA_CH_AUTO);
    if (err == ESP_OK) {
      t->bus_owned = true;
    } else if (err != ESP_ERR_INVALID_STATE) {
      goto fail;
    }
  }
  spi_device_interface_config_t dev = {
      .clock_speed_hz = t->cfg.clock_hz,
      .mode = 0,
      .spics_io_num = t->cfg.cs,
      .queue_size = 2,
  };
  if ((err = spi_bus_add_device(t->cfg.host, &dev, &t->spi)) != ESP_OK) {
    goto fail;
  }
  t->initialized = true;
  return ESP_OK;

fail:
  if (t->bus_owned) {
    spi_bus_free(t->cfg.host);
  }
  vSemaphoreDelete(t->lock);
  memset(t, 0, sizeof(*t));
  return err;
}

// Lectura

static inline esp_err_t xpt2046_read_raw(xpt2046_t *t, uint16_t *rx,
                                         uint16_t *ry, uint16_t *rz) {
  esp_err_t err = xpt2046_check_(t);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = xpt2046_lock_(t)) != ESP_OK) {
    return err;
  }
  err = xpt2046_read_raw_locked_(t, rx, ry, rz);
  xpt2046_unlock_(t);
  return err;
}

static inline void xpt2046_raw_to_point_(const xpt2046_t *t, uint16_t rx,
                                         uint16_t ry, uint16_t rz,
                                         xpt2046_point_t *p) {
  float nx = t->cal.a * (float)rx + t->cal.b * (float)ry + t->cal.c;
  float ny = t->cal.d * (float)rx + t->cal.e * (float)ry + t->cal.f;
  int ix = (int)lroundf(nx), iy = (int)lroundf(ny);
  if (ix < 0)
    ix = 0;
  if (ix > TFT_NATIVE_WIDTH - 1)
    ix = TFT_NATIVE_WIDTH - 1;
  if (iy < 0)
    iy = 0;
  if (iy > TFT_NATIVE_HEIGHT - 1)
    iy = TFT_NATIVE_HEIGHT - 1;
  int x, y;
  tft_native_to_rot(t->rotation, ix, iy, &x, &y);
  p->x = (int16_t)x;
  p->y = (int16_t)y;
  p->z = rz;
}

static inline esp_err_t xpt2046_read(xpt2046_t *t, xpt2046_point_t *p) {
  esp_err_t err = xpt2046_check_(t);
  if (err != ESP_OK) {
    return err;
  }
  if (p == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = xpt2046_lock_(t)) != ESP_OK) {
    return err;
  }
  uint16_t rx = 0, ry = 0, rz = 0;
  err = xpt2046_read_raw_locked_(t, &rx, &ry, &rz);
  if (err == ESP_OK) {
    xpt2046_raw_to_point_(t, rx, ry, rz, p);
  }
  xpt2046_unlock_(t);
  return err;
}

static inline bool xpt2046_is_pressed(xpt2046_t *t) {
  uint16_t rx, ry, rz;
  return xpt2046_read_raw(t, &rx, &ry, &rz) == ESP_OK;
}

static inline esp_err_t xpt2046_set_rotation(xpt2046_t *t, tft_rotation_t rot) {
  esp_err_t err = xpt2046_check_(t);
  if (err != ESP_OK) {
    return err;
  }
  if ((unsigned)rot > 3U) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = xpt2046_lock_(t)) != ESP_OK) {
    return err;
  }
  t->rotation = rot;
  xpt2046_unlock_(t);
  return ESP_OK;
}

__attribute__((unused)) static void IRAM_ATTR xpt2046_irq_isr_(void *arg) {
  xpt2046_t *t = (xpt2046_t *)arg;
  BaseType_t hpw = pdFALSE;
  xSemaphoreGiveFromISR(t->irq_sem, &hpw);
  if (hpw == pdTRUE) {
    TFT_YIELD_FROM_ISR(hpw);
  }
}

static inline void xpt2046_post_(xpt2046_t *t, const xpt2046_event_t *ev) {
  if (xQueueSend(t->queue, ev, 0) != pdTRUE && ev->type != XPT2046_EVT_MOVE) {
    xpt2046_event_t old;                    /* PRESS/RELEASE no se pierden: */
    (void)xQueueReceive(t->queue, &old, 0); /* se descarta el mas antiguo */
    (void)xQueueSend(t->queue, ev, 0);
  }
}

static inline bool xpt2046_track_step_(xpt2046_t *t, xpt2046_track_t *s,
                                       xpt2046_event_t *ev) {
  xpt2046_point_t p;
  esp_err_t err = xpt2046_read(t, &p);

  if (err == ESP_OK) {
    s->miss = 0;
    ev->x = p.x;
    ev->y = p.y;
    ev->z = p.z;
    ev->time_us = esp_timer_get_time();
    if (!s->pressed) {
      s->pressed = true;
      s->last_x = p.x;
      s->last_y = p.y;
      ev->type = XPT2046_EVT_PRESS;
      return true;
    }
    int dx = p.x - s->last_x, dy = p.y - s->last_y;
    if ((dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy) >=
        (int)t->cfg.move_threshold) {
      s->last_x = p.x;
      s->last_y = p.y;
      ev->type = XPT2046_EVT_MOVE;
      return true;
    }
    return false;
  }
  if (err == ESP_ERR_INVALID_RESPONSE) {
    return false; // muestra ruidosa: ignorar
  }
  if (s->pressed && ++s->miss >= t->cfg.release_samples) {
    s->pressed = false;
    s->miss = 0;
    ev->type = XPT2046_EVT_RELEASE;
    ev->x = s->last_x;
    ev->y = s->last_y;
    ev->z = 0;
    ev->time_us = esp_timer_get_time();
    return true;
  }
  return false;
}

static inline void xpt2046_task_(void *arg) {
  xpt2046_t *t = (xpt2046_t *)arg;
  xpt2046_track_t trk = {0};
  TickType_t poll = pdMS_TO_TICKS(t->cfg.poll_ms);
  if (poll == 0) {
    poll = 1;
  }

  while (!t->stop) {
    if (!trk.pressed && t->cfg.irq != GPIO_NUM_NC) {
      // Reposo: dormir hasta el flanco de T_IRQ (o 100 ms para revisar 'stop')
      if (xSemaphoreTake(t->irq_sem, pdMS_TO_TICKS(100)) != pdTRUE) {
        continue;
      }
    }
    xpt2046_event_t ev;
    bool was_pressed = trk.pressed;
    if (xpt2046_track_step_(t, &trk, &ev)) {
      xpt2046_post_(t, &ev);
      if (was_pressed && !trk.pressed && t->cfg.irq != GPIO_NUM_NC) {
        (void)xSemaphoreTake(t->irq_sem, 0); // limpiar flancos acumulados
      }
    }
    vTaskDelay(poll);
  }
  xSemaphoreGive(t->exited);
  vTaskDelete(NULL);
}

// Crea la cola de eventos y la tarea. Con T_IRQ, instala la ISR del pin.
static inline esp_err_t xpt2046_start(xpt2046_t *t) {
  esp_err_t err = xpt2046_check_(t);
  if (err != ESP_OK) {
    return err;
  }
  if (t->running) {
    return ESP_ERR_INVALID_STATE;
  }
  t->stop = false;
  t->queue = xQueueCreate(XPT2046_EVENT_QUEUE_LEN, sizeof(xpt2046_event_t));
  t->exited = xSemaphoreCreateBinary();
  t->irq_sem = xSemaphoreCreateBinary();
  if (t->queue == NULL || t->exited == NULL || t->irq_sem == NULL) {
    err = ESP_ERR_NO_MEM;
    goto fail;
  }
  if (t->cfg.irq != GPIO_NUM_NC) {
    esp_err_t e = gpio_install_isr_service(0);
    if (e != ESP_OK && e != ESP_ERR_INVALID_STATE) {
      err = e;
      goto fail;
    }
    gpio_set_intr_type(t->cfg.irq, GPIO_INTR_NEGEDGE);
    if ((err = gpio_isr_handler_add(t->cfg.irq, xpt2046_irq_isr_, t)) !=
        ESP_OK) {
      goto fail;
    }
    t->irq_isr_added = true;
    gpio_intr_enable(t->cfg.irq);
  }

  BaseType_t ok;
  if (t->cfg.task_core < 0) {
    ok = xTaskCreate(xpt2046_task_, "xpt2046", t->cfg.task_stack, t,
                     t->cfg.task_priority, &t->task);
  } else {
    ok = xTaskCreatePinnedToCore(xpt2046_task_, "xpt2046", t->cfg.task_stack, t,
                                 t->cfg.task_priority, &t->task,
                                 t->cfg.task_core);
  }
  if (ok != pdPASS) {
    err = ESP_ERR_NO_MEM;
    goto fail;
  }
  t->running = true;
  return ESP_OK;

fail:
  if (t->irq_isr_added) {
    gpio_intr_disable(t->cfg.irq);
    gpio_isr_handler_remove(t->cfg.irq);
    t->irq_isr_added = false;
  }
  if (t->queue) {
    vQueueDelete(t->queue);
    t->queue = NULL;
  }
  if (t->exited) {
    vSemaphoreDelete(t->exited);
    t->exited = NULL;
  }
  if (t->irq_sem) {
    vSemaphoreDelete(t->irq_sem);
    t->irq_sem = NULL;
  }
  return err;
}

// Espera el siguiente evento. timeout_ms = UINT32_MAX: indefinido.
static inline esp_err_t xpt2046_get_event(xpt2046_t *t, xpt2046_event_t *ev,
                                          uint32_t timeout_ms) {
  if (xpt2046_check_(t) != ESP_OK || !t->running) {
    return ESP_ERR_INVALID_STATE;
  }
  if (ev == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  TickType_t ticks =
      (timeout_ms == UINT32_MAX) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
  return (xQueueReceive(t->queue, ev, ticks) == pdTRUE) ? ESP_OK
                                                        : ESP_ERR_TIMEOUT;
}

static inline QueueHandle_t xpt2046_event_queue(const xpt2046_t *t) {
  return (t != NULL && t->running) ? t->queue : NULL;
}

static inline esp_err_t xpt2046_stop(xpt2046_t *t) {
  esp_err_t err = xpt2046_check_(t);
  if (err != ESP_OK) {
    return err;
  }
  if (!t->running) {
    return ESP_ERR_INVALID_STATE;
  }
  t->stop = true;
  if (t->cfg.irq != GPIO_NUM_NC) {
    xSemaphoreGive(t->irq_sem); // despertar la tarea
  }
  if (xSemaphoreTake(t->exited, pdMS_TO_TICKS(1000)) != pdTRUE) {
    return ESP_ERR_TIMEOUT;
  }
  if (t->irq_isr_added) {
    gpio_intr_disable(t->cfg.irq);
    gpio_isr_handler_remove(t->cfg.irq);
    t->irq_isr_added = false;
  }
  vQueueDelete(t->queue);
  t->queue = NULL;
  vSemaphoreDelete(t->exited);
  t->exited = NULL;
  vSemaphoreDelete(t->irq_sem);
  t->irq_sem = NULL;
  t->task = NULL;
  t->running = false;
  return ESP_OK;
}

static inline esp_err_t xpt2046_deinit(xpt2046_t *t) {
  esp_err_t err = xpt2046_check_(t);
  if (err != ESP_OK) {
    return err;
  }
  if (t->running && (err = xpt2046_stop(t)) != ESP_OK) {
    return err;
  }
  t->initialized = false;
  spi_bus_remove_device(t->spi);
  t->spi = NULL;
  if (t->bus_owned) {
    spi_bus_free(t->cfg.host);
    t->bus_owned = false;
  }
  vSemaphoreDelete(t->lock);
  t->lock = NULL;
  return ESP_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* XPT2046_H */
