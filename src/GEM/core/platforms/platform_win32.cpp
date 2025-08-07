#include "pch.hpp"

#include "GEM/core/platform.hpp"

#include "GEM/logger.hpp"
#include "GEM/core/memory.hpp"

// TODO: Window functions don't care if windowCtx is NULL???

// -------------------------------------------
// Helper
// -------------------------------------------

// -- Display
static BOOL CALLBACK display_enum_proc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    platform::DisplayEnumContext* ctx = (platform::DisplayEnumContext*)dwData;
    if (!ctx || ctx->count >= GEM_MAX_DISPLAYS) return FALSE;

    MONITORINFOEXA info = {};
    info.cbSize = sizeof(info);

    if (GetMonitorInfoA(hMonitor, &info)) {
        platform::Display* display = &ctx->instances[ctx->count];

        strncpy(display->name, info.szDevice, GEM_MAX_STRING_LENGTH - 1);
        display->name[GEM_MAX_STRING_LENGTH - 1] = '\0';

        display->resolution.w = info.rcMonitor.right - info.rcMonitor.left;
        display->resolution.h = info.rcMonitor.bottom - info.rcMonitor.top;

        display->origin.x = info.rcMonitor.left;
        display->origin.y = info.rcMonitor.top;

        display->workableArea.w = info.rcWork.right - info.rcWork.left;
        display->workableArea.h = info.rcWork.bottom - info.rcWork.top;

        display->originWorkable.x = info.rcWork.left;
        display->originWorkable.y = info.rcWork.top;

        if (FLAG_GET(info.dwFlags, MONITORINFOF_PRIMARY)) ctx->primaryIdx = ctx->count;

        ctx->count++;
    }

    return TRUE;
}

// -- Window
static u32 MouseButtonMaskTable[BUTTON_COUNT] = {
    0x0001, // MOUSE_BUTTON_LEFT    -> MK_LBUTTON
    0x0010, // MOUSE_BUTTON_MIDDLE  -> MK_MBUTTON
    0x0002, // MOUSE_BUTTON_RIGHT   -> MK_RBUTTON
};

