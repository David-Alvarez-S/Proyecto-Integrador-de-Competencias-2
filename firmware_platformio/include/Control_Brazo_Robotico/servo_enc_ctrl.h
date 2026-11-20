#ifndef SERVO_ENC_CTRL_H
#define SERVO_ENC_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "esp_err.h"
#include "Drivers/as5600.h"
#include "Drivers/pca9685.h"

#define SEC_PI              3.14159265358979323846f
#define SEC_VEL_HIST_LEN    4

// ======================
// Configuración por eje 
// ======================
typedef struct {
    // Hardware asociado
    pca9685_t *pca;
    uint8_t pca_ch;
    as5600_bus_t *enc_bus;
    bool enc_use_hw_i2c;
    gpio_num_t enc_scl_pin;

    uint16_t min_us;            
    uint16_t neutral_us;        
    uint16_t max_us;            
    float us_umbral_pos;        
    float us_umbral_neg;        
    float us_max_offset;        
    float vel_umbral_deg_s;     
    bool servo_reversed;        

    // Calibracion del encoder AS5600 
    float enc_zero_deg;
    bool enc_reversed;         
    float filt_alpha_min;       
    float filt_alpha_max;       

    // Límites mecanicos del eje 
    float min_deg;              
    float max_deg;              

    // Parametros PID
    float kp;                   
    float ki;                   
    float kd;                   
    float kff_vel;              
    float k_grav_deg_s;         
    float grav_zero_deg;        

    float i_max_deg_s;          
    float vel_max_deg_s;        
    float error_deadband_deg;   
    float lock_tol_deg;         
} sec_config_t;

// ===============================
// Estado en tiempo real de un eje
// ===============================
typedef struct {
    sec_config_t cfg;
    as5600_sensor_t encoder;
    as5600_filter_t filtro;

    float angle_rad;
    float vel_rad_s;
    float hist_rad[SEC_VEL_HIST_LEN];
    uint8_t hist_idx;
    bool sensor_primed;

    // Consignas
    float sp_rad;
    float vel_ff_rad_s;

    // Estados internos PID
    float integrator_rad_s;
    float sigma_acc;
    float last_offset_us;

    // Diagnóstico y seguridad
    bool enabled;
    bool locked;
    bool fault;
    uint8_t mag_fail_count;
} sec_axis_t;

// ===============================
// Utilidades matemáticas internas
// ===============================
static inline float sec_clamp_(float v, float lo, float hi) {
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}

static inline float sec_deg2rad_(float deg) {
    return deg * (SEC_PI / 180.0f);
}

static inline float sec_rad2deg_(float rad) {
    return rad * (180.0f / SEC_PI);
}

static inline float sec_wrap_pi_(float rad) {
    while (rad > SEC_PI)  rad -= 2.0f * SEC_PI;
    while (rad <= -SEC_PI) rad += 2.0f * SEC_PI;
    return rad;
}

static inline void sec_apply_offset_us_(sec_axis_t *ax, float offset_us) {
    ax->last_offset_us = offset_us;
    uint8_t ch = ax->cfg.pca_ch;

    if (fabsf(offset_us) < 0.5f) {
        pca9685_cr_stop(ax->cfg.pca, ch);
        return;
    }

    const pca9685_cr_config_t *cr = &ax->cfg.pca->ch[ch].cfg.cr;
    float span = (offset_us > 0.0f)
                     ? (float)(cr->max_us - cr->neutral_us)
                     : (float)(cr->neutral_us - cr->min_us);
    if (span < 1.0f) span = 650.0f;

    float speed_pct = sec_clamp_((offset_us / span) * 100.0f, -100.0f, 100.0f);
    pca9685_cr_set_speed(ax->cfg.pca, ch, speed_pct);
}

// ============
// API Publica 
// ============

