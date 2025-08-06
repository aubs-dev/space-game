#pragma once

#include "pch.hpp"

#include "GEM/assets.hpp"
#include "GEM/math/vector.hpp"
#include "GEM/graphics/drawing.hpp"
#include "GEM/graphics/renderer.hpp"

namespace game {
    struct Entity {
        // [ Engine ]
        u32 renderableId;
        renderer::RenderableData renderData;

        geometry::Transform transform;

        // [ Game ]
        // ...

        bool isValid;
    };
}
