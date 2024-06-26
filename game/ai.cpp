/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ai.hpp"
#include "engine/physics/collision_shape_utils_2d.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include <cassert>
#include <random>
using namespace Game;
using namespace Object;
using namespace Engine;
using namespace glm;

void AI::init(Object::GameObject& game_object)
{
    m_engine = game_object.first<CarEngine>();
    m_transform = game_object.first<Transform>();
}

void AI::update(Object::GameObject&, float delta)
{
    assert(m_engine);
    assert(m_transform);
    m_engine->set_action(CarEngine::Action::Forward, true);
    m_timer_until_next_action -= delta;

    auto distance_from_center = glm::length(vec_3to2(m_transform->position()) * vec2(1, 0.5f));
    if (distance_from_center >= 15) {
        auto forward = vec_3to2(m_transform->left());
        auto to_center = glm::normalize(vec_3to2(m_transform->position()));
        auto dot = glm::dot(forward, to_center);
        m_engine->set_action(CarEngine::Action::TurnLeft, dot > 0);
        m_engine->set_action(CarEngine::Action::TurnRight, dot < 0);
    } else {
        if (m_timer_until_next_action <= 0) {
            std::random_device random;
            m_engine->set_action(CarEngine::Action::TurnLeft, (random() % 2) == 0);
            m_engine->set_action(CarEngine::Action::TurnRight, (random() % 2) == 0);
            m_timer_until_next_action = (static_cast<float>(random() % 100) / 100.0f) * 0.5f;
        }
    }
}
