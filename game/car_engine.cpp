#include "car_engine.hpp"
#include "engine/physics/collision_shape_utils_2d.hpp"
#include "gameobject/physics/physics_body_2d.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/attributes.hpp"
#include "gameobject/physics/collider_2d.hpp"
#include <glm/glm.hpp>
#include <iostream>
using namespace Engine;
using namespace Object;
using namespace Game;
using namespace glm;

static constexpr float s_turn_speed = 15.0f;
static constexpr float s_max_turn_direction = 2.0f;

void CarEngine::init(Object::GameObject &gameobject)
{
    m_physics_body = gameobject.first<PhysicsBody2D>();
    m_transform = gameobject.first<Transform>();
    m_collider = gameobject.first<Collider2D>();

    gameobject.for_each([this](GameObject &object)
    {
        auto *attributes = object.first<Attributes>();
        if (!attributes)
            return IteratorDecision::Continue;
 
        if (attributes->has("Wheel"))
            m_wheel_transform = object.first<Transform>();
        if (attributes->has("WheelSupport"))
            m_wheel_support_transform = object.first<Transform>();

        if (m_wheel_transform && m_wheel_support_transform)
            return IteratorDecision::Break;

        return IteratorDecision::Continue;
    });

    m_action_enabled.fill(false);
}

void CarEngine::update(Object::GameObject&, float delta)
{
    assert (m_physics_body);
    assert (m_transform);
    assert (m_collider);
    assert (m_wheel_transform);
    assert (m_wheel_support_transform);

    float speed = 40.0f * delta;
    m_physics_body->apply_torque(m_wheel_direction * m_physics_body->speed() * delta);
    if (is_action_enabled(Action::Forward))
        m_physics_body->apply_force(-vec_3to2(m_transform->forward()) * speed);
    else if (is_action_enabled(Action::Reverse))
        m_physics_body->apply_force(vec_3to2(m_transform->forward()) * speed);

    if (is_action_enabled(Action::TurnLeft))
        m_wheel_direction = std::max(m_wheel_direction - s_turn_speed * delta, -s_max_turn_direction);
    else if (is_action_enabled(Action::TurnRight))
        m_wheel_direction = std::min(m_wheel_direction + s_turn_speed * delta, s_max_turn_direction);
    else
        m_wheel_direction *= 1.0f - (10.0f * delta);

    auto wheel_rotation = m_wheel_transform->rotation();
    auto support_rotation = m_wheel_support_transform->rotation();
    m_wheel_transform->set_rotation(vec3(wheel_rotation.x, glm::radians(m_wheel_direction * 30.0f), wheel_rotation.z));
    m_wheel_support_transform->set_rotation(vec3(support_rotation.x, support_rotation.y, glm::radians(m_wheel_direction * 30.0f)));
}

