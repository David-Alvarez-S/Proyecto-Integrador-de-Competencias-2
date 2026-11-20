#ifndef GC9306_H
#define GC9306_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "tft_common.h"
#include "tft_font5x7.h"

// ---------- 
// Parametros 
// ---------- 

#ifndef GC9306_DEFAULT_CLOCK_HZ
#define GC9306_DEFAULT_CLOCK_HZ 8000000
#endif
#ifndef GC9306_DEFAULT_DMA_BYTES
#define GC9306_DEFAULT_DMA_BYTES 4096U
#endif
#ifndef GC9306_LOCK_TIMEOUT_MS
#define GC9306_LOCK_TIMEOUT_MS 1000U
#endif
#ifndef GC9306_BL_PWM_FREQ_HZ
#define GC9306_BL_PWM_FREQ_HZ 5000U
#endif

#define GC9306_TRANSPARENT (-1)

#define GC9306_RGB565(r, g, b)                                                 \
  ((uint16_t)((((uint16_t)(r) & 0xF8U) << 8) |                                 \
              (((uint16_t)(g) & 0xFCU) << 3) | ((uint16_t)(b) >> 3)))
#define GC9306_BLACK 0x0000
#define GC9306_WHITE 0xFFFF
#define GC9306_RED 0xF800
#define GC9306_GREEN 0x07E0
#define GC9306_BLUE 0x001F
#define GC9306_YELLOW 0xFFE0
#define GC9306_CYAN 0x07FF
#define GC9306_MAGENTA 0xF81F
#define GC9306_ORANGE 0xFD20
#define GC9306_GRAY 0x8410
#define GC9306_DARKGRAY 0x4208

#define GC9306_CMD_SWRESET 0x01
#define GC9306_CMD_SLPIN 0x10
#define GC9306_CMD_SLPOUT 0x11
#define GC9306_CMD_INVOFF 0x20
#define GC9306_CMD_INVON 0x21
#define GC9306_CMD_DISPOFF 0x28
#define GC9306_CMD_DISPON 0x29
#define GC9306_CMD_CASET 0x2A
#define GC9306_CMD_PASET 0x2B
#define GC9306_CMD_RAMWR 0x2C
#define GC9306_CMD_MADCTL 0x36
#define GC9306_CMD_PIXFMT 0x3A
#define GC9306_CMD_INTER_REG_EN1 0xFE
#define GC9306_CMD_INTER_REG_EN2 0xEF

// ----- 
// Tipos 
// ----- 

typedef struct {
  spi_host_device_t host; 
  bool init_bus;          
  gpio_num_t mosi;        
  gpio_num_t miso;        
  gpio_num_t sclk;        
  gpio_num_t cs;
  gpio_num_t dc;
  gpio_num_t rst; 
  gpio_num_t bl;  
  bool bl_active_high;
  bool bl_pwm; 
  ledc_timer_t bl_timer;
  ledc_channel_t bl_channel;
  uint8_t bl_initial;   
  int clock_hz;         
  size_t dma_buf_bytes; 
  tft_rotation_t rotation;
  bool bgr;    
  bool invert; 
} gc9306_config_t;

#define GC9306_CONFIG_DEFAULT(_mosi, _miso, _sclk, _cs, _dc, _rst, _bl)        \
  {                                                                            \
      .host = SPI2_HOST,                                                       \
      .init_bus = true,                                                        \
      .mosi = (_mosi),                                                         \
      .miso = (_miso),                                                         \
      .sclk = (_sclk),                                                         \
      .cs = (_cs),                                                             \
      .dc = (_dc),                                                             \
      .rst = (_rst),                                                           \
      .bl = (_bl),                                                             \
      .bl_active_high = true,                                                  \
      .bl_pwm = true,                                                          \
      .bl_timer = LEDC_TIMER_1,                                                \
      .bl_channel = LEDC_CHANNEL_1,                                            \
      .bl_initial = 100,                                                       \
      .clock_hz = 0,                                                           \
      .dma_buf_bytes = 0,                                                      \
      .rotation = TFT_ROTATION_0,                                              \
      .bgr = true,                                                             \
      .invert = false,                                                         \
  }

