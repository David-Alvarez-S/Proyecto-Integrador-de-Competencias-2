#ifndef AS5600_H
#define AS5600_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------- Definiciones y Registros AS5600 ---------------- */
#define AS5600_ADDR 0x36
#define AS5600_REG_STATUS 0x0B
#define AS5600_REG_RAWANG 0x0C
#define AS5600_REG_ANGLE 0x0E
#define AS5600_REG_AGC 0x1A
#define AS5600_REG_MAG 0x1B

#define AS5600_STATUS_MD (1 << 5) /* Imán detectado */
#define AS5600_STATUS_ML (1 << 4) /* Imán muy débil */
#define AS5600_STATUS_MH (1 << 3) /* Imán muy fuerte */

#define AS5600_TAG "AS5600"

/* ---------------- Estructura de Control de Bus ---------------- */
typedef struct {
  gpio_num_t sda_pin;
  const gpio_num_t *scl_pins;
  size_t num_sensors;
  SemaphoreHandle_t mutex;
  uint32_t half_period_us;
  uint32_t stretch_timeout_us;
  TickType_t mutex_timeout_ticks;
} as5600_bus_t;

/* ---------------- Auxiliares I2C (Bit-Banging) ---------------- */
static inline void as5600_i2c_delay(uint32_t us) { esp_rom_delay_us(us); }

static inline void as5600_sda_release(gpio_num_t sda) {
  gpio_set_level(sda, 1);
}

static inline void as5600_sda_low(gpio_num_t sda) { gpio_set_level(sda, 0); }

static inline int as5600_sda_read(gpio_num_t sda) {
  return gpio_get_level(sda);
}

static inline void as5600_scl_low(gpio_num_t scl) { gpio_set_level(scl, 0); }

static inline bool as5600_scl_release(gpio_num_t scl, uint32_t timeout_us) {
  gpio_set_level(scl, 1);
  uint32_t t = timeout_us;
  while (!gpio_get_level(scl)) {
    if (t == 0)
      return false;
    t--;
    esp_rom_delay_us(1);
  }
  return true;
}

/* ---------------- Selección de Bus ---------------- */
static inline void as5600_bus_select(as5600_bus_t *bus, size_t idx) {
  for (size_t k = 0; k < bus->num_sensors; k++) {
    if (k != idx) {
      as5600_scl_low(bus->scl_pins[k]);
    }
  }
  esp_rom_delay_us(2);
  as5600_scl_release(bus->scl_pins[idx], bus->stretch_timeout_us);
  as5600_sda_release(bus->sda_pin);
  as5600_i2c_delay(bus->half_period_us);
}

static inline void as5600_bus_deselect(as5600_bus_t *bus) {
  as5600_sda_release(bus->sda_pin);
  for (size_t k = 0; k < bus->num_sensors; k++) {
    gpio_set_level(bus->scl_pins[k], 1);
  }
  as5600_i2c_delay(bus->half_period_us);
}

/* ---------------- Primitivas I2C Low Level ---------------- */
static inline bool as5600_i2c_start(as5600_bus_t *bus, size_t idx) {
  as5600_sda_release(bus->sda_pin);
  if (!as5600_scl_release(bus->scl_pins[idx], bus->stretch_timeout_us)) {
    return false;
  }
  as5600_i2c_delay(bus->half_period_us);
  as5600_sda_low(bus->sda_pin);
  as5600_i2c_delay(bus->half_period_us);
  as5600_scl_low(bus->scl_pins[idx]);
  as5600_i2c_delay(bus->half_period_us);
  return true;
}

static inline void as5600_i2c_stop(as5600_bus_t *bus, size_t idx) {
  as5600_sda_low(bus->sda_pin);
  as5600_i2c_delay(bus->half_period_us);
  as5600_scl_release(bus->scl_pins[idx], bus->stretch_timeout_us);
  as5600_i2c_delay(bus->half_period_us);
  as5600_sda_release(bus->sda_pin);
  as5600_i2c_delay(bus->half_period_us);
}

static inline bool as5600_i2c_write_byte(as5600_bus_t *bus, size_t idx,
                                         uint8_t b) {
  for (int bit = 7; bit >= 0; bit--) {
    if (b & (1 << bit)) {
      as5600_sda_release(bus->sda_pin);
    } else {
      as5600_sda_low(bus->sda_pin);
    }
    as5600_i2c_delay(bus->half_period_us);
    if (!as5600_scl_release(bus->scl_pins[idx], bus->stretch_timeout_us)) {
      return false;
    }
    as5600_i2c_delay(bus->half_period_us);
    as5600_scl_low(bus->scl_pins[idx]);
  }
  as5600_sda_release(bus->sda_pin);
  as5600_i2c_delay(bus->half_period_us);
  if (!as5600_scl_release(bus->scl_pins[idx], bus->stretch_timeout_us)) {
    return false;
  }
  as5600_i2c_delay(bus->half_period_us);
  bool ack = (as5600_sda_read(bus->sda_pin) == 0);
  as5600_scl_low(bus->scl_pins[idx]);
  return ack;
}

