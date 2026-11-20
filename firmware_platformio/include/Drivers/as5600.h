#ifndef AS5600_H
#define AS5600_H

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
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"

#if defined(AS5600_FORCE_LEGACY_I2C) || \
    (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0))
#define AS5600_USE_LEGACY_I2C 1
#include "driver/i2c.h"
typedef i2c_port_t as5600_hw_bus_t;
#else
#define AS5600_USE_LEGACY_I2C 0
#include "driver/i2c_master.h"
typedef i2c_master_bus_handle_t as5600_hw_bus_t;
#endif

// ----------------------------------
// Constantes y registros del AS5600 
// ----------------------------------
#define AS5600_I2C_ADDR             0x36U
#define AS5600_RESOLUTION           4096U
#define AS5600_DEG_PER_LSB          (360.0f / 4096.0f)

#define AS5600_REG_STATUS           0x0BU
#define AS5600_REG_RAW_ANGLE_H      0x0CU
#define AS5600_REG_RAW_ANGLE_L      0x0DU
#define AS5600_REG_ANGLE_H          0x0EU
#define AS5600_REG_ANGLE_L          0x0FU
#define AS5600_REG_AGC              0x1AU
#define AS5600_REG_MAGNITUDE_H      0x1BU

#define AS5600_STATUS_MD            0x20U   
#define AS5600_STATUS_ML            0x10U   
#define AS5600_STATUS_MH            0x08U   

// -----
// Tipos
// -----
typedef struct {
    as5600_hw_bus_t hw_bus;         
    int i2c_port;                   
    gpio_num_t sda_pin;             
    uint32_t i2c_hz;                
    uint32_t i2c_timeout_ms;        
    uint32_t bb_half_period_us;     
    SemaphoreHandle_t shared_lock;  
} as5600_bus_config_t;

typedef struct {
    as5600_bus_config_t cfg;
    SemaphoreHandle_t lock;
    bool owns_lock;
#if !AS5600_USE_LEGACY_I2C
    i2c_master_dev_handle_t hw_dev;
#endif
    bool hw_dev_ready;
    bool initialized;
} as5600_bus_t;

typedef struct {
    bool use_hw_i2c;        
    gpio_num_t scl_pin;     
    float zero_offset_deg;  
    bool reversed;          
} as5600_sensor_config_t;

typedef struct {
    as5600_bus_t *bus;
    as5600_sensor_config_t cfg;
    uint16_t raw_angle;     
    float angle_deg;        
    uint8_t status;         
    uint32_t errors;        
    bool initialized;
} as5600_sensor_t;

