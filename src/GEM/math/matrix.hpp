#pragma once

#include "pch.hpp"

#include "GEM/math/mathf.hpp"

// -------------------------------------------
// Matrix 3x3 (Column-Major)
// -------------------------------------------

template<typename T = f32>
struct Mat3_Array {
    static_assert(std::is_floating_point<T>::value, "Mat3_Array can only be instantiated with floating point types.");

    Mat3_Array() {
        values[0] = T(0.0); values[1] = T(0.0); values[2] = T(0.0);
        values[3] = T(0.0); values[4] = T(0.0); values[5] = T(0.0);
        values[6] = T(0.0); values[7] = T(0.0); values[8] = T(0.0);
    }

    Mat3_Array(
        T _0, T _1, T _2,
        T _3, T _4, T _5,
        T _6, T _7, T _8
    ) {
        values[0] = _0; values[1] = _1; values[2] = _2;
        values[3] = _3; values[4] = _4; values[5] = _5;
        values[6] = _6; values[7] = _7; values[8] = _8;
    }

    T values[9];

    T& operator[](size_t index) {
        assert(index < 9 && "Index out of range");
        return values[index];
    }

    const T& operator[](size_t index) const {
        assert(index < 9 && "Index out of range");
        return values[index];
    }
};

template<typename T = f32>
struct Mat3 {
    static_assert(std::is_floating_point<T>::value, "Mat3 can only be instantiated with floating point types.");

    Mat3() :
        m0(T(0.0)), m3(T(0.0)), m6(T(0.0)),
        m1(T(0.0)), m4(T(0.0)), m7(T(0.0)),
        m2(T(0.0)), m5(T(0.0)), m8(T(0.0)) {}

    Mat3(
        T _m0, T _m3, T _m6,
        T _m1, T _m4, T _m7,
        T _m2, T _m5, T _m8
    ) : m0(_m0), m3(_m3), m6(_m6),
        m1(_m1), m4(_m4), m7(_m7),
        m2(_m2), m5(_m5), m8(_m8) {}

    Mat3(const Mat3_Array<T> &arr) :
        m0(arr[0]), m3(arr[1]), m6(arr[2]),
        m1(arr[3]), m4(arr[4]), m7(arr[5]),
        m2(arr[6]), m5(arr[7]), m8(arr[8]) {}

    union {
        struct {
            T m0, m3, m6; // First row
            T m1, m4, m7; // Second row
            T m2, m5, m8; // Third row
        };
        T values[9];
    };

    T& operator[](size_t index) {
        assert(index < 9 && "Index out of range");
        return values[index];
    }

    const T& operator[](size_t index) const {
        assert(index < 9 && "Index out of range");
        return values[index];
    }

    // Base Values
    static const Mat3 identity;

    // Operators
    Mat3 operator*(const Mat3& rhs) const;

    Mat3& operator=(const Mat3& rhs) = default;
    Mat3& operator*=(const Mat3& rhs);

    // Functions
    Mat3_Array<T> as_ordered_array() const;
};

using Mat3f = Mat3<f32>;
using Mat3d = Mat3<f64>;

// -------------------------------------------
// Matrix 4x4 (Column-Major)
// -------------------------------------------

template<typename T = f32>
struct Mat4_Array {
    static_assert(std::is_floating_point<T>::value, "Mat4_Array can only be instantiated with floating point types.");

    Mat4_Array() {
        values[0 ] = T(0.0); values[1 ] = T(0.0); values[2 ] = T(0.0); values[3 ] = T(0.0);
        values[4 ] = T(0.0); values[5 ] = T(0.0); values[6 ] = T(0.0); values[7 ] = T(0.0);
        values[8 ] = T(0.0); values[9 ] = T(0.0); values[10] = T(0.0); values[11] = T(0.0);
        values[12] = T(0.0); values[13] = T(0.0); values[14] = T(0.0); values[15] = T(0.0);
    }

    Mat4_Array(
        T _0 , T _1 , T _2 , T _3 ,
        T _4 , T _5 , T _6 , T _7 ,
        T _8 , T _9 , T _10, T _11,
        T _12, T _13, T _14, T _15
    ) {
        values[0 ] = _0 ; values[1 ] = _1 ; values[2 ] = _2 ; values[3 ] = _3 ;
        values[4 ] = _4 ; values[5 ] = _5 ; values[6 ] = _6 ; values[7 ] = _7 ;
        values[8 ] = _8 ; values[9 ] = _9 ; values[10] = _10; values[11] = _11;
        values[12] = _12; values[13] = _13; values[14] = _14; values[15] = _15;
    }

