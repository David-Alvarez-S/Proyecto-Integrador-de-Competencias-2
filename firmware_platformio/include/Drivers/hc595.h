#ifndef HC595_H
#define HC595_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#ifndef HC595_DEFAULT_CLOCK_HZ
#define HC595_DEFAULT_CLOCK_HZ 2000000
#endif

#ifndef HC595_MAX_CLOCK_HZ
#define HC595_MAX_CLOCK_HZ 10000000
#endif
#ifndef HC595_DEFAULT_LOCK_TIMEOUT_MS
#define HC595_DEFAULT_LOCK_TIMEOUT_MS 100U
#endif
#ifndef HC595_DEINIT_GRACE_MS
#define HC595_DEINIT_GRACE_MS 20U
#endif

#define HC595_MAX_OUTPUTS 16

typedef enum {
  HC595_TRANSPORT_SPI = 0,
  HC595_TRANSPORT_GPIO,
} hc595_transport_t;

typedef struct {
  hc595_transport_t transport;
  spi_host_device_t host; 
  bool init_bus;        
  gpio_num_t data;      
  gpio_num_t clk;      
  gpio_num_t latch;     
  gpio_num_t oe;       
  int clock_hz;        
  uint8_t chips;      
  uint16_t invert_mask; 
  uint16_t initial_state;   
  bool enable_on_init;      
  uint32_t lock_timeout_ms; 
} hc595_config_t;


#define HC595_CONFIG_SHARED_SPI(_host, _latch, _oe)                            \
  {                                                                            \
      .transport = HC595_TRANSPORT_SPI,                                        \
      .host = (_host),                                                         \
      .init_bus = false,                                                       \
      .data = GPIO_NUM_NC,                                                     \
      .clk = GPIO_NUM_NC,                                                      \
      .latch = (_latch),                                                       \
      .oe = (_oe),                                                             \
      .clock_hz = 0,                                                           \
      .chips = 2,                                                              \
      .invert_mask = 0,                                                        \
      .initial_state = 0,                                                      \
      .enable_on_init = true,                                                  \
      .lock_timeout_ms = 0,                                                    \
  }


#define HC595_CONFIG_DEFAULT(_data, _clk, _latch, _oe)                         \
  {                                                                            \
      .transport = HC595_TRANSPORT_SPI,                                        \
      .host = SPI2_HOST,                                                       \
      .init_bus = true,                                                        \
      .data = (_data),                                                         \
      .clk = (_clk),                                                           \
      .latch = (_latch),                                                       \
      .oe = (_oe),                                                             \
      .clock_hz = 0,                                                           \
      .chips = 2,                                                              \
      .invert_mask = 0,                                                        \
      .initial_state = 0,                                                      \
      .enable_on_init = true,                                                  \
      .lock_timeout_ms = 0,                                                    \
  }

typedef struct {
  bool active;
  bool phase_on;
  bool infinite;
  uint32_t on_ms;
  uint32_t off_ms;
  uint32_t cycles_left;
  int64_t due_us;
} hc595_blink_t;

typedef struct {
  hc595_config_t cfg;
  spi_device_handle_t spi;
  SemaphoreHandle_t lock;
  esp_timer_handle_t blink_timer;
  hc595_blink_t blink[HC595_MAX_OUTPUTS];

  uint16_t state;      
  uint16_t claimed;    
  uint16_t valid_mask; 
  uint32_t lock_timeout_ms;
  uint32_t tx_count; 
  bool dirty;        
  bool output_enabled;
  bool bus_owned;
  bool closing;
  bool initialized;
} hc595_t;

static inline esp_err_t hc595_lock_(hc595_t *h) {
  return (xSemaphoreTake(h->lock, pdMS_TO_TICKS(h->lock_timeout_ms)) == pdTRUE)
             ? ESP_OK
             : ESP_ERR_TIMEOUT;
}

static inline void hc595_unlock_(hc595_t *h) { xSemaphoreGive(h->lock); }

static inline esp_err_t hc595_check_(const hc595_t *h) {
  return (h != NULL && h->initialized && !h->closing) ? ESP_OK
                                                      : ESP_ERR_INVALID_STATE;
}
static inline esp_err_t hc595_flush_locked_(hc595_t *h) {
  const uint16_t w =
      (uint16_t)((h->state ^ h->cfg.invert_mask) & h->valid_mask);
  esp_err_t err = ESP_OK;

  if (h->cfg.transport == HC595_TRANSPORT_SPI) {
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.flags = SPI_TRANS_USE_TXDATA;
    if (h->cfg.chips == 2) {
      t.length = 16;
      t.tx_data[0] = (uint8_t)(w >> 8);   
      t.tx_data[1] = (uint8_t)(w & 0xFF); 
    } else {
      t.length = 8;
      t.tx_data[0] = (uint8_t)(w & 0xFF);
    }
    err = spi_device_polling_transmit(h->spi, &t); 
  } else {
    for (int i = (int)h->cfg.chips * 8 - 1; i >= 0; i--) {
      gpio_set_level(h->cfg.data, (w >> i) & 1U);
      gpio_set_level(h->cfg.clk, 1);
      gpio_set_level(h->cfg.clk, 0);
    }
    gpio_set_level(h->cfg.latch, 1);
    gpio_set_level(h->cfg.latch, 0);
  }
  h->tx_count++;
  h->dirty = (err != ESP_OK);
  return err;
}


