#include "debug_camera_movement.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include "engine/input.hpp"
#include <GL/freeglut.h>
#include <glm/gtx/string_cast.hpp>
#include <cassert>
#include <iostream>
using namespace Object;
using namespace Game;
using namespace glm;

void DebugCameraMovement::init(GameObject &gameobject)
{
    m_transform = gameobject.first<Transform>();
    assert(m_transform);
}

void DebugCameraMovement::update(GameObject&, float delta)
{
    assert(m_transform);

    float speed = 4.0f * delta;
    float sensitivity = 0.3f * delta;

    auto delta_mouse_x = m_last_mouse_x - Engine::Input::mouse_x();
    auto delta_mouse_y = m_last_mouse_y - Engine::Input::mouse_y();
    m_transform->rotate(vec3(0, 1, 0), delta_mouse_x * sensitivity);
    m_transform->rotate(vec3(1, 0, 0), delta_mouse_y * sensitivity);

    m_last_mouse_x = Engine::Input::mouse_x();
    m_last_mouse_y = Engine::Input::mouse_y();

    auto forward = m_transform->forward();
    if (Engine::Input::is_key_down('w'))
        m_transform->translate(forward * speed);
    if (Engine::Input::is_key_down('s'))
        m_transform->translate(-forward * speed);

    auto left = m_transform->left();
    if (Engine::Input::is_key_down('a'))
        m_transform->translate(left * speed);
    if (Engine::Input::is_key_down('d'))
        m_transform->translate(-left * speed);

    if (Engine::Input::is_key_down(' '))
        m_transform->translate(vec3(0, speed, 0));
    if (Engine::Input::is_key_down(GLUT_KEY_SHIFT_L))
        m_transform->translate(vec3(0, -speed, 0));
}

