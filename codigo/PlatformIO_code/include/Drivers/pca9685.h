#ifndef PCA9685_H
#define PCA9685_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_idf_version.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#if defined(PCA9685_FORCE_LEGACY_I2C) ||                                       \
    (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0))
#define PCA9685_USE_LEGACY_I2C 1
#include "driver/i2c.h"
typedef i2c_port_t pca9685_bus_t;
#else
#define PCA9685_USE_LEGACY_I2C 0
#include "driver/i2c_master.h"
typedef i2c_master_bus_handle_t pca9685_bus_t;
#endif

/* ------------------------------------------------------------------------- */
/* Constantes del chip                                                       */
/* ------------------------------------------------------------------------- */

#define PCA9685_NUM_CHANNELS 16
#define PCA9685_RESOLUTION 4096U

#define PCA9685_REG_MODE1 0x00
#define PCA9685_REG_MODE2 0x01
#define PCA9685_REG_LED0_ON_L 0x06
#define PCA9685_REG_ALL_ON_L 0xFA
#define PCA9685_REG_PRESCALE 0xFE

#define PCA9685_MODE1_RESTART 0x80
#define PCA9685_MODE1_AI 0x20
#define PCA9685_MODE1_SLEEP 0x10
#define PCA9685_MODE2_INVRT 0x10
#define PCA9685_MODE2_OUTDRV 0x04
#define PCA9685_FULL_BIT 0x10 /* bit 4 de ON_H / OFF_H: "siempre on/off" */

/* ------------------------------------------------------------------------- */
/* Parametros (sobrescribibles con #define antes del #include)               */
/* ------------------------------------------------------------------------- */

#ifndef PCA9685_DEFAULT_ADDR
#define PCA9685_DEFAULT_ADDR 0x40
#endif
#ifndef PCA9685_DEFAULT_OSC_HZ
#define PCA9685_DEFAULT_OSC_HZ 25000000U
#endif
#ifndef PCA9685_DEFAULT_FREQ_HZ
#define PCA9685_DEFAULT_FREQ_HZ 50U
#endif
#ifndef PCA9685_DEFAULT_I2C_HZ
#define PCA9685_DEFAULT_I2C_HZ 400000U
#endif
#ifndef PCA9685_DEFAULT_I2C_TIMEOUT_MS
#define PCA9685_DEFAULT_I2C_TIMEOUT_MS 50U
#endif
#ifndef PCA9685_DEFAULT_LOCK_TIMEOUT_MS
#define PCA9685_DEFAULT_LOCK_TIMEOUT_MS 200U
#endif
/** Desfase (en cuentas) entre canales cuando stagger = true. */
#ifndef PCA9685_STAGGER_STEP
#define PCA9685_STAGGER_STEP 200U
#endif

/* Valores por defecto de servos */
#ifndef PCA9685_SERVO_MIN_US
#define PCA9685_SERVO_MIN_US 500U
#endif
#ifndef PCA9685_SERVO_MAX_US
#define PCA9685_SERVO_MAX_US 2400U
#endif
#ifndef PCA9685_SERVO_MAX_ANGLE_DEG
#define PCA9685_SERVO_MAX_ANGLE_DEG 180.0f
#endif
#ifndef PCA9685_CR_MIN_US
#define PCA9685_CR_MIN_US 1000U
#endif
#ifndef PCA9685_CR_NEUTRAL_US
#define PCA9685_CR_NEUTRAL_US 1500U
#endif
#ifndef PCA9685_CR_MAX_US
#define PCA9685_CR_MAX_US 2000U
#endif
#ifndef PCA9685_CR_DEADBAND_US
#define PCA9685_CR_DEADBAND_US 30U
#endif

/* ------------------------------------------------------------------------- */
/* Tipos                                                                     */
/* ------------------------------------------------------------------------- */

typedef struct {
  pca9685_bus_t bus; /**< i2c_master_bus_handle_t (IDF>=5.2) o i2c_port_t */
  uint8_t addr;      /**< 0x40..0x7F                               */
  uint32_t i2c_hz;   /**< Solo backend nuevo                       */
  uint32_t i2c_timeout_ms;
  uint32_t freq_hz;     /**< Frecuencia PWM (50 Hz para servos)       */
  uint32_t osc_hz;      /**< Oscilador real (~25 MHz, ajustable)      */
  gpio_num_t oe_pin;    /**< GPIO_NUM_NC si no esta conectado         */
  bool stagger;         /**< Desfasa los canales (menos pico de corriente) */
  bool invert;          /**< Salidas invertidas (INVRT)               */
  bool open_drain;      /**< false = totem-pole (lo normal)           */
  bool all_off_on_init; /**< Apaga todos los canales al iniciar       */
  uint32_t lock_timeout_ms;
} pca9685_config_t;

#define PCA9685_CONFIG_DEFAULT(_bus)                                           \
  {                                                                            \
      .bus = (_bus),                                                           \
      .addr = PCA9685_DEFAULT_ADDR,                                            \
      .i2c_hz = PCA9685_DEFAULT_I2C_HZ,                                        \
      .i2c_timeout_ms = PCA9685_DEFAULT_I2C_TIMEOUT_MS,                        \
      .freq_hz = PCA9685_DEFAULT_FREQ_HZ,                                      \
      .osc_hz = PCA9685_DEFAULT_OSC_HZ,                                        \
      .oe_pin = GPIO_NUM_NC,                                                   \
      .stagger = true,                                                         \
      .invert = false,                                                         \
      .open_drain = false,                                                     \
      .all_off_on_init = true,                                                 \
      .lock_timeout_ms = PCA9685_DEFAULT_LOCK_TIMEOUT_MS,                      \
  }

