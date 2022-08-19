/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "gameobject/component.hpp"
#include "engine/forward.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Object
{

    class BoxBounds3D : public ComponentBase<BoxBounds3D>
    {
        friend ComponentBase<BoxBounds3D>;

    public:
        struct Box
        {
            glm::vec3 position;
            glm::vec3 half_extents;
        };

        virtual void init(GameObject&) final;
        virtual void update(GameObject&, float by) final;
        
    private:
        BoxBounds3D(const BoxBounds3D&) = default;

        BoxBounds3D(std::vector<Box> boxes)
            : m_boxes(boxes)
        {
        }

        Object::Transform *m_transform { nullptr };
        std::vector<Box> m_boxes;

    };

}