static inline esp_err_t sec_init(sec_axis_t *ax, const sec_config_t *cfg) {
    if (!ax || !cfg || !cfg->pca || !cfg->enc_bus) {
        return ESP_ERR_INVALID_ARG;
    }
    memset(ax, 0, sizeof(*ax));
    ax->cfg = *cfg;

    if (ax->cfg.min_us == 0U) ax->cfg.min_us = 1000U;
    if (ax->cfg.max_us == 0U) ax->cfg.max_us = 2300U;
    if (ax->cfg.us_max_offset <= 0.0f) ax->cfg.us_max_offset = 180.0f;
    if (ax->cfg.vel_umbral_deg_s < 5.0f) ax->cfg.vel_umbral_deg_s = 45.0f;
    if (ax->cfg.filt_alpha_min <= 0.0f) ax->cfg.filt_alpha_min = 0.25f;
    if (ax->cfg.filt_alpha_max <= 0.0f) ax->cfg.filt_alpha_max = 0.70f;
    if (ax->cfg.i_max_deg_s <= 0.0f) ax->cfg.i_max_deg_s = 25.0f;
    if (ax->cfg.vel_max_deg_s <= 0.0f) ax->cfg.vel_max_deg_s = 40.0f;
    if (ax->cfg.lock_tol_deg <= 0.0f) ax->cfg.lock_tol_deg = 1.5f;

    pca9685_cr_config_t cr_cfg = PCA9685_CR_CONFIG_DEFAULT();
    cr_cfg.min_us      = ax->cfg.min_us;
    cr_cfg.neutral_us  = ax->cfg.neutral_us;
    cr_cfg.max_us      = ax->cfg.max_us;
    cr_cfg.deadband_us = 0U;
    cr_cfg.reversed    = ax->cfg.servo_reversed;

    esp_err_t err = pca9685_cr_attach(ax->cfg.pca, ax->cfg.pca_ch, &cr_cfg);
    if (err != ESP_OK) return err;
    pca9685_release(ax->cfg.pca, ax->cfg.pca_ch);

    as5600_sensor_config_t enc_cfg = {
        .use_hw_i2c      = ax->cfg.enc_use_hw_i2c,
        .scl_pin         = ax->cfg.enc_scl_pin,
        .zero_offset_deg = ax->cfg.enc_zero_deg,
        .reversed        = ax->cfg.enc_reversed,
    };
    err = as5600_sensor_init(&ax->encoder, ax->cfg.enc_bus, &enc_cfg);
    if (err != ESP_OK) return err;

    as5600_filter_init(&ax->filtro, ax->cfg.filt_alpha_min, ax->cfg.filt_alpha_max, 0.30f, 1.50f);
    return ESP_OK;
}

static inline esp_err_t sec_poll_sensor(sec_axis_t *ax, float dt_s, bool check_magnet) {
    if (!ax) return ESP_ERR_INVALID_ARG;

    float deg_raw = 0.0f;
    esp_err_t err = as5600_read_angle(&ax->encoder, &deg_raw);
    if (err != ESP_OK) {
        ax->fault = true;
        return err;
    }

    if (check_magnet) {
        uint8_t st = 0;
        if (as5600_read_status(&ax->encoder, &st) != ESP_OK) {
            ax->fault = true;
            return ESP_FAIL;
        }
        bool md = (st & AS5600_STATUS_MD) != 0U;
        if (!md) {
            ax->mag_fail_count++;
            if (st == 0x00U || ax->mag_fail_count >= 8U) {
                ax->fault = true;
                return ESP_ERR_INVALID_RESPONSE;
            }
        } else {
            ax->mag_fail_count = 0;
        }
    }

    float deg_filt = as5600_filter_update(&ax->filtro, deg_raw);
    float new_rad = sec_wrap_pi_(sec_deg2rad_(deg_filt));

    if (!ax->sensor_primed || dt_s <= 0.0f) {
        ax->angle_rad = new_rad;
        ax->vel_rad_s = 0.0f;
        for (int k = 0; k < SEC_VEL_HIST_LEN; k++) {
            ax->hist_rad[k] = new_rad;
        }
        ax->hist_idx = 0;
        ax->sensor_primed = true;
    } else {
        float old_rad = ax->hist_rad[ax->hist_idx];
        ax->hist_rad[ax->hist_idx] = new_rad;
        ax->hist_idx = (ax->hist_idx + 1) % SEC_VEL_HIST_LEN;

        float vel_inst = sec_wrap_pi_(new_rad - old_rad) / ((float)SEC_VEL_HIST_LEN * dt_s);
        ax->vel_rad_s += 0.35f * (vel_inst - ax->vel_rad_s);
        ax->angle_rad = new_rad;
    }

    return ESP_OK;
}