/** Servo de posicion (SG90, MG90S de 180 grados, etc.). */
typedef struct {
  uint16_t min_us;     /**< Pulso para 0 grados                          */
  uint16_t max_us;     /**< Pulso para max_angle_deg                     */
  float max_angle_deg; /**< Recorrido total                              */
  bool reversed;       /**< Invierte el sentido                          */
} pca9685_servo_config_t;

#define PCA9685_SERVO_CONFIG_DEFAULT()                                         \
  {                                                                            \
      .min_us = PCA9685_SERVO_MIN_US,                                          \
      .max_us = PCA9685_SERVO_MAX_US,                                          \
      .max_angle_deg = PCA9685_SERVO_MAX_ANGLE_DEG,                            \
      .reversed = false,                                                       \
  }

/** Servo de giro continuo (el pulso fija velocidad y sentido). */
typedef struct {
  uint16_t min_us;      /**< Velocidad maxima en sentido negativo         */
  uint16_t neutral_us;  /**< Parado                                       */
  uint16_t max_us;      /**< Velocidad maxima en sentido positivo         */
  uint16_t deadband_us; /**< Desplazamiento minimo para que empiece a girar */
  bool reversed;
} pca9685_cr_config_t;

#define PCA9685_CR_CONFIG_DEFAULT()                                            \
  {                                                                            \
      .min_us = PCA9685_CR_MIN_US,                                             \
      .neutral_us = PCA9685_CR_NEUTRAL_US,                                     \
      .max_us = PCA9685_CR_MAX_US,                                             \
      .deadband_us = PCA9685_CR_DEADBAND_US,                                   \
      .reversed = false,                                                       \
  }

typedef enum {
  PCA9685_CH_NONE = 0,
  PCA9685_CH_SERVO, /**< 180 grados                                   */
  PCA9685_CH_CR,    /**< Giro continuo                                */
} pca9685_chan_type_t;

typedef struct {
  pca9685_chan_type_t type;
  union {
    pca9685_servo_config_t servo;
    pca9685_cr_config_t cr;
  } cfg;
  uint32_t last_us; /**< Ultimo pulso aplicado                        */
  float value;      /**< Angulo (servo) o velocidad % (cr)            */
  bool active;      /**< true = el canal esta emitiendo pulsos        */
} pca9685_channel_t;

typedef struct {
  pca9685_config_t cfg;
  SemaphoreHandle_t lock;
#if !PCA9685_USE_LEGACY_I2C
  i2c_master_dev_handle_t i2c_dev;
#endif
  uint32_t osc_hz;
  uint8_t prescale;
  uint32_t i2c_errors; /**< Transacciones I2C fallidas (diagnostico) */
  pca9685_channel_t ch[PCA9685_NUM_CHANNELS];
  bool initialized;
} pca9685_t;

/* ------------------------------------------------------------------------- */
/* Internas: I2C                                                             */
/* ------------------------------------------------------------------------- */

static inline bool pca9685_ready_(const pca9685_t *d) {
  return d != NULL && d->initialized;
}

static inline esp_err_t pca9685_lock_(pca9685_t *d) {
  return (xSemaphoreTake(d->lock, pdMS_TO_TICKS(d->cfg.lock_timeout_ms)) ==
          pdTRUE)
             ? ESP_OK
             : ESP_ERR_TIMEOUT;
}

static inline void pca9685_unlock_(pca9685_t *d) { xSemaphoreGive(d->lock); }

/** Escribe 'len' bytes (el primero es el registro; el resto, datos con
 * auto-incremento). */
static inline esp_err_t pca9685_i2c_write_(pca9685_t *d, const uint8_t *buf,
                                           size_t len) {
  esp_err_t err;
#if PCA9685_USE_LEGACY_I2C
  err = i2c_master_write_to_device(d->cfg.bus, d->cfg.addr, buf, len,
                                   pdMS_TO_TICKS(d->cfg.i2c_timeout_ms));
#else
  err = i2c_master_transmit(d->i2c_dev, buf, len, (int)d->cfg.i2c_timeout_ms);
#endif
  if (err != ESP_OK) {
    d->i2c_errors++;
  }
  return err;
}

static inline esp_err_t pca9685_i2c_read_(pca9685_t *d, uint8_t reg,
                                          uint8_t *out, size_t len) {
  esp_err_t err;
#if PCA9685_USE_LEGACY_I2C
  err = i2c_master_write_read_device(d->cfg.bus, d->cfg.addr, &reg, 1, out, len,
                                     pdMS_TO_TICKS(d->cfg.i2c_timeout_ms));
#else
  err = i2c_master_transmit_receive(d->i2c_dev, &reg, 1, out, len,
                                    (int)d->cfg.i2c_timeout_ms);
#endif
  if (err != ESP_OK) {
    d->i2c_errors++;
  }
  return err;
}

static inline esp_err_t pca9685_wr_(pca9685_t *d, uint8_t reg, uint8_t val) {
  uint8_t b[2] = {reg, val};
  return pca9685_i2c_write_(d, b, 2);
}

/* ------------------------------------------------------------------------- */
/* Internas: conversion de unidades y escritura de canales                   */
/* ------------------------------------------------------------------------- */

static inline float pca9685_actual_freq_(const pca9685_t *d) {
  return (float)d->osc_hz /
         ((float)PCA9685_RESOLUTION * (float)(d->prescale + 1U));
}

