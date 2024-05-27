/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <GL/glew.h>

#include "display.hpp"
#include "gameobject/scene.hpp"
#include "input.hpp"
#include <GL/freeglut.h>
#include <chrono>
#include <iostream>
using namespace Engine;
using namespace Object;

static std::unique_ptr<Scene> s_current_scene { nullptr };
static std::unique_ptr<Scene> s_new_scene { nullptr };
static auto s_last_frame_time = std::chrono::system_clock::now();

static int s_width;
static int s_height;

static void reshape(int width, int height)
{
    if (s_current_scene) {
        s_current_scene->on_resize(width, height);
    }

    glViewport(0, 0, width, height);
    s_width = width;
    s_height = height;
}

static void display()
{
    if (s_new_scene) {
        s_current_scene = std::move(s_new_scene);
        s_current_scene->on_resize(s_width, s_height);
        s_new_scene = nullptr;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto now = std::chrono::system_clock::now();
    float delta = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - s_last_frame_time).count()) / 1000.0f;
    s_last_frame_time = now;

    if (s_current_scene) {
        s_current_scene->on_render(delta);
    }

    glutSwapBuffers();
    Input::end_frame();
}

static void idle()
{
    glutPostRedisplay();
}

static Input::Key char_to_key(unsigned char c)
{
    switch (std::toupper(c)) {
    case 'A':
        return Input::Key::A;
    case 'B':
        return Input::Key::B;
    case 'C':
        return Input::Key::C;
    case 'D':
        return Input::Key::D;
    case 'E':
        return Input::Key::E;
    case 'F':
        return Input::Key::F;
    case 'G':
        return Input::Key::G;
    case 'H':
        return Input::Key::H;
    case 'I':
        return Input::Key::I;
    case 'J':
        return Input::Key::J;
    case 'K':
        return Input::Key::K;
    case 'L':
        return Input::Key::L;
    case 'M':
        return Input::Key::M;
    case 'N':
        return Input::Key::N;
    case 'O':
        return Input::Key::O;
    case 'P':
        return Input::Key::P;
    case 'Q':
        return Input::Key::Q;
    case 'R':
        return Input::Key::R;
    case 'S':
        return Input::Key::S;
    case 'T':
        return Input::Key::T;
    case 'U':
        return Input::Key::U;
    case 'V':
        return Input::Key::V;
    case 'W':
        return Input::Key::W;
    case 'X':
        return Input::Key::X;
    case 'Y':
        return Input::Key::Y;
    case 'Z':
        return Input::Key::Z;

    case '0':
        return Input::Key::Zero;
    case '1':
        return Input::Key::One;
    case '2':
        return Input::Key::Two;
    case '3':
        return Input::Key::Three;
    case '4':
        return Input::Key::Four;
    case '5':
        return Input::Key::Five;
    case '6':
        return Input::Key::Six;
    case '7':
        return Input::Key::Seven;
    case '8':
        return Input::Key::Eight;
    case '9':
        return Input::Key::Nine;

    case ' ':
        return Input::Key::Space;
    case '\n':
        return Input::Key::Enter;
    case '\t':
        return Input::Key::Tab;
    default:
        return Input::Key::None;
    }
}

static void keyboard(unsigned char c, int, int)
{
    Input::update_key_state(char_to_key(c), true);
}
static void keyboard_up(unsigned char c, int, int)
{
    Input::update_key_state(char_to_key(c), false);
}

static Input::Key glut_keycode_to_key(int keycode)
{
    switch (keycode) {
    case GLUT_KEY_UP:
        return Input::Key::Up;
    case GLUT_KEY_DOWN:
        return Input::Key::Down;
    case GLUT_KEY_LEFT:
        return Input::Key::Left;
    case GLUT_KEY_RIGHT:
        return Input::Key::Right;

    // TODO: BackSpace Escape CapsLock
    case GLUT_KEY_SHIFT_L:
        return Input::Key::LeftShift;
    case GLUT_KEY_SHIFT_R:
        return Input::Key::RightShift;
    case GLUT_KEY_CTRL_L:
        return Input::Key::LeftControl;
    case GLUT_KEY_CTRL_R:
        return Input::Key::RightControl;
    case GLUT_KEY_ALT_L:
        return Input::Key::LeftAlt;
    case GLUT_KEY_ALT_R:
        return Input::Key::RightAlt;

    default:
        return Input::Key::None;
    }
}

static void special(int keycode, int, int)
{
    Input::update_key_state(glut_keycode_to_key(keycode), true);
}
static void special_up(int keycode, int, int)
{
    Input::update_key_state(glut_keycode_to_key(keycode), false);
}

static void mouse_button(int button, int state, int, int)
{
    if (button < 3) {
        Input::update_mouse_button_state((Input::MouseButton)button, state == 0);
    }
}

static void mouse_moved(int x, int y)
{
    Input::update_mouse_position(x, y);
}

bool Display::open(std::string_view, int width, int height)
{
    s_width = width;
    s_height = height;

    int argc = 0;
    char** argv = nullptr;
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Car Game");

    glutReshapeFunc(reshape);

    // This initialises GLEW - it must be called after the window is created.
    GLenum const err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: Could not initialise GLEW\n";
        return false;
    }

    // Check the OpenGL version being used
    int OpenGLVersion[2];
    glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboard_up);
    glutSpecialFunc(special);
    glutSpecialUpFunc(special_up);
    glutMouseFunc(mouse_button);
    glutPassiveMotionFunc(mouse_moved);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    return true;
}

void Display::start_main_loop()
{
    glutMainLoop();
}

void Display::set_scene(std::unique_ptr<Object::Scene> scene)
{
    if (!scene->init()) {
        return;
    }

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