typedef struct {
  gc9306_config_t cfg;
  spi_device_handle_t spi;
  SemaphoreHandle_t lock;
  uint8_t *dma_buf;
  size_t dma_buf_bytes;
  uint16_t width;
  uint16_t height;
  tft_rotation_t rotation;
  bool bus_owned;
  bool bl_ready;
  bool initialized;
} gc9306_t;

typedef struct {
  spi_transaction_t t;
  gpio_num_t dc_pin;
  uint8_t dc;
} gc9306_txn_t;

__attribute__((unused)) static void IRAM_ATTR
gc9306_pre_cb_(spi_transaction_t *t) {
  const gc9306_txn_t *x = (const gc9306_txn_t *)t;
  gpio_set_level(x->dc_pin, x->dc);
}

static inline esp_err_t gc9306_lock_(gc9306_t *d) {
  return (xSemaphoreTake(d->lock, pdMS_TO_TICKS(GC9306_LOCK_TIMEOUT_MS)) ==
          pdTRUE)
             ? ESP_OK
             : ESP_ERR_TIMEOUT;
}

static inline void gc9306_unlock_(gc9306_t *d) { xSemaphoreGive(d->lock); }

static inline esp_err_t gc9306_check_(const gc9306_t *d) {
  return (d != NULL && d->initialized) ? ESP_OK : ESP_ERR_INVALID_STATE;
}

static inline esp_err_t gc9306_tx_cmd_(gc9306_t *d, uint8_t cmd) {
  gc9306_txn_t x;
  memset(&x, 0, sizeof(x));
  x.t.length = 8;
  x.t.flags = SPI_TRANS_USE_TXDATA;
  x.t.tx_data[0] = cmd;
  x.dc_pin = d->cfg.dc;
  x.dc = 0;
  return spi_device_polling_transmit(d->spi, &x.t);
}

static inline esp_err_t gc9306_tx_cmd_params_(gc9306_t *d, uint8_t cmd,
                                              const uint8_t *p, size_t n) {
  if (n == 0 || p == NULL) {
    return gc9306_tx_cmd_(d, cmd);
  }

  esp_err_t err = spi_device_acquire_bus(d->spi, portMAX_DELAY);
  if (err != ESP_OK) {
    return err;
  }

  gc9306_txn_t tx_cmd;
  memset(&tx_cmd, 0, sizeof(tx_cmd));
  tx_cmd.t.length = 8;
  tx_cmd.t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_CS_KEEP_ACTIVE;
  tx_cmd.t.tx_data[0] = cmd;
  tx_cmd.dc_pin = d->cfg.dc;
  tx_cmd.dc = 0;

  err = spi_device_polling_transmit(d->spi, &tx_cmd.t);
  size_t offset = 0;
  while (offset < n && err == ESP_OK) {
    size_t chunk = (n - offset > 4) ? 4 : (n - offset);
    bool last = (offset + chunk >= n);

    gc9306_txn_t tx_p;
    memset(&tx_p, 0, sizeof(tx_p));
    tx_p.t.length = chunk * 8;
    tx_p.t.flags = SPI_TRANS_USE_TXDATA | (last ? 0 : SPI_TRANS_CS_KEEP_ACTIVE);
    memcpy(tx_p.t.tx_data, p + offset, chunk);
    tx_p.dc_pin = d->cfg.dc;
    tx_p.dc = 1;

    err = spi_device_polling_transmit(d->spi, &tx_p.t);
    offset += chunk;
  }

  spi_device_release_bus(d->spi);
  return err;
}