static LRESULT CALLBACK wnd_proc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        // -- Window
        case WM_MOVE:
            {
                platform::g_State.windowCtx.position.current.x = (i16)LOWORD(lParam);
                platform::g_State.windowCtx.position.current.y = (i16)HIWORD(lParam);
            }
            break;
        case WM_SIZE:
            {
                // TODO: Rescale the renderer's viewport (letterboxing?)
                platform::g_State.windowCtx.size.current.w = (i16)LOWORD(lParam);
                platform::g_State.windowCtx.size.current.h = (i16)HIWORD(lParam);
                platform::g_State.windowCtx.hasSizeChanged = true;
            }
            break;
        case WM_CLOSE:
        case WM_QUIT:
            {
                platform::g_State.windowCtx.hasQuit = true;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        // -- Input: Keyboard
        case WM_CHAR:
            platform::g_State.hid.keyboard.lastChar = (char)wParam;
            platform::g_State.hid.keyboard.lastKey = (Keycode)wParam;
            break;
        case WM_KEYDOWN:
            {
                Keycode key = (Keycode)wParam;
                if (platform::g_State.hid.keyboard.keysDown[as_index(key)]) {
                    platform::g_State.hid.keyboard.keysHeld[as_index(key)] = true;
                } else {
                    platform::g_State.hid.keyboard.keysPressed[as_index(key)] = true;
                }

                platform::g_State.hid.keyboard.keysDown[as_index(key)] = true;
                platform::g_State.hid.keyboard.isKeyAny = true;
            }
            break;
        case WM_KEYUP:
            {
                Keycode key = (Keycode)wParam;
                platform::g_State.hid.keyboard.keysDown[as_index(key)] = false;
                platform::g_State.hid.keyboard.keysReleased[as_index(key)] = true;
            }
            break;

        // -- Window: Focus
        case WM_KILLFOCUS:
            platform::g_State.windowCtx.isFocused = false;
            break;
        case WM_SETFOCUS:
            platform::g_State.windowCtx.isFocused = true;
            break;

        // -- Input: Mouse
        case WM_LBUTTONDOWN:
            // Click + drag
            platform::g_State.hid.mouse.positionDragStart = Vec2f(GET_X_LPARAM(lParam), platform::g_State.windowCtx.size.current.h - GET_Y_LPARAM(lParam) - 1.0f);
            platform::g_State.hid.mouse.isDragging = true;
            // Fall-through to handle button down logic
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            {
                for (u32 i = 0; i < as_index(BUTTON_COUNT); i++) {
                    bool buttonActive = wParam & MouseButtonMaskTable[i];

                    if (buttonActive) {
                        if (!platform::g_State.hid.mouse.buttonsDown[i]) {
                            platform::g_State.hid.mouse.buttonsPressed[i] = true;
                        }

                        platform::g_State.hid.mouse.buttonsDown[i] = true;
                    }
                }
            }
            break;
        case WM_LBUTTONUP:
            // Click + drag
            platform::g_State.hid.mouse.positionDragEnd = Vec2f(GET_X_LPARAM(lParam), platform::g_State.windowCtx.size.current.h - GET_Y_LPARAM(lParam) - 1.0f);
            platform::g_State.hid.mouse.isDragging = false;
            // Button released
            platform::g_State.hid.mouse.buttonsDown[as_index(BUTTON_LEFT)] = false;
            platform::g_State.hid.mouse.buttonsReleased[as_index(BUTTON_LEFT)] = true;
            break;
        case WM_MBUTTONUP:
            platform::g_State.hid.mouse.buttonsDown[as_index(BUTTON_MIDDLE)] = false;
            platform::g_State.hid.mouse.buttonsReleased[as_index(BUTTON_MIDDLE)] = true;
            break;
        case WM_RBUTTONUP:
            platform::g_State.hid.mouse.buttonsDown[as_index(BUTTON_RIGHT)] = false;
            platform::g_State.hid.mouse.buttonsReleased[as_index(BUTTON_RIGHT)] = true;
            break;
#ifdef GEM_HANDLE_DOUBLE_CLICK
        case WM_LBUTTONDBLCLK:
            platform::g_State.hid.mouse.buttonsDoubleClicked[BUTTON_LEFT] = true;
            break;
        case WM_MBUTTONDBLCLK:
            platform::g_State.hid.mouse.buttonsDoubleClicked[BUTTON_MIDDLE] = true;
            break;
        case WM_RBUTTONDBLCLK:
            platform::g_State.hid.mouse.buttonsDoubleClicked[as_index(BUTTON_RIGHT)] = true;
            break;
#endif
        case WM_MOUSEWHEEL:
            platform::g_State.hid.mouse.mouseWheelDelta.y = (f32)GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA;
            break;
        case WM_MOUSEHWHEEL:
            platform::g_State.hid.mouse.mouseWheelDelta.x = (f32)GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA;
            break;
        case WM_MOUSEMOVE:
            {
                f32 mouseX = (f32)GET_X_LPARAM(lParam);
                f32 mouseY = platform::g_State.windowCtx.size.current.h - (f32)GET_Y_LPARAM(lParam) - 1.0f;

                // TODO: Scale the mouse position using the viewport
                // f32 virtualX = mouseX * (platform::g_State.windowCtx.renderSize.w / (f32)platform::g_State.windowCtx.size.w);
                f32 virtualX = mouseX;
                // f32 virtualY = mouseY * (platform::g_State.windowCtx.renderSize.h / (f32)platform::g_State.windowCtx.size.h);
                f32 virtualY = mouseY;

                platform::g_State.hid.mouse.position = { virtualX, virtualY };
            }
            break;
    }

    return DefWindowProcA(handle, message, wParam, lParam);
}

static Vec2i window_get_adjusted_size(Vec2i size) {
    u32 baseStyle     = (u32)GetWindowLongA(platform::g_State.windowCtx.handle, GWL_STYLE);
    u32 extendedStyle = (u32)GetWindowLongA(platform::g_State.windowCtx.handle, GWL_EXSTYLE);
    RECT rect = { 0, 0, size.w, size.h };
    AdjustWindowRectEx(&rect, baseStyle, false, extendedStyle);
    return { rect.right - rect.left, rect.bottom - rect.top };
}

// -------------------------------------------
// Window
// -------------------------------------------

namespace window {
    // -------------------------------------------
    // Lifetime
    // -------------------------------------------

