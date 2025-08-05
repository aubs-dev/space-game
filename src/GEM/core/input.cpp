#include "pch.hpp"

#include "GEM/core/input.hpp"

#include "GEM/core/platform.hpp"
#include "GEM/math/vector.hpp"

namespace input {
    // -------------------------------------------
    // Keyboard
    // -------------------------------------------

    GAPI Keycode get_last_key() {
        return platform::g_State.hid.keyboard.lastKey;
    }

    GAPI char get_last_char() {
        return platform::g_State.hid.keyboard.lastChar;
    }

    GAPI bool key_any() {
        return platform::g_State.hid.keyboard.isKeyAny;
    }

    GAPI bool key_poll(Keycode key) {
        return platform::keyboard_poll(key);
    }

    GAPI bool key_pressed(Keycode key) {
        return platform::g_State.hid.keyboard.keysPressed[key];
    }

    GAPI bool key_held(Keycode key) {
        return platform::g_State.hid.keyboard.keysHeld[key];
    }

    GAPI bool key_released(Keycode key) {
        return platform::g_State.hid.keyboard.keysReleased[key];
    }

    GAPI bool key_consume_pressed(Keycode key) {
        bool state = platform::g_State.hid.keyboard.keysPressed[key];
        if (state) platform::g_State.hid.keyboard.keysPressed[key] = false;
        return state;
    }

    GAPI bool key_consume_held(Keycode key) {
        bool state = platform::g_State.hid.keyboard.keysHeld[key];
        if (state) platform::g_State.hid.keyboard.keysHeld[key] = false;
        return state;
    }

    GAPI bool key_consume_released(Keycode key) {
        bool state = platform::g_State.hid.keyboard.keysReleased[key];
        if (state) platform::g_State.hid.keyboard.keysReleased[key] = false;
        return state;
    }

    // -------------------------------------------
    // Mouse
    // -------------------------------------------

    GAPI Vec2f mouse_get_position_global() {
        Vec2i position = platform::cursor_get_position();
        return { (f32)position.x, (f32)position.y };
    }

    GAPI Vec2f mouse_get_position() {
        return platform::g_State.hid.mouse.position;
    }

    GAPI f32 mouse_get_x() {
        return platform::g_State.hid.mouse.position.x;
    }

    GAPI f32 mouse_get_y() {
        return platform::g_State.hid.mouse.position.y;
    }

    GAPI bool mouse_is_dragging() {
        return platform::g_State.hid.mouse.isDragging;
    }

    GAPI Vec2f mouse_get_drag_start() {
        return platform::g_State.hid.mouse.positionDragStart;
    }

    GAPI Vec2f mouse_get_drag_end() {
        return platform::g_State.hid.mouse.positionDragEnd;
    }

    GAPI bool mouse_button_held(MouseButton button) {
        return platform::g_State.hid.mouse.buttonsDown[button];
    }

    GAPI bool mouse_button_pressed(MouseButton button) {
        return platform::g_State.hid.mouse.buttonsPressed[button];
    }

    GAPI bool mouse_button_released(MouseButton button) {
        return platform::g_State.hid.mouse.buttonsReleased[button];
    }

#ifdef GEM_HANDLE_DOUBLE_CLICK
    GAPI bool mouse_button_double_click(MouseButton button) {
        return platform::g_State.hid.mouse.buttonsDoubleClicked[button];
    }
#endif

    GAPI Vec2f mouse_wheel_get_delta() {
        return platform::g_State.hid.mouse.mouseWheelDelta;
    }

    // -------------------------------------------
    // Cursor
    // -------------------------------------------

    GAPI void cursor_show() {
        platform::cursor_set_hidden(false);
    }

    GAPI void cursor_hide() {
        platform::cursor_set_hidden(true);
    }

    GAPI bool cursor_is_hidden() {
        return platform::g_State.hid.cursor.isHidden;
    }

    GAPI void cursor_lock() {
        platform::g_State.hid.cursor.isLocked = true;
    }

    GAPI void cursor_unlock() {
        platform::g_State.hid.cursor.isLocked = false;
    }

    GAPI bool cursor_is_locked() {
        return platform::g_State.hid.cursor.isLocked;
    }
}
