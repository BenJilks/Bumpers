#pragma once

#include "collision_resolver_2d.hpp"
#include "gameobject/forward.hpp"
#include <functional>

namespace Engine::BroadPhaseCollision2D
{
    
    void for_each_narrow_phase_pair(
        std::vector<CollisionResolver2D::CollisionObject> &collition_objects,
        std::function<void(CollisionResolver2D::CollisionObject&, Object::Collider2D&, CollisionResolver2D::CollisionObject&, Object::Collider2D&)> callback);
    
}

