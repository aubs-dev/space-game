#pragma once

#include "pch.hpp"

#include "GEM/math/mathf.hpp"

// -------------------------------------------
// Vector 2
// -------------------------------------------

template<typename T = f32>
struct Vec2 {
    static_assert(std::is_arithmetic<T>::value, "Vec2 can only be instantiated with arithmetic types.");

    Vec2() : x(T(0)), y(T(0)) {}
    Vec2(T _x, T _y) : x(_x), y(_y) {}
    Vec2(const Vec2 &v0) : x(v0.x), y(v0.y) {}

    union {
        struct {
            T x, y;
        };
        struct {
            T w, h;
        };
        struct {
            T u, v;
        };
        T values[2];
    };

    T& operator[](size_t index) {
        assert(index < 2 && "Index out of range");
        return values[index];
    }

    const T& operator[](size_t index) const {
        assert(index < 2 && "Index out of range");
        return values[index];
    }

    // Base Values
    static const Vec2 zero;
    static const Vec2 one;
    static const Vec2 right;
    static const Vec2 left;
    static const Vec2 up;
    static const Vec2 down;

    // Operators
    Vec2 operator+(const Vec2& v0) const;
    Vec2 operator-(const Vec2& v0) const;

    Vec2 operator*(T val) const;
    Vec2 operator/(T val) const;

    Vec2 operator-() const;

    Vec2& operator=(const Vec2& v0) = default;
    Vec2& operator+=(const Vec2& v0);
    Vec2& operator-=(const Vec2& v0);
    Vec2& operator*=(T val);
    Vec2& operator/=(T val);

    // Functions
    // TODO: Functions should return T, why do they return doubles for everything???
    inline f64 length() const;
    inline f64 length_squared() const;
    inline Vec2 normalize() const;
    static Vec2 clamp(const Vec2& v0, const Vec2& min, const Vec2& max);

    static f64 distance(const Vec2& v0, const Vec2& v1);
    static f64 distance_squared(const Vec2& v0, const Vec2& v1);

    static f64 dot(const Vec2& v0, const Vec2& v1);
    static f64 cross(const Vec2& v0, const Vec2& v1);

    static f64 angle_direction(const Vec2& origin, const Vec2& v0);
    static f64 angle_between(const Vec2& v0, const Vec2& v1, const Vec2& origin);
    static Vec2 rotate(const Vec2& v0, const Vec2& origin, f64 angle);
};

using Vec2f = Vec2<f32>;
using Vec2d = Vec2<f64>;
using Vec2u = Vec2<u32>;
using Vec2i = Vec2<i32>;

// -------------------------------------------
// Vector 3
// -------------------------------------------

template<typename T = f32>
struct Vec3 {
    static_assert(std::is_arithmetic<T>::value, "Vec3 can only be instantiated with arithmetic types.");

    Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
    Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    Vec3(const Vec3 &v0) : x(v0.x), y(v0.y), z(v0.z) {}

    union {
        struct {
            T x, y, z;
        };
        T values[3];
    };

    T& operator[](size_t index) {
        assert(index < 3 && "Index out of range");
        return values[index];
    }

    const T& operator[](size_t index) const {
        assert(index < 3 && "Index out of range");
        return values[index];
    }

    // Base Values
    static const Vec3 zero;
    static const Vec3 one;
    static const Vec3 right;
    static const Vec3 left;
    static const Vec3 up;
    static const Vec3 down;
    static const Vec3 front;
    static const Vec3 back;

    // Operators
    Vec3 operator+(const Vec3& v0) const;
    Vec3 operator-(const Vec3& v0) const;

    Vec3 operator*(T val) const;
    Vec3 operator/(T val) const;

    Vec3 operator-() const;

    Vec3& operator=(const Vec3& v0) = default;
    Vec3& operator+=(const Vec3& v0);
    Vec3& operator-=(const Vec3& v0);
    Vec3& operator*=(T val);
    Vec3& operator/=(T val);

    // Functions
    // TODO: Functions should return T, why do they return doubles for everything???
    inline f64 length() const;
    inline f64 length_squared() const;
    inline Vec3 normalize() const;
    static Vec3 clamp(const Vec3& v0, const Vec3& min, const Vec3& max);

    static f64 distance(const Vec3& v0, const Vec3& v1);
    static f64 distance_squared(const Vec3& v0, const Vec3& v1);

    static f64 dot(const Vec3& v0, const Vec3& v1);
    static f64 cross(const Vec3& v0, const Vec3& v1);
};

using Vec3f = Vec3<f32>;
using Vec3d = Vec3<f64>;
using Vec3u = Vec3<u32>;
using Vec3i = Vec3<i32>;

// -------------------------------------------
// Vector 4
// -------------------------------------------

