#pragma once

#include "pch.hpp"

#include "GEM/math/vector.hpp"

// TODO: Look into pooling input events until the next fixed update (make input work with both update events)
namespace input {
    // -------------------------------------------
    // Functions
    // -------------------------------------------

    // -- Keyboard
    GAPI Keycode get_last_key();
    GAPI char    get_last_char();

    GAPI bool key_any();
    GAPI bool key_poll(Keycode key);

    GAPI bool key_pressed(Keycode key);
    GAPI bool key_held(Keycode key);
    GAPI bool key_released(Keycode key);

    GAPI bool key_consume_pressed(Keycode key);
    GAPI bool key_consume_held(Keycode key);
    GAPI bool key_consume_released(Keycode key);

    // -- Mouse
    GAPI Vec2f mouse_get_position_global();
    GAPI Vec2f mouse_get_position();
    GAPI f32   mouse_get_x();
    GAPI f32   mouse_get_y();

    GAPI bool  mouse_is_dragging();
    GAPI Vec2f mouse_get_drag_start();
    GAPI Vec2f mouse_get_drag_end();

    GAPI bool mouse_button_held(MouseButton button);
    GAPI bool mouse_button_pressed(MouseButton button);
    GAPI bool mouse_button_released(MouseButton button);
#ifdef GEM_HANDLE_DOUBLE_CLICK
    GAPI bool mouse_button_double_click(MouseButton button);
#endif

    GAPI Vec2f mouse_wheel_get_delta();

    // -- Cursor
    GAPI void cursor_show();
    GAPI void cursor_hide();
    GAPI bool cursor_is_hidden();

    GAPI void cursor_lock();
    GAPI void cursor_unlock();
    GAPI bool cursor_is_locked();
}
