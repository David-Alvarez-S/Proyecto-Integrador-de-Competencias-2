#ifndef ILI9341_H
#define ILI9341_H

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

/* ------------------------------------------------------------------------- */
/* Parametros (sobrescribibles con #define antes del #include)               */
/* ------------------------------------------------------------------------- */

#ifndef ILI9341_DEFAULT_CLOCK_HZ
#define ILI9341_DEFAULT_CLOCK_HZ 26000000
#endif
/** Tamano del buffer DMA en bytes (par). Tambien es el max_transfer_sz del bus.
 */
#ifndef ILI9341_DEFAULT_DMA_BYTES
#define ILI9341_DEFAULT_DMA_BYTES 4096U
#endif
#ifndef ILI9341_LOCK_TIMEOUT_MS
#define ILI9341_LOCK_TIMEOUT_MS 1000U
#endif
#ifndef ILI9341_BL_PWM_FREQ_HZ
#define ILI9341_BL_PWM_FREQ_HZ 5000U
#endif

/** Usar como 'bg' en draw_char/draw_string para texto con fondo transparente.
 */
#define ILI9341_TRANSPARENT (-1)

/* Colores RGB565 (orden del host) */
#define ILI9341_RGB565(r, g, b)                                                \
  ((uint16_t)((((uint16_t)(r) & 0xF8U) << 8) |                                 \
              (((uint16_t)(g) & 0xFCU) << 3) | ((uint16_t)(b) >> 3)))
#define ILI9341_BLACK 0x0000
#define ILI9341_WHITE 0xFFFF
#define ILI9341_RED 0xF800
#define ILI9341_GREEN 0x07E0
#define ILI9341_BLUE 0x001F
#define ILI9341_YELLOW 0xFFE0
#define ILI9341_CYAN 0x07FF
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_ORANGE 0xFD20
#define ILI9341_GRAY 0x8410
#define ILI9341_DARKGRAY 0x4208

/* Comandos ILI9341 */
#define ILI9341_CMD_SWRESET 0x01
#define ILI9341_CMD_SLPIN 0x10
#define ILI9341_CMD_SLPOUT 0x11
#define ILI9341_CMD_INVOFF 0x20
#define ILI9341_CMD_INVON 0x21
#define ILI9341_CMD_DISPOFF 0x28
#define ILI9341_CMD_DISPON 0x29
#define ILI9341_CMD_CASET 0x2A
#define ILI9341_CMD_PASET 0x2B
#define ILI9341_CMD_RAMWR 0x2C
#define ILI9341_CMD_MADCTL 0x36
#define ILI9341_CMD_PIXFMT 0x3A

/* ------------------------------------------------------------------------- */
/* Tipos                                                                     */
/* ------------------------------------------------------------------------- */

typedef struct {
  spi_host_device_t host; /**< SPI2_HOST (HSPI) o SPI3_HOST (VSPI)      */
  bool init_bus;          /**< true: esta libreria inicializa el bus    */
  gpio_num_t mosi;        /**< Solo si init_bus                         */
  gpio_num_t miso;        /**< Solo si init_bus (GPIO_NUM_NC si no hay) */
  gpio_num_t sclk;        /**< Solo si init_bus                         */
  gpio_num_t cs;
  gpio_num_t dc;
  gpio_num_t rst; /**< GPIO_NUM_NC si RESET va a EN/3V3         */
  gpio_num_t bl;  /**< Backlight; GPIO_NUM_NC si no se controla */
  bool bl_active_high;
  bool bl_pwm; /**< true: brillo por LEDC; false: on/off     */
  ledc_timer_t bl_timer;
  ledc_channel_t bl_channel;
  uint8_t bl_initial;   /**< Brillo inicial 0..100 al terminar init   */
  int clock_hz;         /**< 0 = ILI9341_DEFAULT_CLOCK_HZ             */
  size_t dma_buf_bytes; /**< 0 = ILI9341_DEFAULT_DMA_BYTES         */
  tft_rotation_t rotation;
  bool bgr;    /**< true: orden BGR (tipico en estos modulos)*/
  bool invert; /**< true: inversion de colores (INVON)       */
} ili9341_config_t;

