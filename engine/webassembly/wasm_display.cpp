#include "engine/display.hpp"
#include "gameobject/scene.hpp"
#include <GLES2/gl2.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <chrono>
#include <iostream>

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
