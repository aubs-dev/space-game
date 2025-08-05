#include "pch.hpp"

#include "GEM/graphics/color.hpp"

// -------------------------------------------
// Base Values
// -------------------------------------------

const Color Color::black   = Color(0  , 0  , 0  );
const Color Color::red     = Color(255, 0  , 0  );
const Color Color::green   = Color(0  , 255, 0  );
const Color Color::blue    = Color(0  , 0  , 255);
const Color Color::yellow  = Color(255, 255, 0  );
const Color Color::magenta = Color(255, 0  , 255);
const Color Color::cyan    = Color(0  , 255, 255);
const Color Color::white   = Color(255, 255, 255);

const Color Color::cornflowerBlue = Color(100, 149, 237);

// -------------------------------------------
// Conversion
// -------------------------------------------

Color Color::from_hex(u32 hex) {
    u8 r = (hex >> 24) & 0xFF;
    u8 g = (hex >> 16) & 0xFF;
    u8 b = (hex >>  8) & 0xFF;
    u8 a = (hex >>  0) & 0xFF;
    return Color(r, g, b, a);
}
