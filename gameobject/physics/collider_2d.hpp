/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "engine/forward.hpp"
#include "gameobject/component.hpp"
#include <set>

namespace Object {

class Collider2D : public ComponentBase<Collider2D> {
    friend ComponentBase<Collider2D>;
    friend Engine::CollisionResolver2D;

public:
    inline Engine::CollisionShape2D& shape() { return *m_shape; }
    inline Engine::CollisionShape2D const& shape() const { return *m_shape; }
    inline std::set<GameObject*> const& objects_in_collision_with() const { return m_objects_in_collision_with; }

private:
    Collider2D(Collider2D const&) = default;
    Collider2D(std::shared_ptr<Engine::CollisionShape2D>);

    std::shared_ptr<Engine::CollisionShape2D> m_shape;
    std::set<GameObject*> m_objects_in_collision_with;
};

}
