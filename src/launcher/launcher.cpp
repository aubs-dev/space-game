// -------------------------------------------
// Includes
// -------------------------------------------

#include "pch.hpp"

#include "GEM/logger.hpp"
#include "GEM/core/input.hpp"
#include "GEM/core/platform.hpp"
#include "GEM/graphics/renderer.hpp"

// -------------------------------------------
// Launcher Entry
// -------------------------------------------

i32 main(i32 argc, char* argv[]) {
    // Allocate common error title
    char errorTitle[GEM_MAX_STRING_LENGTH];
    strcpy(errorTitle, strrchr(argv[0], '\\') + 1);
    strcat(errorTitle, " - Error");

    // Create the window
    u32 windowFlags = 0;
    // FLAG_ADD(windowFlags, window::Flags::FULLSCREEN);
    FLAG_ADD(windowFlags, window::Flags::RESIZABLE);
    // FLAG_ADD(windowFlags, window::Flags::MAXIMIZED);
    // FLAG_ADD(windowFlags, window::Flags::MINIMIZED);
    // FLAG_ADD(windowFlags, window::Flags::POS_CENTERED);
    // FLAG_ADD(windowFlags, window::Flags::ALWAYS_ON_TOP);
    FLAG_ADD(windowFlags, window::Flags::VSYNC_HINT);

    // TODO: Need error checking for if the window was actually created
    if (!window::create("3D Game Test", 1280, 720, windowFlags)) {
        platform::message_box_error("Failed to create window!", errorTitle);
        return -1;
    }

    // Initialize the renderer
    bool success = true;

    if (!success) window::close();

    while (!window::should_close()) {
        // Render - Base
        Vec2u size = window::get_size();
        glViewport(0, 0, size.w, size.h);
        glClearColor(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        window::swap_buffers();

        // User
        if (input::key_pressed(KEY_ESCAPE)) window::close();
        if (input::key_pressed(KEY_F)) window::toggle_fullscreen();
    }

    // Cleanup
    window::shutdown();

    return 0;
}
