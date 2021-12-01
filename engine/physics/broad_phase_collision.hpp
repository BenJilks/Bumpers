#pragma once

#include "collision_resolver.hpp"
#include "gameobject/forward.hpp"
#include <functional>

namespace Engine::BroadPhaseCollision
{
    
    void for_each_narrow_phase_pair(
        std::vector<CollisionResolver::CollisionObject> &collition_objects,
        std::function<void(CollisionResolver::CollisionObject&, Object::Collider&, CollisionResolver::CollisionObject&, Object::Collider&)> callback);
    
}