static inline esp_err_t as5600_lock_(as5600_bus_t *b) {
    if (b == NULL || b->lock == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return (xSemaphoreTake(b->lock, pdMS_TO_TICKS(100)) == pdTRUE)
               ? ESP_OK
               : ESP_ERR_TIMEOUT;
}

static inline void as5600_unlock_(as5600_bus_t *b) {
    if (b != NULL && b->lock != NULL) {
        xSemaphoreGive(b->lock);
    }
}

static inline void as5600_bb_delay_(uint32_t us) {
    if (us > 0U) {
        esp_rom_delay_us(us);
    }
}

static inline void as5600_bb_scl_high_(gpio_num_t scl, uint32_t half_us) {
    gpio_set_level(scl, 1);
    as5600_bb_delay_(half_us);
    for (int i = 0; i < 50 && gpio_get_level(scl) == 0; i++) {
        esp_rom_delay_us(2);
    }
}

static inline void as5600_bb_start_(gpio_num_t sda, gpio_num_t scl, uint32_t half_us) {
    gpio_set_level(sda, 1);
    as5600_bb_scl_high_(scl, half_us);
    gpio_set_level(sda, 0);
    as5600_bb_delay_(half_us);
    gpio_set_level(scl, 0);
    as5600_bb_delay_(half_us);
}

static inline void as5600_bb_stop_(gpio_num_t sda, gpio_num_t scl, uint32_t half_us) {
    gpio_set_level(sda, 0);
    as5600_bb_delay_(half_us);
    as5600_bb_scl_high_(scl, half_us);
    gpio_set_level(sda, 1);
    as5600_bb_delay_(half_us);
}

static inline bool as5600_bb_write_byte_(gpio_num_t sda, gpio_num_t scl,
                                         uint8_t byte, uint32_t half_us) {
    for (int bit = 7; bit >= 0; bit--) {
        gpio_set_level(sda, (byte >> bit) & 0x01U);
        as5600_bb_delay_(half_us);
        as5600_bb_scl_high_(scl, half_us);
        gpio_set_level(scl, 0);
    }
    gpio_set_level(sda, 1);
    as5600_bb_delay_(half_us);
    as5600_bb_scl_high_(scl, half_us);
    bool ack = (gpio_get_level(sda) == 0);
    gpio_set_level(scl, 0);
    as5600_bb_delay_(half_us);
    return ack;
}

static inline uint8_t as5600_bb_read_byte_(gpio_num_t sda, gpio_num_t scl,
                                           bool send_ack, uint32_t half_us) {
    uint8_t val = 0;
    gpio_set_level(sda, 1);
    for (int bit = 7; bit >= 0; bit--) {
        as5600_bb_delay_(half_us);
        as5600_bb_scl_high_(scl, half_us);
        if (gpio_get_level(sda)) {
            val |= (uint8_t)(1U << bit);
        }
        gpio_set_level(scl, 0);
    }
    gpio_set_level(sda, send_ack ? 0 : 1);
    as5600_bb_delay_(half_us);
    as5600_bb_scl_high_(scl, half_us);
    gpio_set_level(scl, 0);
    gpio_set_level(sda, 1);
    as5600_bb_delay_(half_us);
    return val;
}

static inline esp_err_t as5600_bb_read_reg_(as5600_bus_t *b, gpio_num_t scl_pin,
                                            uint8_t reg, uint8_t *out, size_t len) {
    gpio_num_t sda = b->cfg.sda_pin;
    uint32_t half_us = (b->cfg.bb_half_period_us > 0U) ? b->cfg.bb_half_period_us : 5U;
    uint32_t sda_in_sig = (b->cfg.i2c_port == 1) ? I2CEXT1_SDA_IN_IDX : I2CEXT0_SDA_IN_IDX;

    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, sda_in_sig, false);

    gpio_set_level(sda, 1);
    gpio_set_level(scl_pin, 1);
    uint32_t saved_sda_sig = GPIO.func_out_sel_cfg[sda].func_sel;
    esp_rom_gpio_connect_out_signal(sda, SIG_GPIO_OUT_IDX, false, false);
    as5600_bb_delay_(half_us);

    esp_err_t err = ESP_OK;
    as5600_bb_start_(sda, scl_pin, half_us);

    if (!as5600_bb_write_byte_(sda, scl_pin, (uint8_t)(AS5600_I2C_ADDR << 1), half_us)) {
        err = ESP_ERR_NOT_FOUND;
        goto finish;
    }
    if (!as5600_bb_write_byte_(sda, scl_pin, reg, half_us)) {
        err = ESP_FAIL;
        goto finish;
    }

    as5600_bb_start_(sda, scl_pin, half_us);
    if (!as5600_bb_write_byte_(sda, scl_pin, (uint8_t)((AS5600_I2C_ADDR << 1) | 0x01U), half_us)) {
        err = ESP_ERR_NOT_FOUND;
        goto finish;
    }

    for (size_t i = 0; i < len; i++) {
        bool ack = (i + 1U < len);
        out[i] = as5600_bb_read_byte_(sda, scl_pin, ack, half_us);
    }

finish:
    as5600_bb_stop_(sda, scl_pin, half_us);
    gpio_set_level(sda, 1);
    esp_rom_gpio_connect_out_signal(sda, saved_sda_sig, false, false);
    esp_rom_delay_us(5);
    /* Reconectar SDA al periférico I2C hardware ahora que la línea ya está en reposo (1) */
    esp_rom_gpio_connect_in_signal(sda, sda_in_sig, false);
    return err;
}

