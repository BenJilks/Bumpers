#pragma once

#include "gameobject/component.hpp"
#include "engine/forward.hpp"
#include <set>

namespace Object
{

    class Collider2D : public ComponentBase<Collider2D>
    {
        friend ComponentBase<Collider2D>;
        friend Engine::CollisionResolver2D;

    public:
        inline Engine::CollisionShape2D &shape() { return *m_shape; }
        inline const Engine::CollisionShape2D &shape() const { return *m_shape; }
        inline const std::set<GameObject*> &objects_in_collision_with() const { return m_objects_in_collision_with; }
        
    private:
        Collider2D(const Collider2D&) = default;
        Collider2D(std::shared_ptr<Engine::CollisionShape2D>);

        std::shared_ptr<Engine::CollisionShape2D> m_shape;
        std::set<GameObject*> m_objects_in_collision_with;

    };

}