    T values[16];

    T& operator[](size_t index) {
        assert(index < 16 && "Index out of range");
        return values[index];
    }

    const T& operator[](size_t index) const {
        assert(index < 16 && "Index out of range");
        return values[index];
    }
};

template<typename T = f32>
struct Mat4 {
    static_assert(std::is_floating_point<T>::value, "Mat4 can only be instantiated with floating point types.");

    Mat4() :
        m0(T(0.0)), m4(T(0.0)), m8 (T(0.0)), m12(T(0.0)),
        m1(T(0.0)), m5(T(0.0)), m9 (T(0.0)), m13(T(0.0)),
        m2(T(0.0)), m6(T(0.0)), m10(T(0.0)), m14(T(0.0)),
        m3(T(0.0)), m7(T(0.0)), m11(T(0.0)), m15(T(0.0)) {}

    Mat4(
        T _m0, T _m4, T _m8 , T _m12,
        T _m1, T _m5, T _m9 , T _m13,
        T _m2, T _m6, T _m10, T _m14,
        T _m3, T _m7, T _m11, T _m15
    ) : m0(_m0), m4(_m4), m8 (_m8 ), m12(_m12),
        m1(_m1), m5(_m5), m9 (_m9 ), m13(_m13),
        m2(_m2), m6(_m6), m10(_m10), m14(_m14),
        m3(_m3), m7(_m7), m11(_m11), m15(_m15) {}

    Mat4(const Mat4_Array<T> &arr) :
        m0(arr[0 ]), m4(arr[1 ]), m8 (arr[2 ]), m12(arr[3 ]),
        m1(arr[4 ]), m5(arr[5 ]), m9 (arr[6 ]), m13(arr[7 ]),
        m2(arr[8 ]), m6(arr[9 ]), m10(arr[10]), m14(arr[11]),
        m3(arr[12]), m7(arr[13]), m11(arr[14]), m15(arr[15]) {}

    union {
        struct {
            T m0, m4, m8 , m12; // First row
            T m1, m5, m9 , m13; // Second row
            T m2, m6, m10, m14; // Third row
            T m3, m7, m11, m15; // Fourth row
        };
        T values[16];
    };

    T& operator[](size_t index) {
        assert(index < 16 && "Index out of range");
        return values[index];
    }

    const T& operator[](size_t index) const {
        assert(index < 16 && "Index out of range");
        return values[index];
    }

    // Base Values
    static const Mat4 identity;

    // Operators
    Mat4 operator*(const Mat4& rhs) const;

    Mat4& operator=(const Mat4& rhs) = default;
    Mat4& operator*=(const Mat4& rhs);

    // Functions
    Mat4_Array<T> as_ordered_array() const;
    static Mat4 orthographic(T left, T right, T bottom, T top, T zNear, T zFar);
    static Mat4 perspective(T fovY, T aspect, T zNear, T zFar);
};

using Mat4f = Mat4<f32>;
using Mat4d = Mat4<f64>;

// -------------------------------------------
// Base Values
// -------------------------------------------

// -- Matrix 3x3
template<typename T> const Mat3<T> Mat3<T>::identity = Mat3<T>(
    T(1), T(0), T(0),
    T(0), T(1), T(0),
    T(0), T(0), T(1)
);

// -- Matrix 4x4
template<typename T> const Mat4<T> Mat4<T>::identity = Mat4<T>(
    T(1), T(0), T(0), T(0),
    T(0), T(1), T(0), T(0),
    T(0), T(0), T(1), T(0),
    T(0), T(0), T(0), T(1)
);

// -------------------------------------------
// Operators
// -------------------------------------------

// -- Matrix 3x3
template<typename T>
Mat3<T> Mat3<T>::operator*(const Mat3& rhs) const {
    return Mat3<T>(
        (m0 * rhs.m0) + (m3 * rhs.m1) + (m6 * rhs.m2), // m0
        (m0 * rhs.m3) + (m3 * rhs.m4) + (m6 * rhs.m5), // m3
        (m0 * rhs.m6) + (m3 * rhs.m7) + (m6 * rhs.m8), // m6

        (m1 * rhs.m0) + (m4 * rhs.m1) + (m7 * rhs.m2), // m1
        (m1 * rhs.m3) + (m4 * rhs.m4) + (m7 * rhs.m5), // m4
        (m1 * rhs.m6) + (m4 * rhs.m7) + (m7 * rhs.m8), // m7

        (m2 * rhs.m0) + (m5 * rhs.m1) + (m8 * rhs.m2), // m2
        (m2 * rhs.m3) + (m5 * rhs.m4) + (m8 * rhs.m5), // m5
        (m2 * rhs.m6) + (m5 * rhs.m7) + (m8 * rhs.m8)  // m8
    );
}

