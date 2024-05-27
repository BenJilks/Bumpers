/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "box_bounds_3d.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
using namespace Object;
using namespace glm;

void BoxBounds3D::init(GameObject& game_object)
{
    m_transform = game_object.first<Transform>();
}

void BoxBounds3D::update(GameObject&, float by)
{
    assert(m_transform);

    auto position = m_transform->position();
    for (auto const& box : m_boxes) {
        auto penetration = -(glm::abs(position - box.position) - box.half_extents);
        if (penetration.x < 0 || penetration.y < 0 || penetration.z < 0) {
            continue;
        }

        auto min_penetration = std::min({ penetration.x, penetration.y, penetration.z });
        auto adjustment = vec3(0);
        if (penetration.x == min_penetration) {
            adjustment.x = penetration.x * (position.x < box.position.x ? -1 : 1);
        } else if (penetration.y == min_penetration) {
            adjustment.y = penetration.y * (position.y < box.position.y ? -1 : 1);
        } else if (penetration.z == min_penetration) {
            adjustment.z = penetration.z * (position.z < box.position.z ? -1 : 1);
        }

        m_transform->translate(adjustment);
    }

    if (position.y < 1) {
        m_transform->translate(vec3(0, 1 - position.y, 0));
    }
}