static inline uint8_t as5600_i2c_read_byte(as5600_bus_t *bus, size_t idx,
                                           bool ack) {
  uint8_t b = 0;
  as5600_sda_release(bus->sda_pin);
  for (int bit = 7; bit >= 0; bit--) {
    as5600_i2c_delay(bus->half_period_us);
    as5600_scl_release(bus->scl_pins[idx], bus->stretch_timeout_us);
    as5600_i2c_delay(bus->half_period_us);
    if (as5600_sda_read(bus->sda_pin)) {
      b |= (1 << bit);
    }
    as5600_scl_low(bus->scl_pins[idx]);
  }
  if (ack) {
    as5600_sda_low(bus->sda_pin);
  } else {
    as5600_sda_release(bus->sda_pin);
  }
  as5600_i2c_delay(bus->half_period_us);
  as5600_scl_release(bus->scl_pins[idx], bus->stretch_timeout_us);
  as5600_i2c_delay(bus->half_period_us);
  as5600_scl_low(bus->scl_pins[idx]);
  as5600_sda_release(bus->sda_pin);
  return b;
}

/* ---------------- API Pública ---------------- */

static inline esp_err_t as5600_bus_init(as5600_bus_t *bus, gpio_num_t sda_pin,
                                        const gpio_num_t *scl_pins,
                                        size_t num_sensors) {
  if (!bus || !scl_pins || num_sensors == 0) {
    return ESP_ERR_INVALID_ARG;
  }

  bus->sda_pin = sda_pin;
  bus->scl_pins = scl_pins;
  bus->num_sensors = num_sensors;
  bus->half_period_us = 5;
  bus->stretch_timeout_us = 1000;
  bus->mutex_timeout_ticks = pdMS_TO_TICKS(100);

  bus->mutex = xSemaphoreCreateMutex();
  if (!bus->mutex) {
    return ESP_ERR_NO_MEM;
  }

  gpio_config_t io = {
      .mode = GPIO_MODE_INPUT_OUTPUT_OD,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };

  io.pin_bit_mask = (1ULL << sda_pin);
  esp_err_t err = gpio_config(&io);
  if (err != ESP_OK)
    return err;

  for (size_t k = 0; k < num_sensors; k++) {
    io.pin_bit_mask = (1ULL << scl_pins[k]);
    err = gpio_config(&io);
    if (err != ESP_OK)
      return err;
  }

  as5600_bus_deselect(bus);
  return ESP_OK;
}

static inline esp_err_t as5600_bus_deinit(as5600_bus_t *bus) {
  if (!bus)
    return ESP_ERR_INVALID_ARG;
  if (bus->mutex) {
    vSemaphoreDelete(bus->mutex);
    bus->mutex = NULL;
  }
  return ESP_OK;
}

static inline esp_err_t as5600_read_regs(as5600_bus_t *bus, size_t idx,
                                         uint8_t reg, uint8_t *buf,
                                         size_t len) {
  if (!bus || idx >= bus->num_sensors || !buf || len == 0) {
    return ESP_ERR_INVALID_ARG;
  }
  if (xSemaphoreTake(bus->mutex, bus->mutex_timeout_ticks) != pdTRUE) {
    return ESP_ERR_TIMEOUT;
  }

  esp_err_t ret = ESP_FAIL;
  as5600_bus_select(bus, idx);

  do {
    if (!as5600_i2c_start(bus, idx)) {
      ret = ESP_ERR_TIMEOUT;
      break;
    }
    if (!as5600_i2c_write_byte(bus, idx, (AS5600_ADDR << 1) | 0)) {
      ret = ESP_ERR_NOT_FOUND;
      break;
    }
    if (!as5600_i2c_write_byte(bus, idx, reg)) {
      break;
    }
    if (!as5600_i2c_start(bus, idx)) {
      ret = ESP_ERR_TIMEOUT;
      break;
    }
    if (!as5600_i2c_write_byte(bus, idx, (AS5600_ADDR << 1) | 1)) {
      ret = ESP_ERR_NOT_FOUND;
      break;
    }
    for (size_t n = 0; n < len; n++) {
      buf[n] = as5600_i2c_read_byte(bus, idx, n < len - 1);
    }
    ret = ESP_OK;
  } while (0);

  as5600_i2c_stop(bus, idx);
  as5600_bus_deselect(bus);
  xSemaphoreGive(bus->mutex);
  return ret;
}

static inline esp_err_t as5600_get_raw_angle(as5600_bus_t *bus, size_t idx,
                                             uint16_t *raw) {
  if (!raw)
    return ESP_ERR_INVALID_ARG;
  uint8_t d[2];
  esp_err_t r = as5600_read_regs(bus, idx, AS5600_REG_RAWANG, d, 2);
  if (r == ESP_OK) {
    *raw = (uint16_t)(((d[0] & 0x0F) << 8) | d[1]);
  }
  return r;
}

static inline esp_err_t as5600_get_angle(as5600_bus_t *bus, size_t idx,
                                         uint16_t *angle) {
  if (!angle)
    return ESP_ERR_INVALID_ARG;
  uint8_t d[2];
  esp_err_t r = as5600_read_regs(bus, idx, AS5600_REG_ANGLE, d, 2);
  if (r == ESP_OK) {
    *angle = (uint16_t)(((d[0] & 0x0F) << 8) | d[1]);
  }
  return r;
}

static inline esp_err_t as5600_get_status(as5600_bus_t *bus, size_t idx,
                                          uint8_t *status) {
  return as5600_read_regs(bus, idx, AS5600_REG_STATUS, status, 1);
}

static inline float as5600_to_degrees(uint16_t v) {
  return (float)v * 360.0f / 4096.0f;
}

#ifdef __cplusplus
}
#endif

#endif /* AS5600_H */