    static bool dummy_get_wgl_functions() {
        i32 success = 0;

        // Create a dummy window and get it's device context
        const HWND dummyHandle = CreateWindowExA(
            0,
            "STATIC",
            "Dummy Window",
            WS_OVERLAPPED,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            NULL,
            NULL,
            NULL,
            NULL
        );

        if (!dummyHandle) {
            log_error("Platform: Failed to create the dummy window!");
            return false;
        }

        const HDC dummyDeviceCtx = GetDC(dummyHandle);
        if (!dummyDeviceCtx) {
            DestroyWindow(dummyHandle);

            log_error("Platform: Failed to get device context for dummy window!");
            return false;
        }

        // Set a temporary pixel format
        PIXELFORMATDESCRIPTOR pixelFormatDesc = {};
        pixelFormatDesc.nSize      = sizeof(pixelFormatDesc);
        pixelFormatDesc.nVersion   = 1;
        pixelFormatDesc.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
        pixelFormatDesc.cColorBits = 24;

        i32 formatIdx = ChoosePixelFormat(dummyDeviceCtx, &pixelFormatDesc);
        if (!formatIdx) {
            ReleaseDC(dummyHandle, dummyDeviceCtx);
            DestroyWindow(dummyHandle);

            log_error("Platform: Failed to choose a suitable pixel format for dummy window!");
            return false;
        }

        success = DescribePixelFormat(dummyDeviceCtx, formatIdx, sizeof(pixelFormatDesc), &pixelFormatDesc);
        if (!success) {
            ReleaseDC(dummyHandle, dummyDeviceCtx);
            DestroyWindow(dummyHandle);

            log_error("Platform: Failed to describe OpenGL pixel format for dummy window!");
            return false;
        }

        // NOTE: The reason to create a dummy window is that SetPixelFormat can be called only once per window
        if (!SetPixelFormat(dummyDeviceCtx, formatIdx, &pixelFormatDesc)) {
            ReleaseDC(dummyHandle, dummyDeviceCtx);
            DestroyWindow(dummyHandle);

            log_error("Platform: Cannot set OpenGL pixel format for dummy window!");
            return false;
        }

        const HGLRC dummyGLCtx = wglCreateContext(dummyDeviceCtx);
        if (!dummyGLCtx) {
            ReleaseDC(dummyHandle, dummyDeviceCtx);
            DestroyWindow(dummyHandle);

            log_error("Platform: Failed to create an OpenGL context for the dummy window!");
            return false;
        }

        success = wglMakeCurrent(dummyDeviceCtx, dummyGLCtx);
        if (!success) {
            wglDeleteContext(dummyGLCtx);
            ReleaseDC(dummyHandle, dummyDeviceCtx);
            DestroyWindow(dummyHandle);

            log_error("Platform: Failed to use OpenGL for the dummy window!");
            return false;
        }

        // Load all WGL extensions
        if (!gladLoaderLoadWGL(dummyDeviceCtx)) {
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(dummyGLCtx);
            ReleaseDC(dummyHandle, dummyDeviceCtx);
            DestroyWindow(dummyHandle);

            log_error("Platform: Failed to load WGL extensions through GLAD!");
            return false;
        }

        // Cleanup
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(dummyGLCtx);
        ReleaseDC(dummyHandle, dummyDeviceCtx);
        DestroyWindow(dummyHandle);

        return true;
    };