static inline esp_err_t gc9306_write_window_dma_(gc9306_t *d, int x0, int y0,
                                                 int x1, int y1,
                                                 size_t pixel_bytes) {
  uint8_t c[4] = {(uint8_t)(x0 >> 8), (uint8_t)x0, (uint8_t)(x1 >> 8),
                  (uint8_t)x1};
  uint8_t r[4] = {(uint8_t)(y0 >> 8), (uint8_t)y0, (uint8_t)(y1 >> 8),
                  (uint8_t)y1};

  esp_err_t err = gc9306_tx_cmd_params_(d, GC9306_CMD_CASET, c, 4);
  if (err != ESP_OK) {
    return err;
  }
  err = gc9306_tx_cmd_params_(d, GC9306_CMD_PASET, r, 4);
  if (err != ESP_OK) {
    return err;
  }

  err = spi_device_acquire_bus(d->spi, portMAX_DELAY);
  if (err != ESP_OK) {
    return err;
  }

  gc9306_txn_t tx_cmd;
  memset(&tx_cmd, 0, sizeof(tx_cmd));
  tx_cmd.t.length = 8;
  tx_cmd.t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_CS_KEEP_ACTIVE;
  tx_cmd.t.tx_data[0] = GC9306_CMD_RAMWR;
  tx_cmd.dc_pin = d->cfg.dc;
  tx_cmd.dc = 0;
  err = spi_device_polling_transmit(d->spi, &tx_cmd.t);

  if (err == ESP_OK) {
    gc9306_txn_t tx_px;
    memset(&tx_px, 0, sizeof(tx_px));
    tx_px.t.length = pixel_bytes * 8;
    tx_px.t.tx_buffer = d->dma_buf;
    tx_px.t.flags = 0; 
    tx_px.dc_pin = d->cfg.dc;
    tx_px.dc = 1;
    err = spi_device_polling_transmit(d->spi, &tx_px.t);
  }

  spi_device_release_bus(d->spi);
  return err;
}

static inline bool gc9306_clip_(const gc9306_t *d, int *x, int *y, int *w,
                                int *h, int *skip_x, int *skip_y) {
  int sx = 0, sy = 0;
  if (*w <= 0 || *h <= 0) {
    return false;
  }
  if (*x < 0) {
    sx = -*x;
    *w += *x;
    *x = 0;
  }
  if (*y < 0) {
    sy = -*y;
    *h += *y;
    *y = 0;
  }
  if (*x + *w > (int)d->width) {
    *w = (int)d->width - *x;
  }
  if (*y + *h > (int)d->height) {
    *h = (int)d->height - *y;
  }
  if (*w <= 0 || *h <= 0) {
    return false;
  }
  if (skip_x)
    *skip_x = sx;
  if (skip_y)
    *skip_y = sy;
  return true;
}

static inline esp_err_t gc9306_fill_rect_locked_(gc9306_t *d, int x, int y,
                                                 int w, int h, uint16_t color) {
  if (!gc9306_clip_(d, &x, &y, &w, &h, NULL, NULL)) {
    return ESP_OK;
  }

  size_t bufpx = d->dma_buf_bytes / 2;
  int rows_per_chunk = (int)(bufpx / (size_t)w);
  if (rows_per_chunk < 1) {
    rows_per_chunk = 1;
  }
  if (rows_per_chunk > h) {
    rows_per_chunk = h;
  }

  size_t fill_px = (size_t)w * (size_t)rows_per_chunk;
  if (fill_px > bufpx) {
    fill_px = bufpx;
  }

  uint16_t sw = __builtin_bswap16(color);
  uint16_t *b = (uint16_t *)d->dma_buf;
  for (size_t i = 0; i < fill_px; i++) {
    b[i] = sw;
  }

  int cur_y = y;
  int end_y = y + h;
  while (cur_y < end_y) {
    int chunk_h = end_y - cur_y;
    if (chunk_h > rows_per_chunk) {
      chunk_h = rows_per_chunk;
    }
    size_t chunk_px = (size_t)w * (size_t)chunk_h;
    esp_err_t err = gc9306_write_window_dma_(
        d, x, cur_y, x + w - 1, cur_y + chunk_h - 1, chunk_px * 2);
    if (err != ESP_OK) {
      return err;
    }
    cur_y += chunk_h;
  }
  return ESP_OK;
}

static inline esp_err_t gc9306_draw_pixel_locked_(gc9306_t *d, int x, int y,
                                                  uint16_t color) {
  if (x < 0 || y < 0 || x >= (int)d->width || y >= (int)d->height) {
    return ESP_OK;
  }
  uint8_t c[4] = {(uint8_t)(x >> 8), (uint8_t)x, (uint8_t)(x >> 8),
                  (uint8_t)x};
  uint8_t r[4] = {(uint8_t)(y >> 8), (uint8_t)y, (uint8_t)(y >> 8),
                  (uint8_t)y};
  uint8_t px[2] = {(uint8_t)(color >> 8), (uint8_t)color};

  esp_err_t err = gc9306_tx_cmd_params_(d, GC9306_CMD_CASET, c, 4);
  if (err == ESP_OK) {
    err = gc9306_tx_cmd_params_(d, GC9306_CMD_PASET, r, 4);
  }
  if (err == ESP_OK) {
    err = gc9306_tx_cmd_params_(d, GC9306_CMD_RAMWR, px, 2);
  }
  return err;
}

