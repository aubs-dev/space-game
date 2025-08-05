#pragma once

// Virtual Key Code reference: https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
enum Keycode {
    KEY_BACKSPACE        = 0x08, // Backspace
    KEY_TAB              = 0x09, // Tab

    KEY_ENTER            = 0x0D, // Enter
    KEY_SHIFT            = 0x10, // Any Shift
    KEY_CONTROL          = 0x11, // Control/ctrl
    KEY_ALT              = 0x12, // Alt
    KEY_PAUSE            = 0x13, // Pause
    KEY_CAPS_LOCK        = 0x14, // Caps Lock
    KEY_ESCAPE           = 0x1B, // Escape
    KEY_SPACE            = 0x20, // Spacebar
    KEY_PAGE_UP          = 0x21, // Page Up
    KEY_PAGE_DOWN        = 0x22, // Page Down
    KEY_END              = 0x23, // End
    KEY_HOME             = 0x24, // Home
    KEY_LEFT             = 0x25, // Left Arrow
    KEY_UP               = 0x26, // Up Arrow
    KEY_RIGHT            = 0x27, // Right Arrow
    KEY_DOWN             = 0x28, // Down Arrow
    KEY_PRINT_SCREEN     = 0x2C, // Print Screen
    KEY_INSERT           = 0x2D, // Insert
    KEY_DELETE           = 0x2E, // Delete

    KEY_D0               = 0x30, // 0 digit
    KEY_D1               = 0x31, // 1 digit
    KEY_D2               = 0x32, // 2 digit
    KEY_D3               = 0x33, // 3 digit
    KEY_D4               = 0x34, // 4 digit
    KEY_D5               = 0x35, // 5 digit
    KEY_D6               = 0x36, // 6 digit
    KEY_D7               = 0x37, // 7 digit
    KEY_D8               = 0x38, // 8 digit
    KEY_D9               = 0x39, // 9 digit

    KEY_A                = 0x41, // 'A' or 'a'
    KEY_B                = 0x42, // 'B' or 'b'
    KEY_C                = 0x43, // 'C' or 'c'
    KEY_D                = 0x44, // 'D' or 'd'
    KEY_E                = 0x45, // 'E' or 'e'
    KEY_F                = 0x46, // 'F' or 'f'
    KEY_G                = 0x47, // 'G' or 'g'
    KEY_H                = 0x48, // 'H' or 'h'
    KEY_I                = 0x49, // 'I' or 'i'
    KEY_J                = 0x4A, // 'J' or 'j'
    KEY_K                = 0x4B, // 'K' or 'k'
    KEY_L                = 0x4C, // 'L' or 'l'
    KEY_M                = 0x4D, // 'M' or 'm'
    KEY_N                = 0x4E, // 'N' or 'n'
    KEY_O                = 0x4F, // 'O' or 'o'
    KEY_P                = 0x50, // 'P' or 'p'
    KEY_Q                = 0x51, // 'Q' or 'q'
    KEY_R                = 0x52, // 'R' or 'r'
    KEY_S                = 0x53, // 'S' or 's'
    KEY_T                = 0x54, // 'T' or 't'
    KEY_U                = 0x55, // 'U' or 'u'
    KEY_V                = 0x56, // 'V' or 'v'
    KEY_W                = 0x57, // 'W' or 'w'
    KEY_X                = 0x58, // 'X' or 'x'
    KEY_Y                = 0x59, // 'Y' or 'y'
    KEY_Z                = 0x5A, // 'Z' or 'z'

    KEY_SUPER_LEFT       = 0x5B, // Left Windows/super
    KEY_SUPER_RIGHT      = 0x5C, // Right Windows/super
    KEY_CONTEXT_MENU     = 0x5D, // Applications/context menu

    KEY_KP_0             = 0x60, // Numeric keypad 0
    KEY_KP_1             = 0x61, // Numeric keypad 1
    KEY_KP_2             = 0x62, // Numeric keypad 2
    KEY_KP_3             = 0x63, // Numeric keypad 3
    KEY_KP_4             = 0x64, // Numeric keypad 4
    KEY_KP_5             = 0x65, // Numeric keypad 5
    KEY_KP_6             = 0x66, // Numeric keypad 6
    KEY_KP_7             = 0x67, // Numeric keypad 7
    KEY_KP_8             = 0x68, // Numeric keypad 8
    KEY_KP_9             = 0x69, // Numeric keypad 9

    KEY_MULTIPLY         = 0x6A, // Multiply
    KEY_ADD              = 0x6B, // Add
    KEY_SEPARATOR        = 0x6C, // Separator
    KEY_SUBTRACT         = 0x6D, // Subtract
    KEY_DECIMAL          = 0x6E, // Decimal
    KEY_DIVIDE           = 0x6F, // Divide

    KEY_F1               = 0x70, // F1
    KEY_F2               = 0x71, // F2
    KEY_F3               = 0x72, // F3
    KEY_F4               = 0x73, // F4
    KEY_F5               = 0x74, // F5
    KEY_F6               = 0x75, // F6
    KEY_F7               = 0x76, // F7
    KEY_F8               = 0x77, // F8
    KEY_F9               = 0x78, // F9
    KEY_F10              = 0x79, // F10
    KEY_F11              = 0x7A, // F11
    KEY_F12              = 0x7B, // F12

    KEY_NUM_LOCK         = 0x90, // Number lock
    KEY_SCROLL_LOCK      = 0x91, // Scroll lock
    KEY_KP_EQUAL         = 0x92, // Numeric keypad equal

    KEY_SHIFT_LEFT       = 0xA0, // Left shift
    KEY_SHIFT_RIGHT      = 0xA1, // Right shift
    KEY_CONTROL_LEFT     = 0xA2, // Left control
    KEY_CONTROL_RIGHT    = 0xA3, // Right control
    KEY_ALT_LEFT         = 0xA4, // Left alt
    KEY_ALT_RIGHT        = 0xA5, // Right alt

    KEY_SEMICOLON        = 0xBA, // Semicolon
    KEY_EQUAL            = 0xBB, // Equal/plus
    KEY_COMMA            = 0xBC, // Comma
    KEY_MINUS            = 0xBD, // Minus
    KEY_PERIOD           = 0xBE, // Period
    KEY_SLASH            = 0xBF, // Slash
    KEY_GRAVE            = 0xC0, // Grave

    KEY_BRACKET_LEFT     = 0xDB, // Left (square) bracket
    KEY_BACKSLASH        = 0xDC, // Pipe/backslash
    KEY_RBRACKET         = 0xDD, // Right (square) bracket
    KEY_APOSTROPHE       = 0xDE, // Apostrophe/single-quote

    KEY_VOLUME_MUTE      = 0xAD, // Volume Mute
    KEY_VOLUME_DOWN      = 0xAE, // Volume Down
    KEY_VOLUME_UP        = 0xAF, // Volume Up
    KEY_MEDIA_NEXT_TRACK = 0xB0, // Next Track
    KEY_MEDIA_PREV_TRACK = 0xB1, // Previous Track
    KEY_MEDIA_STOP       = 0xB2, // Stop Media
    KEY_MEDIA_PLAY_PAUSE = 0xB3, // Play/Pause Media

    KEY_COUNT            = 0xFF, // Key count
};

enum MouseButton {
    BUTTON_LEFT = 0, // Left mouse button
    BUTTON_MIDDLE,   // Middle mouse button
    BUTTON_RIGHT,    // Right mouse button

    BUTTON_COUNT,
};
