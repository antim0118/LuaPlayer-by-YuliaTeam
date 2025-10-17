/* made by k0dil0 */

#include <stdlib.h>
#include <math.h>

#define DEG_TO_RAD (M_PI / 180.0f)

/* Возвращает абсолютное значение х */
float vfpu_absf(float x);

/* Возвращает арккосинус х */
float vfpu_acosf(float x);

/* Возвращает арксинус х */
float vfpu_asinf(float x);

/* Возвращает арктангенс от х/у */
float vfpu_atan2f(float x, float y);

/* Возвращает арктангенс х */
float vfpu_atanf(float x);

/* Округление х вверх */
float vfpu_ceilf(float x);

/* Возвращает гиперболический косинус х */
float vfpu_coshf(float x);

/* Возвращает косинус угла x */
float vfpu_cosf(float x);

/* Преобразование радиан в градусы */
float vfpu_degf(float radians);

/* Возвращает экспоненту х */
float vfpu_expf(float x);

/* Округление х вниз */
float vfpu_floorf(float x);

/* Возвращает остаток от деления х/у */
float vfpu_fmodf(float x, float y);

/* Разложение х на мантиссу и экспоненту */
float vfpu_frexpf(float x, int* exp);

/* Умножение х на 2 в степени */
float vfpu_ldexpf(float x, int exp);

/* Возвращает логарифм по основанию 10 */
float vfpu_log10f(float x);

/* Возвращает логарифм */
float vfpu_logf(float x);

/* Возвращает максимальное из двух чисел */
float vfpu_maxf(float a, float b);

/* Возвращает минимальное из двух чисел */
float vfpu_minf(float a, float b);

/* Разделение х на целую и дробную части */
float vfpu_modff(float x, float* intpart);

/* Возведение х в степень у */
float vfpu_powf(float x, float y);

/* Преобразование градусов в радианы */
float vfpu_radf(float degrees);

/* Возвращает случайное число */
float vfpu_random();

/* Установка сида для генератора случайных чисел */
void vfpu_randomseed(unsigned int seed);

/* Возвращает гиперболический синус х */
float vfpu_sinhf(float x);

/* Возвращает синус угла х */
float vfpu_sinf(float x);

/* Возвращает квадратный корень х */
float vfpu_sqrtf(float x);

/* Возвращает гиперболический тангенс х */
float vfpu_tanhf(float x);

/* Возвращает тангенс угла х */
float vfpu_tanf(float x);

/* Возвращает котангенс угла х */
float vfpu_cotf(float x);

/* Возвращает гиперболический котангенс угла х */
float vfpu_cothf(float x);

//EOF