static inline esp_err_t gc9306_draw_char_locked_(gc9306_t *d, int x, int y,
                                                 char c, uint16_t fg,
                                                 int32_t bg, int scale) {
  if (scale < 1) {
    scale = 1;
  }
  const uint8_t *g = tft_font5x7_glyph(c);
  const int cw = TFT_FONT_CELL_W * scale;
  const int ch = TFT_FONT_CELL_H * scale;
  const size_t cell_px = (size_t)cw * (size_t)ch;

  if (x >= (int)d->width || y >= (int)d->height || x + cw <= 0 || y + ch <= 0) {
    return ESP_OK;
  }

  /* Camino rapido: fondo opaco en una unica rafaga DMA */
  if (bg >= 0 && x >= 0 && y >= 0 && x + cw <= (int)d->width &&
      y + ch <= (int)d->height && cell_px * 2 <= d->dma_buf_bytes) {
    uint16_t *b = (uint16_t *)d->dma_buf;
    uint16_t fgsw = __builtin_bswap16(fg);
    uint16_t bgsw = __builtin_bswap16((uint16_t)bg);
    for (int py = 0; py < ch; py++) {
      int gy = py / scale;
      for (int px = 0; px < cw; px++) {
        int gx = px / scale;
        bool on = (gx < TFT_FONT_GLYPH_W) && ((g[gx] >> gy) & 1U);
        b[(size_t)py * (size_t)cw + (size_t)px] = on ? fgsw : bgsw;
      }
    }
    return gc9306_write_window_dma_(d, x, y, x + cw - 1, y + ch - 1,
                                    cell_px * 2);
  }

  esp_err_t err = ESP_OK;
  if (bg >= 0) {
    err = gc9306_fill_rect_locked_(d, x, y, cw, ch, (uint16_t)bg);
    if (err != ESP_OK) {
      return err;
    }
  }
  for (int gx = 0; gx < TFT_FONT_GLYPH_W; gx++) {
    int gy = 0;
    while (gy < 8) {
      if (!((g[gx] >> gy) & 1U)) {
        gy++;
        continue;
      }
      int run = 1;
      while (gy + run < 8 && ((g[gx] >> (gy + run)) & 1U)) {
        run++;
      }
      err = gc9306_fill_rect_locked_(d, x + gx * scale, y + gy * scale, scale,
                                     run * scale, fg);
      if (err != ESP_OK) {
        return err;
      }
      gy += run;
    }
  }
  return ESP_OK;
}

static inline esp_err_t gc9306_backlight_init_(gc9306_t *d) {
  if (d->cfg.bl == GPIO_NUM_NC) {
    return ESP_OK;
  }
  esp_err_t err;
  if (d->cfg.bl_pwm) {
    ledc_timer_config_t tc = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = d->cfg.bl_timer,
        .freq_hz = GC9306_BL_PWM_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    err = ledc_timer_config(&tc);
    if (err != ESP_OK) {
      return err;
    }
    ledc_channel_config_t cc = {
        .gpio_num = d->cfg.bl,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = d->cfg.bl_channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = d->cfg.bl_timer,
        .duty = d->cfg.bl_active_high ? 0U : 1023U,
        .hpoint = 0,
    };
    err = ledc_channel_config(&cc);
  } else {
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << d->cfg.bl,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    err = gpio_config(&io);
    if (err == ESP_OK) {
      gpio_set_level(d->cfg.bl, d->cfg.bl_active_high ? 0 : 1);
    }
  }
  d->bl_ready = (err == ESP_OK);
  return err;
}

static inline esp_err_t gc9306_set_backlight(gc9306_t *d, uint8_t percent) {
  if (d == NULL || d->lock == NULL) {
    return ESP_ERR_INVALID_STATE;
  }
  if (d->cfg.bl == GPIO_NUM_NC || !d->bl_ready) {
    return ESP_ERR_INVALID_STATE;
  }
  if (percent > 100) {
    percent = 100;
  }
  if (d->cfg.bl_pwm) {
    uint32_t duty = (uint32_t)percent * 1023U / 100U;
    if (!d->cfg.bl_active_high) {
      duty = 1023U - duty;
    }
    esp_err_t err = ledc_set_duty(LEDC_LOW_SPEED_MODE, d->cfg.bl_channel, duty);
    return (err == ESP_OK)
               ? ledc_update_duty(LEDC_LOW_SPEED_MODE, d->cfg.bl_channel)
               : err;
  }
  bool on = percent > 0;
  return gpio_set_level(d->cfg.bl, (on == d->cfg.bl_active_high) ? 1 : 0);
}

