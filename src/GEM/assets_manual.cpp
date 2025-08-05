#include "pch.hpp"

#include "GEM/assets_manual.hpp"

namespace asset {
    // -------------------------------------------
    // Shader
    // -------------------------------------------

    Shader gShaderBank[ASSET_SHADER_COUNT] = {
        // {
        //     .name = "Default Line",
        //     .vertSrcPath = "resources/shader/default/line.vert",
        //     .fragSrcPath = "resources/shader/default/shape.frag"
        // }
    };

    // -------------------------------------------
    // Animation
    // -------------------------------------------

    Animation gAnimationBank[ASSET_ANIMATION_COUNT] = {
        // {
        //     .duration = 300, .frameCount = 2,
        //     .frames = { TEX_TEST_0, TEX_TEST_1 }
        // },
    };
}