/** Config tipica. Ajusta el resto de campos si hace falta. */
#define ILI9341_CONFIG_DEFAULT(_mosi, _miso, _sclk, _cs, _dc, _rst, _bl)       \
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
  ili9341_config_t cfg;
  spi_device_handle_t spi;
  SemaphoreHandle_t lock;
  uint8_t *dma_buf;
  size_t dma_buf_bytes;
  uint16_t width; /**< Ancho con la rotacion actual */
  uint16_t height;
  tft_rotation_t rotation;
  bool bus_owned;
  bool bl_ready;
  bool initialized;
} ili9341_t;

/** Transaccion SPI + nivel del pin DC (t DEBE ser el primer miembro). */
typedef struct {
  spi_transaction_t t;
  gpio_num_t dc_pin;
  uint8_t dc;
} ili9341_txn_t;

/* ------------------------------------------------------------------------- */
/* Capa baja (internas)                                                      */
/* ------------------------------------------------------------------------- */

/** Pone DC antes de cada transaccion. Se ejecuta en contexto de interrupcion.
 */
__attribute__((unused)) static void IRAM_ATTR
ili9341_pre_cb_(spi_transaction_t *t) {
  const ili9341_txn_t *x = (const ili9341_txn_t *)t;
  gpio_set_level(x->dc_pin, x->dc);
}

static inline esp_err_t ili9341_lock_(ili9341_t *d) {
  return (xSemaphoreTake(d->lock, pdMS_TO_TICKS(ILI9341_LOCK_TIMEOUT_MS)) ==
          pdTRUE)
             ? ESP_OK
             : ESP_ERR_TIMEOUT;
}

static inline void ili9341_unlock_(ili9341_t *d) { xSemaphoreGive(d->lock); }

static inline esp_err_t ili9341_check_(const ili9341_t *d) {
  return (d != NULL && d->initialized) ? ESP_OK : ESP_ERR_INVALID_STATE;
}

static inline esp_err_t ili9341_tx_cmd_(ili9341_t *d, uint8_t cmd) {
  ili9341_txn_t x;
  memset(&x, 0, sizeof(x));
  x.t.length = 8;
  x.t.flags = SPI_TRANS_USE_TXDATA;
  x.t.tx_data[0] = cmd;
  x.dc_pin = d->cfg.dc;
  x.dc = 0;
  return spi_device_polling_transmit(d->spi, &x.t);
}

/** Datos de parametros (<= 16 bytes). Copia a pila: seguro con tablas en flash.
 */
static inline esp_err_t ili9341_tx_params_(ili9341_t *d, const uint8_t *p,
                                           size_t n) {
  uint8_t tmp[16];
  if (n == 0 || n > sizeof(tmp)) {
    return ESP_ERR_INVALID_ARG;
  }
  memcpy(tmp, p, n);

  ili9341_txn_t x;
  memset(&x, 0, sizeof(x));
  x.t.length = n * 8;
  if (n <= 4) {
    x.t.flags = SPI_TRANS_USE_TXDATA;
    memcpy(x.t.tx_data, tmp, n);
  } else {
    x.t.tx_buffer = tmp;
  }
  x.dc_pin = d->cfg.dc;
  x.dc = 1;
  return spi_device_polling_transmit(d->spi, &x.t);
}

static inline esp_err_t ili9341_tx_cmd_params_(ili9341_t *d, uint8_t cmd,
                                               const uint8_t *p, size_t n) {
  esp_err_t err = ili9341_tx_cmd_(d, cmd);
  if (err != ESP_OK || n == 0) {
    return err;
  }
  return ili9341_tx_params_(d, p, n);
}

/** Envia 'bytes' del buffer DMA (pixeles) por DMA; la tarea duerme mientras. */
static inline esp_err_t ili9341_tx_pixels_(ili9341_t *d, size_t bytes) {
  ili9341_txn_t x;
  memset(&x, 0, sizeof(x));
  x.t.length = bytes * 8;
  x.t.tx_buffer = d->dma_buf;
  x.dc_pin = d->cfg.dc;
  x.dc = 1;
  return spi_device_transmit(d->spi, &x.t);
}