static inline void gc9306_cleanup_(gc9306_t *d) {
  if (d->spi) {
    spi_bus_remove_device(d->spi);
    d->spi = NULL;
  }
  if (d->bus_owned) {
    spi_bus_free(d->cfg.host);
    d->bus_owned = false;
  }
  if (d->dma_buf) {
    heap_caps_free(d->dma_buf);
    d->dma_buf = NULL;
  }
  if (d->lock) {
    vSemaphoreDelete(d->lock);
    d->lock = NULL;
  }
}

static inline esp_err_t gc9306_run_init_seq_(gc9306_t *d) {
  typedef struct {
    uint8_t cmd;
    uint8_t val;
  } reg_step_t;

  static const reg_step_t seq[] = {
      {0x3A, 0x05}, 
      {0xA4, 0x44}, 
      {0xA5, 0x42},
      {0xAA, 0x88},
      {0xAE, 0x2B},
      {0xE8, 0x11},
      {0xE3, 0x01},
      {0xE1, 0x10},
      {0xFF, 0x61},
      {0xAC, 0x00},
      {0xAF, 0x67}, 
      {0xA6, 0x2A},
      {0xA7, 0x2B},
      {0xA8, 0x18},
      {0xA9, 0x2A},
  };

  esp_err_t err = gc9306_tx_cmd_(d, GC9306_CMD_INTER_REG_EN1);
  if (err != ESP_OK) {
    return err;
  }
  err = gc9306_tx_cmd_(d, GC9306_CMD_INTER_REG_EN2);
  if (err != ESP_OK) {
    return err;
  }

  for (size_t i = 0; i < sizeof(seq) / sizeof(seq[0]); i++) {
    err = gc9306_tx_cmd_params_(d, seq[i].cmd, &seq[i].val, 1);
    if (err != ESP_OK) {
      return err;
    }
  }
  return ESP_OK;
}