static inline esp_err_t sec_enable(sec_axis_t *ax) {
    if (!ax) return ESP_ERR_INVALID_ARG;
    if (!ax->sensor_primed) {
        esp_err_t err = sec_poll_sensor(ax, 0.0f, true);
        if (err != ESP_OK) return err;
    }
    ax->sp_rad           = ax->angle_rad;
    ax->vel_ff_rad_s     = 0.0f;
    ax->integrator_rad_s = 0.0f;
    ax->sigma_acc        = 0.0f;
    ax->last_offset_us   = 0.0f;
    ax->fault            = false;
    ax->enabled          = true;
    return ESP_OK;
}

static inline void sec_disable(sec_axis_t *ax, bool release_pwm) {
    if (!ax) return;
    ax->enabled          = false;
    ax->locked           = false;
    ax->integrator_rad_s = 0.0f;
    ax->sigma_acc        = 0.0f;
    ax->last_offset_us   = 0.0f;
    if (release_pwm) {
        pca9685_release(ax->cfg.pca, ax->cfg.pca_ch);
    } else {
        pca9685_cr_stop(ax->cfg.pca, ax->cfg.pca_ch);
    }
}


static inline void sec_set_target_deg(sec_axis_t *ax, float target_deg) {
    if (!ax) return;
    float clamped = sec_clamp_(target_deg, ax->cfg.min_deg, ax->cfg.max_deg);
    ax->sp_rad       = sec_wrap_pi_(sec_deg2rad_(clamped));
    ax->vel_ff_rad_s = 0.0f;
}


static inline void sec_set_trajectory_deg(sec_axis_t *ax, float target_deg, float vel_ff_deg_s) {
    if (!ax) return;
    float clamped = sec_clamp_(target_deg, ax->cfg.min_deg, ax->cfg.max_deg);
    ax->sp_rad       = sec_wrap_pi_(sec_deg2rad_(clamped));
    ax->vel_ff_rad_s = sec_deg2rad_(vel_ff_deg_s);
}

static inline void sec_set_trajectory_rad(sec_axis_t *ax, float target_rad, float vel_ff_rad_s) {
    if (!ax) return;
    float min_r = sec_deg2rad_(ax->cfg.min_deg);
    float max_r = sec_deg2rad_(ax->cfg.max_deg);
    ax->sp_rad       = sec_wrap_pi_(sec_clamp_(target_rad, min_r, max_r));
    ax->vel_ff_rad_s = vel_ff_rad_s;
}

