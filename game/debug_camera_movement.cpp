#include "debug_camera_movement.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include "engine/input.hpp"
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

    float speed = 1.0f * delta;

    // TODO: Mouse movement

    auto forward = m_transform->forward();
    if (Engine::Input::is_key_down('w'))
        m_transform->translate(forward * speed);
    if (Engine::Input::is_key_down('s'))
        m_transform->translate(-forward * speed);

    auto left = 0; // TODO
    if (Engine::Input::is_key_down('a'))
        m_transform->translate(left * speed);
    if (Engine::Input::is_key_down('d'))
        m_transform->translate(-left * speed);
}
