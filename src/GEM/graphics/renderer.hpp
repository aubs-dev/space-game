#pragma once

#include "GEM/assets_generated.hpp"
#include "GEM/math/geometry.hpp"
#include "pch.hpp"

#include "GEM/assets.hpp"
#include "GEM/math/matrix.hpp"
#include "GEM/math/vector.hpp"
#include "GEM/graphics/color.hpp"
#include "GEM/graphics/drawing.hpp"

namespace renderer {
    // -------------------------------------------
    // Constants
    // -------------------------------------------

    // -- Batch
    // TODO: Set more realistic limits
    // TODO: Add automatic batch resizing
    const u32 BATCH_MAX_POINTS    = 65536;
    const u32 BATCH_MAX_LINES     = 32768; // NOTE: Needs to be divisible by 2
    const u32 BATCH_MAX_TRIANGLES = 16383; // NOTE: Needs to be divisible by 3
    const u32 BATCH_MAX_QUADS     = 8192;
    const u32 BATCH_MAX_TILES     = 8192;
    const u32 BATCH_MAX_PARTICLES = 16384;

    // -- Camera
    const f32 CAMERA_NEAR = 0.01f;
    const f32 CAMERA_FAR  = 1000.0f;

    // -------------------------------------------
    // Shader
    // -------------------------------------------

    struct Shader {
        const char* name;
        u32 vertId;
        u32 fragId;
        u32 programid;
    };

    struct Buffer {
        u32 id;
    };

    struct BufferStorage {
        u32 id;
        void* data;
        u32 elementCount;
        u32 elementLimit;
    };

    // -------------------------------------------
    // Camera
    // -------------------------------------------

    enum class CameraMode {
        ORTHOGRAPHIC = 0,
        PERSPECTIVE,
    };

    struct Camera {
        Mat4f viewMat;
        Mat4f projMat;
    };

    // -------------------------------------------
    // Instance
    // -------------------------------------------
    // NOTE: Instances -> Element -> GL Primitive

    enum class GLPrimitive {
        POINT = 0,
        LINE,
        TRIANGLE,
        TRIANGLE_STRIP,
    };

    enum class ElementType {
        NONE = 0,
        POINT,
        LINE,
        TRIANGLE,
        QUAD,
        TILE,
        PARTICLE,
    };

    struct ElementPoint {
        Vec2f positionA;    //  2 * 4b =  8b |- [vec4]
        u32 layer;          //  1 * 4b =  4b |
        Color tint;         //  4 * 1b =  4b |
    };

    struct ElementLine {
        Vec2f positionA;    //  2 * 4b =  8b |- [vec4]
        Vec2f positionB;    //  2 * 4b =  8b |
        i32 pad[2];         //  2 * 4b =  8b |- [vec4]
        u32 layer;          //  1 * 4b =  4b |
        Color tint;         //  4 * 1b =  4b |
    };

    struct ElementTriangle {
        Vec2f positionA;    //  2 * 4b =  8b |- [vec4]
        Vec2f positionB;    //  2 * 4b =  8b |
        Vec2f positionC;    //  2 * 4b =  8b |- [vec4]
        u32 layer;          //  1 * 4b =  4b |
        Color tint;         //  4 * 1b =  4b |
    };

    struct ElementQuad {
        Mat4f modelMat;     // 16 * 4b = 64b |- [4x vec4]
        Vec2f origin;       //  2 * 4b =  8b |- [vec4]
        Color tint;         //  4 * 1b =  4b |
        u32 flags;          //  1 * 4b =  4b |
        Vec2f texCoords[4]; //  8 * 4b = 32b |- [2x vec4]
    };

    struct ElementTile {
        Mat4f modelMat;     // 16 * 4b = 64b |- [4x vec4]
        u32 tileIdx;        //  1 * 4b =  4b |- [vec4]
        u32 tilemapIdx;     //  1 * 4b =  4b |
        Color tint;         //  4 * 1b =  4b |
        i32 pad[1];         //  1 * 4b =  4b |
    };