    GAPI bool create(const char* title, u32 width, u32 height, u32 flags) {
        assert(!platform::g_State.windowCtx.handle && "Window already exists!");

        platform::g_State.windowCtx.title = string_is_valid(title) ? title : "No title provided!";
        platform::g_State.windowCtx.size.original = { (i32)width, (i32)height };
        platform::g_State.windowCtx.size.preFullscreen = platform::g_State.windowCtx.size.original;
        platform::g_State.windowCtx.flags = flags;

        // Index all available displays/monitors
        EnumDisplayMonitors(NULL, NULL, display_enum_proc, (LPARAM)(&platform::g_State.display.ctx));
        const platform::Display* primaryDisplay = platform::display_get_primary();

        // Create a dummy window and pull all WGL functions
        if (!dummy_get_wgl_functions()) return false;

        // Create the window
        const HINSTANCE instance = GetModuleHandleA(NULL);

        WNDCLASSA winClass = {};
#ifdef GEM_HANDLE_DOUBLE_CLICK
        winClass.style = CS_DBLCLKS;
#endif
        winClass.lpfnWndProc = wnd_proc;
        winClass.hInstance = instance;
        winClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
        winClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
        winClass.lpszClassName = "GEM_WINDOW_CLASS";

        if (!RegisterClassA(&winClass)) {
            log_error("Platform: Failed to register the window class!");
            return false;
        }

        // -- Set the base style
        u32 baseStyle = WS_VISIBLE;
        if (check_flag(Flags::FULLSCREEN)) {
            FLAG_ADD(baseStyle, WS_POPUP);
        } else {
            FLAG_ADD(baseStyle, WS_OVERLAPPED);
            FLAG_ADD(baseStyle, WS_CAPTION);
            FLAG_ADD(baseStyle, WS_SYSMENU);
            FLAG_ADD(baseStyle, WS_MINIMIZEBOX);
        }

        if (check_flag(Flags::MAXIMIZED)) {
            set_flag(Flags::RESIZABLE, true); // You need to 'resize' to window to make it maximized
            FLAG_ADD(baseStyle, WS_MAXIMIZE);
        }

        if (check_flag(Flags::RESIZABLE)) {
            if (check_flag(Flags::FULLSCREEN)) {
                log_warn("Platform: Resizable mode isn't supported when 'fullscreen' is enabled!");
            } else {
                FLAG_ADD(baseStyle, WS_THICKFRAME);
                FLAG_ADD(baseStyle, WS_MAXIMIZEBOX);
            }
        }

        if (check_flag(Flags::MINIMIZED)) FLAG_ADD(baseStyle, WS_MINIMIZE);

        platform::g_State.windowCtx.baseStyle = baseStyle;

        // -- Set the extended style
        u32 extendedStyle = 0;
        if (check_flag(Flags::FULLSCREEN) || check_flag(Flags::ALWAYS_ON_TOP)) FLAG_ADD(extendedStyle, WS_EX_TOPMOST);

        // -- Set the window size & position, taking its decorations into account
        RECT rect = { 0, 0, (i32)width, (i32)height };
        AdjustWindowRectEx(&rect, baseStyle, false, extendedStyle);

        Vec2u adjustedSize { (u32)(rect.right - rect.left), (u32)(rect.bottom - rect.top) };
        if (check_flag(Flags::FULLSCREEN)) {
            adjustedSize = primaryDisplay->resolution;
        }

        Vec2i adjustedPosition = primaryDisplay->origin;
        if (check_flag(Flags::POS_CENTERED)) {
            adjustedPosition.x = primaryDisplay->originWorkable.x + primaryDisplay->workableArea.w / 2 - adjustedSize.w / 2;
            adjustedPosition.y = primaryDisplay->originWorkable.y + primaryDisplay->workableArea.h / 2 - adjustedSize.h / 2;
        }

        platform::g_State.windowCtx.position.original = adjustedPosition;
        platform::g_State.windowCtx.position.preFullscreen = platform::g_State.windowCtx.position.original;

        // -- Actually create the window
        platform::g_State.windowCtx.handle = CreateWindowExA(
            extendedStyle,
            winClass.lpszClassName,
            title,
            baseStyle,
            adjustedPosition.x,
            adjustedPosition.y,
            adjustedSize.w,
            adjustedSize.h,
            NULL, // window parent
            NULL, // menu
            instance,
            NULL
        );

        if (!platform::g_State.windowCtx.handle) {
            log_error("Platform: Failed to create the window!");
            return false;
        }

        // Configure and initialize the OpenGL context
        platform::g_State.windowCtx.deviceCtx = GetDC(platform::g_State.windowCtx.handle);
        if (!platform::g_State.windowCtx.deviceCtx) {
            DestroyWindow(platform::g_State.windowCtx.handle);
            platform::g_State.windowCtx.handle = NULL;

            log_error("Platform: Failed to get device context for window!");
            return false;
        }

        // -- Specify the desired pixel format
        // source: https://registry.khronos.org/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
        i32 formatAttributes[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,     24,
            WGL_DEPTH_BITS_ARB,     24,
            WGL_STENCIL_BITS_ARB,   8,
            0, // End
        };

        // -- Set the suggested pixel format
        i32 suggestedFormatIdx = 0;
        u32 formatCount = 0;
        if (!wglChoosePixelFormatARB(platform::g_State.windowCtx.deviceCtx, formatAttributes, NULL, 1, &suggestedFormatIdx, &formatCount) || formatCount == 0) {
            ReleaseDC(platform::g_State.windowCtx.handle, platform::g_State.windowCtx.deviceCtx);
            platform::g_State.windowCtx.deviceCtx = NULL;

            DestroyWindow(platform::g_State.windowCtx.handle);
            platform::g_State.windowCtx.handle = NULL;

            log_error("Platform: Failed to choose a suitable pixel format for window!");
            return false;
        }

        PIXELFORMATDESCRIPTOR suggestedFormatDesc = {};
        suggestedFormatDesc.nSize = sizeof(suggestedFormatDesc);
        if (!DescribePixelFormat(platform::g_State.windowCtx.deviceCtx, suggestedFormatIdx, sizeof(suggestedFormatDesc), &suggestedFormatDesc)) {
            ReleaseDC(platform::g_State.windowCtx.handle, platform::g_State.windowCtx.deviceCtx);
            platform::g_State.windowCtx.deviceCtx = NULL;

            DestroyWindow(platform::g_State.windowCtx.handle);
            platform::g_State.windowCtx.handle = NULL;

            log_error("Platform: Failed to describe OpenGL pixel format for window!");
            return false;
        }

        if (!SetPixelFormat(platform::g_State.windowCtx.deviceCtx, suggestedFormatIdx, &suggestedFormatDesc)) {
            ReleaseDC(platform::g_State.windowCtx.handle, platform::g_State.windowCtx.deviceCtx);
            platform::g_State.windowCtx.deviceCtx = NULL;

            DestroyWindow(platform::g_State.windowCtx.handle);
            platform::g_State.windowCtx.handle = NULL;

            log_error("Platform: Cannot set OpenGL pixel format for window!");
            return false;
        }

        // -- Create a modern context
        // source: https://registry.khronos.org/OpenGL/extensions/ARB/WGL_ARB_create_context.txt
        i32 contextAttributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 5,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef GEM_DEBUG
            // Ask for a debug context and enable debug callbacks
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
            0, // End
        };