static inline esp_err_t hc595_update_locked_(hc595_t *h, uint16_t mask,
                                             uint16_t val) {
  uint16_t ns = (uint16_t)((h->state & ~mask) | (val & mask));
  ns &= h->valid_mask;
  if (ns == h->state && !h->dirty) {
    return ESP_OK; 
  }
  h->state = ns;
  return hc595_flush_locked_(h);
}

static inline void hc595_blink_rearm_locked_(hc595_t *h) {
  int64_t next = INT64_MAX;
  for (int i = 0; i < HC595_MAX_OUTPUTS; i++) {
    if (h->blink[i].active && h->blink[i].due_us < next) {
      next = h->blink[i].due_us;
    }
  }
  (void)esp_timer_stop(h->blink_timer);
  if (next == INT64_MAX) {
    return;
  }
  int64_t wait = next - esp_timer_get_time();
  if (wait < 100) {
    wait = 100;
  }
  (void)esp_timer_start_once(h->blink_timer, (uint64_t)wait);
}

static inline void hc595_blink_cancel_locked_(hc595_t *h, uint16_t mask) {
  bool any = false;
  for (int i = 0; i < HC595_MAX_OUTPUTS; i++) {
    if (((mask >> i) & 1U) && h->blink[i].active) {
      h->blink[i].active = false;
      any = true;
    }
  }
  if (any) {
    hc595_blink_rearm_locked_(h);
  }
}

static inline void hc595_blink_cb_(void *arg) {
  hc595_t *h = (hc595_t *)arg;
  if (xSemaphoreTake(h->lock, portMAX_DELAY) != pdTRUE) {
    return;
  }
  if (h->closing) {
    xSemaphoreGive(h->lock);
    return;
  }
  const int64_t now = esp_timer_get_time();
  uint16_t mask = 0, val = 0;

  for (int pin = 0; pin < HC595_MAX_OUTPUTS; pin++) {
    hc595_blink_t *b = &h->blink[pin];
    if (!b->active || b->due_us > now) {
      continue;
    }
    mask |= (uint16_t)(1U << pin);
    uint32_t period_ms;
    if (b->phase_on) { 
      b->phase_on = false;
      if (!b->infinite && --b->cycles_left == 0) {
        b->active = false;
        continue;
      }
      period_ms = b->off_ms;
    } else { 
      b->phase_on = true;
      val |= (uint16_t)(1U << pin);
      period_ms = b->on_ms;
    }
    b->due_us += (int64_t)period_ms * 1000;
    if (b->due_us < now) { 
      b->due_us = now + (int64_t)period_ms * 1000;
    }
  }
  if (mask) {
    (void)hc595_update_locked_(h, mask, val);
  }
  hc595_blink_rearm_locked_(h);
  xSemaphoreGive(h->lock);
}

static inline void hc595_cleanup_(hc595_t *h) {
  if (h->blink_timer) {
    esp_timer_delete(h->blink_timer);
    h->blink_timer = NULL;
  }
  if (h->spi) {
    spi_bus_remove_device(h->spi);
    h->spi = NULL;
  }
  if (h->bus_owned) {
    spi_bus_free(h->cfg.host);
    h->bus_owned = false;
  }
  if (h->lock) {
    vSemaphoreDelete(h->lock);
    h->lock = NULL;
  }
}