/** Define la ventana de escritura y deja el controlador listo para RAMWR. */
static inline esp_err_t ili9341_set_window_locked_(ili9341_t *d, int x0, int y0,
                                                   int x1, int y1) {
  uint8_t c[4] = {(uint8_t)(x0 >> 8), (uint8_t)x0, (uint8_t)(x1 >> 8),
                  (uint8_t)x1};
  uint8_t r[4] = {(uint8_t)(y0 >> 8), (uint8_t)y0, (uint8_t)(y1 >> 8),
                  (uint8_t)y1};
  esp_err_t err = ili9341_tx_cmd_params_(d, ILI9341_CMD_CASET, c, 4);
  if (err != ESP_OK) {
    return err;
  }
  err = ili9341_tx_cmd_params_(d, ILI9341_CMD_PASET, r, 4);
  if (err != ESP_OK) {
    return err;
  }
  return ili9341_tx_cmd_(d, ILI9341_CMD_RAMWR);
}

/** Recorta un rectangulo a la pantalla. false si queda vacio. */
static inline bool ili9341_clip_(const ili9341_t *d, int *x, int *y, int *w,
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

static inline esp_err_t ili9341_fill_rect_locked_(ili9341_t *d, int x, int y,
                                                  int w, int h,
                                                  uint16_t color) {
  if (!ili9341_clip_(d, &x, &y, &w, &h, NULL, NULL)) {
    return ESP_OK;
  }
  esp_err_t err = ili9341_set_window_locked_(d, x, y, x + w - 1, y + h - 1);
  if (err != ESP_OK) {
    return err;
  }

  size_t total = (size_t)w * (size_t)h;
  size_t bufpx = d->dma_buf_bytes / 2;
  size_t first = total < bufpx ? total : bufpx;
  uint16_t sw = __builtin_bswap16(color);
  uint16_t *b = (uint16_t *)d->dma_buf;
  for (size_t i = 0; i < first; i++) {
    b[i] = sw;
  }
  while (total > 0) {
    size_t n = total < bufpx ? total : bufpx;
    err = ili9341_tx_pixels_(d, n * 2);
    if (err != ESP_OK) {
      return err;
    }
    total -= n;
  }
  return ESP_OK;
}

static inline esp_err_t ili9341_draw_pixel_locked_(ili9341_t *d, int x, int y,
                                                   uint16_t color) {
  if (x < 0 || y < 0 || x >= (int)d->width || y >= (int)d->height) {
    return ESP_OK;
  }
  esp_err_t err = ili9341_set_window_locked_(d, x, y, x, y);
  if (err != ESP_OK) {
    return err;
  }
  uint8_t px[2] = {(uint8_t)(color >> 8), (uint8_t)color};
  return ili9341_tx_params_(d, px, 2);
}

static inline esp_err_t ili9341_draw_char_locked_(ili9341_t *d, int x, int y,
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

  /* Camino rapido: fondo opaco, celda completa en pantalla y cabe en el buffer
   */
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
    esp_err_t err = ili9341_set_window_locked_(d, x, y, x + cw - 1, y + ch - 1);
    if (err != ESP_OK) {
      return err;
    }
    return ili9341_tx_pixels_(d, cell_px * 2);
  }

  /* Camino general: fondo (si lo hay) + pixeles encendidos como rectangulos */
  esp_err_t err = ESP_OK;
  if (bg >= 0) {
    err = ili9341_fill_rect_locked_(d, x, y, cw, ch, (uint16_t)bg);
    if (err != ESP_OK) {
      return err;
    }
  }
  for (int gx = 0; gx < TFT_FONT_GLYPH_W; gx++) {
    int gy = 0;
    while (gy < 8) { /* agrupa pixeles verticales contiguos */
      if (!((g[gx] >> gy) & 1U)) {
        gy++;
        continue;
      }
      int run = 1;
      while (gy + run < 8 && ((g[gx] >> (gy + run)) & 1U)) {
        run++;
      }
      err = ili9341_fill_rect_locked_(d, x + gx * scale, y + gy * scale, scale,
                                      run * scale, fg);
      if (err != ESP_OK) {
        return err;
      }
      gy += run;
    }
  }
  return ESP_OK;
}

/* ------------------------------------------------------------------------- */
/* Backlight                                                                 */
/* ------------------------------------------------------------------------- */