        platform::g_State.windowCtx.glCtx = wglCreateContextAttribsARB(platform::g_State.windowCtx.deviceCtx, NULL, contextAttributes);
        if (!platform::g_State.windowCtx.glCtx) {
            ReleaseDC(platform::g_State.windowCtx.handle, platform::g_State.windowCtx.deviceCtx);
            platform::g_State.windowCtx.deviceCtx = NULL;

            DestroyWindow(platform::g_State.windowCtx.handle);
            platform::g_State.windowCtx.handle = NULL;

            log_error("Platform: Cannot create a modern OpenGL context for window! OpenGL version 4.5 not supported?");
            return false;
        }

        if (!wglMakeCurrent(platform::g_State.windowCtx.deviceCtx, platform::g_State.windowCtx.glCtx)) {
            wglDeleteContext(platform::g_State.windowCtx.glCtx);
            platform::g_State.windowCtx.glCtx = NULL;

            ReleaseDC(platform::g_State.windowCtx.handle, platform::g_State.windowCtx.deviceCtx);
            platform::g_State.windowCtx.deviceCtx = NULL;

            DestroyWindow(platform::g_State.windowCtx.handle);
            platform::g_State.windowCtx.handle = NULL;

            log_error("Platform: Failed to use OpenGL for window!");
            return false;
        }

        // -- Enable VSync
        if (check_flag(window::Flags::VSYNC_HINT)) {
            wglSwapIntervalEXT(1);
        }

        // -- Load all GL functions
        if (!gladLoaderLoadGL()) {
            wglMakeCurrent(NULL, NULL);

            wglDeleteContext(platform::g_State.windowCtx.glCtx);
            platform::g_State.windowCtx.glCtx = NULL;

            ReleaseDC(platform::g_State.windowCtx.handle, platform::g_State.windowCtx.deviceCtx);
            platform::g_State.windowCtx.deviceCtx = NULL;

            DestroyWindow(platform::g_State.windowCtx.handle);
            platform::g_State.windowCtx.handle = NULL;

            log_error("Platform: Failed to load GL functions through GLAD!");
            return false;
        }

        // Show and update the main window
        ShowWindow(platform::g_State.windowCtx.handle, SW_SHOWNORMAL);
        UpdateWindow(platform::g_State.windowCtx.handle);

        log_info("Platform: Window created | \"%s\" @ %ix%i%s", title, width, height, check_flag(Flags::FULLSCREEN) ? " Fullscreen" : "");