/** Microsegundos -> cuentas de 12 bits segun prescaler y oscilador reales. */
static inline uint32_t pca9685_us_to_ticks_(const pca9685_t *d, uint32_t us) {
  uint64_t num = (uint64_t)us * (uint64_t)d->osc_hz;
  uint64_t den = 1000000ULL * (uint64_t)(d->prescale + 1U);
  uint64_t t = (num + den / 2U) / den;
  return (t > PCA9685_RESOLUTION) ? PCA9685_RESOLUTION : (uint32_t)t;
}

/** Rellena los 4 bytes ON_L, ON_H, OFF_L, OFF_H de un canal para un ancho dado
 * en cuentas. */
static inline void pca9685_fill_channel_(const pca9685_t *d, uint8_t ch,
                                         uint32_t ticks, uint8_t out[4]) {
  if (ticks == 0) { /* siempre apagado */
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    out[3] = PCA9685_FULL_BIT;
  } else if (ticks >= PCA9685_RESOLUTION) { /* siempre encendido */
    out[0] = 0;
    out[1] = PCA9685_FULL_BIT;
    out[2] = 0;
    out[3] = 0;
  } else {
    uint32_t on = 0;
    if (d->cfg.stagger) {
      on = (uint32_t)ch * PCA9685_STAGGER_STEP;
      if (on + ticks >
          PCA9685_RESOLUTION - 1U) { /* nunca cruzar el final del periodo */
        on = PCA9685_RESOLUTION - 1U - ticks;
      }
    }
    uint32_t off = on + ticks;
    out[0] = (uint8_t)(on & 0xFF);
    out[1] = (uint8_t)(on >> 8);
    out[2] = (uint8_t)(off & 0xFF);
    out[3] = (uint8_t)(off >> 8);
  }
}

/** Escribe un canal (mutex tomado). */
static inline esp_err_t pca9685_write_ticks_locked_(pca9685_t *d, uint8_t ch,
                                                    uint32_t ticks) {
  uint8_t buf[5];
  buf[0] = (uint8_t)(PCA9685_REG_LED0_ON_L + 4U * ch);
  pca9685_fill_channel_(d, ch, ticks, &buf[1]);
  return pca9685_i2c_write_(d, buf, sizeof(buf));
}

/** Aplica un pulso en us a un canal y actualiza su estado (mutex tomado). */
static inline esp_err_t pca9685_write_us_locked_(pca9685_t *d, uint8_t ch,
                                                 uint32_t us) {
  esp_err_t err =
      pca9685_write_ticks_locked_(d, ch, pca9685_us_to_ticks_(d, us));
  if (err == ESP_OK) {
    d->ch[ch].last_us = us;
    d->ch[ch].active = true;
  }
  return err;
}

/** Re-aplica los pulsos de los canales activos (tras cambiar
 * frecuencia/oscilador). */
static inline esp_err_t pca9685_reapply_locked_(pca9685_t *d) {
  for (uint8_t ch = 0; ch < PCA9685_NUM_CHANNELS; ch++) {
    if (d->ch[ch].active) {
      esp_err_t err = pca9685_write_us_locked_(d, ch, d->ch[ch].last_us);
      if (err != ESP_OK) {
        return err;
      }
    }
  }
  return ESP_OK;
}

static inline esp_err_t pca9685_calc_prescale_(uint32_t osc_hz,
                                               uint32_t freq_hz, uint8_t *out) {
  if (freq_hz == 0 || osc_hz == 0) {
    return ESP_ERR_INVALID_ARG;
  }
  float p = (float)osc_hz / ((float)PCA9685_RESOLUTION * (float)freq_hz) - 1.0f;
  int v = (int)(p + 0.5f);
  if (v < 3 || v > 255) { /* limites del chip: ~24 Hz .. ~1526 Hz */
    return ESP_ERR_INVALID_ARG;
  }
  *out = (uint8_t)v;
  return ESP_OK;
}

/**
 * Programa el prescaler (el chip debe estar dormido) y lo reactiva con RESTART.
 * Mutex tomado.
 */
static inline esp_err_t pca9685_apply_freq_locked_(pca9685_t *d,
                                                   uint32_t freq_hz) {
  uint8_t pre;
  esp_err_t err = pca9685_calc_prescale_(d->osc_hz, freq_hz, &pre);
  if (err != ESP_OK) {
    return err;
  }
  err =
      pca9685_wr_(d, PCA9685_REG_MODE1, PCA9685_MODE1_AI | PCA9685_MODE1_SLEEP);
  if (err == ESP_OK)
    err = pca9685_wr_(d, PCA9685_REG_PRESCALE, pre);
  if (err == ESP_OK)
    err = pca9685_wr_(d, PCA9685_REG_MODE1, PCA9685_MODE1_AI);
  if (err != ESP_OK) {
    return err;
  }
  vTaskDelay(2); /* el oscilador necesita >= 500 us */
  err = pca9685_wr_(d, PCA9685_REG_MODE1,
                    PCA9685_MODE1_AI | PCA9685_MODE1_RESTART);
  if (err == ESP_OK) {
    d->prescale = pre;
  }
  return err;
}

static inline esp_err_t pca9685_chan_check_(const pca9685_t *d, uint8_t ch,
                                            pca9685_chan_type_t type) {
  if (!pca9685_ready_(d)) {
    return ESP_ERR_INVALID_STATE;
  }
  if (ch >= PCA9685_NUM_CHANNELS) {
    return ESP_ERR_INVALID_ARG;
  }
  if (d->ch[ch].type != type) {
    return ESP_ERR_INVALID_STATE; /* sin attach o de otro tipo */
  }
  return ESP_OK;
}