static inline esp_err_t gc9306_init(gc9306_t *d, const gc9306_config_t *cfg) {
  if (d == NULL || cfg == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if (d->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  if (!GPIO_IS_VALID_OUTPUT_GPIO(cfg->dc) ||
      !GPIO_IS_VALID_OUTPUT_GPIO(cfg->cs) ||
      (cfg->rst != GPIO_NUM_NC && !GPIO_IS_VALID_OUTPUT_GPIO(cfg->rst)) ||
      (cfg->bl != GPIO_NUM_NC && !GPIO_IS_VALID_OUTPUT_GPIO(cfg->bl))) {
    return ESP_ERR_INVALID_ARG;
  }

  memset(d, 0, sizeof(*d));
  d->cfg = *cfg;
  if (d->cfg.clock_hz <= 0) {
    d->cfg.clock_hz = GC9306_DEFAULT_CLOCK_HZ;
  }
  if (d->cfg.dma_buf_bytes == 0) {
    d->cfg.dma_buf_bytes = GC9306_DEFAULT_DMA_BYTES;
  }
  d->cfg.dma_buf_bytes &= ~(size_t)1;
  if (d->cfg.dma_buf_bytes < 640) {
    d->cfg.dma_buf_bytes = 640;
  }
  d->dma_buf_bytes = d->cfg.dma_buf_bytes;
  d->rotation = d->cfg.rotation;
  tft_rotation_size(d->rotation, &d->width, &d->height);

  esp_err_t err = ESP_OK;

  d->lock = xSemaphoreCreateMutex();
  d->dma_buf = (uint8_t *)heap_caps_malloc(d->dma_buf_bytes,
                                           MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
  if (d->lock == NULL || d->dma_buf == NULL) {
    err = ESP_ERR_NO_MEM;
    goto fail;
  }

  uint64_t mask = 1ULL << d->cfg.dc;
  if (d->cfg.rst != GPIO_NUM_NC) {
    mask |= 1ULL << d->cfg.rst;
  }
  gpio_config_t io = {
      .pin_bit_mask = mask,
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  if ((err = gpio_config(&io)) != ESP_OK) {
    goto fail;
  }

  if (d->cfg.init_bus) {
    spi_bus_config_t bus = {
        .mosi_io_num = d->cfg.mosi,
        .miso_io_num = d->cfg.miso,
        .sclk_io_num = d->cfg.sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = (int)d->dma_buf_bytes,
    };
    err = spi_bus_initialize(d->cfg.host, &bus, SPI_DMA_CH_AUTO);
    if (err == ESP_OK) {
      d->bus_owned = true;
    } else if (err != ESP_ERR_INVALID_STATE) {
      goto fail;
    }
  }

  spi_device_interface_config_t dev = {
      .clock_speed_hz = d->cfg.clock_hz,
      .mode = 3,
      .spics_io_num = d->cfg.cs,
      .cs_ena_pretrans = 2,
      .cs_ena_posttrans = 2,
      .queue_size = 4,
      .pre_cb = gc9306_pre_cb_,
  };
  if ((err = spi_bus_add_device(d->cfg.host, &dev, &d->spi)) != ESP_OK) {
    goto fail;
  }

  (void)gc9306_backlight_init_(d);

  if (d->cfg.rst != GPIO_NUM_NC) {
    gpio_set_level(d->cfg.rst, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(d->cfg.rst, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
  }
  d->initialized = true;

  if ((err = gc9306_tx_cmd_(d, GC9306_CMD_SWRESET)) != ESP_OK) {
    goto fail_init;
  }
  vTaskDelay(pdMS_TO_TICKS(120));

  if ((err = gc9306_run_init_seq_(d)) != ESP_OK) {
    goto fail_init;
  }

  uint8_t madctl = tft_rotation_madctl(d->rotation, d->cfg.bgr);
  if ((err = gc9306_tx_cmd_params_(d, GC9306_CMD_MADCTL, &madctl, 1)) !=
      ESP_OK) {
    goto fail_init;
  }

  if ((err = gc9306_tx_cmd_(d, GC9306_CMD_INTER_REG_EN1)) != ESP_OK) {
    goto fail_init;
  }

  if (d->cfg.invert &&
      (err = gc9306_tx_cmd_(d, GC9306_CMD_INVON)) != ESP_OK) {
    goto fail_init;
  }

  if ((err = gc9306_tx_cmd_(d, GC9306_CMD_SLPOUT)) != ESP_OK) {
    goto fail_init;
  }
  vTaskDelay(pdMS_TO_TICKS(120));

  if ((err = gc9306_fill_rect_locked_(d, 0, 0, d->width, d->height,
                                      GC9306_BLACK)) != ESP_OK) {
    goto fail_init;
  }

  if ((err = gc9306_tx_cmd_(d, GC9306_CMD_DISPON)) != ESP_OK) {
    goto fail_init;
  }
  vTaskDelay(pdMS_TO_TICKS(20));
  (void)gc9306_set_backlight(d, d->cfg.bl_initial);
  return ESP_OK;

fail_init:
  d->initialized = false;
fail:
  gc9306_cleanup_(d);
  return err;
}

static inline esp_err_t gc9306_deinit(gc9306_t *d) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }
  (void)gc9306_set_backlight(d, 0);
  (void)gc9306_tx_cmd_(d, GC9306_CMD_DISPOFF);
  d->initialized = false;
  gc9306_unlock_(d);
  gc9306_cleanup_(d);
  return ESP_OK;
}

static inline uint16_t gc9306_width(const gc9306_t *d) {
  return d ? d->width : 0;
}
static inline uint16_t gc9306_height(const gc9306_t *d) {
  return d ? d->height : 0;
}
static inline tft_rotation_t gc9306_get_rotation(const gc9306_t *d) {
  return d ? d->rotation : TFT_ROTATION_0;
}

static inline esp_err_t gc9306_set_rotation(gc9306_t *d, tft_rotation_t rot) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((unsigned)rot > 3U) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }
  uint8_t madctl = tft_rotation_madctl(rot, d->cfg.bgr);
  err = gc9306_tx_cmd_params_(d, GC9306_CMD_MADCTL, &madctl, 1);
  if (err == ESP_OK) {
    d->rotation = rot;
    tft_rotation_size(rot, &d->width, &d->height);
  }
  gc9306_unlock_(d);
  return err;
}

static inline esp_err_t gc9306_simple_cmd_(gc9306_t *d, uint8_t cmd,
                                           uint32_t delay_ms) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }
  err = gc9306_tx_cmd_(d, cmd);
  gc9306_unlock_(d);
  if (err == ESP_OK && delay_ms) {
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
  }
  return err;
}

