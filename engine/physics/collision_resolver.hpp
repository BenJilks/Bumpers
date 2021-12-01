#pragma once
#include "gameobject/forward.hpp"

namespace Engine
{
    
    class CollisionResolver
    {
    public:
        struct CollisionObject
        {
            Object::GameObject &object;
            Object::Transform& transform;
            Object::PhysicsBody *body;
        };
        
        CollisionResolver() = default;

        void resolve(Object::World&);

    private:

    };

}

