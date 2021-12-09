#include "physics_body_2d.hpp"
#include "engine/physics/collision_shape_utils_2d.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include <iostream>
#include <cmath>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <limits>
using namespace Engine;
using namespace Object;
using namespace glm;

void PhysicsBody2D::init(GameObject &gameobject) 
{
    m_transform = gameobject.first<Transform>();
}

void PhysicsBody2D::step_physics(GameObject&, float by) 
{
    assert (m_transform);

    m_transform->translate(vec_2to3(m_velocity * by));
    m_transform->rotate(vec3(0, 1, 0), -m_angular_velocity * by);

    auto factor = abs(glm::dot(glm::normalize(m_velocity), glm::normalize(vec_3to2(m_transform->forward()))));
    if (!std::isnan(factor) && !std::isinf(factor))
    {
        auto friction = factor * m_friction.y + (1.0 - factor) * m_friction.x;
        m_velocity *= 1.0 - (friction * by);
    }
    m_angular_velocity *= 1.0 - 0.1;

    assert(!std::isnan(m_velocity.x));
}

void PhysicsBody2D::apply_impulse(glm::vec2 impulse, glm::vec2 contact_point)
{
    if (m_mass != std::numeric_limits<float>::infinity())
        apply_force(1.0f / m_mass * impulse);
    if (m_inertia != std::numeric_limits<float>::infinity())
        apply_torque(1.0f / m_inertia * glm::dot(contact_point * vec2(-1, 1), vec2(impulse.y, impulse.x)));
}

float PhysicsBody2D::speed() const
{
    return glm::length(m_velocity);
}

float PhysicsBody2D::sideways_speed() const
{
    auto a = glm::normalize(m_velocity);
    auto b = glm::normalize(vec_3to2(m_transform->forward()));
    return speed() * (1.0 - abs(glm::dot(a, b)));
}

float PhysicsBody2D::velocity_angle() const
{
    return glm::orientedAngle(glm::vec2(0, 1), glm::normalize(m_velocity));
}