static inline esp_err_t gc9306_display_on(gc9306_t *d, bool on) {
  return gc9306_simple_cmd_(d, on ? GC9306_CMD_DISPON : GC9306_CMD_DISPOFF, 0);
}

static inline esp_err_t gc9306_invert(gc9306_t *d, bool on) {
  return gc9306_simple_cmd_(d, on ? GC9306_CMD_INVON : GC9306_CMD_INVOFF, 0);
}

static inline esp_err_t gc9306_sleep(gc9306_t *d, bool sleep) {
  return gc9306_simple_cmd_(d, sleep ? GC9306_CMD_SLPIN : GC9306_CMD_SLPOUT,
                            120);
}
static inline esp_err_t gc9306_fill_rect(gc9306_t *d, int x, int y, int w,
                                         int h, uint16_t color) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }
  err = gc9306_fill_rect_locked_(d, x, y, w, h, color);
  gc9306_unlock_(d);
  return err;
}

static inline esp_err_t gc9306_fill_screen(gc9306_t *d, uint16_t color) {
  return gc9306_fill_rect(d, 0, 0, d ? d->width : 0, d ? d->height : 0, color);
}

static inline esp_err_t gc9306_draw_pixel(gc9306_t *d, int x, int y,
                                          uint16_t color) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }
  err = gc9306_draw_pixel_locked_(d, x, y, color);
  gc9306_unlock_(d);
  return err;
}

static inline esp_err_t gc9306_draw_hline(gc9306_t *d, int x, int y, int w,
                                          uint16_t color) {
  return gc9306_fill_rect(d, x, y, w, 1, color);
}

static inline esp_err_t gc9306_draw_vline(gc9306_t *d, int x, int y, int h,
                                          uint16_t color) {
  return gc9306_fill_rect(d, x, y, 1, h, color);
}

static inline esp_err_t gc9306_draw_line(gc9306_t *d, int x0, int y0, int x1,
                                         int y1, uint16_t color) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (y0 == y1) {
    return gc9306_draw_hline(d, x0 < x1 ? x0 : x1, y0,
                             (x0 < x1 ? x1 - x0 : x0 - x1) + 1, color);
  }
  if (x0 == x1) {
    return gc9306_draw_vline(d, x0, y0 < y1 ? y0 : y1,
                             (y0 < y1 ? y1 - y0 : y0 - y1) + 1, color);
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }
  int dx = x1 > x0 ? x1 - x0 : x0 - x1;
  int dy = y1 > y0 ? y0 - y1 : y1 - y0;
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int e = dx + dy;
  for (;;) {
    err = gc9306_draw_pixel_locked_(d, x0, y0, color);
    if (err != ESP_OK || (x0 == x1 && y0 == y1)) {
      break;
    }
    int e2 = 2 * e;
    if (e2 >= dy) {
      e += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      e += dx;
      y0 += sy;
    }
  }
  gc9306_unlock_(d);
  return err;
}

static inline esp_err_t gc9306_draw_rect(gc9306_t *d, int x, int y, int w,
                                         int h, uint16_t color) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (w <= 0 || h <= 0) {
    return ESP_OK;
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }
  err = gc9306_fill_rect_locked_(d, x, y, w, 1, color);
  if (err == ESP_OK) {
    err = gc9306_fill_rect_locked_(d, x, y + h - 1, w, 1, color);
  }
  if (err == ESP_OK) {
    err = gc9306_fill_rect_locked_(d, x, y, 1, h, color);
  }
  if (err == ESP_OK) {
    err = gc9306_fill_rect_locked_(d, x + w - 1, y, 1, h, color);
  }
  gc9306_unlock_(d);
  return err;
}

static inline esp_err_t gc9306_fill_circle(gc9306_t *d, int cx, int cy, int r,
                                           uint16_t color) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (r < 0) {
    return ESP_OK;
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }
  for (int dy = -r; dy <= r && err == ESP_OK; dy++) {
    int dx = (int)sqrtf((float)(r * r - dy * dy) + 0.5f);
    err = gc9306_fill_rect_locked_(d, cx - dx, cy + dy, 2 * dx + 1, 1, color);
  }
  gc9306_unlock_(d);
  return err;
}

