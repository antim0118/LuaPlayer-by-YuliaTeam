/* made by k0dil0 */

#include "vfpu_math.h"

float vfpu_absf(float x)
{
    float result;
    __asm__ volatile (
        "mtv    %1, s000\n"          // s000 = x
        "vabs.s s000, s000\n"        // s000 = |s000|
        "mfv    %0, s000\n"          // result = s000
        : "=r"(result) : "r"(x)
    );
    return result;
}

float vfpu_acosf(float x)
{
    // Аппроксимация арккосинуса с использованием полинома
    float result = (-0.69813170079773212f * x * x - 0.87266462599716477f) * x + 1.5707963267948966f;
    return result;
}

float vfpu_asinf(float x)
{
    // Аппроксимация арксинуса с использованием полинома
    float result = (0.69813170079773212f * x * x + 0.87266462599716477f) * x;
    return result;
}

float vfpu_atan2f(float x, float y)
{
    //float result;
    //__asm__ volatile (
    //    "mtv    %1, s000\n"          // s000 = y
    //    "mtv    %2, s001\n"          // s001 = x
    //    "vatan2.t s010, s000, s001\n" // s010 = atan2(s000, s001)
    //    "mfv    %0, s010\n"          // result = s010
    //    : "=r"(result) : "r"(y), "r"(x)
    //);
    return atan2f(x, y);
}

float vfpu_atanf(float x)
{
    // Аппроксимация арктангенса с использованием полинома
    float result = (0.7853981633974483f * x) / (fabsf(x) + 1.0f);
    return result;
}

float vfpu_ceilf(float x)
{
    //float result;
    //__asm__ volatile (
    //    "mtv    %1, s000\n"          // s000 = x
    //    "vceil.s s000, s000\n"       // s000 = ceil(s000)
    //    "mfv    %0, s000\n"          // result = s000
    //    : "=r"(result) : "r"(x)
    //);
    return ceilf(x);
}

float vfpu_coshf(float x)
{
    float exp_x = expf(x);
    return (exp_x + 1.0f / exp_x) * 0.5f;
}

float vfpu_cosf(float radians)
{
    float result;
    __asm__ volatile (
        "mtv    %1, s000\n"          // s000 = radians
        "vcst.s s001, VFPU_2_PI\n"   // s001 = 2/π
        "vmul.s s000, s000, s001\n"  // s000 = radians * (2/π)
        "vrot.p c010, s000, [s, c]\n" // s010 = sin(s000), s011 = cos(s000)
        "mfv    %0, s011\n"          // result = s011
        : "=r"(result) : "r"(radians)
    );
    return result;
}

float vfpu_degf(float radians)
{
    return radians * (180.0f / M_PI);
}

float vfpu_expf(float x)
{
    // Аппроксимация экспоненты с использованием ряда Тейлора
    float result = 1.0f + x + (x * x) / 2.0f + (x * x * x) / 6.0f;
    return result;
}

float vfpu_floorf(float x)
{
    //float result;
    //__asm__ volatile (
    //    "mtv    %1, s000\n"          // s000 = x
    //    "vfloor.s s000, s000\n"      // s000 = floor(s000)
    //    "mfv    %0, s000\n"          // result = s000
    //    : "=r"(result) : "r"(x)
    //);
    return floorf(x);
}

float vfpu_fmodf(float x, float y)
{
    return x - y * vfpu_floorf(x / y);
}

float vfpu_frexpf(float x, int* exp)
{
    return frexpf(x, exp); // Используем стандартную функцию
}

float vfpu_ldexpf(float x, int exp)
{
    return x * (1 << exp); // Используем битовый сдвиг
}

float vfpu_log10f(float x)
{
    return log10f(x); // Используем стандартную функцию
}

float vfpu_logf(float x)
{
    return logf(x); // Используем стандартную функцию
}

float vfpu_maxf(float a, float b)
{
    float result;
    __asm__ volatile (
        "mtv    %1, s000\n"          // s000 = a
        "mtv    %2, s001\n"          // s001 = b
        "vmax.s s000, s000, s001\n"  // s000 = max(s000, s001)
        "mfv    %0, s000\n"          // result = s000
        : "=r"(result) : "r"(a), "r"(b)
    );
    return result;
}

float vfpu_minf(float a, float b)
{
    float result;
    __asm__ volatile (
        "mtv    %1, s000\n"          // s000 = a
        "mtv    %2, s001\n"          // s001 = b
        "vmin.s s000, s000, s001\n"  // s000 = min(s000, s001)
        "mfv    %0, s000\n"          // result = s000
        : "=r"(result) : "r"(a), "r"(b)
    );
    return result;
}