static inline TickType_t pca9685_ticks_min1_(uint32_t ms) {
  TickType_t t = pdMS_TO_TICKS(ms);
  return (t == 0) ? 1 : t;
}

/* ------------------------------------------------------------------------- */
/* API: ciclo de vida y chip                                                 */
/* ------------------------------------------------------------------------- */

/**
 * @brief Inicializa el chip: detecta el dispositivo, configura MODE1/MODE2, la
 *        frecuencia PWM y (opcional) apaga todos los canales.
 *
 * Tras init no se emiten pulsos hasta la primera orden a un canal, de modo que
 * los servos no se mueven al arrancar.
 */
static inline esp_err_t pca9685_init(pca9685_t *d,
                                     const pca9685_config_t *cfg) {
  if (d == NULL || cfg == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if (d->initialized) {
    return ESP_ERR_INVALID_STATE;
  }
  if (cfg->addr < 0x40 || cfg->addr > 0x7F || cfg->osc_hz == 0 ||
      cfg->freq_hz == 0) {
    return ESP_ERR_INVALID_ARG;
  }
#if PCA9685_USE_LEGACY_I2C
  if ((int)cfg->bus < 0) {
    return ESP_ERR_INVALID_ARG;
  }
#else
  if (cfg->bus == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
#endif

  memset(d, 0, sizeof(*d));
  d->cfg = *cfg;
  d->osc_hz = cfg->osc_hz;
  esp_err_t err = ESP_OK;

  d->lock = xSemaphoreCreateMutex();
  if (d->lock == NULL) {
    return ESP_ERR_NO_MEM;
  }

#if !PCA9685_USE_LEGACY_I2C
  i2c_device_config_t dc = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = cfg->addr,
      .scl_speed_hz = cfg->i2c_hz,
  };
  err = i2c_master_bus_add_device(cfg->bus, &dc, &d->i2c_dev);
  if (err != ESP_OK) {
    goto fail_lock;
  }
#endif

  /* OE: activo en bajo. Se deja deshabilitado hasta tener todo configurado. */
  if (cfg->oe_pin >= 0) {
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << cfg->oe_pin,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    err = gpio_config(&io);
    if (err != ESP_OK) {
      goto fail_dev;
    }
    gpio_set_level(cfg->oe_pin, 1);
  }

  /* Deteccion: leer MODE1 */
  uint8_t m1;
  err = pca9685_i2c_read_(d, PCA9685_REG_MODE1, &m1, 1);
  if (err != ESP_OK) {
    err = ESP_ERR_NOT_FOUND;
    goto fail_dev;
  }

  /* Auto-incremento ANTES de cualquier escritura en rafaga (el valor por
   * defecto del chip es AI = 0). Se deja dormido y con ALLCALL desactivado
   * (evita respuestas a broadcasts a 0x70); apply_freq lo despierta al final.
   */
  err =
      pca9685_wr_(d, PCA9685_REG_MODE1, PCA9685_MODE1_AI | PCA9685_MODE1_SLEEP);

  uint8_t mode2 = (cfg->invert ? PCA9685_MODE2_INVRT : 0) |
                  (cfg->open_drain ? 0 : PCA9685_MODE2_OUTDRV);
  if (err == ESP_OK) {
    err = pca9685_wr_(d, PCA9685_REG_MODE2, mode2);
  }
  if (err == ESP_OK && cfg->all_off_on_init) {
    const uint8_t all_off[5] = {PCA9685_REG_ALL_ON_L, 0, 0, 0,
                                PCA9685_FULL_BIT};
    err = pca9685_i2c_write_(d, all_off, sizeof(all_off));
  }
  if (err == ESP_OK) {
    err = pca9685_apply_freq_locked_(
        d, cfg->freq_hz); /* tambien deja AI activo, ALLCALL off */
  }
  if (err != ESP_OK) {
    goto fail_dev;
  }

  if (cfg->oe_pin >= 0) {
    gpio_set_level(cfg->oe_pin, 0); /* habilita salidas */
  }
  d->initialized = true;
  return ESP_OK;

fail_dev:
#if !PCA9685_USE_LEGACY_I2C
  i2c_master_bus_rm_device(d->i2c_dev);
fail_lock:
#endif
  vSemaphoreDelete(d->lock);
  memset(d, 0, sizeof(*d));
  return err;
}

/** Apaga todos los canales, duerme el chip y libera recursos. */
static inline esp_err_t pca9685_deinit(pca9685_t *d) {
  if (!pca9685_ready_(d)) {
    return ESP_ERR_INVALID_STATE;
  }
  esp_err_t err = pca9685_lock_(d);
  if (err != ESP_OK) {
    return err;
  }
  d->initialized = false;
  const uint8_t all_off[5] = {PCA9685_REG_ALL_ON_L, 0, 0, 0, PCA9685_FULL_BIT};
  (void)pca9685_i2c_write_(d, all_off, sizeof(all_off));
  (void)pca9685_wr_(d, PCA9685_REG_MODE1,
                    PCA9685_MODE1_AI | PCA9685_MODE1_SLEEP);
  if (d->cfg.oe_pin >= 0) {
    gpio_set_level(d->cfg.oe_pin, 1);
    gpio_reset_pin(d->cfg.oe_pin);
  }
#if !PCA9685_USE_LEGACY_I2C
  i2c_master_bus_rm_device(d->i2c_dev);
  d->i2c_dev = NULL;
#endif
  SemaphoreHandle_t lock = d->lock;
  d->lock = NULL;
  xSemaphoreGive(lock);
  vSemaphoreDelete(lock);
  return ESP_OK;
}

/** Cambia la frecuencia PWM (24..1526 Hz). Conserva los anchos de pulso
 * activos. */
static inline esp_err_t pca9685_set_frequency(pca9685_t *d, uint32_t freq_hz) {
  if (!pca9685_ready_(d)) {
    return ESP_ERR_INVALID_STATE;
  }
  esp_err_t err = pca9685_lock_(d);
  if (err != ESP_OK) {
    return err;
  }
  err = pca9685_apply_freq_locked_(d, freq_hz);
  if (err == ESP_OK) {
    d->cfg.freq_hz = freq_hz;
    err = pca9685_reapply_locked_(d);
  }
  pca9685_unlock_(d);
  return err;
}

/** Frecuencia PWM real (Hz) segun el prescaler y el oscilador configurados. */
static inline esp_err_t pca9685_get_frequency(pca9685_t *d, float *hz) {
  if (!pca9685_ready_(d) || hz == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  esp_err_t err = pca9685_lock_(d);
  if (err != ESP_OK) {
    return err;
  }
  *hz = pca9685_actual_freq_(d);
  pca9685_unlock_(d);
  return ESP_OK;
}

/**
 * @brief Ajusta la frecuencia del oscilador interno (el real varia entre
 *        ~23 y 27 MHz). Si mides la salida y no son 50.0 Hz, corrige aqui para
 *        que los anchos de pulso en microsegundos sean exactos.
 */
static inline esp_err_t pca9685_set_oscillator_hz(pca9685_t *d,
                                                  uint32_t osc_hz) {
  if (!pca9685_ready_(d) || osc_hz < 20000000U || osc_hz > 30000000U) {
    return ESP_ERR_INVALID_ARG;
  }
  esp_err_t err = pca9685_lock_(d);
  if (err != ESP_OK) {
    return err;
  }
  uint32_t old = d->osc_hz;
  d->osc_hz = osc_hz;
  err = pca9685_apply_freq_locked_(d, d->cfg.freq_hz);
  if (err == ESP_OK) {
    err = pca9685_reapply_locked_(d);
  } else {
    d->osc_hz = old;
  }
  pca9685_unlock_(d);
  return err;
}

/** Habilita/deshabilita TODAS las salidas por el pin OE (parada de emergencia
 * por hardware). */
static inline esp_err_t pca9685_set_output_enabled(pca9685_t *d, bool enabled) {
  if (!pca9685_ready_(d)) {
    return ESP_ERR_INVALID_STATE;
  }
  if (d->cfg.oe_pin < 0) {
    return ESP_ERR_NOT_SUPPORTED;
  }
  return gpio_set_level(d->cfg.oe_pin, enabled ? 0 : 1);
}

/** Duerme/despierta el chip (dormido: oscilador parado y servos sin pulsos). */
static inline esp_err_t pca9685_sleep(pca9685_t *d, bool sleep) {
  if (!pca9685_ready_(d)) {
    return ESP_ERR_INVALID_STATE;
  }
  esp_err_t err = pca9685_lock_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (sleep) {
    err = pca9685_wr_(d, PCA9685_REG_MODE1,
                      PCA9685_MODE1_AI | PCA9685_MODE1_SLEEP);
  } else {
    err = pca9685_wr_(d, PCA9685_REG_MODE1, PCA9685_MODE1_AI);
    if (err == ESP_OK) {
      vTaskDelay(2);
      err = pca9685_wr_(d, PCA9685_REG_MODE1,
                        PCA9685_MODE1_AI | PCA9685_MODE1_RESTART);
    }
  }
  pca9685_unlock_(d);
  return err;
}

/** Apaga TODOS los canales de golpe (una sola transaccion) y los marca
 * inactivos. */
static inline esp_err_t pca9685_all_off(pca9685_t *d) {
  if (!pca9685_ready_(d)) {
    return ESP_ERR_INVALID_STATE;
  }
  esp_err_t err = pca9685_lock_(d);
  if (err != ESP_OK) {
    return err;
  }
  const uint8_t all_off[5] = {PCA9685_REG_ALL_ON_L, 0, 0, 0, PCA9685_FULL_BIT};
  err = pca9685_i2c_write_(d, all_off, sizeof(all_off));
  if (err == ESP_OK) {
    for (int i = 0; i < PCA9685_NUM_CHANNELS; i++) {
      d->ch[i].active = false;
    }
  }
  pca9685_unlock_(d);
  return err;
}

/**
 * @brief Apaga un canal (deja de emitir pulsos): el servo queda libre y sin
 *        consumo de mantenimiento. Funciona para ambos tipos de servo.
 *        Un servo de giro continuo queda parado de forma garantizada.
 */
static inline esp_err_t pca9685_release(pca9685_t *d, uint8_t ch) {
  if (!pca9685_ready_(d) || ch >= PCA9685_NUM_CHANNELS) {
    return pca9685_ready_(d) ? ESP_ERR_INVALID_ARG : ESP_ERR_INVALID_STATE;
  }
  esp_err_t err = pca9685_lock_(d);
  if (err != ESP_OK) {
    return err;
  }
  err = pca9685_write_ticks_locked_(d, ch, 0);
  if (err == ESP_OK) {
    d->ch[ch].active = false;
    if (d->ch[ch].type == PCA9685_CH_CR) {
      d->ch[ch].value = 0.0f;
    }
  }
  pca9685_unlock_(d);
  return err;
}

/**
 * @brief Escritura directa de los registros ON/OFF de un canal (12 bits + bit
 *        0x1000 = "siempre"). Es una via de escape: NO actualiza el estado de
 *        servo del canal.
 */
static inline esp_err_t pca9685_set_pwm_raw(pca9685_t *d, uint8_t ch,
                                            uint16_t on, uint16_t off) {
  if (!pca9685_ready_(d) || ch >= PCA9685_NUM_CHANNELS) {
    return pca9685_ready_(d) ? ESP_ERR_INVALID_ARG : ESP_ERR_INVALID_STATE;
  }
  uint8_t buf[5] = {(uint8_t)(PCA9685_REG_LED0_ON_L + 4U * ch),
                    (uint8_t)(on & 0xFF), (uint8_t)(on >> 8),
                    (uint8_t)(off & 0xFF), (uint8_t)(off >> 8)};
  esp_err_t err = pca9685_lock_(d);
  if (err != ESP_OK) {
    return err;
  }
  err = pca9685_i2c_write_(d, buf, sizeof(buf));
  pca9685_unlock_(d);
  return err;
}

/* ------------------------------------------------------------------------- */
/* API: servo de 180 grados (SG90)                                           */
/* ------------------------------------------------------------------------- */

static inline uint32_t
pca9685_servo_angle_to_us_(const pca9685_servo_config_t *c, float angle) {
  if (angle < 0.0f) {
    angle = 0.0f;
  } else if (angle > c->max_angle_deg) {
    angle = c->max_angle_deg;
  }
  if (c->reversed) {
    angle = c->max_angle_deg - angle;
  }
  return c->min_us +
         (uint32_t)(angle / c->max_angle_deg * (float)(c->max_us - c->min_us) +
                    0.5f);
}

static inline float pca9685_servo_us_to_angle_(const pca9685_servo_config_t *c,
                                               uint32_t us) {
  float a;
  if (us <= c->min_us)
    a = 0.0f;
  else if (us >= c->max_us)
    a = c->max_angle_deg;
  else
    a = (float)(us - c->min_us) / (float)(c->max_us - c->min_us) *
        c->max_angle_deg;
  return c->reversed ? c->max_angle_deg - a : a;
}

/**
 * @brief Registra un canal como servo de 180 grados. No emite pulsos todavia.
 *        Si el canal esta emitiendo, primero hay que liberarlo
 * (pca9685_release).
 */
static inline esp_err_t
pca9685_servo_attach(pca9685_t *d, uint8_t ch,
                     const pca9685_servo_config_t *cfg) {
  if (!pca9685_ready_(d)) {
    return ESP_ERR_INVALID_STATE;
  }
  if (ch >= PCA9685_NUM_CHANNELS || cfg == NULL || cfg->min_us >= cfg->max_us ||
      cfg->max_angle_deg <= 0.0f) {
    return ESP_ERR_INVALID_ARG;
  }
  esp_err_t err = pca9685_lock_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (d->ch[ch].active) {
    err = ESP_ERR_INVALID_STATE;
  } else {
    memset(&d->ch[ch], 0, sizeof(d->ch[ch]));
    d->ch[ch].type = PCA9685_CH_SERVO;
    d->ch[ch].cfg.servo = *cfg;
  }
  pca9685_unlock_(d);
  return err;
}

/** Pulso directo en us (limitado a [min_us, max_us]). */
static inline esp_err_t pca9685_servo_write_us(pca9685_t *d, uint8_t ch,
                                               uint32_t us) {
  esp_err_t err = pca9685_chan_check_(d, ch, PCA9685_CH_SERVO);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = pca9685_lock_(d)) != ESP_OK) {
    return err;
  }
  const pca9685_servo_config_t *c = &d->ch[ch].cfg.servo;
  if (us < c->min_us)
    us = c->min_us;
  else if (us > c->max_us)
    us = c->max_us;
  err = pca9685_write_us_locked_(d, ch, us);
  if (err == ESP_OK) {
    d->ch[ch].value = pca9685_servo_us_to_angle_(c, us);
  }
  pca9685_unlock_(d);
  return err;
}

