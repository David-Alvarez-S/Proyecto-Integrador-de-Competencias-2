#ifndef TFT24_H
#define TFT24_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "hc595.h"
#include "ili9341.h"
#include "xpt2046.h"

typedef struct {
  spi_host_device_t host;
  gpio_num_t mosi, miso, sclk;
  gpio_num_t lcd_cs, lcd_dc, lcd_rst, lcd_bl;
  gpio_num_t touch_cs, touch_irq;
  int lcd_clock_hz;   // 0 = por defecto (26 MHz)
  int touch_clock_hz; // 0 = por defecto (1 MHz)
  tft_rotation_t rotation;
  bool bl_pwm; // Brillo por PWM
  size_t dma_buf_bytes;
  gpio_num_t shift_latch;     // RCLK del 595 (= su CS)
  gpio_num_t shift_oe;        // OE del 595 o GPIO_NUM_NC
  uint8_t shift_chips;        // 1 o 2 registros (0 = 2)
  int shift_clock_hz;         // 0 = 2 MHz
  uint16_t shift_invert_mask; // Salidas de logica invertida
} tft24_config_t;

#ifndef TFT24_SHARED_DMA_BYTES
#define TFT24_SHARED_DMA_BYTES 1024U
#endif

#define TFT24_CONFIG_DEFAULT(_mosi, _miso, _sclk, _lcd_cs, _lcd_dc, _lcd_rst,  \
                             _lcd_bl, _t_cs, _t_irq)                           \
  {                                                                            \
      .host = SPI2_HOST,                                                       \
      .mosi = (_mosi),                                                         \
      .miso = (_miso),                                                         \
      .sclk = (_sclk),                                                         \
      .lcd_cs = (_lcd_cs),                                                     \
      .lcd_dc = (_lcd_dc),                                                     \
      .lcd_rst = (_lcd_rst),                                                   \
      .lcd_bl = (_lcd_bl),                                                     \
      .touch_cs = (_t_cs),                                                     \
      .touch_irq = (_t_irq),                                                   \
      .lcd_clock_hz = 0,                                                       \
      .touch_clock_hz = 0,                                                     \
      .rotation = TFT_ROTATION_0,                                              \
      .bl_pwm = true,                                                          \
      .dma_buf_bytes = 0,                                                      \
      .shift_latch = GPIO_NUM_NC,                                              \
      .shift_oe = GPIO_NUM_NC,                                                 \
      .shift_chips = 2,                                                        \
      .shift_clock_hz = 0,                                                     \
      .shift_invert_mask = 0,                                                  \
  }

typedef struct {
  ili9341_t lcd;
  xpt2046_t touch;
  hc595_t shift;
  spi_host_device_t host;
  bool bus_owned;
  bool has_shift;
  bool initialized;
} tft24_t;

static inline hc595_t *tft24_shift(tft24_t *t) {
  return (t != NULL && t->initialized && t->has_shift) ? &t->shift : NULL;
}