template<typename T>
Mat3<T>& Mat3<T>::operator*=(const Mat3& rhs) {
    Mat3<T> result;

    result.m0 = (m0 * rhs.m0) + (m3 * rhs.m1) + (m6 * rhs.m2);
    result.m3 = (m0 * rhs.m3) + (m3 * rhs.m4) + (m6 * rhs.m5);
    result.m6 = (m0 * rhs.m6) + (m3 * rhs.m7) + (m6 * rhs.m8);

    result.m1 = (m1 * rhs.m0) + (m4 * rhs.m1) + (m7 * rhs.m2);
    result.m4 = (m1 * rhs.m3) + (m4 * rhs.m4) + (m7 * rhs.m5);
    result.m7 = (m1 * rhs.m6) + (m4 * rhs.m7) + (m7 * rhs.m8);

    result.m2 = (m2 * rhs.m0) + (m5 * rhs.m1) + (m8 * rhs.m2);
    result.m5 = (m2 * rhs.m3) + (m5 * rhs.m4) + (m8 * rhs.m5);
    result.m8 = (m2 * rhs.m6) + (m5 * rhs.m7) + (m8 * rhs.m8);

    *this = result;
    return *this;
}

// -- Matrix 4x4
template<typename T>
Mat4<T> Mat4<T>::operator*(const Mat4& rhs) const {
    return Mat4<T>(
        (m0 * rhs.m0 ) + (m4 * rhs.m1 ) + (m8  * rhs.m2 ) + (m12 * rhs.m3 ), // m0
        (m0 * rhs.m4 ) + (m4 * rhs.m5 ) + (m8  * rhs.m6 ) + (m12 * rhs.m7 ), // m4
        (m0 * rhs.m8 ) + (m4 * rhs.m9 ) + (m8  * rhs.m10) + (m12 * rhs.m11), // m8
        (m0 * rhs.m12) + (m4 * rhs.m13) + (m8  * rhs.m14) + (m12 * rhs.m15), // m12

        (m1 * rhs.m0 ) + (m5 * rhs.m1 ) + (m9  * rhs.m2 ) + (m13 * rhs.m3 ), // m1
        (m1 * rhs.m4 ) + (m5 * rhs.m5 ) + (m9  * rhs.m6 ) + (m13 * rhs.m7 ), // m5
        (m1 * rhs.m8 ) + (m5 * rhs.m9 ) + (m9  * rhs.m10) + (m13 * rhs.m11), // m9
        (m1 * rhs.m12) + (m5 * rhs.m13) + (m9  * rhs.m14) + (m13 * rhs.m15), // m13

        (m2 * rhs.m0 ) + (m6 * rhs.m1 ) + (m10 * rhs.m2 ) + (m14 * rhs.m3 ), // m2
        (m2 * rhs.m4 ) + (m6 * rhs.m5 ) + (m10 * rhs.m6 ) + (m14 * rhs.m7 ), // m6
        (m2 * rhs.m8 ) + (m6 * rhs.m9 ) + (m10 * rhs.m10) + (m14 * rhs.m11), // m10
        (m2 * rhs.m12) + (m6 * rhs.m13) + (m10 * rhs.m14) + (m14 * rhs.m15), // m14

        (m3 * rhs.m0 ) + (m7 * rhs.m1 ) + (m11 * rhs.m2 ) + (m15 * rhs.m3 ), // m3
        (m3 * rhs.m4 ) + (m7 * rhs.m5 ) + (m11 * rhs.m6 ) + (m15 * rhs.m7 ), // m7
        (m3 * rhs.m8 ) + (m7 * rhs.m9 ) + (m11 * rhs.m10) + (m15 * rhs.m11), // m11
        (m3 * rhs.m12) + (m7 * rhs.m13) + (m11 * rhs.m14) + (m15 * rhs.m15)  // m15
    );
}