/** Mueve el servo a un angulo en grados (se limita a [0, max_angle_deg]). */
static inline esp_err_t pca9685_servo_write_angle(pca9685_t *d, uint8_t ch,
                                                  float angle_deg) {
  esp_err_t err = pca9685_chan_check_(d, ch, PCA9685_CH_SERVO);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = pca9685_lock_(d)) != ESP_OK) {
    return err;
  }
  const pca9685_servo_config_t *c = &d->ch[ch].cfg.servo;
  uint32_t us = pca9685_servo_angle_to_us_(c, angle_deg);
  err = pca9685_write_us_locked_(d, ch, us);
  if (err == ESP_OK) {
    d->ch[ch].value = pca9685_servo_us_to_angle_(c, us);
  }
  pca9685_unlock_(d);
  return err;
}

/**
 * @brief Mueve 'n' servos de canales CONSECUTIVOS en UNA sola transaccion I2C,
 *        de modo que arrancan a la vez (p. ej. patas de un robot). Todos deben
 *        estar registrados con pca9685_servo_attach().
 */
static inline esp_err_t pca9685_servo_write_angles(pca9685_t *d,
                                                   uint8_t first_ch, uint8_t n,
                                                   const float *angles_deg) {
  if (!pca9685_ready_(d)) {
    return ESP_ERR_INVALID_STATE;
  }
  if (angles_deg == NULL || n == 0 ||
      (unsigned)first_ch + n > PCA9685_NUM_CHANNELS) {
    return ESP_ERR_INVALID_ARG;
  }
  esp_err_t err = pca9685_lock_(d);
  if (err != ESP_OK) {
    return err;
  }
  uint8_t buf[1 + 4 * PCA9685_NUM_CHANNELS];
  uint32_t us[PCA9685_NUM_CHANNELS];
  buf[0] = (uint8_t)(PCA9685_REG_LED0_ON_L + 4U * first_ch);
  for (uint8_t i = 0; i < n; i++) {
    const pca9685_channel_t *c = &d->ch[first_ch + i];
    if (c->type != PCA9685_CH_SERVO) {
      pca9685_unlock_(d);
      return ESP_ERR_INVALID_STATE;
    }
    us[i] = pca9685_servo_angle_to_us_(&c->cfg.servo, angles_deg[i]);
    pca9685_fill_channel_(d, (uint8_t)(first_ch + i),
                          pca9685_us_to_ticks_(d, us[i]), &buf[1 + 4 * i]);
  }
  err = pca9685_i2c_write_(d, buf, 1U + 4U * n);
  if (err == ESP_OK) {
    for (uint8_t i = 0; i < n; i++) {
      pca9685_channel_t *c = &d->ch[first_ch + i];
      c->last_us = us[i];
      c->active = true;
      c->value = pca9685_servo_us_to_angle_(&c->cfg.servo, us[i]);
    }
  }
  pca9685_unlock_(d);
  return err;
}

