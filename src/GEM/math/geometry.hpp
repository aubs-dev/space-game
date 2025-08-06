#pragma once

#include "pch.hpp"

#include "GEM/math/vector.hpp"

namespace geometry {
    // -------------------------------------------
    // Component
    // -------------------------------------------

    struct Transform {
        Vec3f position;
        f32   rotation;
        Vec2f scale;
        Vec2f origin;
    };

    // -------------------------------------------
    // Shapes
    // -------------------------------------------
    // TODO: Add more shapes

    struct Rectangle {
        i32 x;
        i32 y;
        i32 width;
        i32 height;
    };
}
