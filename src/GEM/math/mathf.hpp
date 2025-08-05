#pragma once

#include "pch.hpp"

namespace mathf {
    // -------------------------------------------
    // Constants
    // -------------------------------------------

    const f64 E    = 2.71828182845904523536f;
    const f64 PI   = 3.14159265358979323846f;
    const f64 PI_2 = 1.57079632679489661923f;
    const f64 PI_4 = 0.785398163397448309616f;
    const f64 TAU  = 6.28318530717958647692f;

    const f64 DEG_TO_RAD = PI / 180.0;
    const f64 RAD_TO_DEG = 180.0 / PI;

    const f64 SQRT2  = 1.41421356237309504880f;
    const f64 SQRT3  = 1.73205080756887729352f;
    const f64 RSQRT2 = 0.707106781186547524401f;
    const f64 RSQRT3 = 0.577350269189625764511f;

    const f64 F32_EPSILON = 1.192092896e-07;
    const f64 F64_EPSILON = 2.2204460492503131e-016;

    // -------------------------------------------
    // Base
    // -------------------------------------------

    inline f32 sqrt(f32 x) { return sqrtf(x); }
    inline f64 sqrt(f64 x) { return sqrtl(x); }

    inline f32 pow(f32 x, f32 p) { return powf(x, p); }
    inline f64 pow(f64 x, f64 p) { return powl(x, p); }
    inline f32 exp(f32 x) { return powf(E, x); }
    inline f64 exp(f64 x) { return powl(E, x); }

    inline f32 log(f32 x) { return logf(x); }
    inline f64 log(f64 x) { return logl(x); }
    inline f32 log10(f32 x) { return log10f(x); }
    inline f64 log10(f64 x) { return log10l(x); }
    inline f32 logB(f32 b, f32 x) { return logf(x) / logf(b); }
    inline f64 logB(f64 b, f64 x) { return logl(x) / logl(b); }

    // -------------------------------------------
    // Rounding
    // -------------------------------------------

    inline f32 ceil(f32 x) { return ceilf(x); }
    inline f64 ceil(f64 x) { return ceill(x); }
    inline f32 floor(f32 x) { return floorf(x); }
    inline f64 floor(f64 x) { return floorl(x); }
    inline f32 trunc(f32 x) { return truncf(x); }
    inline f64 trunc(f64 x) { return truncl(x); }
    inline f32 round(f32 x) { return roundf(x); }
    inline f64 round(f64 x) { return roundl(x); }

    inline f32 fract(f32 x) { return x - floorf(x); }
    inline f64 fract(f64 x) { return x - floorl(x); }

    inline i32 ceil_to_int(f32 x) { return (i32)ceilf(x); }
    inline i64 ceil_to_int(f64 x) { return (i64)ceill(x); }
    inline i32 floor_to_int(f32 x) { return (i32)floorf(x); }
    inline i64 floor_to_int(f64 x) { return (i64)floorl(x); }
    inline i32 round_to_int(f32 x) { return (i32)roundf(x); }
    inline i64 round_to_int(f64 x) { return (i64)roundl(x); }

    // -------------------------------------------
    // Trigonometry
    // -------------------------------------------

    inline f32 sin(f32 x) { return sinf(x); }
    inline f64 sin(f64 x) { return sinl(x); }
    inline f32 arcsin(f32 x) { return asinf(x); }
    inline f64 arcsin(f64 x) { return asinl(x); }

    inline f32 cos(f32 x) { return cosf(x); }
    inline f64 cos(f64 x) { return cosl(x); }
    inline f32 arccos(f32 x) { return acosf(x); }
    inline f64 arccos(f64 x) { return acosl(x); }

    inline f32 tan(f32 x) { return tanf(x); }
    inline f64 tan(f64 x) { return tanl(x); }
    inline f32 arctan(f32 x) { return atanf(x); }
    inline f64 arctan(f64 x) { return atanl(x); }
    inline f32 arctan2(f32 y, f32 x) { return atan2f(y, x); }
    inline f64 arctan2(f64 y, f64 x) { return atan2l(y, x); }

    // -------------------------------------------
    // Utility
    // -------------------------------------------

    template<typename T>
    inline T min(T a, T b) {
        return a < b ? a : b;
    }

    template<typename T>
    inline T max(T a, T b) {
        return a > b ? a : b;
    }

    template<typename T>
    inline T square(T x) {
        return x * x;
    }

    template<typename T>
    inline T abs(T x) {
        return x < 0 ? -x : x;
    }

    template<typename T>
    inline T sign(T x) {
        return x == 0 ? 0 : (x > 0 ? 1 : -1);
    }

    template<typename T>
    inline T clamp(T val, T min, T max) {
        T out = (val < min) ? min : val; // min bounds
        return (out > max) ? max : out;  // max bounds
    }

    template<typename T>
    inline T clamp01(T val, T min, T max) {
        T out = (val < 0) ? 0 : val; // min bounds
        return (out > 1) ? 1 : out;  // max bounds
    }

    template<typename T>
    inline T lerp(T a, T b, T t) {
        return a + (b - a) * t;
    }

    template<typename T>
    inline T lerp01(T a, T b, T t) {
        return a + (b - a) * clamp01(t);
    }

    template<typename T>
    inline T normalize(T val, T min, T max) {
        return (val - min) / (max - min);
    }

    template<typename T>
    inline T remap(T val, T min0, T max0, T min1, T max1) {
        return ((max1 - min1) / (max0 - min0)) * (val - min0) + min1;
    }

    template<typename T>
    inline T wrap(T val, T min, T max) {
        if (min == max) return 0;
        return val - (max - min) * floor((val - min) / (max - min));
    }

    // -------------------------------------------
    // Comparison
    // -------------------------------------------

    inline bool compare_float(f32 f0, f32 f1) {
        return fabsf(f0 - f1) < F32_EPSILON;
    }

    inline bool compare_float(f64 f0, f64 f1) {
        return fabsl(f0 - f1) < F64_EPSILON;
    }

    // -------------------------------------------
    // Power of Two
    // -------------------------------------------

    template<typename T>
    inline bool is_power_of_two(T x) {
        static_assert(std::is_unsigned<T>::value, "T must be an unsigned integer type.");
        return x && !(x & (x - 1));
    }

    template<typename T>
    inline T next_power_of_two(T x) {
        static_assert(std::is_unsigned<T>::value, "T must be an unsigned integer type.");

        if (x == 0) return 1;
        u64 bits = sizeof(T) * 8; // 8 bits in a byte

        x -= 1;
        for (u64 shift = 1; shift < bits; shift *= 2) {
            x |= x >> shift;
        }
        x += 1;

        return x;
    }
}
