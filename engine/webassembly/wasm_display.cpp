#include "engine/display.hpp"
#include "engine/input.hpp"
#include "gameobject/scene.hpp"
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#include <chrono>
#include <iostream>
#include <cstring>

using namespace Engine;
using namespace Object;

static std::unique_ptr<Scene> s_current_scene { nullptr };
static std::unique_ptr<Scene> s_new_scene { nullptr };
static auto s_last_frame_time = std::chrono::system_clock::now();

static int s_width;
static int s_height;

static void display()
{
    if (s_new_scene)
    {
        s_current_scene = std::move(s_new_scene);
        s_current_scene->on_resize(s_width, s_height);
        s_new_scene = nullptr;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto now = std::chrono::system_clock::now();
    float delta = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - s_last_frame_time).count()) / 1000.0f;
    s_last_frame_time = now;

    if (s_current_scene)
        s_current_scene->on_render(delta);
}

static Input::Key keycode_to_key(const char *code)
{
    if (strlen(code) == 4 && std::string_view(code, 3) == "Key")
    {
        // NOTE: Keys in the format Key*
        switch (code[3])
        {
            case 'A': return Input::Key::A;
            case 'B': return Input::Key::B;
            case 'C': return Input::Key::C;
            case 'D': return Input::Key::D;
            case 'E': return Input::Key::E;
            case 'F': return Input::Key::F;
            case 'G': return Input::Key::G;
            case 'H': return Input::Key::H;
            case 'I': return Input::Key::I;
            case 'J': return Input::Key::J;
            case 'K': return Input::Key::K;
            case 'L': return Input::Key::L;
            case 'M': return Input::Key::M;
            case 'N': return Input::Key::N;
            case 'O': return Input::Key::O;
            case 'P': return Input::Key::P;
            case 'Q': return Input::Key::Q;
            case 'R': return Input::Key::R;
            case 'S': return Input::Key::S;
            case 'T': return Input::Key::T;
            case 'U': return Input::Key::U;
            case 'V': return Input::Key::V;
            case 'W': return Input::Key::W;
            case 'X': return Input::Key::X;
            case 'Y': return Input::Key::Y;
            case 'Z': return Input::Key::Z;
        }
    }

    if (strlen(code) == 6 && std::string_view(code, 5) == "Digit")
    {
        // NOTE: Keys in the format Digit*
        switch (code[5])
        {
            case '0': return Input::Key::Zero;
            case '1': return Input::Key::One;
            case '2': return Input::Key::Two;
            case '3': return Input::Key::Three;
            case '4': return Input::Key::Four;
            case '5': return Input::Key::Five;
            case '6': return Input::Key::Six;
            case '7': return Input::Key::Seven;
            case '8': return Input::Key::Eight;
            case '9': return Input::Key::Nine;
        }
    }

    if (strcmp(code, "ArrowUp") == 0) return Input::Key::Up;
    if (strcmp(code, "ArrowDown") == 0) return Input::Key::Down;
    if (strcmp(code, "ArrowLeft") == 0) return Input::Key::Left;
    if (strcmp(code, "ArrowRight") == 0) return Input::Key::Right;

    if (strcmp(code, "Space") == 0) return Input::Key::Space;
    if (strcmp(code, "Backspace") == 0) return Input::Key::BackSpace;
    if (strcmp(code, "Enter") == 0) return Input::Key::Enter;
    if (strcmp(code, "Tab") == 0) return Input::Key::Tab;
    if (strcmp(code, "Escape") == 0) return Input::Key::Escape;
    if (strcmp(code, "CapsLock") == 0) return Input::Key::CapsLock;
    if (strcmp(code, "ShiftLeft") == 0) return Input::Key::LeftShift;
    if (strcmp(code, "ShiftRight") == 0) return Input::Key::RightShift;
    if (strcmp(code, "ControlLeft") == 0) return Input::Key::LeftControl;
    if (strcmp(code, "ControlRight") == 0) return Input::Key::RightControl;
    if (strcmp(code, "AltLeft") == 0) return Input::Key::LeftAlt;
    if (strcmp(code, "AltRight") == 0) return Input::Key::RightAlt;

    std::cerr << "Unknown keycode '" << code << "'\n";
    return Input::Key::None;
}

static int on_keyboard_event(int event_type, const EmscriptenKeyboardEvent *event, void*)
{
    auto keycode = keycode_to_key(event->code);
    switch (event_type)
    {
        case EMSCRIPTEN_EVENT_KEYDOWN:
            Input::update_key_state(keycode, true);
            return true;

        case EMSCRIPTEN_EVENT_KEYUP:
            Input::update_key_state(keycode, false);
            return true;

        default:
            return false;
    }
}

static Input::MouseButton code_to_mouse_button(unsigned short code)
{
    switch (code)
    {
        case 0: return Input::MouseButton::Left;
        case 1: return Input::MouseButton::Middle;
        case 2: return Input::MouseButton::Right;

        default:
            return Input::MouseButton::Left;
    }
}

static int on_mouse_event(int event_type, const EmscriptenMouseEvent *event, void*)
{
    auto button = code_to_mouse_button(event->button);
    switch (event_type)
    {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
            Input::update_mouse_button_state(button, true);
            return true;

        case EMSCRIPTEN_EVENT_MOUSEUP:
            Input::update_mouse_button_state(button, false);
            return true;

        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            Input::update_mouse_position((int)event->clientX, (int)event->clientY);
            return true;

        default:
            return false;
    }
}

bool Display::open(std::string_view, int width, int height)
{
    s_width = width;
    s_height = height;

    EmscriptenWebGLContextAttributes attributes {
        .majorVersion = 2,
        .minorVersion = 0,
    };

    auto context = emscripten_webgl_create_context("canvas", &attributes);
    if (!context)
    {
        std::cerr << "Unable to create webgl context\n";
        return false;
    }

    emscripten_webgl_make_context_current(context);
    emscripten_set_keydown_callback("canvas", nullptr, true, on_keyboard_event);
    emscripten_set_keyup_callback("canvas", nullptr, true, on_keyboard_event);

    emscripten_set_mousemove_callback("canvas", nullptr, true, on_mouse_event);
//    emscripten_set_mousedown_callback("canvas", nullptr, true, on_mouse_event);
//    emscripten_set_mouseup_callback("canvas", nullptr, true, on_mouse_event);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return true;
}

void Display::start_main_loop()
{
    emscripten_set_main_loop(&display, 0, 0);
}

void Display::set_scene(std::unique_ptr<Object::Scene> scene)
{
    if (!scene->init())
        return;

    s_new_scene = std::move(scene);
}

int Display::width()
{
    return s_width;
}

int Display::height()
{
    return s_height;
}