static inline bool tft24_pins_distinct_(const tft24_config_t *c) {
  const int pins[] = {c->mosi,      c->miso,        c->sclk,    c->lcd_cs,
                      c->lcd_dc,    c->lcd_rst,     c->lcd_bl,  c->touch_cs,
                      c->touch_irq, c->shift_latch, c->shift_oe};
  const int n = (int)(sizeof(pins) / sizeof(pins[0]));
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

static inline esp_err_t tft24_init(tft24_t *t, const tft24_config_t *cfg) {
  if (t == NULL || cfg == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if (t->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  if (!tft24_pins_distinct_(cfg)) {
    return ESP_ERR_INVALID_ARG;
  }
  const bool use_shift = (cfg->shift_latch != GPIO_NUM_NC);
  memset(t, 0, sizeof(*t));
  t->host = cfg->host;

  size_t dma = cfg->dma_buf_bytes ? cfg->dma_buf_bytes
                                  : (use_shift ? TFT24_SHARED_DMA_BYTES
                                               : ILI9341_DEFAULT_DMA_BYTES);
  spi_bus_config_t bus = {
      .mosi_io_num = cfg->mosi,
      .miso_io_num = cfg->miso,
      .sclk_io_num = cfg->sclk,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = (int)dma,
  };
  esp_err_t err = spi_bus_initialize(cfg->host, &bus, SPI_DMA_CH_AUTO);
  if (err == ESP_OK) {
    t->bus_owned = true;
  } else if (err != ESP_ERR_INVALID_STATE) {
    return err;
  }

  ili9341_config_t lc =
      ILI9341_CONFIG_DEFAULT(cfg->mosi, cfg->miso, cfg->sclk, cfg->lcd_cs,
                             cfg->lcd_dc, cfg->lcd_rst, cfg->lcd_bl);
  lc.host = cfg->host;
  lc.init_bus = false;
  lc.clock_hz = cfg->lcd_clock_hz;
  lc.rotation = cfg->rotation;
  lc.bl_pwm = cfg->bl_pwm;
  lc.dma_buf_bytes = dma;
  if ((err = ili9341_init(&t->lcd, &lc)) != ESP_OK) {
    goto fail_bus;
  }

  xpt2046_config_t tc = XPT2046_CONFIG_DEFAULT(cfg->mosi, cfg->miso, cfg->sclk,
                                               cfg->touch_cs, cfg->touch_irq);
  tc.host = cfg->host;
  tc.init_bus = false;
  tc.clock_hz = cfg->touch_clock_hz;
  tc.rotation = cfg->rotation;
  if ((err = xpt2046_init(&t->touch, &tc)) != ESP_OK) {
    (void)ili9341_deinit(&t->lcd);
    goto fail_bus;
  }

  if (use_shift) {
    hc595_config_t sc =
        HC595_CONFIG_SHARED_SPI(cfg->host, cfg->shift_latch, cfg->shift_oe);
    sc.chips = cfg->shift_chips;
    sc.clock_hz = cfg->shift_clock_hz;
    sc.invert_mask = cfg->shift_invert_mask;
    if ((err = hc595_init(&t->shift, &sc)) != ESP_OK) {
      (void)xpt2046_deinit(&t->touch);
      (void)ili9341_deinit(&t->lcd);
      goto fail_bus;
    }
    t->has_shift = true;
  }

  t->initialized = true;
  return ESP_OK;

fail_bus:
  if (t->bus_owned) {
    spi_bus_free(cfg->host);
  }
  memset(t, 0, sizeof(*t));
  return err;
}

static inline esp_err_t tft24_deinit(tft24_t *t) {
  if (t == NULL || !t->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  esp_err_t e0 = ESP_OK;
  if (t->has_shift) {
    e0 = hc595_deinit(&t->shift);
    t->has_shift = false;
  }
  esp_err_t e1 = xpt2046_deinit(&t->touch);
  esp_err_t e2 = ili9341_deinit(&t->lcd);
  if (t->bus_owned) {
    spi_bus_free(t->host);
    t->bus_owned = false;
  }
  t->initialized = false;
  return (e0 != ESP_OK) ? e0 : ((e1 != ESP_OK) ? e1 : e2);
}

static inline esp_err_t tft24_set_rotation(tft24_t *t, tft_rotation_t rot) {
  if (t == NULL || !t->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  esp_err_t err = ili9341_set_rotation(&t->lcd, rot);
  if (err == ESP_OK) {
    err = xpt2046_set_rotation(&t->touch, rot);
  }
  return err;
}

// Calibracion interactiva

static inline void tft24_draw_cross_(ili9341_t *lcd, int x, int y,
                                     uint16_t color) {
  ili9341_draw_hline(lcd, x - 10, y, 21, color);
  ili9341_draw_vline(lcd, x, y - 10, 21, color);
  ili9341_draw_circle(lcd, x, y, 6, color);
}

static inline esp_err_t tft24_wait_raw_point_(xpt2046_t *tp,
                                              uint32_t timeout_ms, float *rx,
                                              float *ry) {
  int64_t deadline = esp_timer_get_time() + (int64_t)timeout_ms * 1000;
  uint16_t x, y, z;

  for (;;) {
    if (xpt2046_read_raw(tp, &x, &y, &z) == ESP_OK) {
      break;
    }
    if (esp_timer_get_time() > deadline) {
      return ESP_ERR_TIMEOUT;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  float sx = 0, sy = 0;
  int n = 0, tries = 0;
  while (n < 10 && tries < 60) {
    if (xpt2046_read_raw(tp, &x, &y, &z) == ESP_OK) {
      sx += x;
      sy += y;
      n++;
    }
    tries++;
    vTaskDelay(pdMS_TO_TICKS(8));
  }
  if (n < 4) {
    return ESP_ERR_INVALID_RESPONSE;
  }
  *rx = sx / (float)n;
  *ry = sy / (float)n;

  int released = 0;
  while (released < 15) {
    if (xpt2046_read_raw(tp, &x, &y, &z) == ESP_OK) {
      released = 0;
    } else {
      released++;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    if (esp_timer_get_time() > deadline + 5000000) {
      break;
    }
  }
  return ESP_OK;
}

static inline esp_err_t tft24_calibrate(tft24_t *t, uint32_t point_timeout_ms,
                                        xpt2046_cal_t *out_cal) {
  if (t == NULL || !t->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  ili9341_t *lcd = &t->lcd;
  const int w = ili9341_width(lcd), h = ili9341_height(lcd);
  const tft_rotation_t rot = ili9341_get_rotation(lcd);

  const int tx[3] = {w / 8, w / 2, (w * 7) / 8};
  const int ty[3] = {h / 8, (h * 7) / 8, h / 2};

  float raw[3][2], nat[3][2];
  esp_err_t err = ESP_OK;

  for (int i = 0; i < 3 && err == ESP_OK; i++) {
    ili9341_fill_screen(lcd, ILI9341_BLACK);
    ili9341_draw_string(lcd, 8, 8, "Calibracion", ILI9341_WHITE,
                        ILI9341_TRANSPARENT, 2);
    ili9341_draw_string(lcd, 8, 30, "Toca el centro de la cruz", ILI9341_GRAY,
                        ILI9341_TRANSPARENT, 1);
    char msg[16];
    snprintf(msg, sizeof(msg), "Punto %d/3", i + 1);
    ili9341_draw_string(lcd, 8, 44, msg, ILI9341_GRAY, ILI9341_TRANSPARENT, 1);
    tft24_draw_cross_(lcd, tx[i], ty[i], ILI9341_RED);

    err = tft24_wait_raw_point_(&t->touch, point_timeout_ms, &raw[i][0],
                                &raw[i][1]);

    int nx, ny;
    tft_rot_to_native(rot, tx[i], ty[i], &nx, &ny);
    nat[i][0] = (float)nx;
    nat[i][1] = (float)ny;
  }

  if (err == ESP_OK) {
    xpt2046_cal_t cal;
    err = xpt2046_cal_from_3pt((const float (*)[2])raw, (const float (*)[2])nat,
                               &cal);
    if (err == ESP_OK) {
      err = xpt2046_set_calibration(&t->touch, &cal);
      if (err == ESP_OK && out_cal) {
        *out_cal = cal;
      }
    }
  }

  ili9341_fill_screen(lcd, err == ESP_OK ? ILI9341_BLACK : ILI9341_RED);
  ili9341_draw_string(lcd, 8, 8,
                      err == ESP_OK ? "Calibracion OK" : "Calibracion fallida",
                      ILI9341_WHITE, ILI9341_TRANSPARENT, 2);
  vTaskDelay(pdMS_TO_TICKS(600));
  ili9341_fill_screen(lcd, ILI9341_BLACK);
  return err;
}

#ifdef __cplusplus
}
#endif

#endif /* TFT24_H */