/** Ultimo angulo ordenado (el SG90 no tiene realimentacion, no es una
 * medicion). */
static inline esp_err_t pca9685_servo_get_angle(pca9685_t *d, uint8_t ch,
                                                float *angle_deg) {
  esp_err_t err = pca9685_chan_check_(d, ch, PCA9685_CH_SERVO);
  if (err != ESP_OK) {
    return err;
  }
  if (angle_deg == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = pca9685_lock_(d)) != ESP_OK) {
    return err;
  }
  if (!d->ch[ch].active) {
    err = ESP_ERR_INVALID_STATE;
  } else {
    *angle_deg = d->ch[ch].value;
  }
  pca9685_unlock_(d);
  return err;
}

/**
 * @brief Movimiento suave BLOQUEANTE solo para la tarea que llama.
 *
 * Interpola linealmente desde la posicion actual hasta target_deg en
 * duration_ms, actualizando cada step_ms (minimo 1 tick). El mutex se libera
 * entre pasos. Si el servo aun no ha recibido ninguna orden, salta directo.
 */
static inline esp_err_t pca9685_servo_move_smooth(pca9685_t *d, uint8_t ch,
                                                  float target_deg,
                                                  uint32_t duration_ms,
                                                  uint32_t step_ms) {
  esp_err_t err = pca9685_chan_check_(d, ch, PCA9685_CH_SERVO);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = pca9685_lock_(d)) != ESP_OK) {
    return err;
  }
  const pca9685_servo_config_t *c = &d->ch[ch].cfg.servo;
  uint32_t target_us = pca9685_servo_angle_to_us_(c, target_deg);
  bool was_active = d->ch[ch].active;
  uint32_t start_us = d->ch[ch].last_us;
  pca9685_unlock_(d);

  if (!was_active || duration_ms == 0 || step_ms == 0 ||
      start_us == target_us) {
    return pca9685_servo_write_us(d, ch, target_us);
  }

  TickType_t step_ticks = pca9685_ticks_min1_(step_ms);
  uint32_t steps = duration_ms / step_ms;
  if (steps == 0) {
    steps = 1;
  }
  TickType_t last_wake = xTaskGetTickCount();
  for (uint32_t i = 1; i <= steps; i++) {
    int64_t diff = (int64_t)target_us - (int64_t)start_us;
    uint32_t us =
        (uint32_t)((int64_t)start_us + diff * (int64_t)i / (int64_t)steps);
    err = pca9685_servo_write_us(d, ch, us);
    if (err != ESP_OK) {
      return err;
    }
    vTaskDelayUntil(&last_wake, step_ticks);
  }
  return ESP_OK;
}

