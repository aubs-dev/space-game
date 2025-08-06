#pragma once

#include "pch.hpp"

#include "GEM/math/vector.hpp"

// -------------------------------------------
// Constants
// -------------------------------------------

#ifndef GEM_TARGET_FRAMERATE
#define GEM_TARGET_FRAMERATE 60
#endif

#ifndef GEM_TARGET_ASPECT_RATIO
#define GEM_TARGET_ASPECT_RATIO (16 / 9.0f)
#endif

// Most GPUs & Drivers seem to support between 4-6 monitors, not conclusive so...
// IMO, no one need more than 2-3 monitors anyway ¯\_(ツ)_/¯
#ifndef GEM_MAX_DISPLAYS
#define GEM_MAX_DISPLAYS 4
#endif

namespace window {
    // -------------------------------------------
    // Data Types
    // -------------------------------------------

    enum class Flags {
        FULLSCREEN    = 1 << 0, // Enter fullscreen on startup
        RESIZABLE     = 1 << 1, // Allow window resizing
        MAXIMIZED     = 1 << 2, // Maximize the window on startup
        MINIMIZED     = 1 << 3, // Minimize the window on startup
        POS_CENTERED  = 1 << 4, // Center the window to the current display on startup
        ALWAYS_ON_TOP = 1 << 5, // Keep the window always on top
        VSYNC_HINT    = 1 << 6, // Enable V-Sync on the GPU
    };

    struct Window {
        const char* title;

        HWND handle;
        u32 baseStyle;

        HDC deviceCtx;
        HGLRC glCtx;

        struct {
            Vec2i original;
            Vec2i current;
            Vec2i preFullscreen;
        } position;
        struct {
            Vec2i original;
            Vec2i current;
            Vec2i preFullscreen;
        } size;

        u32 flags;
        bool isFocused;
        bool hasSizeChanged;
        bool hasQuit;
    };

    // -------------------------------------------
    // Functions
    // -------------------------------------------

    // -- Lifetime
    GAPI bool create(const char* title, u32 width, u32 height, u32 flags);
    GAPI bool should_close();
    GAPI void close();
    GAPI void shutdown();

    // -- State
    GAPI void swap_buffers();

    GAPI const char* get_title();
    GAPI void set_title(const char* title);
    GAPI Vec2i get_position();
    GAPI Vec2i get_position_original();
    GAPI void set_position(Vec2i position);
    GAPI void set_position_center();
    GAPI Vec2u get_size();
    GAPI Vec2u get_size_original();
    GAPI void set_size(Vec2u size);
    GAPI void set_size_adjusted(Vec2u size);
    GAPI bool has_size_changed();

    GAPI void minimize();
    GAPI bool is_minimized();
    GAPI void maximize();
    GAPI bool is_maximized();
    GAPI void restore();

    GAPI void focus();
    GAPI bool is_focused();
    GAPI void set_floating(bool isFloating);
    GAPI bool is_floating();
    GAPI void toggle_fullscreen();
    GAPI bool is_fullscreen();

    // -- Flags
    GAPI u32 get_flags();
    GAPI void set_flag(Flags flag, bool enabled);
    GAPI bool check_flag(Flags flag);
}

namespace platform {
    // -------------------------------------------
    // Data Types
    // -------------------------------------------

    struct Display {
        char name[GEM_MAX_STRING_LENGTH];
        Vec2u resolution;
        Vec2i origin;
        Vec2u workableArea;
        Vec2i originWorkable;
    };

    struct DisplayEnumContext {
        Display instances[GEM_MAX_DISPLAYS];
        u32 count;
        u32 primaryIdx;
    };

    struct Platform {
        struct {
            DisplayEnumContext ctx;
        } display;

        // TODO: Support having multiple windows
        window::Window windowCtx;

        struct {
            struct {
                bool keysDown[as_index(KEY_COUNT)];

                bool keysHeld[as_index(KEY_COUNT)];
                bool keysPressed[as_index(KEY_COUNT)];
                bool keysReleased[as_index(KEY_COUNT)];

                char lastChar;
                Keycode lastKey;

                bool isKeyAny;
            } keyboard;

            struct {
                bool buttonsDown[as_index(BUTTON_COUNT)];

                bool buttonsDoubleClicked[as_index(BUTTON_COUNT)];
                bool buttonsPressed[as_index(BUTTON_COUNT)];
                bool buttonsReleased[as_index(BUTTON_COUNT)];

                Vec2f position;
                Vec2f positionDragStart;
                Vec2f positionDragEnd;

                Vec2f mouseWheelDelta;

                bool isDragging;
            } mouse;

            struct {
                bool isHidden;
                bool isLocked;
            } cursor;
        } hid;

        struct {
            i64 startTick;
            f64 movingSecond;

            f64 targetFrameTime;
            f64 frequency;
        } clock;

        struct {
            i32 frames;
            i32 framesPerSecond;
            f64 frameTime;

            i32 updates;
            i32 updatesPerSecond;
        } metrics;
    };

    extern Platform g_State;

    // -------------------------------------------
    // Functions
    // -------------------------------------------

    // -- Shared Libraries
    GAPI void* dynamic_library_load(const char* filePath);
    GAPI bool  dynamic_library_free(void* handle);
    GAPI void* dynamic_library_get_function(const void* handle, const char* name);

    // -- Display
    GAPI u32            display_get_count();
    GAPI const Display* display_get_primary();
    GAPI const Display* display_get_from_position(Vec2i position);
    GAPI const Display* display_get_from_window();
    GAPI const char*    display_get_name(u32 index);
    GAPI Vec2u          display_get_resolution(u32 index);
    GAPI Vec2i          display_get_origin(u32 index);
    GAPI Vec2u          display_get_workable_area(u32 index);
    GAPI Vec2i          display_get_origin_workable(u32 index);

    // -- Window
    GAPI const window::Window* window_get_instance();

    // -- Input
    GAPI bool  keyboard_poll(Keycode key);

    GAPI Vec2i cursor_get_position();
    GAPI void  cursor_set_hidden(bool hidden);
    GAPI void  cursor_lock();

    // -- Time
    GAPI void sleep(u32 ms);
    GAPI void clock_start();
    GAPI f64  clock_get_time();

    // -- Clipboard
    // TODO: Implement clipboard detection
    GAPI const char* clipboard_get_text();
    GAPI void        clipboard_set_text(const char* text);

    // -- Message Box
    GAPI void message_box_error(const char* msg, const char* title);
}
