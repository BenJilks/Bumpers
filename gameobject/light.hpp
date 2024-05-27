/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "component.hpp"
#include <glm/glm.hpp>

namespace Object {

class Light : public ComponentBase<Light> {
    friend ComponentBase<Light>;

public:
    ~Light() override = default;

    [[nodiscard]] inline glm::vec3 const& color() const { return m_color; }

private:
    Light(Light const&) = default;

    explicit Light(glm::vec3 color)
        : m_color(color)
    {
    }

    glm::vec3 m_color;
};

}