static inline esp_err_t ili9341_backlight_init_(ili9341_t *d) {
  if (d->cfg.bl == GPIO_NUM_NC) {
    return ESP_OK;
  }
  esp_err_t err;
  if (d->cfg.bl_pwm) {
    ledc_timer_config_t tc = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = d->cfg.bl_timer,
        .freq_hz = ILI9341_BL_PWM_FREQ_HZ,
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
        .duty = d->cfg.bl_active_high ? 0U : 1023U, /* apagado */
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

/** Brillo 0..100 (sin PWM: 0 = apagado, >0 = encendido). */
static inline esp_err_t ili9341_set_backlight(ili9341_t *d, uint8_t percent) {
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

/* ------------------------------------------------------------------------- */
/* Ciclo de vida                                                             */
/* ------------------------------------------------------------------------- */

static inline void ili9341_cleanup_(ili9341_t *d) {
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

static inline esp_err_t ili9341_run_init_seq_(ili9341_t *d) {
  typedef struct {
    uint8_t cmd;
    uint8_t n;
    uint8_t data[15];
  } step_t;
  static const step_t seq[] = {
      {0xEF, 3, {0x03, 0x80, 0x02}},
      {0xCF, 3, {0x00, 0xC1, 0x30}},
      {0xED, 4, {0x64, 0x03, 0x12, 0x81}},
      {0xE8, 3, {0x85, 0x00, 0x78}},
      {0xCB, 5, {0x39, 0x2C, 0x00, 0x34, 0x02}},
      {0xF7, 1, {0x20}},
      {0xEA, 2, {0x00, 0x00}},
      {0xC0, 1, {0x23}},             /* Power control 1 */
      {0xC1, 1, {0x10}},             /* Power control 2 */
      {0xC5, 2, {0x3E, 0x28}},       /* VCOM 1 */
      {0xC7, 1, {0x86}},             /* VCOM 2 */
      {0x3A, 1, {0x55}},             /* 16 bits/pixel (RGB565) */
      {0xB1, 2, {0x00, 0x18}},       /* Frame rate */
      {0xB6, 3, {0x08, 0x82, 0x27}}, /* Display function */
      {0xF2, 1, {0x00}},             /* 3-gamma off */
      {0x26, 1, {0x01}},             /* Gamma curve */
      {0xE0,
       15,
       {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03,
        0x0E, 0x09, 0x00}},
      {0xE1,
       15,
       {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C,
        0x31, 0x36, 0x0F}},
  };
  for (size_t i = 0; i < sizeof(seq) / sizeof(seq[0]); i++) {
    esp_err_t err =
        ili9341_tx_cmd_params_(d, seq[i].cmd, seq[i].data, seq[i].n);
    if (err != ESP_OK) {
      return err;
    }
  }
  return ESP_OK;
}

/**
 * @brief Inicializa SPI, GPIO, buffer DMA, backlight y el controlador.
 *        Al terminar la pantalla queda en negro, encendida y con el brillo
 *        bl_initial.
 */
static inline esp_err_t ili9341_init(ili9341_t *d,
                                     const ili9341_config_t *cfg) {
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
    d->cfg.clock_hz = ILI9341_DEFAULT_CLOCK_HZ;
  }
  if (d->cfg.dma_buf_bytes == 0) {
    d->cfg.dma_buf_bytes = ILI9341_DEFAULT_DMA_BYTES;
  }
  d->cfg.dma_buf_bytes &= ~(size_t)1; /* par */
  if (d->cfg.dma_buf_bytes < 128) {
    return ESP_ERR_INVALID_ARG;
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

  /* GPIO de control: DC y RST */
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

  /* Bus SPI (opcional) y dispositivo */
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
    } else if (err !=
               ESP_ERR_INVALID_STATE) { /* ya inicializado: se reutiliza */
      goto fail;
    }
  }
  spi_device_interface_config_t dev = {
      .clock_speed_hz = d->cfg.clock_hz,
      .mode = 0,
      .spics_io_num = d->cfg.cs,
      .queue_size = 4,
      .pre_cb = ili9341_pre_cb_,
  };
  if ((err = spi_bus_add_device(d->cfg.host, &dev, &d->spi)) != ESP_OK) {
    goto fail;
  }

  (void)ili9341_backlight_init_(d); /* si falla, la pantalla funciona igual */

  /* Reset por hardware y por software */
  if (d->cfg.rst != GPIO_NUM_NC) {
    gpio_set_level(d->cfg.rst, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(d->cfg.rst, 1);
    vTaskDelay(pdMS_TO_TICKS(130));
  }
  d->initialized = true; /* a partir de aqui se usan las funciones normales */

  if ((err = ili9341_tx_cmd_(d, ILI9341_CMD_SWRESET)) != ESP_OK) {
    goto fail_init;
  }
  vTaskDelay(pdMS_TO_TICKS(150));
  if ((err = ili9341_run_init_seq_(d)) != ESP_OK) {
    goto fail_init;
  }
  if ((err = ili9341_tx_cmd_(d, ILI9341_CMD_SLPOUT)) != ESP_OK) {
    goto fail_init;
  }
  vTaskDelay(pdMS_TO_TICKS(130));

  uint8_t madctl = tft_rotation_madctl(d->rotation, d->cfg.bgr);
  if ((err = ili9341_tx_cmd_params_(d, ILI9341_CMD_MADCTL, &madctl, 1)) !=
      ESP_OK) {
    goto fail_init;
  }
  if (d->cfg.invert &&
      (err = ili9341_tx_cmd_(d, ILI9341_CMD_INVON)) != ESP_OK) {
    goto fail_init;
  }
  /* Limpiar la RAM antes de encender para no mostrar basura */
  if ((err = ili9341_fill_rect_locked_(d, 0, 0, d->width, d->height,
                                       ILI9341_BLACK)) != ESP_OK) {
    goto fail_init;
  }
  if ((err = ili9341_tx_cmd_(d, ILI9341_CMD_DISPON)) != ESP_OK) {
    goto fail_init;
  }
  vTaskDelay(pdMS_TO_TICKS(20));
  (void)ili9341_set_backlight(d, d->cfg.bl_initial);
  return ESP_OK;

fail_init:
  d->initialized = false;
fail:
  ili9341_cleanup_(d);
  return err;
}

/** Apaga el backlight y libera todo (incluido el bus si lo inicializo init). */
static inline esp_err_t ili9341_deinit(ili9341_t *d) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  (void)ili9341_set_backlight(d, 0);
  (void)ili9341_tx_cmd_(d, ILI9341_CMD_DISPOFF);
  d->initialized = false;
  ili9341_unlock_(d);
  ili9341_cleanup_(d);
  return ESP_OK;
}

/* ------------------------------------------------------------------------- */
/* Control del panel                                                         */
/* ------------------------------------------------------------------------- */

static inline uint16_t ili9341_width(const ili9341_t *d) {
  return d ? d->width : 0;
}
static inline uint16_t ili9341_height(const ili9341_t *d) {
  return d ? d->height : 0;
}
static inline tft_rotation_t ili9341_get_rotation(const ili9341_t *d) {
  return d ? d->rotation : TFT_ROTATION_0;
}

static inline esp_err_t ili9341_set_rotation(ili9341_t *d, tft_rotation_t rot) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((unsigned)rot > 3U) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  uint8_t madctl = tft_rotation_madctl(rot, d->cfg.bgr);
  err = ili9341_tx_cmd_params_(d, ILI9341_CMD_MADCTL, &madctl, 1);
  if (err == ESP_OK) {
    d->rotation = rot;
    tft_rotation_size(rot, &d->width, &d->height);
  }
  ili9341_unlock_(d);
  return err;
}

static inline esp_err_t ili9341_simple_cmd_(ili9341_t *d, uint8_t cmd,
                                            uint32_t delay_ms) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  err = ili9341_tx_cmd_(d, cmd);
  ili9341_unlock_(d);
  if (err == ESP_OK && delay_ms) {
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
  }
  return err;
}

