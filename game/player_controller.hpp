/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "gameobject/component.hpp"
#include "car_engine.hpp"

namespace Game
{

    class PlayerController : public Object::ComponentBase<PlayerController>
    {
        friend Object::ComponentBase<PlayerController>;

    public:
        virtual void init(Object::GameObject&) override;
        virtual void update(Object::GameObject&, float delta) override;

    private:
        PlayerController(const PlayerController&) = default;
        PlayerController() = default;

        CarEngine *m_engine { nullptr };

    };

}