template<typename T = f32>
struct Vec4 {
    static_assert(std::is_arithmetic<T>::value, "Vec4 can only be instantiated with arithmetic types.");

    Vec4() : x(T(0)), y(T(0)), z(T(0)), w(T(0)) {}
    Vec4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vec4(const Vec4 &v0) : x(v0.x), y(v0.y), z(v0.z), w(v0.w) {}

    union {
        struct {
            T x, y, z, w;
        };
        T values[4];
    };

    T& operator[](size_t index) {
        assert(index < 4 && "Index out of range");
        return values[index];
    }

    const T& operator[](size_t index) const {
        assert(index < 4 && "Index out of range");
        return values[index];
    }

    // Base Values
    static const Vec4 zero;
    static const Vec4 one;

    // Operators
    Vec4 operator+(const Vec4& v0) const;
    Vec4 operator-(const Vec4& v0) const;

    Vec4 operator*(T val) const;
    Vec4 operator/(T val) const;

    Vec4 operator-() const;

    Vec4& operator=(const Vec4& v0) = default;
    Vec4& operator+=(const Vec4& v0);
    Vec4& operator-=(const Vec4& v0);
    Vec4& operator*=(T val);
    Vec4& operator/=(T val);

    // Functions
    // TODO: Functions should return T, why do they return doubles for everything???
    inline f64 length() const;
    inline f64 length_squared() const;
    inline Vec4 normalize() const;
    static Vec4 clamp(const Vec4& v0, const Vec4& min, const Vec4& max);
};

using Vec4f = Vec4<f32>;
using Vec4d = Vec4<f64>;
using Vec4u = Vec4<u32>;
using Vec4i = Vec4<i32>;

// -------------------------------------------
// Base Values
// -------------------------------------------

// -- Vector 2
template<typename T> const Vec2<T> Vec2<T>::zero  = Vec2<T>();
template<typename T> const Vec2<T> Vec2<T>::one   = Vec2<T>(T( 1), T( 1));
template<typename T> const Vec2<T> Vec2<T>::right = Vec2<T>(T( 1), T( 0));
template<typename T> const Vec2<T> Vec2<T>::left  = Vec2<T>(T(-1), T( 0));
template<typename T> const Vec2<T> Vec2<T>::up    = Vec2<T>(T( 0), T( 1));
template<typename T> const Vec2<T> Vec2<T>::down  = Vec2<T>(T( 0), T(-1));

// -- Vector 3
template<typename T> const Vec3<T> Vec3<T>::zero  = Vec3<T>();
template<typename T> const Vec3<T> Vec3<T>::one   = Vec3<T>(T( 1), T( 1), T( 1));
template<typename T> const Vec3<T> Vec3<T>::right = Vec3<T>(T( 1), T( 0), T( 0));
template<typename T> const Vec3<T> Vec3<T>::left  = Vec3<T>(T(-1), T( 0), T( 0));
template<typename T> const Vec3<T> Vec3<T>::up    = Vec3<T>(T( 0), T( 1), T( 0));
template<typename T> const Vec3<T> Vec3<T>::down  = Vec3<T>(T( 0), T(-1), T( 0));
template<typename T> const Vec3<T> Vec3<T>::front = Vec3<T>(T( 0), T( 0), T( 1));
template<typename T> const Vec3<T> Vec3<T>::back  = Vec3<T>(T( 0), T( 0), T(-1));

// -- Vector 4
template<typename T> const Vec4<T> Vec4<T>::zero  = Vec4<T>();
template<typename T> const Vec4<T> Vec4<T>::one   = Vec4<T>(T(1), T(1), T(1), T(1));

// -------------------------------------------
// Addition
// -------------------------------------------

// Vector 2
template<typename T>
Vec2<T> Vec2<T>::operator+(const Vec2& v0) const {
    return Vec2<T>(x + v0.x, y + v0.y);
}

template<typename T>
Vec2<T>& Vec2<T>::operator+=(const Vec2& v0) {
    x += v0.x;
    y += v0.y;
    return *this;
}

// Vector 3
template<typename T>
Vec3<T> Vec3<T>::operator+(const Vec3& v0) const {
    return Vec3<T>(x + v0.x, y + v0.y, z + v0.z);
}

template<typename T>
Vec3<T>& Vec3<T>::operator+=(const Vec3& v0) {
    x += v0.x;
    y += v0.y;
    z += v0.z;
    return *this;
}

// Vector 4
template<typename T>
Vec4<T> Vec4<T>::operator+(const Vec4& v0) const {
    return Vec4<T>(x + v0.x, y + v0.y, z + v0.z, w + v0.w);
}

template<typename T>
Vec4<T>& Vec4<T>::operator+=(const Vec4& v0) {
    x += v0.x;
    y += v0.y;
    z += v0.z;
    w += v0.w;
    return *this;
}

