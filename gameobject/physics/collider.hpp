#pragma once

#include "gameobject/component.hpp"
#include "engine/forward.hpp"
#include <set>

namespace Object
{

    class Collider : public ComponentBase<Collider>
    {
        friend ComponentBase<Collider>;
        friend Engine::CollisionResolver;

    public:
        inline Engine::CollisionShape &shape() { return *m_shape; }
        inline const Engine::CollisionShape &shape() const { return *m_shape; }
        inline const std::set<GameObject*> &objects_in_collision_with() const { return m_objects_in_collision_with; }
        
    private:
        Collider(const Collider&) = default;
        Collider(std::shared_ptr<Engine::CollisionShape>);

        std::shared_ptr<Engine::CollisionShape> m_shape;
        std::set<GameObject*> m_objects_in_collision_with;

    };

}
