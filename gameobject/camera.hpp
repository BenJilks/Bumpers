/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "component.hpp"

namespace Object
{

    class Camera : public ComponentBase<Camera>
    {
        friend ComponentBase<Camera>;

    public:

    private:
        Camera() = default;
    };

}
