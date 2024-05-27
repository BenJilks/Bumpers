/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace Engine::Input {

enum class MouseButton {
    Left = 0,
    Middle,
    Right,
};

enum class Key {
    // TODO: All keys should be covered here.
    Up,
    Down,
    Left,
    Right,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    Zero,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Space,
    BackSpace,
    Enter,
    Tab,
    Escape,
    CapsLock,
    LeftShift,
    RightShift,
    LeftControl,
    RightControl,
    LeftAlt,
    RightAlt,

    None,
};

bool is_key_down(Key key);
bool is_mouse_button_down(MouseButton);
bool is_mouse_button_click(MouseButton);
int mouse_x();
int mouse_y();

void update_key_state(Key key_code, bool state);
void update_mouse_position(int x, int y);
void update_mouse_button_state(MouseButton, bool state);
void end_frame();

}