static inline esp_err_t as5600_hw_read_reg_(as5600_bus_t *b, uint8_t reg,
                                            uint8_t *out, size_t len) {
#if AS5600_USE_LEGACY_I2C
    return i2c_master_write_read_device(b->cfg.hw_bus, AS5600_I2C_ADDR,
                                        &reg, 1, out, len,
                                        pdMS_TO_TICKS(b->cfg.i2c_timeout_ms));
#else
    if (!b->hw_dev_ready) {
        return ESP_ERR_INVALID_STATE;
    }
    return i2c_master_transmit_receive(b->hw_dev, &reg, 1, out, len,
                                       (int)b->cfg.i2c_timeout_ms);
#endif
}

static inline esp_err_t as5600_bus_init(as5600_bus_t *b,
                                        const as5600_bus_config_t *cfg) {
    if (b == NULL || cfg == NULL || cfg->sda_pin < 0) {
        return ESP_ERR_INVALID_ARG;
    }
    memset(b, 0, sizeof(*b));
    b->cfg = *cfg;
    if (b->cfg.i2c_hz == 0U) {
        b->cfg.i2c_hz = 100000U;
    }
    if (b->cfg.i2c_timeout_ms == 0U) {
        b->cfg.i2c_timeout_ms = 50U;
    }
    if (b->cfg.bb_half_period_us == 0U) {
        b->cfg.bb_half_period_us = 5U;
    }

    if (cfg->shared_lock != NULL) {
        b->lock = cfg->shared_lock;
        b->owns_lock = false;
    } else {
        b->lock = xSemaphoreCreateMutex();
        if (b->lock == NULL) {
            return ESP_ERR_NO_MEM;
        }
        b->owns_lock = true;
    }

#if !AS5600_USE_LEGACY_I2C
    if (cfg->hw_bus != NULL) {
        i2c_device_config_t dc = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = AS5600_I2C_ADDR,
            .scl_speed_hz = b->cfg.i2c_hz,
        };
        esp_err_t err = i2c_master_bus_add_device(cfg->hw_bus, &dc, &b->hw_dev);
        if (err != ESP_OK) {
            if (b->owns_lock) {
                vSemaphoreDelete(b->lock);
            }
            return err;
        }
        b->hw_dev_ready = true;
    }
#else
    b->hw_dev_ready = ((int)cfg->hw_bus >= 0);
#endif

    b->initialized = true;
    return ESP_OK;
}