static inline esp_err_t hc595_init(hc595_t *h, const hc595_config_t *cfg) {
  if (h == NULL || cfg == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if (h->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  const bool uses_data_clk =
      (cfg->transport == HC595_TRANSPORT_GPIO) || cfg->init_bus;
  if ((uses_data_clk && (!GPIO_IS_VALID_OUTPUT_GPIO(cfg->data) ||
                         !GPIO_IS_VALID_OUTPUT_GPIO(cfg->clk))) ||
      !GPIO_IS_VALID_OUTPUT_GPIO(cfg->latch) ||
      (cfg->oe != GPIO_NUM_NC && !GPIO_IS_VALID_OUTPUT_GPIO(cfg->oe)) ||
      cfg->chips > 2 || cfg->clock_hz < 0 ||
      cfg->clock_hz > HC595_MAX_CLOCK_HZ) {
    return ESP_ERR_INVALID_ARG;
  }

  memset(h, 0, sizeof(*h));
  h->cfg = *cfg;
  if (h->cfg.chips == 0) {
    h->cfg.chips = 2;
  }
  if (h->cfg.clock_hz == 0) {
    h->cfg.clock_hz = HC595_DEFAULT_CLOCK_HZ;
  }
  h->lock_timeout_ms = cfg->lock_timeout_ms ? cfg->lock_timeout_ms
                                            : HC595_DEFAULT_LOCK_TIMEOUT_MS;
  h->valid_mask = (h->cfg.chips == 2) ? 0xFFFFU : 0x00FFU;
  h->state = (uint16_t)(cfg->initial_state & h->valid_mask);

  esp_err_t err = ESP_OK;
  h->lock = xSemaphoreCreateMutex();
  if (h->lock == NULL) {
    return ESP_ERR_NO_MEM;
  }

  if (h->cfg.oe != GPIO_NUM_NC) {
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << h->cfg.oe,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    if ((err = gpio_config(&io)) != ESP_OK) {
      goto fail;
    }
    gpio_set_level(h->cfg.oe, 1);
  }

  if (h->cfg.transport == HC595_TRANSPORT_SPI) {
    if (h->cfg.init_bus) {
      spi_bus_config_t bus = {
          .mosi_io_num = h->cfg.data,
          .miso_io_num = -1,
          .sclk_io_num = h->cfg.clk,
          .quadwp_io_num = -1,
          .quadhd_io_num = -1,
          .max_transfer_sz = 4,
      };
      err = spi_bus_initialize(h->cfg.host, &bus, SPI_DMA_DISABLED);
      if (err != ESP_OK) {
        goto fail;
      }
      h->bus_owned = true;
    }
    spi_device_interface_config_t dev = {
        .clock_speed_hz = h->cfg.clock_hz,
        .mode = 0,
        .spics_io_num = h->cfg.latch, 
        .queue_size = 2,
    };
    if ((err = spi_bus_add_device(h->cfg.host, &dev, &h->spi)) != ESP_OK) {
      goto fail;
    }
  } else if (h->cfg.transport == HC595_TRANSPORT_GPIO) {
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << h->cfg.data) | (1ULL << h->cfg.clk) |
                        (1ULL << h->cfg.latch),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    if ((err = gpio_config(&io)) != ESP_OK) {
      goto fail;
    }
    gpio_set_level(h->cfg.data, 0);
    gpio_set_level(h->cfg.clk, 0);
    gpio_set_level(h->cfg.latch, 0);
  } else {
    err = ESP_ERR_INVALID_ARG;
    goto fail;
  }

  esp_timer_create_args_t targs = {
      .callback = hc595_blink_cb_,
      .arg = h,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "hc595_blink",
  };
  if ((err = esp_timer_create(&targs, &h->blink_timer)) != ESP_OK) {
    goto fail;
  }
  h->initialized = true;
  h->dirty = true;
  if ((err = hc595_flush_locked_(h)) != ESP_OK) {
    h->initialized = false;
    goto fail;
  }


  if (h->cfg.oe != GPIO_NUM_NC && h->cfg.enable_on_init) {
    gpio_set_level(h->cfg.oe, 0);
    h->output_enabled = true;
  } else if (h->cfg.oe == GPIO_NUM_NC) {
    h->output_enabled = true; 
  }
  return ESP_OK;

fail:
  hc595_cleanup_(h);
  return err;
}

static inline esp_err_t hc595_deinit(hc595_t *h) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  h->closing = true;
  (void)esp_timer_stop(h->blink_timer);
  h->state = 0;
  h->cfg.invert_mask = 0;
  (void)hc595_flush_locked_(h);
  if (h->cfg.oe != GPIO_NUM_NC) {
    gpio_set_level(h->cfg.oe, 1);
  }
  hc595_unlock_(h);

  vTaskDelay(pdMS_TO_TICKS(HC595_DEINIT_GRACE_MS) +
             1); 
  h->initialized = false;
  hc595_cleanup_(h);
  return ESP_OK;
}

static inline esp_err_t hc595_claim(hc595_t *h, uint16_t mask) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if (mask == 0 || (mask & ~h->valid_mask)) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  if (h->claimed & mask) {
    err = ESP_ERR_INVALID_STATE; 
  } else {
    h->claimed |= mask;
    hc595_blink_cancel_locked_(h, mask); 
  }
  hc595_unlock_(h);
  return err;
}

static inline esp_err_t hc595_unclaim(hc595_t *h, uint16_t mask) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  h->claimed &= (uint16_t)~mask;
  hc595_unlock_(h);
  return ESP_OK;
}