    struct ElementParticle {
        Mat4f modelMat;     // 16 * 4b = 64b |- [4x vec4]
        u32 particleIdx;    //  1 * 4b =  4b |- [vec4]
        u32 columnCount;    //  1 * 4b =  4b |
        Color tint;         //  4 * 1b =  4b |
        i32 pad[1];         //  1 * 4b =  4b |
    };

    enum class InstanceType {
        // Immediate
        SHAPE_OUTLINE = 0,
        SHAPE_SOLID,
        TEXTURED_RECT,
        // Retained
        SPRITE,
        BILLBOARD,
        TEXT,
        TILE,
        EFFECT,
        COUNT,
        // ---
        IMMEDIATE_FIRST = SHAPE_OUTLINE,
        IMMEDIATE_LAST  = TEXTURED_RECT,
    };

    // -------------------------------------------
    // Entity
    // -------------------------------------------

    struct Material {
        union {
            asset::SpriteName sprite;
            u32 codepoint;
            u32 tile;
            asset::ParticleName particle;
        } texture;

        Color color;
    };

    // NOTE: The game should implement an entity system that calls upon 'renderable_object_*' functions
    struct RenderableData {
        InstanceType instanceType;
        Material material;

        bool hasChanged;
    };

    // -------------------------------------------
    // Batch
    // -------------------------------------------

    struct Batch {
        ElementType elementType;
        u32 elementSize;

        CameraMode cameraMode;
        asset::AtlasName atlasName;

        u32 shaderId;
        u32 vaoId;

        Buffer baseGeometryBuffer;
        BufferStorage instanceBuffer;

        bool isImmediate;
    };

    // -------------------------------------------
    // Renderer
    // -------------------------------------------

    enum class Flags {
        VSYNC_HINT = 1 << 0, // Enable V-Sync on the GPU
        WIREFRAME  = 1 << 1, // Draw everything in wireframe mode
    };

    struct Renderer {
        struct {
            Shader shape;
            Shader quad;
            Shader tile;
            Shader particle;
        } shader;

        struct {
            u32 quadGeometry;
        } sharedBuffers;

        struct {
            u32 atlas[as_index(asset::AtlasName::ASSET_ATLAS_COUNT)];
        } texture;

        struct {
            Camera orthographic;
            Camera perspective;
        } camera;

        Batch batches[as_index(InstanceType::COUNT)];

        struct {
            // Viewport
            Color clearColor;
            Color tint;

            // Batch
            struct {
                drawing::Mode mode;
                drawing::Layer layer;
            } immediate;

            struct {
                asset::FontName name;
            } font;
        } draw;

        u32 flags;
    };

    extern Renderer g_State;

    // -------------------------------------------
    // Functions
    // -------------------------------------------

    // -- Base
    GAPI void renderer_setup(void);
    GAPI void renderer_shutdown(void);

    // -- Shader
    GAPI u32 shader_compile(const char* shaderSrc, i32 shaderType);
    GAPI u32 shader_link_program(u32 vertShaderId, u32 fragShaderId);

    GAPI Shader shader_create(const char* name, const char* vertShaderSrc, const char* fragShaderSrc);
    GAPI Shader shader_create_from_asset(asset::ShaderName name);
    GAPI void shader_delete(Shader* instance);

    // -- [ Immediate ]
    // -- Batch
    // GAPI void batch_commit(u32 batchId);
    // GAPI void batch_set_type(InstanceType type);
    // GAPI void batch_set_atlas(asset::AtlasName name);

    // GAPI void batch_add_vertex(Vec2f position);
    // GAPI void batch_add_quad(geometry::Transform transform, geometry::Rectangle rect, u32 flags);

    // -- [ Retained ]
    // -- Renderable Object
    // u32  renderable_object_create(const RenderableData& data, const geometry::Transform& transform);
    // void renderable_object_update(const RenderableData& data, const geometry::Transform& transform, u32 id);
    // void renderable_object_destroy(const RenderableData& data, u32 id);

    // -- Texture
    GAPI u32 texture_load_atlases(const char* path);

    // -- Flags
    GAPI u32 get_flags();
    GAPI void set_flag(Flags flag, bool enabled);
    GAPI bool check_flag(Flags flag);
}