        return true;
    }

    GAPI bool should_close() {
        // Reset window state
        platform::g_State.windowCtx.hasSizeChanged = false;

        // Reset keyboard state
        u64 keyboardStateSize = sizeof(bool) * KEY_COUNT;
        memory::zero(&platform::g_State.hid.keyboard.keysHeld, keyboardStateSize);
        memory::zero(&platform::g_State.hid.keyboard.keysPressed, keyboardStateSize);
        memory::zero(&platform::g_State.hid.keyboard.keysReleased, keyboardStateSize);

        platform::g_State.hid.keyboard.isKeyAny = false;

        // Reset mouse state
        u64 mouseStateSize = sizeof(bool) * BUTTON_COUNT;
        memory::zero(&platform::g_State.hid.mouse.buttonsDoubleClicked, mouseStateSize);
        memory::zero(&platform::g_State.hid.mouse.buttonsPressed, mouseStateSize);
        memory::zero(&platform::g_State.hid.mouse.buttonsReleased, mouseStateSize);

        // Update cursor state
        if (platform::g_State.hid.cursor.isLocked) {
            platform::cursor_lock();
        }

        // Push & process messages through "wnd_proc"
        MSG msg;
        if (PeekMessageA(&msg, platform::g_State.windowCtx.handle, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        return platform::g_State.windowCtx.hasQuit;
    }

    GAPI void close() {
        platform::g_State.windowCtx.hasQuit = true;
    }

    GAPI void shutdown() {
        wglMakeCurrent(NULL, NULL);

        wglDeleteContext(platform::g_State.windowCtx.glCtx);
        platform::g_State.windowCtx.glCtx = NULL;

        ReleaseDC(platform::g_State.windowCtx.handle, platform::g_State.windowCtx.deviceCtx);
        platform::g_State.windowCtx.deviceCtx = NULL;

        DestroyWindow(platform::g_State.windowCtx.handle);
        platform::g_State.windowCtx.handle = NULL;
        log_info("Platform: Window destroyed | \"%s\"", platform::g_State.windowCtx.title);
    }

    // -------------------------------------------
    // State
    // -------------------------------------------

    GAPI void swap_buffers() {
        SwapBuffers(platform::g_State.windowCtx.deviceCtx);
    }

    GAPI const char* get_title() {
        return platform::g_State.windowCtx.title;
    }

    GAPI void set_title(const char* title) {
	    SetWindowTextA(platform::g_State.windowCtx.handle, title);
    }

    GAPI Vec2i get_position() {
        return platform::g_State.windowCtx.position.current;
    }

    GAPI Vec2i get_position_original() {
        return platform::g_State.windowCtx.position.original;
    }

    GAPI void set_position(Vec2i position) {
        SetWindowPos(platform::g_State.windowCtx.handle, 0, position.x, position.y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    }

    GAPI void set_position_center() {
        const Window* win = &platform::g_State.windowCtx;

        Vec2i adjustedWindowSize = window_get_adjusted_size({ (i32)win->size.current.w, (i32)win->size.current.h});
        Vec2i windowCenter = win->position.current + (adjustedWindowSize / 2);
        const platform::Display* closestDisplay = platform::display_get_from_position(windowCenter);

        Vec2i newCenter = {};
        newCenter.x = closestDisplay->originWorkable.x + closestDisplay->workableArea.w / 2 - adjustedWindowSize.w / 2;
        newCenter.y = closestDisplay->originWorkable.y + closestDisplay->workableArea.h / 2 - adjustedWindowSize.h / 2;

        set_position(newCenter);
    }

    GAPI Vec2u get_size() {
        return Vec2u(platform::g_State.windowCtx.size.current.w, platform::g_State.windowCtx.size.current.h);
    }

    GAPI Vec2u get_size_original() {
        return Vec2u(platform::g_State.windowCtx.size.original.w, platform::g_State.windowCtx.size.original.h);
    }

    GAPI void set_size(Vec2u size) {
        SetWindowPos(platform::g_State.windowCtx.handle, 0, 0, 0, size.w, size.h, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
    }

    GAPI void set_size_adjusted(Vec2u size) {
        Vec2i adjustedSize = window_get_adjusted_size({ (i32)size.w, (i32)size.h });
        SetWindowPos(platform::g_State.windowCtx.handle, 0, 0, 0, adjustedSize.w, adjustedSize.h, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
    }

    GAPI bool has_size_changed() {
        return platform::g_State.windowCtx.hasSizeChanged;
    }

    GAPI void minimize() {
        ShowWindow(platform::g_State.windowCtx.handle, SW_MINIMIZE);
    }

    GAPI bool is_minimized() {
        WINDOWPLACEMENT placement = {};
        GetWindowPlacement(platform::g_State.windowCtx.handle, &placement);
	    return placement.showCmd == SW_MINIMIZE;
    }

    GAPI void maximize() {
        ShowWindow(platform::g_State.windowCtx.handle, SW_SHOWMAXIMIZED);
    }

    GAPI bool is_maximized() {
        WINDOWPLACEMENT placement = {};
        GetWindowPlacement(platform::g_State.windowCtx.handle, &placement);
	    return placement.showCmd == SW_SHOWMAXIMIZED;
    }

    GAPI void restore() {
        ShowWindow(platform::g_State.windowCtx.handle, SW_RESTORE);
    }

    GAPI void focus() {
        SetForegroundWindow(platform::g_State.windowCtx.handle);
        SetFocus(platform::g_State.windowCtx.handle);
    }

    GAPI bool is_focused() {
        return platform::g_State.windowCtx.isFocused;
    }

    GAPI void set_floating(bool isFloating) {
        if (isFloating) {
            SetWindowPos(platform::g_State.windowCtx.handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        } else {
            SetWindowPos(platform::g_State.windowCtx.handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
    }

    GAPI bool is_floating() {
        return FLAG_GET(GetWindowLongPtrA(platform::g_State.windowCtx.handle, GWL_EXSTYLE), WS_EX_TOPMOST);
    }

    GAPI void toggle_fullscreen() {
        u32 updatedStyle = platform::g_State.windowCtx.baseStyle;

        if (is_fullscreen()) {
            // Remove style: fullscreen
            FLAG_REMOVE(updatedStyle, WS_POPUP);
            // Add style: windowed
            FLAG_ADD(updatedStyle, WS_OVERLAPPED);
            FLAG_ADD(updatedStyle, WS_CAPTION);
            FLAG_ADD(updatedStyle, WS_SYSMENU);
            FLAG_ADD(updatedStyle, WS_MINIMIZEBOX);

            // Set style: windowed
            SetWindowLongA(platform::g_State.windowCtx.handle, GWL_STYLE, updatedStyle);
            platform::g_State.windowCtx.baseStyle = updatedStyle;

            // Set original shape
            set_position(platform::g_State.windowCtx.position.preFullscreen);
            set_size({
                (u32)platform::g_State.windowCtx.size.preFullscreen.w,
                (u32)platform::g_State.windowCtx.size.preFullscreen.h,
            });

            if (!check_flag(Flags::ALWAYS_ON_TOP)) set_floating(false);
        } else {
            // Remove style: windowed
            FLAG_REMOVE(updatedStyle, WS_OVERLAPPED);
            FLAG_REMOVE(updatedStyle, WS_CAPTION);
            FLAG_REMOVE(updatedStyle, WS_SYSMENU);
            FLAG_REMOVE(updatedStyle, WS_MINIMIZEBOX);
            // Add style: fullscreen
            FLAG_ADD(updatedStyle, WS_POPUP);

            // Set style: fullscreen
            SetWindowLongA(platform::g_State.windowCtx.handle, GWL_STYLE, updatedStyle);
            platform::g_State.windowCtx.baseStyle = updatedStyle;

            // Set new shape
            const platform::Display* display = platform::display_get_from_window();

            platform::g_State.windowCtx.position.preFullscreen = platform::g_State.windowCtx.position.current;
            set_position(display->origin);

            platform::g_State.windowCtx.size.preFullscreen = platform::g_State.windowCtx.size.current;
            set_size(display->resolution);

            if (!check_flag(Flags::ALWAYS_ON_TOP)) set_floating(true);
        }
    }

    GAPI bool is_fullscreen() {
        u32 baseStyle = (u32)GetWindowLongA(platform::g_State.windowCtx.handle, GWL_STYLE);
        bool hasPopUpStyle = FLAG_GET(baseStyle, WS_POPUP);

        const Window* win = &platform::g_State.windowCtx;
        const platform::Display* display = platform::display_get_from_window();
        bool hasDisplaySize = (win->size.current.w == (i32)display->resolution.w) && (win->size.current.h == (i32)display->resolution.h);

        return hasPopUpStyle && hasDisplaySize;
    }

    // -------------------------------------------
    // Flags
    // -------------------------------------------

    GAPI u32 get_flags() {
        return platform::g_State.windowCtx.flags;
    }

    GAPI void set_flag(Flags flag, bool enabled) {
        FLAG_SET(platform::g_State.windowCtx.flags, flag, enabled);
    }

    GAPI bool check_flag(Flags flag) {
        return FLAG_GET(platform::g_State.windowCtx.flags, flag);
    }
}

// -------------------------------------------
// Platform
// -------------------------------------------

namespace platform {
    Platform g_State = {};

    // -------------------------------------------
    // Shared Libraries
    // -------------------------------------------

    GAPI void* dynamic_library_load(const char* filePath) {
        HMODULE result = LoadLibraryA(filePath);
        assert(result && "Failed to load DLL!");
        return result;
    }

    GAPI bool dynamic_library_free(void* handle) {
        bool success = FreeLibrary((HMODULE)handle);
        assert(success && "Failed to free DLL!");
        return success;
    }

    GAPI void* dynamic_library_get_function(const void* handle, const char* name) {
        FARPROC proc = GetProcAddress((HMODULE)handle, name);
        assert(proc && "Failed to find function in DLL!");
        return (void*)proc;
    }

    // -------------------------------------------
    // Display
    // -------------------------------------------

    GAPI u32 display_get_count() {
        return g_State.display.ctx.count;
    }

    GAPI const Display* display_get_primary() {
        return &g_State.display.ctx.instances[g_State.display.ctx.primaryIdx];
    }

    GAPI const Display* display_get_from_position(Vec2i position) {
        Display* bestDisplay = NULL;
        f32 bestDist = F32_MAX;

        for (u32 i = 0; i < g_State.display.ctx.count; i++) {
            Display* display = &g_State.display.ctx.instances[i];

            // Get display bounding box
            i32 left   = display->origin.x;
            i32 top    = display->origin.y;
            i32 right  = left + display->resolution.w;
            i32 bottom = top + display->resolution.h;

            // Check if the point is inside the display (exact match)
            if (position.x >= left && position.x < right && position.y >= top && position.y < bottom) {
                return display;
            }

            // Find the closest center of a display to the position (estimate)
            Vec2i center = { (left + right) / 2, (top + bottom) / 2 };
            f32 squareDist = Vec2i::distance_squared(position, center);

            if (squareDist < bestDist) {
                bestDisplay = display;
                bestDist = squareDist;
            }
        }

        return bestDisplay;
    }

    GAPI const Display* display_get_from_window() {
        const window::Window* win = &g_State.windowCtx;

        Vec2i adjustedWindowSize = window_get_adjusted_size({ (i32)win->size.current.w, (i32)win->size.current.h });
        Vec2i windowCenter = win->position.current + (adjustedWindowSize / 2);
        const Display* closestDisplay = display_get_from_position(windowCenter);

        return closestDisplay;
    }

    GAPI const char* display_get_name(u32 index) {
        if (index >= GEM_MAX_DISPLAYS) return "";
        return g_State.display.ctx.instances[index].name;
    }

    GAPI Vec2u display_get_resolution(u32 index) {
        if (index >= GEM_MAX_DISPLAYS) return Vec2u::zero;
        return g_State.display.ctx.instances[index].resolution;
    }

    GAPI Vec2i display_get_origin(u32 index) {
        if (index >= GEM_MAX_DISPLAYS) return Vec2i::zero;
        return g_State.display.ctx.instances[index].origin;
    }

    GAPI Vec2u display_get_workable_area(u32 index) {
        if (index >= GEM_MAX_DISPLAYS) return Vec2u::zero;
        return g_State.display.ctx.instances[index].workableArea;
    }

    GAPI Vec2i display_get_origin_workable(u32 index) {
        if (index >= GEM_MAX_DISPLAYS) return Vec2i::zero;
        return g_State.display.ctx.instances[index].originWorkable;
    }

    // -------------------------------------------
    // Window
    // -------------------------------------------

    GAPI const window::Window* window_get_instance() {
        return &g_State.windowCtx;
    }

    // -------------------------------------------
    // Input
    // -------------------------------------------

    GAPI bool keyboard_poll(Keycode key) {
        // If the high-order bit is 1, the key is down; otherwise, it is up
        if (GetKeyState((i32)key) & 0x8000) {
            return g_State.hid.keyboard.keysDown[as_index(key)];
        } else {
            return false;
        }
    }

    GAPI Vec2i cursor_get_position() {
        POINT point = {};
        GetCursorPos(&point);
        return { point.x, point.y };
    }

    GAPI void cursor_set_hidden(bool hidden) {
        if (g_State.hid.cursor.isHidden == hidden) return;
        g_State.hid.cursor.isHidden = hidden;

        i32 count = 0;
        if (hidden) {
            while ((count = ShowCursor(FALSE)) >= 0); // decrement the display counter
        } else {
            while ((count = ShowCursor(TRUE)) < 0);   // increment the display counter
        }
    }

    GAPI void cursor_lock() {
        i32 x = g_State.windowCtx.position.current.x + (i32)(g_State.windowCtx.size.current.w / 2.0f);
        i32 y = g_State.windowCtx.position.current.y + (i32)(g_State.windowCtx.size.current.h / 2.0f);
        SetCursorPos(x, y);
    }

    // -------------------------------------------
    // Time
    // -------------------------------------------

    GAPI void sleep(u32 ms) {
        Sleep(ms);
    }

    GAPI void clock_start() {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        g_State.clock.frequency = 1.0 / (f64)frequency.QuadPart;

        LARGE_INTEGER tick;
        QueryPerformanceCounter(&tick);
        g_State.clock.startTick = tick.QuadPart;
    }

    GAPI f64 clock_get_time() {
        if (g_State.clock.frequency == 0) {
            clock_start();
        }

        LARGE_INTEGER tick;
        QueryPerformanceCounter(&tick);
        return (f64)(tick.QuadPart - g_State.clock.startTick) * g_State.clock.frequency;
    }

    // -------------------------------------------
    // Clipboard
    // -------------------------------------------

    GAPI const char* clipboard_get_text() {
        log_not_implemented("clipboard_get_text");
        return "";
    }

    GAPI void clipboard_set_text(const char* text) {
        log_not_implemented("clipboard_set_text");
    }

    // -------------------------------------------
    // Message Box
    // -------------------------------------------

    GAPI void message_box_error(const char* msg, const char* title) {
        MessageBoxA(NULL, msg, title, MB_ICONERROR | MB_OK);
    }
}
