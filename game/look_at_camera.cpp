/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "look_at_camera.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/vector_angle.hpp"
#include <cassert>
using namespace Object;
using namespace Game;

void LookAtCamera::init(GameObject& game_object)
{
    m_transform = game_object.first<Transform>();
    m_player_transform = m_player->first<Transform>();
}

void LookAtCamera::update(GameObject&, float)
{
    assert(m_transform);
    assert(m_player_transform);

    auto to_player = m_player_transform->position() - m_transform->position();
    auto to_player_direction = glm::normalize(to_player);
    auto y_angle = glm::orientedAngle(glm::vec2(0, 1), glm::vec2(to_player_direction.x, -to_player_direction.z));

    auto distance_from_player = glm::length(glm::vec2(to_player.x, to_player.z));
    auto theta = std::tan((m_transform->position().y - 1.0f) / distance_from_player);
    auto x_angle = glm::radians(90.0f) - (glm::radians(180.0f - 90.0f) - theta);

    m_transform->set_rotation(glm::vec3(-x_angle, y_angle, 0));
}
