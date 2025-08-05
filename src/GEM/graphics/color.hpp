#pragma once

#include "pch.hpp"

#include "GEM/math/vector.hpp"

// -------------------------------------------
// Color
// -------------------------------------------

struct Color {
    Color() : r(0), g(0), b(0), a(255) {}
    Color(u8 _r, u8 _g, u8 _b) : r(_r), g(_g), b(_b), a(255) {}
    Color(u8 _r, u8 _g, u8 _b, u8 _a) : r(_r), g(_g), b(_b), a(_a) {}
    Color(const Color &c0) : r(c0.r), g(c0.g), b(c0.b), a(c0.a) {}

    union {
        struct {
            u8 r, g, b, a;
        };
        u8 values[4];
    };

    u8& operator[](size_t index) {
        assert(index < 4 && "Index out of range");
        return values[index];
    }

    const u8& operator[](size_t index) const {
        assert(index < 4 && "Index out of range");
        return values[index];
    }

    // Base Values
    static const Color black;
    static const Color red;
    static const Color green;
    static const Color blue;
    static const Color yellow;
    static const Color magenta;
    static const Color cyan;
    static const Color white;

    static const Color cornflowerBlue; // The best color <3

    // Operators
    Color& operator=(const Color& c0) = default;

    // Functions
    inline u32 as_hex() const;
    static Color from_hex(u32 hex);
};

// -------------------------------------------
// Operators
// -------------------------------------------
// ...

// -------------------------------------------
// Functions
// -------------------------------------------

// -- Conversion
inline u32 Color::as_hex() const {
    return (u32)r << 24 | (u32)g << 16 | (u32)b << 8 | (u32)a << 0;
}