static inline esp_err_t ili9341_display_on(ili9341_t *d, bool on) {
  return ili9341_simple_cmd_(d, on ? ILI9341_CMD_DISPON : ILI9341_CMD_DISPOFF,
                             0);
}

static inline esp_err_t ili9341_invert(ili9341_t *d, bool on) {
  return ili9341_simple_cmd_(d, on ? ILI9341_CMD_INVON : ILI9341_CMD_INVOFF, 0);
}

/** Modo de bajo consumo: true = dormir (la RAM se conserva), false = despertar.
 */
static inline esp_err_t ili9341_sleep(ili9341_t *d, bool sleep) {
  return ili9341_simple_cmd_(d, sleep ? ILI9341_CMD_SLPIN : ILI9341_CMD_SLPOUT,
                             120);
}

/* ------------------------------------------------------------------------- */
/* Dibujo                                                                    */
/* ------------------------------------------------------------------------- */

static inline esp_err_t ili9341_fill_rect(ili9341_t *d, int x, int y, int w,
                                          int h, uint16_t color) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  err = ili9341_fill_rect_locked_(d, x, y, w, h, color);
  ili9341_unlock_(d);
  return err;
}

static inline esp_err_t ili9341_fill_screen(ili9341_t *d, uint16_t color) {
  return ili9341_fill_rect(d, 0, 0, d ? d->width : 0, d ? d->height : 0, color);
}

