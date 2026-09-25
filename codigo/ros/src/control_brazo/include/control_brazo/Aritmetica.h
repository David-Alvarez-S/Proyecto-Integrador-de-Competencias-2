
#ifndef ARITMETICA_H
#define ARITMETICA_H

#define PI 3.141592653589793

#define rad_grad (PI / 180)
#define grad_rad (180 / PI)

#define rad_2_grad(x) (x * rad_grad)
#define grad_2_rad(x) (x * grad_rad)

typedef float grad_t;
typedef float rad_t;

typedef struct {
  float r;
  float p;
  float y;
} euler_grad_t;
#endif // ARITMETICA_H
