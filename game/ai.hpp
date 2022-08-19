/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "gameobject/forward.hpp"
#include "gameobject/component.hpp"
#include "car_engine.hpp"

namespace Game
{

    class AI : public Object::ComponentBase<AI>
    {
        friend Object::ComponentBase<AI>;

    public:
        virtual void init(Object::GameObject&) override;
        virtual void update(Object::GameObject&, float delta) override;

    private:
        AI(const AI&) = default;
        AI() = default;

        CarEngine *m_engine { nullptr };
        Object::Transform *m_transform { nullptr };
        float m_timer_until_next_action { 0 };

    };

}