static inline esp_err_t ili9341_draw_pixel(ili9341_t *d, int x, int y,
                                           uint16_t color) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  err = ili9341_draw_pixel_locked_(d, x, y, color);
  ili9341_unlock_(d);
  return err;
}

static inline esp_err_t ili9341_draw_hline(ili9341_t *d, int x, int y, int w,
                                           uint16_t color) {
  return ili9341_fill_rect(d, x, y, w, 1, color);
}

static inline esp_err_t ili9341_draw_vline(ili9341_t *d, int x, int y, int h,
                                           uint16_t color) {
  return ili9341_fill_rect(d, x, y, 1, h, color);
}

/** Linea de Bresenham. */
static inline esp_err_t ili9341_draw_line(ili9341_t *d, int x0, int y0, int x1,
                                          int y1, uint16_t color) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (y0 == y1) {
    return ili9341_draw_hline(d, x0 < x1 ? x0 : x1, y0,
                              (x0 < x1 ? x1 - x0 : x0 - x1) + 1, color);
  }
  if (x0 == x1) {
    return ili9341_draw_vline(d, x0, y0 < y1 ? y0 : y1,
                              (y0 < y1 ? y1 - y0 : y0 - y1) + 1, color);
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  int dx = x1 > x0 ? x1 - x0 : x0 - x1;
  int dy = y1 > y0 ? y0 - y1 : y1 - y0; /* -|dy| */
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int e = dx + dy;
  for (;;) {
    err = ili9341_draw_pixel_locked_(d, x0, y0, color);
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
  ili9341_unlock_(d);
  return err;
}

static inline esp_err_t ili9341_draw_rect(ili9341_t *d, int x, int y, int w,
                                          int h, uint16_t color) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (w <= 0 || h <= 0) {
    return ESP_OK;
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  err = ili9341_fill_rect_locked_(d, x, y, w, 1, color);
  if (err == ESP_OK) {
    err = ili9341_fill_rect_locked_(d, x, y + h - 1, w, 1, color);
  }
  if (err == ESP_OK) {
    err = ili9341_fill_rect_locked_(d, x, y, 1, h, color);
  }
  if (err == ESP_OK) {
    err = ili9341_fill_rect_locked_(d, x + w - 1, y, 1, h, color);
  }
  ili9341_unlock_(d);
  return err;
}

static inline esp_err_t ili9341_fill_circle(ili9341_t *d, int cx, int cy, int r,
                                            uint16_t color) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (r < 0) {
    return ESP_OK;
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  for (int dy = -r; dy <= r && err == ESP_OK; dy++) {
    int dx = (int)sqrtf((float)(r * r - dy * dy) + 0.5f);
    err = ili9341_fill_rect_locked_(d, cx - dx, cy + dy, 2 * dx + 1, 1, color);
  }
  ili9341_unlock_(d);
  return err;
}

/** Circunferencia (algoritmo del punto medio). */
static inline esp_err_t ili9341_draw_circle(ili9341_t *d, int cx, int cy, int r,
                                            uint16_t color) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (r < 0) {
    return ESP_OK;
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  int x = r, y = 0, e = 1 - r;
  while (x >= y && err == ESP_OK) {
    const int px[8] = {cx + x, cx - x, cx + x, cx - x,
                       cx + y, cx - y, cx + y, cx - y};
    const int py[8] = {cy + y, cy + y, cy - y, cy - y,
                       cy + x, cy + x, cy - x, cy - x};
    for (int i = 0; i < 8 && err == ESP_OK; i++) {
      err = ili9341_draw_pixel_locked_(d, px[i], py[i], color);
    }
    y++;
    if (e < 0) {
      e += 2 * y + 1;
    } else {
      x--;
      e += 2 * (y - x) + 1;
    }
  }
  ili9341_unlock_(d);
  return err;
}

/**
 * @brief Dibuja un bitmap RGB565 de w x h (con recorte automatico).
 * @param pixels   Filas consecutivas (puede estar en flash o en cualquier RAM:
 *                 se copia por tramos al buffer DMA).
 * @param swapped  false: pixeles en orden del host (lo normal);
 *                 true : ya vienen con los bytes intercambiados (big-endian),
 *                 p. ej. LVGL con LV_COLOR_16_SWAP.
 * Sirve directamente como funcion "flush" para LVGL.
 */
static inline esp_err_t ili9341_draw_bitmap(ili9341_t *d, int x, int y, int w,
                                            int h, const uint16_t *pixels,
                                            bool swapped) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (pixels == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  const int full_w = w;
  int sx = 0, sy = 0;
  if (!ili9341_clip_(d, &x, &y, &w, &h, &sx, &sy)) {
    return ESP_OK;
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  err = ili9341_set_window_locked_(d, x, y, x + w - 1, y + h - 1);

  const size_t bufpx = d->dma_buf_bytes / 2;
  uint16_t *b = (uint16_t *)d->dma_buf;
  size_t filled = 0;
  for (int row = 0; row < h && err == ESP_OK; row++) {
    const uint16_t *src =
        pixels + (size_t)(sy + row) * (size_t)full_w + (size_t)sx;
    int left = w;
    while (left > 0 && err == ESP_OK) {
      size_t n = bufpx - filled;
      if ((size_t)left < n) {
        n = (size_t)left;
      }
      if (swapped) {
        memcpy(b + filled, src, n * 2);
      } else {
        for (size_t i = 0; i < n; i++) {
          b[filled + i] = __builtin_bswap16(src[i]);
        }
      }
      filled += n;
      src += n;
      left -= (int)n;
      if (filled == bufpx) {
        err = ili9341_tx_pixels_(d, filled * 2);
        filled = 0;
      }
    }
  }
  if (err == ESP_OK && filled > 0) {
    err = ili9341_tx_pixels_(d, filled * 2);
  }
  ili9341_unlock_(d);
  return err;
}

/* ------------------------------------------------------------------------- */
/* Texto                                                                     */
/* ------------------------------------------------------------------------- */

/** Dibuja un caracter. bg = color RGB565 o ILI9341_TRANSPARENT. scale >= 1. */
static inline esp_err_t ili9341_draw_char(ili9341_t *d, int x, int y, char c,
                                          uint16_t fg, int32_t bg, int scale) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  err = ili9341_draw_char_locked_(d, x, y, c, fg, bg, scale);
  ili9341_unlock_(d);
  return err;
}

/** Ancho en pixeles de la primera linea de 's'. */
static inline int ili9341_text_width(const char *s, int scale) {
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

static inline int ili9341_text_height(int scale) {
  return TFT_FONT_CELL_H * (scale < 1 ? 1 : scale);
}

/**
 * @brief Dibuja una cadena (soporta '\n'). No hace ajuste de linea automatico:
 *        lo que se sale de la pantalla se recorta. La operacion completa es
 *        atomica respecto a otras tareas.
 */
static inline esp_err_t ili9341_draw_string(ili9341_t *d, int x, int y,
                                            const char *s, uint16_t fg,
                                            int32_t bg, int scale) {
  esp_err_t err = ili9341_check_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (s == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if (scale < 1) {
    scale = 1;
  }
  if ((err = ili9341_lock_(d)) != ESP_OK) {
    return err;
  }
  int cx = x, cy = y;
  for (; *s != '\0' && err == ESP_OK; s++) {
    if (*s == '\n') {
      cx = x;
      cy += TFT_FONT_CELL_H * scale;
      continue;
    }
    err = ili9341_draw_char_locked_(d, cx, cy, *s, fg, bg, scale);
    cx += TFT_FONT_CELL_W * scale;
  }
  ili9341_unlock_(d);
  return err;
}

#ifdef __cplusplus
}
#endif

#endif /* ILI9341_H */
