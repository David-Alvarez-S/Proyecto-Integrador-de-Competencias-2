#ifndef TFT_COMMON_H
#define TFT_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "esp_idf_version.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TFT_NATIVE_WIDTH 240
#define TFT_NATIVE_HEIGHT 320

typedef enum {
  TFT_ROTATION_0 = 0, // Vertical                    (240 x 320)
  TFT_ROTATION_1,     // Horizontal                  (320 x 240)
  TFT_ROTATION_2,     // Vertical invertido          (240 x 320)
  TFT_ROTATION_3,     // Horizontal invertido        (320 x 240)
} tft_rotation_t;

// Tamano de pantalla para una rotacion.
static inline void tft_rotation_size(tft_rotation_t r, uint16_t *w,
                                     uint16_t *h) {
  bool landscape = (r == TFT_ROTATION_1 || r == TFT_ROTATION_3);
  if (w)
    *w = landscape ? TFT_NATIVE_HEIGHT : TFT_NATIVE_WIDTH;
  if (h)
    *h = landscape ? TFT_NATIVE_WIDTH : TFT_NATIVE_HEIGHT;
}

// Valor del registro MADCTL (0x36) para una rotacion.
static inline uint8_t tft_rotation_madctl(tft_rotation_t r, bool bgr) {
  static const uint8_t base[4] = {0x40, 0x20, 0x80, 0xE0};
  return (uint8_t)(base[(unsigned)r & 3U] | (bgr ? 0x08 : 0x00));
}

// Coordenadas nativas -> coordenadas de la rotacion r.
static inline void tft_native_to_rot(tft_rotation_t r, int x0, int y0, int *x,
                                     int *y) {
  switch (r) {
  default:
  case TFT_ROTATION_0:
    *x = x0;
    *y = y0;
    break;
  case TFT_ROTATION_1:
    *x = y0;
    *y = (TFT_NATIVE_WIDTH - 1) - x0;
    break;
  case TFT_ROTATION_2:
    *x = (TFT_NATIVE_WIDTH - 1) - x0;
    *y = (TFT_NATIVE_HEIGHT - 1) - y0;
    break;
  case TFT_ROTATION_3:
    *x = (TFT_NATIVE_HEIGHT - 1) - y0;
    *y = x0;
    break;
  }
}

static inline void tft_rot_to_native(tft_rotation_t r, int x, int y, int *x0,
                                     int *y0) {
  switch (r) {
  default:
  case TFT_ROTATION_0:
    *x0 = x;
    *y0 = y;
    break;
  case TFT_ROTATION_1:
    *x0 = (TFT_NATIVE_WIDTH - 1) - y;
    *y0 = x;
    break;
  case TFT_ROTATION_2:
    *x0 = (TFT_NATIVE_WIDTH - 1) - x;
    *y0 = (TFT_NATIVE_HEIGHT - 1) - y;
    break;
  case TFT_ROTATION_3:
    *x0 = y;
    *y0 = (TFT_NATIVE_HEIGHT - 1) - x;
    break;
  }
}

// portYIELD_FROM_ISR cambia de firma entre IDF 4.4 y 5.x
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#define TFT_YIELD_FROM_ISR(hpw) portYIELD_FROM_ISR(hpw)
#else
#define TFT_YIELD_FROM_ISR(hpw)                                                \
  do {                                                                         \
    (void)(hpw);                                                               \
    portYIELD_FROM_ISR();                                                      \
  } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* TFT_COMMON_H */