float vfpu_modff(float x, float* intpart)
{
    *intpart = vfpu_floorf(x);
    return x - *intpart;
}

float vfpu_powf(float x, float y)
{
    return expf(y * logf(x)); // Используем стандартные функции
}

float vfpu_radf(float degrees)
{
    return degrees * DEG_TO_RAD;
}

float vfpu_random()
{
    return (float)rand() / RAND_MAX; // Используем стандартную функцию
}

void vfpu_randomseed(unsigned int seed)
{
    srand(seed); // Используем стандартную функцию
}

float vfpu_sinhf(float x)
{
    float exp_x = expf(x);
    return (exp_x - 1.0f / exp_x) * 0.5f;
}

float vfpu_sinf(float radians)
{
    float result;
    __asm__ volatile (
        "mtv    %1, s000\n"          // s000 = radians
        "vcst.s s001, VFPU_2_PI\n"   // s001 = 2/π
        "vmul.s s000, s000, s001\n"  // s000 = radians * (2/π)
        "vrot.p c010, s000, [s, c]\n" // s010 = sin(s000), s011 = cos(s000)
        "mfv    %0, s010\n"          // result = s010
        : "=r"(result) : "r"(radians)
    );
    return result;
}

float vfpu_sqrtf(float x)
{
    float result;
    __asm__ volatile (
        "mtv    %1, s000\n"          // s000 = x
        "vsqrt.s s000, s000\n"      // s000 = sqrt(s000)
        "mfv    %0, s000\n"          // result = s000
        : "=r"(result) : "r"(x)
    );
    return result;
}

float vfpu_tanhf(float x)
{
    float exp_x = expf(x);
    float exp_neg_x = expf(-x);
    return (exp_x - exp_neg_x) / (exp_x + exp_neg_x);
}

float vfpu_tanf(float degrees)
{
    float radians = degrees * DEG_TO_RAD;
    float sin_x, cos_x;
    __asm__ volatile (
        "mtv    %2, s000\n"          // s000 = radians
        "vcst.s s001, VFPU_2_PI\n"   // s001 = 2/π
        "vmul.s s000, s000, s001\n"  // s000 = radians * (2/π)
        "vrot.p c010, s000, [s, c]\n" // s010 = sin(s000), s011 = cos(s000)
        "mfv    %0, s010\n"          // sin_x = s010
        "mfv    %1, s011\n"          // cos_x = s011
        : "=r"(sin_x), "=r"(cos_x) : "r"(radians)
    );
    return sin_x / cos_x; // tan = sin / cos
}

float vfpu_cotf(float degrees)
{
    float radians = degrees * DEG_TO_RAD; // Преобразуем градусы в радианы
    float sin_x, cos_x;

    // Вычисляем синус и косинус через VFPU
    __asm__ volatile (
        "mtv    %2, s000\n"          // s000 = radians
        "vcst.s s001, VFPU_2_PI\n"   // s001 = 2/π
        "vmul.s s000, s000, s001\n"  // s000 = radians * (2/π)
        "vrot.p c010, s000, [s, c]\n" // s010 = sin(s000), s011 = cos(s000)
        "mfv    %0, s010\n"          // sin_x = s010
        "mfv    %1, s011\n"          // cos_x = s011
        : "=r"(sin_x), "=r"(cos_x) : "r"(radians)
    );

    // Проверка на ноль (котангенс не определен при sin(x) = 0)
    if (fabsf(sin_x) < 1e-6f) { // Порог для сравнения с нулём
        return NAN; // Возвращаем NaN (Not a Number)
    }

    return cos_x / sin_x; // cot(x) = cos(x) / sin(x)
}

float vfpu_cothf(float x)
{
    float sinh_x, cosh_x;

    // Вычисляем гиперболический синус и косинус
    float exp_x = expf(x);
    float exp_neg_x = expf(-x);
    sinh_x = (exp_x - exp_neg_x) * 0.5f; // sinh(x) = (e^x - e^(-x)) / 2
    cosh_x = (exp_x + exp_neg_x) * 0.5f; // cosh(x) = (e^x + e^(-x)) / 2

    // Проверка на ноль (гиперболический котангенс не определен при sinh(x) = 0)
    if (fabsf(sinh_x) < 1e-6f) { // Порог для сравнения с нулём
        return NAN; // Возвращаем NaN (Not a Number)
    }

    return cosh_x / sinh_x; // coth(x) = cosh(x) / sinh(x)
}

//EOF