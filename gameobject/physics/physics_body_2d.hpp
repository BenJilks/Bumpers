/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "gameobject/component.hpp"
#include "gameobject/forward.hpp"
#include <glm/glm.hpp>

namespace Object {

class PhysicsBody2D : public ComponentBase<PhysicsBody2D> {
    friend ComponentBase<PhysicsBody2D>;

public:
    virtual void init(GameObject&) final;
    virtual void step_physics(GameObject&, float by) final;

    inline glm::vec2 const& velocity() const { return m_velocity; }
    inline float angular_velocity() const { return m_angular_velocity; }
    inline float restitution() const { return m_restitution; }
    inline float mass() const { return m_mass; }
    inline float inertia() const { return m_inertia; }
    inline void apply_force(glm::vec2 force) { m_velocity += force; }
    inline void apply_torque(float torque) { m_angular_velocity += torque; }
    void apply_impulse(glm::vec2 impulse, glm::vec2 contact_point);

    float speed() const;
    float sideways_speed() const;
    float velocity_angle() const;

    inline bool is_static() const { return m_mass == std::numeric_limits<float>::infinity(); }

private:
    PhysicsBody2D(PhysicsBody2D const&) = default;
    PhysicsBody2D(glm::vec2 friction, float restitution, float mass, float inertia)
        : m_velocity()
        , m_angular_velocity(0)
        , m_friction(friction)
        , m_restitution(restitution)
        , m_mass(mass)
        , m_inertia(inertia)
    {
    }

    glm::vec2 m_velocity;
    float m_angular_velocity;

    glm::vec2 m_friction;
    float m_restitution;
    float m_mass;
    float m_inertia;

    Transform* m_transform { nullptr };
};

}