// -------------------------------------------
// Subtraction
// -------------------------------------------

// Vector 2
template<typename T>
Vec2<T> Vec2<T>::operator-(const Vec2& v0) const {
    return Vec2<T>(x - v0.x, y - v0.y);
}

template<typename T>
Vec2<T> Vec2<T>::operator-() const {
    return Vec2<T>(-x, -y);
}

template<typename T>
Vec2<T>& Vec2<T>::operator-=(const Vec2& v0) {
    x -= v0.x;
    y -= v0.y;
    return *this;
}

// Vector 3
template<typename T>
Vec3<T> Vec3<T>::operator-(const Vec3& v0) const {
    return Vec3<T>(x - v0.x, y - v0.y, z - v0.z);
}

template<typename T>
Vec3<T> Vec3<T>::operator-() const {
    return Vec3<T>(-x, -y, -z);
}

template<typename T>
Vec3<T>& Vec3<T>::operator-=(const Vec3& v0) {
    x -= v0.x;
    y -= v0.y;
    z -= v0.z;
    return *this;
}

// Vector 4
template<typename T>
Vec4<T> Vec4<T>::operator-(const Vec4& v0) const {
    return Vec4<T>(x - v0.x, y - v0.y, z - v0.z, w - v0.w);
}

template<typename T>
Vec4<T> Vec4<T>::operator-() const {
    return Vec4<T>(-x, -y, -z, -w);
}

template<typename T>
Vec4<T>& Vec4<T>::operator-=(const Vec4& v0) {
    x -= v0.x;
    y -= v0.y;
    z -= v0.z;
    w -= v0.w;
    return *this;
}

// -------------------------------------------
// Multiplication
// -------------------------------------------

// Vector 2
template<typename T>
Vec2<T> Vec2<T>::operator*(T val) const {
    return Vec2<T>(x * val, y * val);
}

template<typename T>
Vec2<T>& Vec2<T>::operator*=(T val) {
    x *= val;
    y *= val;
    return *this;
}

// Vector 3
template<typename T>
Vec3<T> Vec3<T>::operator*(T val) const {
    return Vec3<T>(x * val, y * val, z * val);
}

template<typename T>
Vec3<T>& Vec3<T>::operator*=(T val) {
    x *= val;
    y *= val;
    z *= val;
    return *this;
}

// Vector 4
template<typename T>
Vec4<T> Vec4<T>::operator*(T val) const {
    return Vec4<T>(x * val, y * val, z * val, w * val);
}

template<typename T>
Vec4<T>& Vec4<T>::operator*=(T val) {
    x *= val;
    y *= val;
    z *= val;
    w *= val;
    return *this;
}

// -------------------------------------------
// Division
// -------------------------------------------

// Vector 2
template<typename T>
Vec2<T> Vec2<T>::operator/(T val) const {
    return Vec2<T>(x / val, y / val);
}

template<typename T>
Vec2<T>& Vec2<T>::operator/=(T val) {
    x /= val;
    y /= val;
    return *this;
}

// Vector 3
template<typename T>
Vec3<T> Vec3<T>::operator/(T val) const {
    return Vec3<T>(x / val, y / val, z / val);
}

template<typename T>
Vec3<T>& Vec3<T>::operator/=(T val) {
    x /= val;
    y /= val;
    z /= val;
    return *this;
}

// Vector 4
template<typename T>
Vec4<T> Vec4<T>::operator/(T val) const {
    return Vec4<T>(x / val, y / val, z / val, w / val);
}

template<typename T>
Vec4<T>& Vec4<T>::operator/=(T val) {
    x /= val;
    y /= val;
    z /= val;
    w /= val;
    return *this;
}

// -------------------------------------------
// Size
// -------------------------------------------

// Vector 2
template<typename T>
inline f64 Vec2<T>::length() const {
    return mathf::sqrt(x * x + y * y);
}

template<typename T>
inline f64 Vec2<T>::length_squared() const {
    return x * x + y * y;
}

template<typename T>
inline Vec2<T> Vec2<T>::normalize() const {
    f64 len = length();
    if (len > 0) return Vec2<T>(x / len, y / len);
    return Vec2<T>();
}

template<typename T>
Vec2<T> Vec2<T>::clamp(const Vec2& v0, const Vec2& min, const Vec2& max) {
    f64 x = mathf::clamp(v0.x, min.x, max.x);
    f64 y = mathf::clamp(v0.y, min.y, max.y);
    return Vec2<T>(x, y);
}

// Vector 3
template<typename T>
inline f64 Vec3<T>::length() const {
    return mathf::sqrt(x * x + y * y + z * z);
}

template<typename T>
inline f64 Vec3<T>::length_squared() const {
    return x * x + y * y + z * z;
}

