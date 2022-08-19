/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "display.hpp"
#include "engine/assets/thread_pool.hpp"
#include "input.hpp"
#include "gameobject/scene.hpp"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <functional>
#include <chrono>
using namespace Engine;
using namespace Object;

static std::unique_ptr<Scene> s_current_scene { nullptr };
static std::unique_ptr<Scene> s_new_scene { nullptr };
static auto s_last_frame_time = std::chrono::system_clock::now();

static int s_width;
static int s_height;

static void reshape(int width, int height)
{
    if (s_current_scene)
        s_current_scene->on_resize(width, height);

    glViewport(0, 0, width, height);
    s_width = width;
    s_height = height;
}

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
    float delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - s_last_frame_time).count() / 1000.0f;
    s_last_frame_time = now;

    if (s_current_scene)
        s_current_scene->on_render(delta);

    glutSwapBuffers();
    Input::end_frame();
}

static void idle()
{
    glutPostRedisplay();
}

static void keyboard(unsigned char c, int, int)
{
    Input::update_key_state(std::tolower(c), true);
}
static void keyboard_up(unsigned char c, int, int)
{
    Input::update_key_state(std::tolower(c), false);
}

static void special(int key, int, int)
{
    Input::update_key_state(key << 8, true);
}
static void special_up(int key, int, int)
{
    Input::update_key_state(key << 8, false);
}

static void mouse_button(int button, int state, int, int)
{
    if (button < 3)
        Input::update_mouse_button_state((Input::MouseButton)button, state == 0);
}

static void mouse_moved(int x, int y)
{
    Input::update_mouse_position(x, y);
}

bool Display::open(std::string_view title, int width, int height)
{
    s_width = width;
    s_height = height;

    int argc = 0;
    char **argv = nullptr;
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Car Game");

    glutReshapeFunc(reshape);

    //This initialises GLEW - it must be called after the window is created.
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error: Could not initialise GLEW\n";
        return false;
    }

    //Check the OpenGL version being used
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

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    return true;
}

void Display::start_main_loop()
{
    glutMainLoop();
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