static inline esp_err_t as5600_sensor_init(as5600_sensor_t *s,
                                           as5600_bus_t *bus,
                                           const as5600_sensor_config_t *cfg) {
    if (s == NULL || bus == NULL || !bus->initialized || cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!cfg->use_hw_i2c && cfg->scl_pin < 0) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(s, 0, sizeof(*s));
    s->bus = bus;
    s->cfg = *cfg;

    if (!cfg->use_hw_i2c) {
        gpio_config_t scl_io = {
            .pin_bit_mask = (1ULL << cfg->scl_pin),
            .mode = GPIO_MODE_INPUT_OUTPUT_OD,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        esp_err_t err = gpio_config(&scl_io);
        if (err != ESP_OK) {
            return err;
        }
        gpio_set_level(cfg->scl_pin, 1);
    }

    s->initialized = true;
    return ESP_OK;
}

static inline esp_err_t as5600_read_angle(as5600_sensor_t *s, float *out_deg) {
    if (s == NULL || !s->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = as5600_lock_(s->bus);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t buf[2] = {0};
    if (s->cfg.use_hw_i2c) {
        err = as5600_hw_read_reg_(s->bus, AS5600_REG_ANGLE_H, buf, 2);
    } else {
        err = as5600_bb_read_reg_(s->bus, s->cfg.scl_pin, AS5600_REG_ANGLE_H, buf, 2);
    }
    as5600_unlock_(s->bus);

    if (err != ESP_OK) {
        s->errors++;
        return err;
    }

    uint16_t raw = (uint16_t)(((uint16_t)(buf[0] & 0x0FU) << 8) | buf[1]);
    float deg = (float)raw * AS5600_DEG_PER_LSB;

    if (s->cfg.reversed) {
        deg = 360.0f - deg;
        if (deg >= 360.0f) {
            deg -= 360.0f;
        }
    }

    deg -= s->cfg.zero_offset_deg;
    while (deg < 0.0f) {
        deg += 360.0f;
    }
    while (deg >= 360.0f) {
        deg -= 360.0f;
    }

    s->raw_angle = raw;
    s->angle_deg = deg;

    if (out_deg != NULL) {
        *out_deg = deg;
    }
    return ESP_OK;
}

static inline esp_err_t as5600_read_status(as5600_sensor_t *s, uint8_t *out_status) {
    if (s == NULL || !s->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = as5600_lock_(s->bus);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t st = 0;
    if (s->cfg.use_hw_i2c) {
        err = as5600_hw_read_reg_(s->bus, AS5600_REG_STATUS, &st, 1);
    } else {
        err = as5600_bb_read_reg_(s->bus, s->cfg.scl_pin, AS5600_REG_STATUS, &st, 1);
    }
    as5600_unlock_(s->bus);

    if (err != ESP_OK) {
        s->errors++;
        return err;
    }

    s->status = st;
    if (out_status != NULL) {
        *out_status = st;
    }
    return ESP_OK;
}

typedef struct {
    float alpha_min;        
    float alpha_max;        
    float noise_band_deg;   
    float fast_band_deg;    
    float angle_deg;        
    bool initialized;
} as5600_filter_t;

static inline float as5600_wrap_360_(float deg) {
    while (deg < 0.0f) {
        deg += 360.0f;
    }
    while (deg >= 360.0f) {
        deg -= 360.0f;
    }
    return deg;
}

static inline float as5600_shortest_diff_(float target, float current) {
    float diff = target - current;
    while (diff > 180.0f) {
        diff -= 360.0f;
    }
    while (diff < -180.0f) {
        diff += 360.0f;
    }
    return diff;
}

static inline void as5600_filter_init(as5600_filter_t *f,
                                      float alpha_min,
                                      float alpha_max,
                                      float noise_band_deg,
                                      float fast_band_deg) {
    if (f == NULL) {
        return;
    }
    f->alpha_min = alpha_min;
    f->alpha_max = alpha_max;
    f->noise_band_deg = noise_band_deg;
    f->fast_band_deg = (fast_band_deg > noise_band_deg) ? fast_band_deg : (noise_band_deg + 0.1f);
    f->angle_deg = 0.0f;
    f->initialized = false;
}

static inline void as5600_filter_reset(as5600_filter_t *f) {
    if (f != NULL) {
        f->initialized = false;
    }
}

static inline float as5600_filter_update(as5600_filter_t *f, float raw_deg) {
    if (f == NULL) {
        return raw_deg;
    }
    raw_deg = as5600_wrap_360_(raw_deg);

    if (!f->initialized) {
        f->angle_deg = raw_deg;
        f->initialized = true;
        return f->angle_deg;
    }

    float err = as5600_shortest_diff_(raw_deg, f->angle_deg);
    float abs_err = fabsf(err);

    float alpha;
    if (abs_err <= f->noise_band_deg) {
        alpha = f->alpha_min;
    } else if (abs_err >= f->fast_band_deg) {
        alpha = f->alpha_max;
    } else {
        float t = (abs_err - f->noise_band_deg) / (f->fast_band_deg - f->noise_band_deg);
        alpha = f->alpha_min + t * (f->alpha_max - f->alpha_min);
    }

    f->angle_deg = as5600_wrap_360_(f->angle_deg + alpha * err);
    return f->angle_deg;
}

#ifdef __cplusplus
}
#endif

#endif /* AS5600_H */