/* ------------------------------------------------------------------------- */
/* API: servo de giro continuo (MG90S modificado, FS90R...)                  */
/* ------------------------------------------------------------------------- */

static inline float pca9685_cr_clamp_(float s) {
  return (s > 100.0f) ? 100.0f : ((s < -100.0f) ? -100.0f : s);
}

static inline uint32_t pca9685_cr_speed_to_us_(const pca9685_cr_config_t *c,
                                               float speed) {
  speed = pca9685_cr_clamp_(speed);
  if (c->reversed) {
    speed = -speed;
  }
  if (speed > -0.05f && speed < 0.05f) {
    return c->neutral_us;
  }
  float mag, span, dir;
  if (speed > 0.0f) {
    mag = speed / 100.0f;
    span = (float)(c->max_us - c->neutral_us);
    dir = 1.0f;
  } else {
    mag = -speed / 100.0f;
    span = (float)(c->neutral_us - c->min_us);
    dir = -1.0f;
  }
  float db = (float)c->deadband_us;
  if (db > span) {
    db = span;
  }
  return (uint32_t)((float)c->neutral_us + dir * (db + mag * (span - db)) +
                    0.5f);
}

/**
 * @brief Registra un canal como servo de giro continuo. No emite pulsos
 * todavia.
 */
static inline esp_err_t pca9685_cr_attach(pca9685_t *d, uint8_t ch,
                                          const pca9685_cr_config_t *cfg) {
  if (!pca9685_ready_(d)) {
    return ESP_ERR_INVALID_STATE;
  }
  if (ch >= PCA9685_NUM_CHANNELS || cfg == NULL ||
      !(cfg->min_us < cfg->neutral_us && cfg->neutral_us < cfg->max_us)) {
    return ESP_ERR_INVALID_ARG;
  }
  esp_err_t err = pca9685_lock_(d);
  if (err != ESP_OK) {
    return err;
  }
  if (d->ch[ch].active) {
    err = ESP_ERR_INVALID_STATE;
  } else {
    memset(&d->ch[ch], 0, sizeof(d->ch[ch]));
    d->ch[ch].type = PCA9685_CH_CR;
    d->ch[ch].cfg.cr = *cfg;
  }
  pca9685_unlock_(d);
  return err;
}

