/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "car_engine.hpp"
#include "gameobject/component.hpp"

namespace Game {

class PlayerController : public Object::ComponentBase<PlayerController> {
    friend Object::ComponentBase<PlayerController>;

public:
    void init(Object::GameObject&) override;
    void update(Object::GameObject&, float delta) override;

private:
    PlayerController(PlayerController const&) = default;
    PlayerController() = default;

    CarEngine* m_engine { nullptr };
};

}
