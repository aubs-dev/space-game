#include "pch.hpp"

#include "GEM/assets_manual.hpp"

namespace asset {
    // -------------------------------------------
    // Shader
    // -------------------------------------------

    Shader gShaderBank[ASSET_SHADER_COUNT] = {
        {
            .name = "Default Line",
            .vertSrcPath = "resources/shader/default/line.vert",
            .fragSrcPath = "resources/shader/default/shape.frag"
        },
        {
            .name = "Default Triangle",
            .vertSrcPath = "resources/shader/default/triangle.vert",
            .fragSrcPath = "resources/shader/default/shape.frag"
        },
        {
            .name = "Default Sprite",
            .vertSrcPath = "resources/shader/default/sprite.vert",
            .fragSrcPath = "resources/shader/default/quad.frag"
        },
        {
            .name = "Default Billboard",
            .vertSrcPath = "resources/shader/default/billboard.vert",
            .fragSrcPath = "resources/shader/default/quad.frag"
        },
        {
            .name = "Default Tile",
            .vertSrcPath = "resources/shader/default/tile.vert",
            .fragSrcPath = "resources/shader/default/tile.frag"
        },
        {
            .name = "Default Glyph",
            .vertSrcPath = "resources/shader/default/glyph.vert",
            .fragSrcPath = "resources/shader/default/glyph.frag"
        },
        {
            .name = "Default Particle",
            .vertSrcPath = "resources/shader/default/particle.vert",
            .fragSrcPath = "resources/shader/default/particle.frag"
        }
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
