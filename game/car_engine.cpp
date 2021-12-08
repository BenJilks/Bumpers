#include "car_engine.hpp"
#include "engine/physics/collision_shape_utils.hpp"
#include "gameobject/physics/physics_body.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/attributes.hpp"
#include "gameobject/physics/collider.hpp"
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
    m_physics_body = gameobject.first<PhysicsBody>();
    m_transform = gameobject.first<Transform>();
    m_collider = gameobject.first<Collider>();

    m_action_enabled.fill(false);
}

void CarEngine::update(Object::GameObject&, float delta)
{
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
}