static inline esp_err_t hc595_write_claimed(hc595_t *h, uint16_t mask,
                                            uint16_t value) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  if ((mask & ~h->claimed) != 0) {
    err = ESP_ERR_INVALID_ARG;
  } else {
    err = hc595_update_locked_(h, mask, value);
  }
  hc595_unlock_(h);
  return err;
}

static inline esp_err_t hc595_write_masked(hc595_t *h, uint16_t mask,
                                           uint16_t value) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if (mask & ~h->valid_mask) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  if (mask & h->claimed) {
    err = ESP_ERR_INVALID_STATE;
  } else {
    hc595_blink_cancel_locked_(h, mask);
    err = hc595_update_locked_(h, mask, value);
  }
  hc595_unlock_(h);
  return err;
}
static inline esp_err_t hc595_write(hc595_t *h, uint16_t value) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  uint16_t mask = (uint16_t)(h->valid_mask & ~h->claimed);
  hc595_blink_cancel_locked_(h, mask);
  err = hc595_update_locked_(h, mask, value);
  hc595_unlock_(h);
  return err;
}

static inline esp_err_t hc595_set_pin(hc595_t *h, uint8_t pin, bool level) {
  if (pin >= HC595_MAX_OUTPUTS) {
    return ESP_ERR_INVALID_ARG;
  }
  return hc595_write_masked(h, (uint16_t)(1U << pin),
                            level ? (uint16_t)(1U << pin) : 0);
}

static inline esp_err_t hc595_toggle_pin(hc595_t *h, uint8_t pin) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if (pin >= HC595_MAX_OUTPUTS || !((h->valid_mask >> pin) & 1U)) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  const uint16_t bit = (uint16_t)(1U << pin);
  if (h->claimed & bit) {
    err = ESP_ERR_INVALID_STATE;
  } else {
    hc595_blink_cancel_locked_(h, bit);
    err = hc595_update_locked_(
        h, bit, (uint16_t)(~h->state & bit)); 
  }
  hc595_unlock_(h);
  return err;
}

static inline esp_err_t hc595_get_pin(hc595_t *h, uint8_t pin, bool *level) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if (pin >= HC595_MAX_OUTPUTS || level == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  *level = (h->state >> pin) & 1U;
  hc595_unlock_(h);
  return ESP_OK;
}


static inline esp_err_t hc595_get_state(hc595_t *h, uint16_t *state) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if (state == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  *state = h->state;
  hc595_unlock_(h);
  return ESP_OK;
}


static inline esp_err_t hc595_all_off(hc595_t *h) { return hc595_write(h, 0); }


static inline esp_err_t hc595_refresh(hc595_t *h) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  err = hc595_flush_locked_(h);
  hc595_unlock_(h);
  return err;
}


static inline esp_err_t hc595_set_output_enable(hc595_t *h, bool enable) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if (h->cfg.oe == GPIO_NUM_NC) {
    return ESP_ERR_NOT_SUPPORTED;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  gpio_set_level(h->cfg.oe, enable ? 0 : 1);
  h->output_enabled = enable;
  hc595_unlock_(h);
  return ESP_OK;
}

static inline esp_err_t hc595_blink(hc595_t *h, uint8_t pin, uint32_t on_ms,
                                    uint32_t off_ms, uint32_t cycles) {
  esp_err_t err = hc595_check_(h);
  if (err != ESP_OK) {
    return err;
  }
  if (pin >= HC595_MAX_OUTPUTS || !((h->valid_mask >> pin) & 1U) ||
      on_ms == 0 || (cycles != 1 && off_ms == 0)) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = hc595_lock_(h)) != ESP_OK) {
    return err;
  }
  const uint16_t bit = (uint16_t)(1U << pin);
  if (h->claimed & bit) {
    hc595_unlock_(h);
    return ESP_ERR_INVALID_STATE;
  }
  hc595_blink_t *b = &h->blink[pin];
  b->active = true;
  b->phase_on = true;
  b->infinite = (cycles == 0);
  b->on_ms = on_ms;
  b->off_ms = off_ms;
  b->cycles_left = cycles;
  b->due_us = esp_timer_get_time() + (int64_t)on_ms * 1000;
  err = hc595_update_locked_(h, bit, bit);
  hc595_blink_rearm_locked_(h);
  hc595_unlock_(h);
  return err;
}


static inline esp_err_t hc595_pulse(hc595_t *h, uint8_t pin, uint32_t ms) {
  return hc595_blink(h, pin, ms, 0, 1);
}


static inline esp_err_t hc595_blink_stop(hc595_t *h, uint8_t pin,
                                         bool final_level) {
  return hc595_set_pin(h, pin, final_level); 
}

#ifdef __cplusplus
}
#endif

#endif /* HC595_H */
