#include "in_car_camera.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/physics/physics_body_2d.hpp"
#include "gameobject/transform.hpp"
#include "engine/input.hpp"
#include "glm/trigonometric.hpp"
#include <GL/freeglut.h>
#include <glm/gtx/string_cast.hpp>
#include <cassert>
#include <iostream>
using namespace Object;
using namespace Game;
using namespace glm;

void InCarCamera::init(GameObject &gameobject)
{
    m_transform = gameobject.first<Transform>();
    m_player_transform = m_player->first<Transform>();
    m_player_body = m_player->first<PhysicsBody2D>();
    assert(m_transform);
}

void InCarCamera::update(GameObject&, float delta)
{
    assert(m_transform);
    assert(m_player_transform);
    assert(m_player_body);

    auto accel = (m_last_velocity - m_player_body->velocity()) * delta;
    m_last_velocity = m_player_body->velocity();

    m_head_offset += vec3(accel.x, 0, accel.y) * 20.0f;
    m_head_offset += -m_head_offset * 40.0f * delta;

    auto rotation = m_player_transform->rotation().y + glm::radians(180.0f);
    auto position = vec3(std::sin(rotation) * 2, 3, std::cos(rotation) * 2) - m_head_offset;
    m_transform->set_position(m_player_transform->position() + position);
    m_transform->set_rotation(vec3(0, rotation, 0));
}