static inline esp_err_t gc9306_draw_circle(gc9306_t *d, int cx, int cy, int r,
                                           uint16_t color) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (r < 0) {
    return ESP_OK;
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }
  int x = r, y = 0, e = 1 - r;
  while (x >= y && err == ESP_OK) {
    const int px[8] = {cx + x, cx - x, cx + x, cx - x,
                       cx + y, cx - y, cx + y, cx - y};
    const int py[8] = {cy + y, cy + y, cy - y, cy - y,
                       cy + x, cy + x, cy - x, cy - x};
    for (int i = 0; i < 8 && err == ESP_OK; i++) {
      err = gc9306_draw_pixel_locked_(d, px[i], py[i], color);
    }
    y++;
    if (e < 0) {
      e += 2 * y + 1;
    } else {
      x--;
      e += 2 * (y - x) + 1;
    }
  }
  gc9306_unlock_(d);
  return err;
}

static inline esp_err_t gc9306_draw_bitmap(gc9306_t *d, int x, int y, int w,
                                           int h, const uint16_t *pixels,
                                           bool swapped) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (pixels == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  const int full_w = w;
  int sx = 0, sy = 0;
  if (!gc9306_clip_(d, &x, &y, &w, &h, &sx, &sy)) {
    return ESP_OK;
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }

  const size_t bufpx = d->dma_buf_bytes / 2;
  int rows_per_chunk = (int)(bufpx / (size_t)w);
  if (rows_per_chunk < 1) {
    rows_per_chunk = 1;
  }

  uint16_t *b = (uint16_t *)d->dma_buf;
  int row = 0;
  while (row < h && err == ESP_OK) {
    int chunk_h = h - row;
    if (chunk_h > rows_per_chunk) {
      chunk_h = rows_per_chunk;
    }
    for (int r = 0; r < chunk_h; r++) {
      const uint16_t *src =
          pixels + (size_t)(sy + row + r) * (size_t)full_w + (size_t)sx;
      uint16_t *dst = b + (size_t)r * (size_t)w;
      if (swapped) {
        memcpy(dst, src, (size_t)w * 2);
      } else {
        for (int c = 0; c < w; c++) {
          dst[c] = __builtin_bswap16(src[c]);
        }
      }
    }
    size_t chunk_px = (size_t)w * (size_t)chunk_h;
    err = gc9306_write_window_dma_(d, x, y + row, x + w - 1,
                                   y + row + chunk_h - 1, chunk_px * 2);
    row += chunk_h;
  }

  gc9306_unlock_(d);
  return err;
}

static inline esp_err_t gc9306_draw_char(gc9306_t *d, int x, int y, char c,
                                         uint16_t fg, int32_t bg, int scale) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }
  err = gc9306_draw_char_locked_(d, x, y, c, fg, bg, scale);
  gc9306_unlock_(d);
  return err;
}

static inline int gc9306_text_width(const char *s, int scale) {
  if (s == NULL) {
    return 0;
  }
  if (scale < 1) {
    scale = 1;
  }
  int n = 0;
  while (s[n] != '\0' && s[n] != '\n') {
    n++;
  }
  return n * TFT_FONT_CELL_W * scale;
}

static inline int gc9306_text_height(int scale) {
  return TFT_FONT_CELL_H * (scale < 1 ? 1 : scale);
}

static inline esp_err_t gc9306_draw_string(gc9306_t *d, int x, int y,
                                           const char *s, uint16_t fg,
                                           int32_t bg, int scale) {
  esp_err_t err = gc9306_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (s == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if (scale < 1) {
    scale = 1;
  }
  if ((err = gc9306_lock_(d)) != ESP_OK) {
    return err;
  }
  int cx = x, cy = y;
  for (; *s != '\0' && err == ESP_OK; s++) {
    if (*s == '\n') {
      cx = x;
      cy += TFT_FONT_CELL_H * scale;
      continue;
    }
    err = gc9306_draw_char_locked_(d, cx, cy, *s, fg, bg, scale);
    cx += TFT_FONT_CELL_W * scale;
  }
  gc9306_unlock_(d);
  return err;
}

#ifdef __cplusplus
}
#endif

#endif /* GC9306_H */
