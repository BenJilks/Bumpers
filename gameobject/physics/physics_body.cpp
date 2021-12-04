#include "physics_body.hpp"
#include "engine/physics/collision_shape_utils.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include <iostream>
#include <cmath>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>
using namespace Engine;
using namespace Object;
using namespace glm;

void PhysicsBody::init(GameObject &gameobject) 
{
    m_transform = gameobject.first<Transform>();
}

void PhysicsBody::step_physics(GameObject&, float by) 
{
    assert (m_transform);

    m_transform->translate(vec_2to3(m_velocity * by));
    m_transform->rotate(vec3(0, 1, 0), -m_angular_velocity * by);

    auto factor = abs(glm::dot(glm::normalize(m_velocity), glm::normalize(vec_3to2(m_transform->forward()))));
    if (!std::isnan(factor))
    {
        auto friction = factor * m_friction.y + (1.0 - factor) * m_friction.x;
        m_velocity *= 1.0 - (friction * by);
    }
    m_angular_velocity *= 1.0 - 0.05;
}

void PhysicsBody::apply_impulse(glm::vec2 impulse, glm::vec2 contact_point)
{
    apply_force(1.0f / m_mass * impulse);
    apply_torque(1.0f / m_inertia * glm::dot(contact_point, vec2(-impulse.y, impulse.x)));
}

float PhysicsBody::speed() const
{
    return glm::length(m_velocity);
}

float PhysicsBody::sideways_speed() const
{
    auto a = glm::normalize(m_velocity);
    auto b = glm::normalize(vec_3to2(m_transform->forward()));
    return speed() * (1.0 - abs(glm::dot(a, b)));
}

float PhysicsBody::velocity_angle() const
{
    return glm::orientedAngle(glm::vec2(0, 1), glm::normalize(m_velocity));
}