template<typename T>
inline Vec3<T> Vec3<T>::normalize() const {
    f64 len = length();
    if (len > 0) return Vec3<T>(x / len, y / len, z / len);
    return Vec3<T>();
}

template<typename T>
Vec3<T> Vec3<T>::clamp(const Vec3& v0, const Vec3& min, const Vec3& max) {
    f64 x = mathf::clamp(v0.x, min.x, max.x);
    f64 y = mathf::clamp(v0.y, min.y, max.y);
    f64 z = mathf::clamp(v0.z, min.z, max.z);
    return Vec3<T>(x, y, z);
}

// Vector 4
template<typename T>
inline f64 Vec4<T>::length() const {
    return mathf::sqrt(x * x + y * y + z * z + w * w);
}

template<typename T>
inline f64 Vec4<T>::length_squared() const {
    return x * x + y * y + z * z + w * w;
}

template<typename T>
inline Vec4<T> Vec4<T>::normalize() const {
    f64 len = length();
    if (len > 0) return Vec4<T>(x / len, y / len, z / len, w / len);
    return Vec4<T>();
}

template<typename T>
Vec4<T> Vec4<T>::clamp(const Vec4& v0, const Vec4& min, const Vec4& max) {
    f64 x = mathf::clamp(v0.x, min.x, max.x);
    f64 y = mathf::clamp(v0.y, min.y, max.y);
    f64 z = mathf::clamp(v0.z, min.z, max.z);
    f64 w = mathf::clamp(v0.w, min.w, max.w);
    return Vec4<T>(x, y, z, w);
}

// -------------------------------------------
// Distance
// -------------------------------------------

// Vector 2
template<typename T>
f64 Vec2<T>::distance(const Vec2& v0, const Vec2& v1) {
    return mathf::sqrt((v0.x - v1.x) * (v0.x - v1.x) + (v0.y - v1.y) * (v0.y - v1.y));
}

template<typename T>
f64 Vec2<T>::distance_squared(const Vec2& v0, const Vec2& v1) {
    return (v0.x - v1.x) * (v0.x - v1.x) + (v0.y - v1.y) * (v0.y - v1.y);
}

// Vector 3
template<typename T>
f64 Vec3<T>::distance(const Vec3& v0, const Vec3& v1) {
    return mathf::sqrt((v0.x - v1.x) * (v0.x - v1.x) + (v0.y - v1.y) * (v0.y - v1.y) + (v0.z - v1.z) * (v0.z - v1.z));
}

template<typename T>
f64 Vec3<T>::distance_squared(const Vec3& v0, const Vec3& v1) {
    return (v0.x - v1.x) * (v0.x - v1.x) + (v0.y - v1.y) * (v0.y - v1.y) + (v0.z - v1.z) * (v0.z - v1.z);
}

// -------------------------------------------
// Dot/Cross Products
// -------------------------------------------

// Vector 2
template<typename T>
f64 Vec2<T>::dot(const Vec2& v0, const Vec2& v1) {
    return v0.x * v1.x + v0.y * v1.y;
}

template<typename T>
f64 Vec2<T>::cross(const Vec2& v0, const Vec2& v1) {
    return v0.x * v1.y - v0.y * v1.x;
}

// Vector 3
template<typename T>
f64 Vec3<T>::dot(const Vec3& v0, const Vec3& v1) {
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

template<typename T>
f64 Vec3<T>::cross(const Vec3& v0, const Vec3& v1) {
    return v0.y * v1.z - v0.z * v1.y, v0.z * v1.x - v0.x * v1.z, v0.x * v1.y - v0.y * v1.x;
}

// -------------------------------------------
// Angles
// -------------------------------------------

// Vector 2
template<typename T>
f64 Vec2<T>::angle_direction(const Vec2& origin, const Vec2& v0) {
    Vec2<T> v0_offset = v0 - origin;
    return mathf::arctan2(v0_offset.y, v0_offset.x);
}

template<typename T>
f64 Vec2<T>::angle_between(const Vec2& v0, const Vec2& v1, const Vec2& origin) {
    Vec2<T> v0_offset = v0 - origin;
    Vec2<T> v1_offset = v1 - origin;

    f64 angle = mathf::arctan2(cross(v0_offset, v1_offset), dot(v0_offset, v1_offset));
    return mathf::abs(angle);
}

template<typename T>
Vec2<T> Vec2<T>::rotate(const Vec2& v0, const Vec2& origin, f64 angle) {
    f64 cosVal = mathf::cos(angle);
    f64 sinVal = mathf::sin(angle);

    Vec2<T> v0_offset = v0 - origin;

    T x = v0_offset.x * cosVal - v0_offset.y * sinVal;
    T y = v0_offset.x * sinVal + v0_offset.y * cosVal;

    return Vec2<T>(x + origin.x, y + origin.y);
}
