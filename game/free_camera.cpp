/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "free_camera.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include "engine/input.hpp"
#include <cassert>
using namespace Object;
using namespace Game;
using namespace Engine;
using namespace glm;

void FreeCamera::init(GameObject &game_object)
{
    m_transform = game_object.first<Transform>();
    assert(m_transform);
}

void FreeCamera::update(GameObject&, float delta)
{
    assert(m_transform);

    float speed = 10.0f * delta;
    float sensitivity = 0.3f * delta;

    auto delta_mouse_x = m_last_mouse_x - Engine::Input::mouse_x();
    auto delta_mouse_y = m_last_mouse_y - Engine::Input::mouse_y();
    m_transform->rotate(vec3(0, 1, 0), static_cast<float>(delta_mouse_x) * sensitivity);
    m_transform->rotate(vec3(1, 0, 0), static_cast<float>(delta_mouse_y) * sensitivity);

    m_last_mouse_x = Engine::Input::mouse_x();
    m_last_mouse_y = Engine::Input::mouse_y();

    auto forward = m_transform->forward();
    if (Engine::Input::is_key_down(Input::Key::W))
        m_transform->translate(forward * speed);
    if (Engine::Input::is_key_down(Input::Key::S))
        m_transform->translate(-forward * speed);

    auto left = m_transform->left();
    if (Engine::Input::is_key_down(Input::Key::A))
        m_transform->translate(left * speed);
    if (Engine::Input::is_key_down(Input::Key::D))
        m_transform->translate(-left * speed);

    if (Engine::Input::is_key_down(Input::Key::Space))
        m_transform->translate(vec3(0, speed, 0));
    if (Engine::Input::is_key_down(Input::Key::LeftShift))
        m_transform->translate(vec3(0, -speed, 0));
}