static inline esp_err_t sec_update(sec_axis_t *ax, float dt_s, bool check_magnet) {
    if (!ax || dt_s <= 0.0f) return ESP_ERR_INVALID_ARG;

    esp_err_t err = sec_poll_sensor(ax, dt_s, check_magnet);
    if (err != ESP_OK) {
        if (ax->enabled) sec_disable(ax, true);
        return err;
    }

    if (!ax->enabled) {
        return ESP_OK;
    }


    float error_rad = sec_wrap_pi_(ax->sp_rad - ax->angle_rad);
    float abs_err_rad = fabsf(error_rad);

    float lock_tol_rad = sec_deg2rad_(ax->cfg.lock_tol_deg);
    ax->locked = (fabsf(ax->vel_ff_rad_s) < 1e-3f) && (abs_err_rad <= lock_tol_rad);

    float db_rad = sec_deg2rad_(ax->cfg.error_deadband_deg);
    float err_eff = (fabsf(ax->vel_ff_rad_s) < 1e-3f && abs_err_rad < db_rad) ? 0.0f : error_rad;

    float vel_max_rad_s = sec_deg2rad_(ax->cfg.vel_max_deg_s);
    float u_ff_vel = ax->cfg.kff_vel * ax->vel_ff_rad_s;

    float vel_target_rad_s = sec_clamp_(
        u_ff_vel + (ax->cfg.kp * err_eff),
        -vel_max_rad_s,
        +vel_max_rad_s
    );

    float vel_error_rad_s = vel_target_rad_s - ax->vel_rad_s;

    float grav_angle = sec_wrap_pi_(ax->angle_rad - sec_deg2rad_(ax->cfg.grav_zero_deg));
    float u_ff_grav  = sec_deg2rad_(ax->cfg.k_grav_deg_s) * sinf(grav_angle);

    float vel_umbral_rad_s = sec_deg2rad_(ax->cfg.vel_umbral_deg_s);
    if (vel_umbral_rad_s < 0.05f) vel_umbral_rad_s = 0.78f;

    float us_umbral_avg = 0.5f * (ax->cfg.us_umbral_pos + ax->cfg.us_umbral_neg);
    if (us_umbral_avg < 1.0f) us_umbral_avg = 50.0f;

    float max_effort_rad_s = vel_umbral_rad_s * (ax->cfg.us_max_offset / us_umbral_avg);
    float i_max_rad_s      = sec_deg2rad_(ax->cfg.i_max_deg_s);

    float u_pre = vel_target_rad_s + u_ff_grav + ax->integrator_rad_s + (ax->cfg.kd * vel_error_rad_s);
    bool sat_pos = (u_pre >= max_effort_rad_s) && (err_eff > 0.0f);
    bool sat_neg = (u_pre <= -max_effort_rad_s) && (err_eff < 0.0f);

    if (!sat_pos && !sat_neg) {
        if (fabsf(err_eff) > 0.0f) {
            ax->integrator_rad_s = sec_clamp_(
                ax->integrator_rad_s + (ax->cfg.ki * vel_error_rad_s * dt_s),
                -i_max_rad_s,
                +i_max_rad_s
            );
        } else {
            ax->integrator_rad_s *= 0.92f;
        }
    }

    float effort_rad_s = sec_clamp_(
        vel_target_rad_s + u_ff_grav + ax->integrator_rad_s + (ax->cfg.kd * vel_error_rad_s),
        -max_effort_rad_s,
        +max_effort_rad_s
    );
    float offset_us = 0.0f;

    if (fabsf(err_eff) > 0.0f || fabsf(u_ff_grav) > 0.01f) {
        ax->sigma_acc = 0.0f;
        float ratio = sec_clamp_(fabsf(effort_rad_s) / max_effort_rad_s, 0.0f, 1.0f);
        offset_us = ratio * ax->cfg.us_max_offset;
        if (effort_rad_s < 0.0f) {
            offset_us = -offset_us;
        }
    }

    sec_apply_offset_us_(ax, offset_us);
    return ESP_OK;
}

// ============================= 
// Ajuste en caliente y getters  
// ============================= 
static inline void sec_set_pid(sec_axis_t *ax, float kp, float ki, float kd) {
    if (!ax) return;
    ax->cfg.kp = kp;
    ax->cfg.ki = ki;
    ax->cfg.kd = kd;
    ax->integrator_rad_s = 0.0f;
}

static inline void sec_set_gravity(sec_axis_t *ax, float k_grav_deg_s, float grav_zero_deg) {
    if (!ax) return;
    ax->cfg.k_grav_deg_s  = k_grav_deg_s;
    ax->cfg.grav_zero_deg = grav_zero_deg;
}

static inline void sec_set_neutral_us(sec_axis_t *ax, uint16_t neutral_us) {
    if (!ax) return;
    ax->cfg.neutral_us = neutral_us;
    pca9685_cr_set_neutral_us(ax->cfg.pca, ax->cfg.pca_ch, neutral_us);
}

static inline float sec_get_angle_deg(const sec_axis_t *ax) {
    return ax ? sec_rad2deg_(ax->angle_rad) : 0.0f;
}

static inline float sec_get_angle_rad(const sec_axis_t *ax) {
    return ax ? ax->angle_rad : 0.0f;
}

static inline float sec_get_vel_deg_s(const sec_axis_t *ax) {
    return ax ? sec_rad2deg_(ax->vel_rad_s) : 0.0f;
}

static inline bool sec_is_locked(const sec_axis_t *ax) {
    return ax ? ax->locked : false;
}

#ifdef __cplusplus
}
#endif

#endif /* SERVO_ENC_CTRL_H */
