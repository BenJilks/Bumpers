/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once
#include "gameobject/forward.hpp"

namespace Engine
{
    
    class CollisionResolver2D
    {
    public:
        struct CollisionObject
        {
            Object::GameObject &object;
            Object::Transform& transform;
            Object::PhysicsBody2D *body;
        };
        
        CollisionResolver2D() = default;

        void resolve(Object::World&);

    private:

    };

}

