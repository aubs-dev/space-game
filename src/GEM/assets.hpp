#pragma once

#include "pch.hpp"

#include "GEM/assets_generated.hpp"

namespace asset {
    // -------------------------------------------
    // Shader
    // -------------------------------------------

    struct Shader {
        const char* name;
        const char* vertSrcPath;
        const char* fragSrcPath;
    };

    enum ShaderName {
        // SHD_DEFAULT_LINE = 0,
        ASSET_SHADER_COUNT,
    };

    extern Shader gShaderBank[ASSET_SHADER_COUNT];

    // -------------------------------------------
    // Animation
    // -------------------------------------------

    const u32 ANIMATION_MAX_FRAME_COUNT = 16;

    struct Animation {
        u32 duration; // in milliseconds
        u32 frameCount;
        SpriteName frames[ANIMATION_MAX_FRAME_COUNT];

        u32 index;
        u64 nextTime;
    };

    enum AnimationName {
        ANI_NONE = -1,
        // ANI_TEST = 0,
        ASSET_ANIMATION_COUNT,
    };

    extern Animation gAnimationBank[ASSET_ANIMATION_COUNT];
}
