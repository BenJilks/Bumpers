/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "gameobject/component.hpp"
#include "gameobject/forward.hpp"
#include <array>

namespace Game {

class CarEngine : public Object::ComponentBase<CarEngine> {
    friend Object::ComponentBase<CarEngine>;

public:
    void init(Object::GameObject&) override;
    void update(Object::GameObject&, float delta) override;

    enum class Action {
        Forward,
        Reverse,
        TurnLeft,
        TurnRight,

        Count,
    };

    inline void set_action(Action action, bool value) { m_action_enabled[(size_t)action] = value; }

private:
    CarEngine(CarEngine const&) = default;
    CarEngine() = default;

    [[nodiscard]] inline bool is_action_enabled(Action action) const { return m_action_enabled[(size_t)action]; }

    float m_wheel_direction { 0 };
    std::array<bool, (size_t)Action::Count> m_action_enabled {};

    Object::PhysicsBody2D* m_physics_body { nullptr };
    Object::Transform* m_transform { nullptr };
    Object::Collider2D* m_collider { nullptr };

    Object::Transform* m_wheel_transform { nullptr };
    Object::Transform* m_wheel_support_transform { nullptr };
};

}