/** Velocidad en % [-100, +100]; 0 = parado (pulso neutro, la senal sigue
 * activa). */
static inline esp_err_t pca9685_cr_set_speed(pca9685_t *d, uint8_t ch,
                                             float speed_pct) {
  esp_err_t err = pca9685_chan_check_(d, ch, PCA9685_CH_CR);
  if (err != ESP_OK) {
    return err;
  }
  speed_pct = pca9685_cr_clamp_(speed_pct);
  if ((err = pca9685_lock_(d)) != ESP_OK) {
    return err;
  }
  err = pca9685_write_us_locked_(
      d, ch, pca9685_cr_speed_to_us_(&d->ch[ch].cfg.cr, speed_pct));
  if (err == ESP_OK) {
    d->ch[ch].value = speed_pct;
  }
  pca9685_unlock_(d);
  return err;
}

/** Para el servo con el pulso neutro. Para paro garantizado usa
 * pca9685_release(). */
static inline esp_err_t pca9685_cr_stop(pca9685_t *d, uint8_t ch) {
  return pca9685_cr_set_speed(d, ch, 0.0f);
}

static inline esp_err_t pca9685_cr_get_speed(pca9685_t *d, uint8_t ch,
                                             float *speed_pct) {
  esp_err_t err = pca9685_chan_check_(d, ch, PCA9685_CH_CR);
  if (err != ESP_OK) {
    return err;
  }
  if (speed_pct == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  if ((err = pca9685_lock_(d)) != ESP_OK) {
    return err;
  }
  *speed_pct = d->ch[ch].active ? d->ch[ch].value : 0.0f;
  pca9685_unlock_(d);
  return ESP_OK;
}

/** Ajusta el neutro en caliente (calibracion). Si esta parado y activo, lo
 * reaplica. */
static inline esp_err_t pca9685_cr_set_neutral_us(pca9685_t *d, uint8_t ch,
                                                  uint16_t neutral_us) {
  esp_err_t err = pca9685_chan_check_(d, ch, PCA9685_CH_CR);
  if (err != ESP_OK) {
    return err;
  }
  if ((err = pca9685_lock_(d)) != ESP_OK) {
    return err;
  }
  pca9685_cr_config_t *c = &d->ch[ch].cfg.cr;
  if (neutral_us <= c->min_us || neutral_us >= c->max_us) {
    err = ESP_ERR_INVALID_ARG;
  } else {
    c->neutral_us = neutral_us;
    if (d->ch[ch].active && d->ch[ch].value == 0.0f) {
      err = pca9685_write_us_locked_(d, ch, neutral_us);
    }
  }
  pca9685_unlock_(d);
  return err;
}

/**
 * @brief Rampa de velocidad BLOQUEANTE solo para la tarea que llama (evita
 *        picos de corriente y tirones). El mutex se libera entre pasos.
 */
static inline esp_err_t pca9685_cr_ramp_to(pca9685_t *d, uint8_t ch,
                                           float target_pct,
                                           uint32_t duration_ms,
                                           uint32_t step_ms) {
  esp_err_t err = pca9685_chan_check_(d, ch, PCA9685_CH_CR);
  if (err != ESP_OK) {
    return err;
  }
  target_pct = pca9685_cr_clamp_(target_pct);
  float start;
  if ((err = pca9685_lock_(d)) != ESP_OK) {
    return err;
  }
  start = d->ch[ch].active ? d->ch[ch].value : 0.0f;
  pca9685_unlock_(d);

  if (duration_ms == 0 || step_ms == 0 || start == target_pct) {
    return pca9685_cr_set_speed(d, ch, target_pct);
  }
  TickType_t step_ticks = pca9685_ticks_min1_(step_ms);
  uint32_t steps = duration_ms / step_ms;
  if (steps == 0) {
    steps = 1;
  }
  TickType_t last_wake = xTaskGetTickCount();
  for (uint32_t i = 1; i <= steps; i++) {
    float v = start + (target_pct - start) * (float)i / (float)steps;
    err = pca9685_cr_set_speed(d, ch, v);
    if (err != ESP_OK) {
      return err;
    }
    vTaskDelayUntil(&last_wake, step_ticks);
  }
  return ESP_OK;
}

/**
 * @brief Gira a una velocidad durante un tiempo y luego para. BLOQUEANTE solo
 *        para la tarea que llama (vTaskDelay). El tiempo es aproximado: sin
 *        realimentacion no sirve como medida de posicion.
 * @param release_after true: corta los pulsos al terminar (paro garantizado);
 *                      false: envia el pulso neutro.
 */
static inline esp_err_t pca9685_cr_run_for_ms(pca9685_t *d, uint8_t ch,
                                              float speed_pct,
                                              uint32_t duration_ms,
                                              bool release_after) {
  esp_err_t err = pca9685_cr_set_speed(d, ch, speed_pct);
  if (err != ESP_OK) {
    return err;
  }
  vTaskDelay(pdMS_TO_TICKS(duration_ms));
  return release_after ? pca9685_release(d, ch) : pca9685_cr_stop(d, ch);
}

#ifdef __cplusplus
}
#endif

#endif /* PCA9685_H */