template<typename T>
Mat4<T>& Mat4<T>::operator*=(const Mat4& rhs) {
    Mat4<T> result;

    result.m0  = (m0 * rhs.m0 ) + (m4 * rhs.m1 ) + (m8  * rhs.m2 ) + (m12 * rhs.m3 );
    result.m4  = (m0 * rhs.m4 ) + (m4 * rhs.m5 ) + (m8  * rhs.m6 ) + (m12 * rhs.m7 );
    result.m8  = (m0 * rhs.m8 ) + (m4 * rhs.m9 ) + (m8  * rhs.m10) + (m12 * rhs.m11);
    result.m12 = (m0 * rhs.m12) + (m4 * rhs.m13) + (m8  * rhs.m14) + (m12 * rhs.m15);

    result.m1  = (m1 * rhs.m0 ) + (m5 * rhs.m1 ) + (m9  * rhs.m2 ) + (m13 * rhs.m3 );
    result.m5  = (m1 * rhs.m4 ) + (m5 * rhs.m5 ) + (m9  * rhs.m6 ) + (m13 * rhs.m7 );
    result.m9  = (m1 * rhs.m8 ) + (m5 * rhs.m9 ) + (m9  * rhs.m10) + (m13 * rhs.m11);
    result.m13 = (m1 * rhs.m12) + (m5 * rhs.m13) + (m9  * rhs.m14) + (m13 * rhs.m15);

    result.m2  = (m2 * rhs.m0 ) + (m6 * rhs.m1 ) + (m10 * rhs.m2 ) + (m14 * rhs.m3 );
    result.m6  = (m2 * rhs.m4 ) + (m6 * rhs.m5 ) + (m10 * rhs.m6 ) + (m14 * rhs.m7 );
    result.m10 = (m2 * rhs.m8 ) + (m6 * rhs.m9 ) + (m10 * rhs.m10) + (m14 * rhs.m11);
    result.m14 = (m2 * rhs.m12) + (m6 * rhs.m13) + (m10 * rhs.m14) + (m14 * rhs.m15);

    result.m3  = (m3 * rhs.m0 ) + (m7 * rhs.m1 ) + (m11 * rhs.m2 ) + (m15 * rhs.m3 );
    result.m7  = (m3 * rhs.m4 ) + (m7 * rhs.m5 ) + (m11 * rhs.m6 ) + (m15 * rhs.m7 );
    result.m11 = (m3 * rhs.m8 ) + (m7 * rhs.m9 ) + (m11 * rhs.m10) + (m15 * rhs.m11);
    result.m15 = (m3 * rhs.m12) + (m7 * rhs.m13) + (m11 * rhs.m14) + (m15 * rhs.m15);

    *this = result;
    return *this;
}

// -------------------------------------------
// Ordered Arrays
// -------------------------------------------

// Matrix 3x3
template<typename T>
Mat3_Array<T> Mat3<T>::as_ordered_array() const {
    return Mat3_Array<T>(
        m0, m1, m2,
        m3, m4, m5,
        m6, m7, m8
    );
}

// Matrix 4x4
template<typename T>
Mat4_Array<T> Mat4<T>::as_ordered_array() const {
    return Mat4_Array<T>(
        m0 , m1 , m2 , m3 ,
        m4 , m5 , m6 , m7 ,
        m8 , m9 , m10, m11,
        m12, m13, m14, m15
    );
}

// -------------------------------------------
// Projection
// -------------------------------------------

// Matrix 4x4
template<typename T>
Mat4<T> Mat4<T>::orthographic(T left, T right, T bottom, T top, T zNear, T zFar) {
    T width = right - left;
    T height = top - bottom;
    T farDist = zFar - zNear;

    Mat4<T> result = Mat4<T>();
    result.m0  = 2.0 / width;
    result.m5  = 2.0 / height;
    result.m10 = 2.0 / farDist;
    result.m12 = -(left + right) / width;
    result.m13 = -(top + bottom) / height;
    result.m14 = -(zFar + zNear) / farDist;
    result.m15 = 1.0;

    return result;
}

template<typename T>
Mat4<T> Mat4<T>::perspective(T fovY, T aspect, T zNear, T zFar) {
    T farDist = zFar - zNear;
	T rads = fovY * 0.5;
    T cotangent = mathf::cos(rads) / mathf::sin(rads);

    Mat4<T> result = Mat4<T>();
    result.m0  = cotangent / aspect;
	result.m5  = cotangent;
	result.m10 = (zFar + zNear) / farDist;
	result.m11 = 1.0;
	result.m14 = -(zFar * zNear * 2.0) / farDist;

    return result;
}

