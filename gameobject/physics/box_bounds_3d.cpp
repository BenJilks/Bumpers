#include "box_bounds_3d.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include <iostream>
using namespace Object;
using namespace glm;

void BoxBounds3D::init(GameObject &gameobject)
{
    m_transform = gameobject.first<Transform>();
}

void BoxBounds3D::update(GameObject&, float by)
{
    assert (m_transform);

    auto position = m_transform->position();
    for (const auto &box : m_boxes)
    {
        auto penetration = -(glm::abs(position - box.position) - box.half_extents);
        if (penetration.x < 0 || penetration.y < 0 || penetration.z < 0)
            continue;

        auto min_penetration = std::min({ penetration.x, penetration.y, penetration.z });
        auto adjustment = vec3(0);
        if (penetration.x == min_penetration)
            adjustment.x = penetration.x * (position.x < box.position.x ? -1 : 1);
        else if (penetration.y == min_penetration)
            adjustment.y = penetration.y * (position.y < box.position.y ? -1 : 1);
        else if (penetration.z == min_penetration)
            adjustment.z = penetration.z * (position.z < box.position.z ? -1 : 1);

        m_transform->translate(adjustment);
    }

    if (position.y < 1)
        m_transform->translate(vec3(0, 1 - position.y, 0));
}

