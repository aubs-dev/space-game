#pragma once

#include "pch.hpp"

#include "GEM/math/vector.hpp"
#include "GEM/graphics/color.hpp"

namespace drawing {
    // -------------------------------------------
    // Constants
    // -------------------------------------------

    // -- Shape
    const u32 POLYGON_MAX_SIDES = 36;

    // -------------------------------------------
    // Data Types
    // -------------------------------------------

    enum class Mode {
        LINE = 0,
        FILL,
    };

    enum class Layer : u32 {
        DEBUG = 0,
        UI,
        FOREGROUND,
        MIDDLEGROUND,
        BACKGROUND,
        VIEWPORT = U32_MAX,
    };

    // -------------------------------------------
    // Functions
    // -------------------------------------------
    // NOTE: Don't forget about default paramaters

    // -- Base
    // GAPI void clear(void);
    //
    // GAPI void set_mode(Mode mode);
    // GAPI void set_layer(Layer layer);
    //
    // GAPI void set_color(Color col);
    // GAPI void set_color_rgb(u8 r, u8 g, u8 b);

    // -- Shape
    // GAPI void line(Vec2f startPos, Vec2f endPos);
    // GAPI void line_ex(Vec2f startPos, Vec2f endPos, f32 thickness);
    //
    // GAPI void triangle(Vec2f position, f32 base, f32 height);
    // GAPI void triangle_ex(Vec2f position, Vec2f origin, f32 base, f32 height, f32 rotation);
    // GAPI void triangle_vert(Vec2f v0, Vec2f v1, Vec2f v2);
    //
    // GAPI void rectangle(Vec2f position, f32 width, f32 height);
    // GAPI void rectangle_ex(Vec2f position, Vec2f origin, f32 width, f32 height, f32 rotation);
    //
    // GAPI void polygon(Vec2f center, f32 apothem, u32 sides, f32 rotation);
    // GAPI void arc(Vec2f center, f32 radius, f32 angleStart, f32 angleEnd, u32 segments, f32 rotation);
    // GAPI void circle(Vec2f center, f32 radius);

    // -- Quad
    // GAPI Vec2f sprite_get_size(enum sprite_name_e name);
    // GAPI void sprite(enum sprite_name_e spriteName, transform_t transform, u32 flags);
    // GAPI void billboard(enum sprite_name_e spriteName, transform_t transform, u32 flags);

    // -- Animation
    // GAPI u32 animation_get_duration(enum animation_name_e name);
    // GAPI u32 animation_get_frame_count(enum animation_name_e name);
    // GAPI u32 animation_get_index(enum animation_name_e name);
    //
    // GAPI void animation_set_duration(enum animation_name_e name, u32 duration);
    //
    // GAPI void sprite_animated(enum animation_name_e animationName, transform_t transform, u32 frameOffset, u32 flags);
    // GAPI void billboard_animated(enum animation_name_e animationName, transform_t transform, u32 frameOffset, u32 flags);

    // -- Text
    // GAPI void text_set_font(font_name_t name);
    // GAPI void text_set_line_height(f32 scale);
    //
    // GAPI Vec2f text_measure(font_name_t name, const char* text, f32 scale);
    // GAPI void text(const char* text, Vec2f position, f32 scale);
}
