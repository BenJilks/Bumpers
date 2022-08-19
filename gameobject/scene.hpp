/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace Object
{
    
    class Scene
    {
    public:
        virtual ~Scene()
        {
        }
        
        virtual bool init() = 0;
        virtual void on_render(float delta) = 0;
        virtual void on_resize(int width, int height) = 0;

    };

}
