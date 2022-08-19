/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once 

#include "component.hpp"
#include <memory>
#include <glm/glm.hpp>

namespace Object
{

    class Light : public ComponentBase<Light>
    {
        friend ComponentBase<Light>;

    public:
        virtual ~Light() final 
        {
        }

        inline const glm::vec3 &color() const { return m_color; }

    private:
        Light(const Light&) = default;
        Light(glm::vec3 color)
            : m_color(color)
        {
        }

        glm::vec3 m_color;

    };

}
